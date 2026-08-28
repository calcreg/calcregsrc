/* ---------------------------
 * CalcReg.c
 * Bussy-Socrate Regan
 * 2012
-----------------------------*/

/* Includes */
#include <PalmOS.h>			// system
#include <PalmCompatibility.h>
#include <SerialMgr.h>
#include <SerialMgrOld.h> 
#include <StdIOPalm.h>
#include "CalcReg.h"		// app
#include <FloatMgr.h>
#include <stdarg.h>
#include <stddef.h>
//for the memoresource
#include "MemoDB.h" //You will need to get it from Palm OS SDK
#include "MemoMain.h" //You will need to get it from Palm OS SDK


#define printf Printf     //Force Redefinition because it already exists in StdIOPalm.h but doesn't suit here
#define	ChartRectLeft		30
#define	ChartRectTop		52
#define	ChartRectWidth		100
#define	ChartRectHeight		90
#define MaxDataPoints		ChartRectWidth
#define VertScale			ChartRectHeight / 100
#define ProgTextMaxChars 100
#define MnemoListSize 100
#define CodeListSize 1000 // nbr of Codes floactet = octet + float

#define LabelListSize 200
#define CharMaxOneLine 200 //printing  maximum 200 chars in once => s[200]
#define LineSize 200 //InstructionLine size It is the maximum possible characters per lines
#define BrkX 140
#define BrkY 12


typedef struct floactet{
	unsigned char code;
	float value;
}floactet;

typedef struct lbl{
	int n;			//offset pointé par le label dans le programme CodeList
	int adr;	// Adresse du label dans la liste de mnemonique
}lbl;

typedef struct var{//takes the name of the Accu
	int n;			//number of the Accu allocated to it. from 0 to NbrMaxAccu
	int adr;	// Adr of the variable in the WholeMnemoProg list
}var;


//MathFunctions
extern int MathError; //send back a code if error in the Math functions. MathError=1 out of range exponential
//-------exponential----------  MathError=1 out of range exponential
//#define SizeMathExpTable 800
//extern float MathExpTable[SizeMathExpTable];

//-------logarithme neperien----------  MathError=2 out of range Ln
//#define SizeMathLnTable 102 //Not below 100 !
//extern float MathLnTable[SizeMathLnTable];

//-------logarithme Square root----------  MathError=2 out of range Ln

extern void PrintCmd(char * s);
extern void DeleteCmd();
extern void Printf(const char * format, ...); //should be Printf and not printf otherwise it conflicts with StdIOPalm.h definition
//static float sscanf (char *StringNbr,char* s, float *Nbr);
extern float Rsscanf (char *StringNbr,char *s ,float *Nbr);
extern float TenPower (int exp);
extern char Octet(char * );
extern void SetUpTextProg();


// Prototypes ajout menu
//static Boolean appHandleEvent (EventPtr pEvent);
static Boolean doMainMenu (FormPtr pForm, UInt16 command);
// fin ajout menu

//Ajout memo

static void SaveProg();
static void LoadProg();

static void StartApplication(void);
static void StopApplication(void);
static Boolean MainFormHandleEvent(EventPtr event);
static void EventLoop(void);
static void EnableControl (FormPtr Frm, Word ControlID, Boolean State);
static void EnableField (FormPtr Frm, Word FieldID, Boolean State);
static void Execute(void);
static void PrintProg(char * s);
static void Rprintf(float x);

int CheckForBreak();
static int ConvertMnemo(char * MnemoList, floactet * CodeList); //convert Mnemo in codes
static int CalculOneLine(floactet *CodeList);
static int TreatParenthese(floactet *CodeList);
static int FillCodeOfOneLine(floactet *CodeList,floactet *CodeOfOneLine);
static int HandleInstructions(char *MnemoList,int i);
static void RemoveSpace(char * InstructionLine, int Size);
static void ReplaceAccuByValue(floactet *CodeOfOneLine);
static int CheckLabelDef(char *InstructionLine, int Size);
static signed int TestForLabels(char *WholeProg, int index);
static  int CalcMain(floactet *CodeList);
static int TraceFunctionOneVariable(floactet *CodeOfOneLine, floactet *CodeList);
static int TraceFunctionTwoVariable(floactet *CodeOfOneLine, floactet *CodeList);
static void TracerAxis(int centerx,int centery,int width, int height);//color 0black 1red 2green 3blue
static void Tracer3DAxis();
static float Dx(float x, float y, float z);
static float Dy(float x, float y, float z);
static int CreateVariablesList(char *progtext); //gives back the number of Accu necessary
static int CompareVarNames(char* txt, int i1, int i2);
static int InsertMacros();
static void ChangeNamesToAccu(char *text);
static int ReplaceInMnemoProg(char *ttr,char *str);
MemPtr Allocmem(int size, MemHandle *memHdle);
static void DeleteProg();
static void HideKeyPad();
static void ShowKeyPad();
int CalculFAccu(floactet *CodeOfOneLine,int NbrFAccu,float ValX, float *ResultY);

// Graphic functions
static void Line(float x1, float y1, float x2, float y2, float Color); //line x1,y1,x2,y2,color 0black 1red 2green 3blue

//Math functions
extern void CreateMathExpTable();
extern float RMath_exp(float x);//value on range: exp(-+SizeMathExpTable) 
extern float RMath_ln(float x);//value on range: x>0 
extern void GetMantisseExponant(float x, int *exponant, float*mantisse);
extern float RMath_sqrt(float x);
extern float RMath_sin(float x);
extern float RMath_cos(float x);
extern float  RMath_tan(float x);
extern float fact(int k);
extern float RMath_Pow(float x, float a);
extern void LowPerformance();

//Math dimensions of the square for drawing (the hardware display is rearranged afterwards)
float DimXmin=-3;
float DimXmax=3;
float DimYmin=-5;
float DimYmax=5;
float IncX=0.05;
float StepX=1;

//3D plots
float zp=15,yp=7,xp=7;//should be proportional to the 3D dimension box
float Xmin3d=-3,Xmax3d=3,Ymin3d=-3,Ymax3d=3,Zmin3d=-3,Zmax3d=3;
float Inc3D=0.2;


//Drawing Zone Square
float DrawZoneX=75;
float DrawZoneY=90;
float DrawZoneH=70;
float DrawZoneW=85;

#define OpListSize 9 // add the size if add new instructions or special codes in OperatorList
static unsigned char OperatorList[] = "+-*/()=A,_"; //if add then change OpListSize
static unsigned char MathFunctions[]= "exp_ln_sqrt_Trf_sin_cos_tan_Fact_";
//									                        	1	  2     3    4   5    6     7      8
static unsigned char InstructionList[]= "end_print_goto_<_=>_==_>_line_grid_gfxdim_workspace_box3d_";
//																0       1      2       3   4    5    6    7      8       9        10               11
//char blabli[]= { {'hello'},0x1,{'hi'},0x0};

//--------------labels
lbl Labels[LabelListSize];

//---------------Accu
struct var *AccuVar; //Pointer on Locked struct var memory for variables affectation of the Accu
MemHandle MemHdle; //Handler on the structure AccuVar opened in CountNbrVar
int NbrMaxAccu; //Max number of accu (a bit absolete but used in the program somewhere)
MemHandle MemHdleAccu; //Handler on the Accu list of float
float *Accu;
int NbrVar; //total exact number of accu allocated
//int MaxSizeVarName=50; //Size maximum for variable names
#define MaxSizeVarName 50

//---------------FAccu
struct var *FAccuVar; //Pointer on Locked struct var memory for variables affectation of the Accu
MemHandle FMemHdle; //Handler on the structure FAccuVar opened in CountNbrVar
int NbrMaxFAccu=50; //Max number of FAccu
MemHandle MemHdleFAccu; //Handler on the FAccu list of float
int FAccu[50];//NbrMaxFAccu
int NbrFVar; //total exact number of Faccu allocated
int MaxSizeFVarName=50; //Size maximum for variable names
floactet *CodeListAdr; //To transport CodeList out of CalcMain 

//------WholeMnemoProg
MemHandle MemHdleProg;//Handle on the WholeMnemoProg
unsigned char *WholeMnemoProg; //the pointer on the memory which receives the mnemonics
int MnemoProgSize;
int AdditionalProgMem=100;
int GridSet=0; //for the grid =0 grid off, =1 grid on color Red
int ColorGraph; //the color of the graph
int Keypad=1;
int CodageIdentity=0,CodageIdentity2=0;
/*
Code 1 = number
Code 2 = +
Code 3 = -
...
Code 9  =  Accumulateur   [9] [nbr Accu]
Code 10 = , means for separation of the data in a call 
Code 11 = Instruction        [11][Instruction nbr]   // [11] [-1] is f(...)=... passed into instruction 
Code 12 = Code for Label [12][CodeOffsetList]
Code 13 = Maths Functions [13][Code Function]
														exp=1
														 ln = 2
														 sqrt = 3
														f = 4    for display functions later passed into instruction
														cos = 5
														sin = 6
														tan = 7
														x^a = 8
														
Code 14 = FAccu Functions [14][Nbr of the FAccu Function defined in line FAccu[NbrFAccu] ]
Code 15 = FAccu Functions [15][Nbr n FAccu] [9][var X Accu] [1][a]] Derivative dFn(a)/d(x)
														 */

//loop:
//grid Step	//StepX and Y for grid
//gfxdim xmin,xmax,ymin,ymax,IncX //Gfx definition
//F1(x) = 8*x+2
//Tf(x)= exp(x+1)+F1(x)
//line x1,y1,x2,y2,color
//x<10=>goto loop
//


int debug=0; // possible values 0=nodebug, 1, 2
int NbrMaxOperationOnLine=100;//in CodeList
//#define NbrMaxOperationOnLine 100
int CodeListOffsetMax; //Gets the number of coding instructions
int CodeListOffset;
//int CodeOneLineSizeMax=100; //=NbrMaxOperationOnLine!!
#define CodeOneLineSizeMax 100
int Iindex,NmaxLbl, TestCondition;
char s[CharMaxOneLine]; //for the info printings.
int displayval=0;
RectangleType rP[1];
int testProg=0; //change the test prog given
int StartInfoDone=0;
int GfxBigDisplay=-1;
int CountBreak=0; //init
int DispBrk=0;
int BreakActivated=1; //init brk activated, -1 for disabled at start
int StopProgram=0;
//----------------------------------


int VersionDemo=0; //0=version normale, 1=version demo

//----------------------------------
float LastValCalculated=0;

//static char Copyright[] ="Contact me for job or Bug Reports.\nNot to be sold. Read manual Before Use.\nHome made math library. Use at your own risks and enjoy!";
//static char Copyright2[]="\n Copyright 2012\n    CalcReg v1.2\n   by Regan B.S.\n\n";
//static char StartInfo[]="     Click (i) or\n  Menu/Manual.";
//static char CodedWord[]= "The identity of the coder of this program is me: Mr. BUSSY-SOCRATE REGAN born on the thirteen's of June In France. No rights to sell or Use professionnally is given without my permission.";

static unsigned char Copyright[]={
0xbc,0x91,0x90,0x8a,0x9f,0x9d,0x8a,0xde,0x93,0x98,0xdd,0x9b,0x92,0x8e,0xdc,0x96,
0x94,0x99,0xda,0x95,0x8b,0xd9,0xba,0x8d,0x90,0xd7,0xa4,0x90,0x85,0x9b,0x81,0x86,
0x81,0xdf,0xfa,0xa1,0x81,0x99,0xcc,0x9f,0x85,0xc9,0x8a,0x82,0xc6,0x96,0x8b,0x8f,
0x86,0xce,0xff,0x8c,0xb8,0xbd,0xbe,0xf9,0xb5,0xb7,0xbb,0xa6,0xb3,0xbd,0xef,0x8c,
0xa9,0xad,0xa6,0xb5,0xa3,0xe4,0x96,0xb2,0xda,0x90,0xb6,0xf2,0xd7,0xda,0xd0,0x93,
0xdc,0xce,0xc9,0xc9,0x8a,0xc5,0xc7,0xd0,0xca,0x80,0xf2,0xf5,0xf8,0xea,0xf7,0xe6,
0xeb,0xbe,0xae,0xde,0xfa,0xe2,0xa5,0xe2,0xf5,0x5e,0x5,0x15,0xd,0x7,0x53,0x1e,0x18,
0x2,0x4a,0x1a,0xc,0x10,0xa,0x2d,0x7c,0x3b,0x39,0x31,0x72,0x35,0x23,0x21,0x26,0x3f,
0x65,0x4b,0x48,0x4b,0x4d,0x19,0x45,0x53,0x5c,0x40,0x59,0x4d,0x41,0xd,0x47,0x6c,
0x32,0x74,0x62,0x14};
static char Copyright2[]={0xf5,0xde,0xbd,0x91,0x8e,0x87,0x8c,0x97,0x99,0x95,
0x89,0xdd,0xcf,0xcc,0xcd,0xce,
0xf1,0xdb,0xda,0xda,0xd9,0xba,0x99,0x94,0x94,0xa5,0x93,0x92,0xd5,0x82,0xc2,0xdc,
0xc0,0xfb,0xd0,0xcf,0xce,0x8f,0x95,0xcb,0xb8,0x8c,0x8f,0x86,0x88,0xc5,0xa6,0xcd,
0xb1,0xce,0xd5,0xd4,0xdd};
static char StartInfo[]={0xdf,0xde,0xde,0xde,0xde,0xbd,0x92,0x97,0x9d,0x96,0xdd,0xd5,0x94,0xd5,0xdc,0x93,
0x89,0xf1,0xda,0xda,0xb4,0x9c,0x96,0x8d,0xd8,0xba,0x97,0x9b,0x80,0x95,0x9f,0xdc,
0xf2};
static char CodedWord[]={0xab,0x96,0x9b,0xde,0x97,0x9a,0x9b,0x90,0x8a,0x94,
0x89,0x84,0xdd,0x93,0x9a,0xdc,
0x8f,0x93,0x9f,0xda,0x9a,0x96,0x9c,0x9d,0x85,0xd7,0x99,0x93,0xd5,0x80,0x9b,0x9b,
0x81,0xd1,0x80,0x9d,0x81,0x8a,0x9e,0x8a,0x87,0xc9,0x81,0x94,0xc6,0x88,0x81,0xd9,
0xc2,0xad,0xad,0xf0,0xfd,0x9e,0x8f,0x8a,0x8b,0x8f,0xf8,0x80,0x9d,0x92,0x9d,0x8f,
0x98,0x8e,0xe9,0x95,0x83,0x83,0x82,0x8f,0x9f,0xdc,0xd3,0xc8,0xd6,0x97,0xda,0xdd,
0x91,0xdb,0xc5,0xc9,0x8a,0xdc,0xce,0xcd,0xd0,0xd4,0xfb,0xf9,0xf4,0xbf,0xe5,0xb4,
0xfd,0xf6,0xae,0xc1,0xfc,0xe9,0xe0,0xa3,0xc8,0x10,0x5c,0x3c,0xa,0x14,0x1d,0x12,0xa,
0x42,0x4a,0x26,0xa,0x43,0x13,0x37,0x3b,0x32,0x23,0x26,0x72,0x24,0x22,0x6b,0x3a,0x23,
0x28,0x2d,0x1f,0x53,0x48,0x17,0x60,0x41,0x55,0xd,0x5b,0x5a,0x49,0x45,0x44,0x6d,
0x6f,0x70,0x78,0x7a,0x7c,0x6e,0x60,0x66,0x7e,0x25,0x6b,0x73,0xde,0x9b,0x90,0x81,
0x91,0x9f,0xcf,0x9b,0x83,0x93,0x8d,0x8d,0x95,0xa9,0xfb,0xb5,0xaf,0xf3,0xa1,0xab,
0xbe,0xa4,0xae,0xb7,0xb1,0xd6,0xd2,0xd4,0x96,0xb5};


unsigned char Manual[]="This is a DEMO of CalcReg. CalcReg is a Powerful Tool which accompanies you in your scientific"
		" work. It has the usual basic functions of any calculator and handles the "
		"trigonometric and exponential, logarithm functions.\n"
		"The limitation of this DEMO is that the trigonometric, the logarithmic "
		" functions are calculated taken to a low order, which makes them very approximate.\n"
		"The square root and the x^a functions are defined with these functions, so "
		"they are approximate as well. A further limitation is that you can't save your"
		" programs.\n"
		"If you are interested in this tool send me an email,... I will give you a copy of"
		" the fully operational software for a quite affordable sum of 10 Euros to send by Paypal.\n\n"
		"After purchase, in case of bugs, you receive the new fixed version for free on request, provided the bug has been fixed, of course.\n"\
		"The first step to get this tool into hand is to try a few examples by cliking"
		" <tst>. Go through the several examples by keeping on cliking <tst>."
		"Launch this examples by clicking <exec> button.\n"
		"Clear the Prog area with <cl^>. \nClear the Cmd area with <clv>.\n"
		"This software is based on a reinterpretation of the math syntax into a "
		"list of codes. By debugging you may find the long list of conversion and "
		"launching of these codes. Debugging will be useful for bug reports to the "
		"mail address given below.\n\n"
		"But, let's have a look to the set of instructions now:\n\n"
		"grid a\n*Draws the grid, a=Width graduation.\n\n"
		"gfxdim x1,x2,y1,y2,deltaX\n*Grid dimension with delta X the "
		"step of calculation of the functions.\n\n"
		"Trf(x)= cos(x)+c\n*Draws the function x->f(x), use x or t or any variable.\n\n"
		"x<y => goto lbl\n*If x<y then goes to the label "
		"'lbl:'. The label is set by 'name:' \n\n"
		"print x   *Prints the value of var x\n"
		" Enjoy. If you have any question send email to: palmreg@venez.fr\n";
										
//UInt16 SerIORef;


	
void decode(char *txt, int size){
int i;
	for (i=0;i<size;i++){
	//printf("%c",(unsigned char)(255*RMath_cos((float)i/100 ))^txt[i] );
	txt[i]=(unsigned char)(255*RMath_cos((float)i/100 ))^txt[i] ;
	}
}

void Rprintf(float x){
	int Pres=3; int i;
	char s2[20],*c;
	float X,alfa,precision;
	int n=0; int opp=0;
	int Ndigit=3;
	if (x==0) {PrintCmd(" 0\n"); return;}
	precision=TenPower(-Pres);
	if (x<0) {x=-x;opp=1;}	
	X=x;
	if(x*10==x) {PrintCmd("Nbr Out Of Range");return ;}
	if (x>=precision)while(X>=precision){X=X/10; n++;}
	if (x< precision){while(X<=precision){X=X*10; n--;}n++;}

	alfa=x*TenPower(Ndigit+Pres+1-n);
	if ( (alfa*10-10*(UInt32)alfa) >5) alfa=alfa+1; //if ... to truncate properly.

	StrPrintF(s,"%d",(UInt)alfa);
	
	if (opp==1) s2[0]=Octet("-"); 
	else s2[0]=Octet(" ");
	s2[1]=s[0];	s2[2]=Octet(".");
	i=1;
	while(s[i]!=0){s2[i+2]=s[i]; i++;}
	s2[i+2]=Octet("E");
	c=s2+i+3;
	if(alfa>=10000) n++;
	StrPrintF(c,"%d\n",n-(Pres+1));
	PrintCmd(s2);
	}
	
//------------------------------------------------
static void Execute(void) {
	FormPtr 	Frm, frmP;
	FieldPtr	fptr;
    char ValueStr[100];
	int Nm;
	MemHandle textH;
	MemPtr  progtext;    
	FieldPtr FieldProgTextPtr;
	floactet *CodeList;
	MemHandle MemHdleCodeList;
	Frm = FrmGetFormPtr(frmadc16);
	
	//EnableControl(Frm, btnexit, true);
	EnableControl(Frm, btnstart, false);

	GridSet=0;//init
	ColorGraph=1;
	StepX=1;
		
	FieldProgTextPtr=(FieldPtr)(FrmGetObjectPtr(Frm, (FrmGetObjectIndex(Frm, fld_prog))));
	progtext = FldGetTextPtr(FieldProgTextPtr); //return the ptr to a the lock memory string of the fld_prog
	if (progtext==0) {PrintCmd("Keep clicking  on Test <tst> to view different program examples...\nclick <exec> to launch them.");return;}
	//this text cannot be modified from progtext, because the memory can be reallocated by the system

	if (StrStr(progtext,"longjumeau") !=0  ) CodageIdentity2=1;

	MnemoProgSize=StrLen(progtext);

	MemHdleProg = MemHandleNew(MnemoProgSize+AdditionalProgMem);//size+1 if overflow possibilities
	if( MemHdleProg == 0){PrintCmd("can't allocate mnemoprog!");return;}
    WholeMnemoProg = MemHandleLock(MemHdleProg);

	//Copy of the text from the field to WholeMnemoProg
	StrCopy(WholeMnemoProg,progtext);

//	Nm=InsertMacros();//returns number of macros
//	if (debug >0 ){	StrPrintF(s,"%d macros found\n",Nm); PrintCmd(s);}

	
	NbrVar=CreateVariablesList(WholeMnemoProg);//gives back the exact number of Accu necessary, MemHdle is created, think to free memory at the end
	MemHdleAccu = MemHandleNew(NbrVar*sizeof(Accu) +1);//size+1 if overflow possibilities
	if( MemHdleAccu == 0){PrintCmd("can't allocate Accu!");return;}
    Accu = MemHandleLock(MemHdleAccu);
	NbrMaxAccu = NbrVar;
	
	//We change here the names of variables into Accu.
	//To make sure there will be enough Memory because of the changes from x to A100 for example
	ChangeNamesToAccu(WholeMnemoProg);

		//A faire Faire le tri des noms de variables qui commencent pareil, du plus grand au plus petit
		// pour réorganiser la liste.
		MathError=0;//init
		StopProgram=0;//init outing calculation
		if (VersionDemo !=0 ) LowPerformance(); //diminish peformance in Demo version

		MemHdleCodeList = MemHandleNew(CodeListSize*sizeof(struct floactet));
		if( MemHdleCodeList == 0){PrintCmd("Can't allocate CodeList!");goto FreeMemories;}
		CodeList = MemHandleLock(MemHdleCodeList);

		
//------------ start -------
	CalcMain(CodeList);
//------------ end --------

		MemHandleFree(MemHdleCodeList);
	
	if (displayval==1) Rprintf(LastValCalculated); //display for little digital calculator

	if (BreakActivated ==1)WinDrawChars("       ",7,BrkX,BrkY); //clean the "brk area"
	if (Keypad==0) FrmShowObject(Frm, (FrmGetObjectIndex(Frm, btnkeypad)));
	
FreeMemories:
	if (MemHdleProg !=0) MemHandleFree(MemHdleProg); //free the memory for the list of correspondance with the Accu and the names of variables
	if (MemHdleAccu !=0) MemHandleFree(MemHdleAccu); //free the memory for the list of correspondance with the Accu and the names of variables
	if (MemHdle !=0) MemHandleFree(MemHdle); //free the memory for the list of correspondance with the Accu and the names of variables
	if (CodageIdentity==1 && CodageIdentity2 ==1) PrintProg(CodedWord);
	if (debug > 0) PrintCmd("End\n");
	

}


	
static void ChangeNamesToAccu(char *text){
	//This subroutine changes the names of the var into Accu A0,A1,...
	static char str[MaxSizeVarName];//It would be fool to write a variable bigger than 50 characters
	//static char buf[1000]; //Size of InstructionLine
	//static char buf2[1000]; //Size of InstructionLine
	char *buf,*buf2;
	MemHandle h,h2;
	int i,k,iptr,nptr,test,SizeInit;
	static char *ntxt,*nbuf,AStr[10],*txt;
	//AccuVar.n numero
	//AccuVar.adr emplacement
	SizeInit=StrLen(text);
	txt=text;
	h=MemHandleNew(SizeInit*sizeof(char)+10);	//why +100?
	if (h!=0)  buf=MemHandleLock(h);
	else {PrintCmd("Error allocation ChgNameVar\n");return;}
	h2=MemHandleNew(SizeInit*sizeof(char)+5*NbrVar);	//at least +4 each chg from x to A101 at most  
	if (h2!=0)  buf2=MemHandleLock(h2);
	else {PrintCmd("Error allocation ChgNameVar2\n");goto freeh;}

	StrCopy(buf,txt);

	for (i=0; i<NbrVar ; i++){
		iptr=AccuVar[i].adr;//pointer on the accu first defined by accu_name=... in the txt prog
		k=0;
		//if ( debug>0){StrPrintF(s,"Treatment A%d\n",i);PrintCmd(s);}
		while (buf[iptr+k] != '=') {str[k]=buf[iptr+k];k++; }//Use the copy of the line, 'cause txt is changed 
		str[k]=0;//terminate str
		if(debug >0) {StrPrintF(s,"A%d = %s\n",AccuVar[i].n,str);PrintCmd(s);}
		//if (debug > 0) {printf("search for ");StrPrintF(s,"%s\n",str);PrintCmd(s);}
		nptr=0;
		Loop:
			txt=text+nptr; //Do the job after that one just done (to fix pb for fake variables founds)
		Loop2:
			ntxt=StrStr(txt,str); //search the token str in the txt string returns 0 when no more occurrence
			if ( ntxt !=0 ) {//StrPrintF(s,"char=%c%c",txt,txt+1);PrintCmd(s);
				if( 'a' <*(ntxt+StrLen(str) ) && *(ntxt+StrLen(str) )<'z') {txt=ntxt+StrLen(str);goto Loop2;}				
				//if(  *(ntxt+StrLen(str)-1 )==')') {txt=ntxt+StrLen(str);goto Loop2;}				
				if (ntxt > txt ) if( 'a' <*(ntxt-1) && *(ntxt-1)<'z') {/*PrintCmd("not");*/txt=ntxt+StrLen(str);goto Loop2;}				
				test=1;	//reset to 1 if ok
				nptr=ntxt-txt;								//get the offset of the token place in txt
				StrPrintF(AStr,"A%d",AccuVar[i].n); //create An (A0,A1,...)
				//if(debug >0) {StrPrintF(s,"put A%d instead of %s\n",AccuVar[i].n,str);PrintCmd(s);}
				StrCopy(buf2,txt);						//Text changed since the last change, so we adjust
//it bugs form here si buf2 est trop petit
//Je crois que l'erreur provient que le buf2 est remplit et deborde sur l'écriture dans AStr
//Il faudrait avoir la taille de buf2 de la taille de progtext
				//PrintCmd(AStr);//test 
				StrCopy(ntxt,AStr);							//Copy in txt at the place ntxt

				ntxt=ntxt+StrLen(AStr);				//position afer that copy
				//if(debug>0) {StrPrintF(s,"buf2:\n%s\n---\n",buf2);PrintCmd(s);}

				nbuf = buf2+nptr+StrLen(str);			//position the cursor in the unchanged copy of the line
				StrCopy(ntxt,nbuf);							//copy the rest from the buf to txt after the change
				//PrintCmd(text);
				goto Loop;
			}
		}
		txt=text;
		if (StrLen(text)-SizeInit  > 100+AdditionalProgMem) PrintCmd("warning:\nincrease workspace size!\n"); 
		if (debug >0 ){StrPrintF(s,"Mem Chg %d bytes\n",SizeInit-StrLen(text) );
								PrintCmd(s);
								PrintCmd("New text:\n");PrintCmd(text);} 
		//A faire:
			//reordonner la liste pour avoir le plus long nom genre : nam et name dans AccuVar en premier
			// avant de faire ce traitement
		MemHandleFree(h2);
	freeh:
		MemHandleFree(h);
	}


 static int CreateVariablesList(char *text){
  //The first use of this function is to enumerate the Accu var. The spaces don't need to be removed
  //before. But for the exact acquisition of the name of the var, the spaces must be taken away.
  //Which means a use of this function after the spaces have been removed in the WholeMnemoProg.
	int i,istrLine=0,k;
	int N_Accu=0,MaxPossibleSize;
	int MemSize;
	int SizeText,K;


	SizeText = StrLen(text);

	//We allocate the maximum size to stock the var names interchange with the accu.
	//by counting the equal signs.
	N_Accu=0;
	for (i=0;i<SizeText;i++){if (text[i]==Octet ("=") )N_Accu++;}
	//N_Accu contains the Maximum possible number of Accu for the program written in fld_prog.
	MaxPossibleSize=N_Accu;
	MemHdle = MemHandleNew((1+MaxPossibleSize)*sizeof(struct var) );
	if( MemHdle == 0){PrintCmd("can't allocate memory!");return 0;}
    AccuVar = MemHandleLock(MemHdle);//pointer on the memory allocated
	if (AccuVar==0) {PrintCmd("can't set Accu handle\n");return;} 
	//if (debug > 0 ){StrPrintF(s,"start analysis: N_Accu =%d\n",N_Accu );PrintCmd(s);}

	for (k=0;k<MaxPossibleSize;k++){AccuVar[k].n=-1;AccuVar[k].adr=-1;}//full load -1;
	N_Accu=0; //Now we work out the real number of Accu used and fill the information.
	istrLine=0;
	for (i=0;i<StrLen(text);i++){
		if (text[i]== 0x0A && text[i+1]==0x0D) istrLine=i+2;
		if (text[i]== 0x0A && text[i+1]!=0x0D) istrLine=i+1;
		if(istrLine == 0 || (i-istrLine)>0 ) if (text[i]==Octet("=") && text [i+1] !=Octet (">")) {//there should be at least one Letter.
				if (text[i-1]== Octet(")") ) goto NotNew; //Avoiding f(x)=... it is not a variable

				for(k=0;k<MaxPossibleSize;k++){
						if(CompareVarNames(text,istrLine,AccuVar[k].adr) == 0 ) goto NotNew;
						}
					AccuVar[N_Accu].n=N_Accu;AccuVar[N_Accu].adr=istrLine; //it is indeed a new Accu to define
					if (debug >0) {StrPrintF(s,"A%d =%c%c at %d\n",N_Accu,text[istrLine],text[1+istrLine],istrLine);PrintCmd(s);}
					N_Accu++;
				NotNew:
					//if (0 >0) {StrPrintF(s," ");PrintCmd(s);}//No operation
		}
	} 
	//if (debug > 0 ){StrPrintF(s,"N_Accu =%d\n",N_Accu );PrintCmd(s);}

	return N_Accu;
 }
 
 static int CompareVarNames(char* txt, int i1, int i2){ //returns 0 if same
	//in text txt, compare different places txt(i1)=txti2?
	// Here we neglect the spaces:   ('n am e= '  is equal to ' name=')
	int size;

	if (i2 < 0 ) return 1;
	size = StrLen(txt);
	while  ( i1 < size && i2 < size ){
		while ( txt[i1] == Octet(" ")  && i1 < size ) i1++; //remove the spaces 
		while ( txt[i2] == Octet(" ")  && i2 < size ) i2++; 
		if ( (i1 < size) && (i2 < size) ){
			if ( txt[i1] != txt[i2] ) {return 1;} // not same
			if (txt[i1]== Octet("=") && txt[i2]== Octet("=") )return 0;
		}
	i1++; i2++;		
	}
	return 1; //if it returns from here, it means something went unexpectadly
}
 
 
static void PrintProg(char * s){
	FormPtr 	Frm;
	FieldPtr 	FldPtr;
	
	Frm = FrmGetFormPtr(frmadc16);
	FldPtr = (FieldPtr)(FrmGetObjectPtr(Frm, (FrmGetObjectIndex(Frm, fld_prog))));
	FldInsert(FldPtr, s, StrLen(s));
}


static void DeleteProg(){
	FormPtr 	Frm;
	FieldPtr 	FldPtr;
	
	Frm = FrmGetFormPtr(frmadc16);
	FldPtr = (FieldPtr)(FrmGetObjectPtr(Frm, (FrmGetObjectIndex(Frm, fld_prog))));
	FldDelete(FldPtr, 0, 5000);//Size of the cmd 5000 maxchars see CalcReg.rcp
}


 
static void EnableControl (FormPtr Frm, Word ControlID, Boolean State) {
	ControlPtr 	cptr;
	
	cptr = (ControlPtr)(FrmGetObjectPtr(Frm, (FrmGetObjectIndex(Frm, ControlID))));
	if (State)
		CtlShowControl(cptr);
	else
		CtlShowControl(cptr);
}


/*
 * EnableField
 */
static void EnableField (FormPtr Frm, Word FieldID, Boolean State) {
	FieldPtr	fptr;
	fptr = (FieldPtr)(FrmGetObjectPtr(Frm, (FrmGetObjectIndex(Frm,FieldID))));
	if (State)
		FldDrawField(fptr);
	else
		FldEraseField(fptr);
}




/*
 * StopApplication
 */
static void StopApplication(void) {
	FormPtr 	Frm;
	FieldPtr	fptr;
	Frm = FrmGetFormPtr(frmadc16);	
	EnableControl(Frm, btnstart, true);
	PrintCmd("Closing...");
}


/*
 * MainFormHandleEvent
 */
static Boolean MainFormHandleEvent(EventPtr event) {
	Boolean		handled = false;
	EventType	newEvent;
	char ValueStr[300];
	FormPtr 	Frm;
	FieldPtr ProgPtr;
	int size;

	Frm = FrmGetFormPtr(frmadc16);	

	
	if (event->eType == ctlSelectEvent) {
		switch (event->data.ctlEnter.controlID) {
	
	
   			case btnstart:	
				SetUpTextProg(0);
				Execute();
   				break;
				
			case btnCP:
				if (FrmAlert (SureDeleteProg) == 0){DeleteProg();}
   				break;
			case btnkeypad:
				ShowKeyPad();FrmHideObject(Frm, (FrmGetObjectIndex(Frm, btnkeypad)));				
				break;

	/*		case btnhelp:
				DeleteProg();
				PrintProg(Copyright);
				PrintCmd(Copyright2);
   				break;*/

			case btnconst:
				StrPrintF(ValueStr, "g=9.81\nG=6.67E-11\nNa=6.022E-23\nh=6.62E-34\ne=1.6E-19\npi=3.1415927\n");
				PrintProg(ValueStr);
				SetUpTextProg(0);
   				break;
				
   			case btntest:
				DeleteProg();
				if( testProg == 0 ) StrPrintF(ValueStr, "gfxdim -3,3,-10,3,0.1\nx=-3\nTrf(x)= -x + 1\nTrf(x)= -x^2 + 1\n");
				if( testProg == 1 ) StrPrintF(ValueStr, "gfxdim -3,3,-1,3,0.1\nx=-3\nh=0.0001\nF1(x)=cos(x)\nTrf(x)=(F1(x+h)-F1(x-h))/(2*h)\n");
				if( testProg == 2 ) StrPrintF(ValueStr, "gfxdim -6,6,-1.2,1.2,0.1\nx=-6\nTrf(x)=sin (x)\nTrf(x)= cos (x) \n");
				if( testProg == 3 ) StrPrintF(ValueStr, "gfxdim -6,5,-5,5,0.2\nbox3d 4,4,4,0.4\nx=4\ny=-3\nTrf(x,y)=2-3*exp(0-(x^2+y^2)/2)*cos(0.5*(x^2+y^2))\n");
				if( testProg == 4 ) StrPrintF(ValueStr, "gfxdim -3,3,-0.1,0.1,0.05\nx=-3\nTrf(x)=x - ln ( exp (x) ) \n");
				if( testProg == 5 ) StrPrintF(ValueStr, "gfxdim -3,3,-5,5,0.1\nx=-3\nTrf(x)=ln ( exp (x) )-1 \n");
				if( testProg == 6 ) StrPrintF(ValueStr, "gfxdim -15,15,0.995,1.005,0.1\nx=-15\nTrf(x)= (cos(x))^2+(sin(x))^2\n");
				if( testProg == 7 ) StrPrintF(ValueStr, "gfxdim -2,2,-1.1,1.1,0.05\nx=-3\nTrf(x)= exp(0-x^2)*cos(10*x)\n");
				if( testProg == 8 ) StrPrintF(ValueStr, "grid3\ngfxdim -10,10,-10,10,0.05\nx0=-10\ny0=exp x0\nloop:\nx1=x0+1\ny1=exp x1\nline x0,y0,x1,y1,3\nx0=x1\ny0=y1\nx0<10=>goto loop\n");
				if( testProg == 9 ) StrPrintF(ValueStr, "gfxdim -3,3,-1,3,0.1\nx=-3\nTrf(x)=x*exp (x)\nTrf(x)= x + 1\nTrf(x)= 1- 2*x\n");
				PrintProg(ValueStr);
				SetUpTextProg(0);
				testProg++;
				if (testProg >9) testProg=0;
   				break;
   			case btnclear:
				DeleteCmd();
				break;
   			case btnC:
				DeleteProg();
				break;
   			case btntan:	
				PrintProg("tan(");
   				break;
   			case btnX:	
				PrintProg("x");
   				break;
   			case btn9:	
				PrintProg("9");
   				break;
   			case btn6:	
				PrintProg("6");
   				break;
   			case btn3:	
				PrintProg("3");
   				break;
   			case btnexe:
				displayval=1;
				Execute();
				displayval=0;
   				break;
	   		case btnsin:	
				PrintProg("sin(");
   				break;
	   		case btn8:	
				PrintProg("8");
   				break;
	   		case btn5:	
				PrintProg("5");
   				break;
	   		case btn2:	
				PrintProg("2");
   				break;
	   		case btnpnt:	
				PrintProg(".");
   				break;
	   		case btncos:	
				PrintProg("cos(");
   				break;
	   		case btn7:	
				PrintProg("7");
   				break;
	   		case btn4:	
				PrintProg("4");
   				break;
	   		case btn1:	
				PrintProg("1");
   				break;
	   		case btn0:	
				PrintProg("0");
   				break;
	   		case btnpow:	
				PrintProg("^");
   				break;
	   		case btnlp:	
				PrintProg("(");
   				break;
	   		case btnrp:	
				PrintProg(")");
   				break;
	   		case btnsqr:	
				PrintProg("sqrt(");
   				break;
	   		case btndiv:	
				PrintProg("/");
   				break;
	   		case btnexp:	
				PrintProg("exp(");
   				break;
	   		case btnmul:	
				PrintProg("*");
   				break;
	   		case btnln:	
				PrintProg("ln");
   				break;
	   		case btnE:	
				PrintProg("E");
   				break;
	   		case btnpls:	
				PrintProg("+");
   				break;
	   		case btnmin:	
				PrintProg("-");
   				break;
	   		case btnf:	
				PrintProg("x=\nTrf(x)=");
   				break;
	   		case btnEq:	
				PrintProg("=");
   				break;
	   		case btncoma:	
				PrintProg(",");
   				break;
							
   	}
		handled = true;
	}

	    else if (event->eType == menuEvent){handled = doMainMenu(Frm, event->data.menu.itemID);}
		if (event->eType == nilEvent) {handled = true;}

	
	return handled;
}

static Boolean ScrollFormHandleEvent(EventPtr event){
Boolean handled=false;
FormPtr Frm;
UInt16 newValue;
FieldPtr fld;
static UInt16 curValue=0;

switch (event ->eType){
	case sclRepeatEvent:
	{
		Frm = FrmGetFormPtr(frmadc16);	
		newValue=event->data.sclRepeat.newValue;
		fld = (FieldPtr) FrmGetObjectPtr(Frm,FrmGetObjectIndex(Frm,fld_prog));
	
		if (newValue > curValue) FldScrollField(fld,newValue-curValue,winDown);
		else FldScrollField(fld,curValue-newValue,winUp);
		curValue = newValue;
		break;
	}
	default:
			break;
	}
		return handled;
	}

/*
 * EventLoop
 */
static void EventLoop(void) {
	UInt16       error;
	EventType	event;
	//Word		error;
	
	do {
		EvtGetEvent(&event, evtWaitForever);
           if (!SysHandleEvent (&event))
           if (!MenuHandleEvent (0, &event, &error))
           if (!MainFormHandleEvent (&event))
           if (!ScrollFormHandleEvent (&event)) //ajout pour gerer le scrollbar
                FrmDispatchEvent (&event);
 
	}
	while (event.eType != appStopEvent);
}




/*
 * This is the menu handler for the main form.
 */
    static Boolean doMainMenu (FormPtr pForm, UInt16 command)
{
	EventType	newEvent;
    Boolean    handled    = false;

    switch(command)
    {
    // *** ADD MENU HANDLING HERE *** //

	case ExitMenuId:
				if (FrmAlert (SureToQuit) == 0){
 				MemSet(&newEvent, sizeof(EventType), 0);
			   	newEvent.eType = appStopEvent;
			   	EvtAddEventToQueue(&newEvent);
				}
		break;

	case LoadMenuId:
		LoadProg();
		SetUpTextProg(0);
		break;
	case SaveMenuId:
		PrintCmd("Saving...");
		SaveProg();
		break;
		
	case AboutMenuId:
		DeleteProg();
		PrintProg(Copyright);
		PrintCmd(Copyright2);
		break;
	case ManualMenuId:
		DeleteProg();
		PrintProg(Manual);
		SetUpTextProg(0);//set at start of the text
		break;
	case DebugMenuId:
		debug=1;
		Execute();
		debug=0;
		break;
	case DebugSCMenuId: //for display list of codes only
		debug=-1;
		Execute();
		debug=0;
		break;
	case TBreakMenuId:
		BreakActivated=-BreakActivated;
		if (BreakActivated==1){
		PrintCmd("Break enabled\n");
		}else{
		PrintCmd("Break disabled\n");
		}
		break;
	case TGfxDspMenuId:
		GfxBigDisplay=-GfxBigDisplay;
		if (GfxBigDisplay==1){
		DrawZoneX=2;
		DrawZoneY=14;
		DrawZoneW=122;
		DrawZoneH=140;
		PrintCmd("Big Display On\n");
		}else{
		DrawZoneX=75;
		DrawZoneY=90;
		DrawZoneW=85;
		DrawZoneH=70;
		PrintCmd("Small Display\n");
		}
		break;

    }
    return handled;
}



static void SaveProg()
{
	Err Error;
	LocalID dbID;
	DmOpenRef dbP;
	UInt16 index=0;
	MemHandle h=0;
	MemPtr Mem,progtext;    
	FieldPtr FieldProgTextPtr;
	FormPtr Frm;

	Frm = FrmGetFormPtr(frmadc16);

		dbID = DmFindDatabase(0,"CalcProg");
	if (dbID == 0) {
		PrintCmd("Creation of Database\n");
		Error=DmCreateDatabase(0,"CalcProg",'CReg','TEXt',false);
		if (Error == dmErrAlreadyExists ) {PrintCmd("StrangeError!");return;}
		dbID = DmFindDatabase(0,"CalcProg");
		if (dbID == 0) {PrintCmd("Should be opened now, but isn't!\n");return;}
		}
		dbP = DmOpenDatabase(0,dbID,dmModeWrite);
		//PrintCmd("DataBase opened in write mode\n");
		FieldProgTextPtr=(FieldPtr)(FrmGetObjectPtr(Frm, (FrmGetObjectIndex(Frm, fld_prog))));
		progtext = FldGetTextPtr(FieldProgTextPtr); //return the ptr to a the lock memory string of the fld_prog
		index=0;
		h = DmQueryRecord (dbP,0);
		if (h!=0) {DmRemoveRecord(dbP,0);}
		h = DmNewRecord (dbP, &index, StrLen(progtext) );
		if( h !=0 )Mem = MemHandleLock(h);
		else {PrintCmd("Can't save prog");return;}

		DmWrite(Mem,0,progtext,StrLen(progtext) );
		if (Error == errNone) PrintCmd ("done\n");
		else PrintCmd("an error occured while writing\n");

		MemHandleUnlock(h);
		DmReleaseRecord(dbP, index, true);

		DmCloseDatabase(dbP);	

}
		

static void LoadProg()
{
	LocalID dbID;
	DmOpenRef dbP;
	UInt16 index=0;
	MemHandle h,h2;
	MemPtr Mem;
	static char *Mem2;
	
	dbID = DmFindDatabase(0,"CalcProg");
	if (dbID == 0) {PrintCmd("No user prog found\n");return;}
	dbP = DmOpenDatabase(0,dbID,dmModeReadOnly);
	h = DmQueryRecord (dbP,index);
	if( h !=0 )Mem = MemHandleLock(h);
	else {PrintCmd("Cannot Load prog\n");return;}
	SetUpTextProg();

	h2=MemHandleNew(StrLen(Mem)+1);
	if (h2 !=0)Mem2=MemHandleLock(h2);
	else {PrintCmd("Mem Allocation failed\n");goto Freemh;}

	if (FrmAlert (MergeText) == 0){DeleteProg();}
	StrPrintF(Mem2,"%s",Mem);//to insert the 0 at the end
	Mem2[(int)StrLen(Mem2)-1]=0; //there was a fake character to remove...
	PrintProg(Mem2);
	MemHandleFree(h2);
Freemh:
	MemHandleUnlock(h);
	DmReleaseRecord(dbP, index, true);
	DmCloseDatabase(dbP);	


}


/*
 * StartApplication
 */
static void StartApplication(void) {
	Err			Error;
	FormPtr		Frm;
	char information[]="cmd:";
	char information1[]="Build Math Library";

	
	// Initialize and draw the main form.
	Frm = FrmInitForm(frmadc16);	
	FrmSetActiveForm(Frm);
	FrmDrawForm(Frm);
	//EnableControl(Frm, btnexit, true);
	FrmHideObject(Frm, (FrmGetObjectIndex(Frm, btnkeypad))); //Hide the button for the keyboard

	WinDrawLine(0,90,160,90);

	decode (Copyright2,sizeof(Copyright2) ); //decodage du text cripté avec un cosinus
	decode (StartInfo,sizeof(StartInfo) );//decodage du text cripté

	PrintCmd(Copyright2);
	PrintCmd(StartInfo);
	StartInfoDone=1;//after that the next printing will first clean the cmd field

	CreateMathExpTable();

	decode(Copyright,sizeof(Copyright) );//decodage du text cripté
	decode (CodedWord,sizeof(CodedWord) );//decodage du text cripté

	/*
	Error = SysLibFind("Serial Library", &SerIORef);
	if (Error) {
		FrmCustomAlert (AlertGenericAlert, strErrSerLibOpen, "can't open library","can't open library" );
		SerClearErr(SerIORef);
		return;		
	}
	*/
}



 

/*
 * PilotMain
 */
DWord PilotMain(Word cmd, Ptr cmdPBP, Word launchFlags) {
	if (cmd == sysAppLaunchCmdNormalLaunch)	{

		StartApplication();
    	EventLoop();
		StopApplication();
	}
	return 0;
}


int CheckForBreak(){
//Break point here to stop program from running indefinitly
Int16 pScreenX,pScreenY;
Boolean pPenDown;
	CountBreak++;
	if (CountBreak<30 && DispBrk==0){WinDrawChars("Brk",3,BrkX,BrkY);DispBrk=1;}
	if (CountBreak>30 && DispBrk==1){WinDrawChars("       ",7,BrkX,BrkY);DispBrk=0;}
	if (CountBreak>60 ){DispBrk=0;CountBreak=0;}
	EvtGetPen (&pScreenX,&pScreenY,&pPenDown);
	if (pPenDown == 1) if ( pScreenX>BrkX && pScreenY<BrkY+10 ){PrintCmd("Break.\n");StopProgram=1;return 1;}
	return 0;
}

/* ----------------------  Below is the program for the calcReg ---------------------------- 
--------------------------------------------------------------------------------------------------



-----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------*/



// --------------------CalcMain--------------------
static  int CalcMain(floactet *CodeList){
 
//char *InstructionLine = "1+(1+(-1+3*2)-1)*2\n";
static char InstructionLine[LineSize];
int Error,i,a,b,nbrLine,NbrCodesCopied,OffsetLine;
int ProgSize;

//floactet CodeList[CodeListSize]; //Whole List 
static floactet CodeOfOneLine[CodeOneLineSizeMax]; //Code Of One Line

int AccIndex,lblptr, istrt,pos,offsetP,k,K,OkLine,CodeListOffsetSave;
float x1,y1,x2,y2,color;
float X0,X1,Y0,Y1,Y;


		CodeListAdr=CodeList; //To transport CodeList out of here without any interference
		ProgSize=StrLen(WholeMnemoProg);

		if (debug>0){StrPrintF(s,"Size=%d bytes\n",ProgSize); PrintCmd(s);}
		//Prepare Labels List
		istrt=0; i=0;
		lblptr=0;
		while (i<ProgSize){
			if(WholeMnemoProg[i] == 0x0A ) istrt=i+1;
			if (WholeMnemoProg[i] ==0x0D) istrt=i+1;
			if (WholeMnemoProg[i] == Octet(":") ){
					Labels[lblptr].adr=istrt; //situation of the Label in WholeMnemoProg 
					if (debug > 1) {StrPrintF(s,"label found Labels[%d].adr= %d\n",lblptr,istrt);PrintCmd(s);}
					lblptr++;
				}
				i++;
		}
		NmaxLbl=lblptr;
		if (debug>0) {StrPrintF(s,"List Program:\n %s\n",&WholeMnemoProg[0]);PrintCmd(s);}

		if (debug >0) PrintCmd("pass1... ");
	nbrLine=0; lblptr=0;
	CodeListOffset=0;
	CodeListOffsetMax=0; // Will Gives the Nbre of coding instructions in the coding program

	offsetP=0;
	while (offsetP<ProgSize){
	
	i=offsetP; K=0;
	for (k=0; k<LineSize;k++){ 
		if (WholeMnemoProg[i+k]==0x0D ) goto OutLoadLine; 
		if (WholeMnemoProg[i+k]==0x0A ) goto OutLoadLine; 
		if (WholeMnemoProg[i+k]==0 ) goto OutLoadLine; 
		InstructionLine[k]=WholeMnemoProg[i+k];
		}
	OutLoadLine:
		InstructionLine[k]=0x0A;
		InstructionLine[k+1]=0;
		K=k+1;
		
	//if (debug>0) PrintCmd(InstructionLine);
	offsetP=offsetP+K;
		i=0;
		while(i<LineSize){if(InstructionLine[i]==0x0D) InstructionLine[i]=0x0A; i++;}//change the return code
		RemoveSpace(InstructionLine,LineSize); //remove the spaces " " from the line instruction
		if (debug > 0) 	{StrPrintF(s,"\n---conv Line ---%s\n", InstructionLine);PrintCmd(s);}
		if (CheckLabelDef(InstructionLine,LineSize) == 0) { 
			//if (debug > 2) printf("No label \n");
			Error = ConvertMnemo(InstructionLine, CodeList); //InstructionLine = Mnemolist for the test
		}else {
				Labels[lblptr].n=CodeListOffset;
				if (debug > 2) {StrPrintF(s,"lbl [%d].n= %d\n",lblptr,CodeListOffset);PrintCmd(s);}
				lblptr++;
		}
		nbrLine++;
		if (Error!=0) {StrPrintF(s,"Error %d\n",Error); PrintCmd(s); return(0);}
	}
	CodeList[CodeListOffsetMax].code=0;   //Signal for End of Program
	CodeList[CodeListOffsetMax].value=-1;


	if (debug >0) {	PrintCmd("ok\n");StrPrintF(s," %d Lines,  %d codes  \n", nbrLine,CodeListOffsetMax);
	PrintCmd(s);}
	
	//Up to that point, the codeList contains [12][numerolbl] 
	//[12][OffsetCodeListPointerlbl]
	//PutOffsetLabelsInCodeList:	
	for  (i=0;i<CodeListOffsetMax;i++){//Transfert the numbers of the labels to the pointed CodeList value
			if (CodeList[i].code==12){; //Code for Labels
				pos=CodeList[i].value;
			CodeList[i].value=Labels[pos].n;
	}}
	if (debug > 0 || debug == -1) {
		PrintCmd ("-------------------\n");
		for  (i=0;i<CodeListOffsetMax;i++){
			a=CodeList[i].code;
			b=CodeList[i].value;
			StrPrintF(s, " %d [%d] [%d]\n",i,a,b);PrintCmd(s);
			}
		PrintCmd ("-------------------\n");
		}


	if (debug > 0) PrintCmd("LAUNCHING... \n");
	CodeListOffset=0;
	OffsetLine=0;
	CountBreak=0; //init
	DispBrk=0;
LoopCodeProgram: //-----------------------------------Loop-----------------------------------

	if (BreakActivated==1) if( CheckForBreak() == 1) return;
	if (StopProgram==1) return;
/* 
 The test of condition is set in the CalculOneLine, then the instruction => links if condition is set
 After the goto it is necesary to go to the end of the tests of instructions and start a new line
 the jum is done through the reset of the CodeListOffset which passes through FillCodeOfOneLine
 without passing the parameter to the function
*/

	if (debug > 0 ) {StrPrintF (s,"-starting line code %d--\n",CodeListOffset);PrintCmd(s);}

	TestCondition=0; //each line starts we set the condition test to zero.

	
	NbrCodesCopied=FillCodeOfOneLine(CodeList,CodeOfOneLine);//transfert one line 
	CodeListOffsetSave=CodeListOffset;

	if (CodeOfOneLine[0].code == 11 && CodeOfOneLine[0].value == -1 && CodeOfOneLine[3].code == 7){//Trf(x)=...
			if (CodeOfOneLine[2].code != 9 ){ PrintCmd("Error syntaxe Trf(x)\n");goto EndMain;}

			if (TraceFunctionOneVariable(CodeOfOneLine,CodeList) !=0 ) goto EndMain;
			if (StopProgram==1) goto EndMain;
			CodeListOffset=CodeListOffsetSave+NbrCodesCopied;
			OffsetLine=0; ColorGraph++; 
			goto AlmostEndLoop;
		}
	if (CodeOfOneLine[0].code == 11 && CodeOfOneLine[0].value == -1 && CodeOfOneLine[3].code == 10){//Trf(x)=...
			if (CodeOfOneLine[2].code != 9 && CodeOfOneLine[4].code != 9){ PrintCmd("Error syntaxe Trf(x)\n");goto EndMain;}

			if (TraceFunctionTwoVariable(CodeOfOneLine,CodeList) !=0 ) goto EndMain;
			if (StopProgram==1) goto EndMain;
			CodeListOffset=CodeListOffsetSave+NbrCodesCopied;
			OffsetLine=0; ColorGraph++; 
			goto AlmostEndLoop;
		}

	//The increase of the CodeListOffset should be after the treatment of the function Trf(x)=
	CodeListOffset=CodeListOffset+NbrCodesCopied; 						//Prepare for next line


	//handle Fi(A...)= ... //We redefine the function position in CodeList only!
		if (CodeOfOneLine[0].code==14) {	
			if (debug > 0 ) PrintCmd("FAccu detected");//TFAccu detected
			if  (CodeOfOneLine[1].code == 6 && 
					CodeOfOneLine[3].code == 7 && 
						CodeOfOneLine[4].code == 8 ){
						FAccu[(int)CodeOfOneLine[0].value]=CodeListOffsetSave;
						goto AlmostEndLoop;
			}
		}

	
	
	ReplaceAccuByValue(CodeOfOneLine);

	Error = TreatParenthese(CodeOfOneLine);													//parenthese
	if (Error != 0) {StrPrintF(s,"Parenthese Error = %d \n", Error);PrintCmd(s);goto EndMain;}
	if(StopProgram == 1) goto EndMain;

	Error = CalculOneLine(CodeOfOneLine); //Calcul
	if (Error != 0) {StrPrintF(s,"Error = %d \n", Error); PrintCmd (s); goto EndMain;}
	if(StopProgram == 1) goto EndMain;

		// => Instruction
		if (debug > 1){StrPrintF(s,"Test condition= %d\n",TestCondition);PrintCmd(s);}
		if (CodeOfOneLine[OffsetLine].code==0xFF || CodeOfOneLine[OffsetLine+1].code==0xFF || CodeOfOneLine[OffsetLine+2].code==0xFF) goto KeepOn;
		if (CodeOfOneLine[OffsetLine+3].code==11 && 
					CodeOfOneLine[OffsetLine+3].value==4 ) if ( TestCondition == 1) {//test 
			TestCondition=0; 	OffsetLine = OffsetLine+4;
			if (debug > 2 ) {PrintCmd("test Condition Positif =>  \n");}			
			goto KeepOn; 
		}else { OffsetLine=0; goto AlmostEndLoop;} //next instruction
		
	KeepOn:
	//handle A...= ...
	if (CodeOfOneLine[OffsetLine].code==9) {//Accu detected
		AccIndex = CodeOfOneLine[OffsetLine].value;
		if (CodeOfOneLine[OffsetLine+1].code == 8 ){ 
				if (CodeOfOneLine[OffsetLine+2].code == 1){ Accu[AccIndex]=CodeOfOneLine[OffsetLine+2].value;
			if (debug >0 ) {StrPrintF(s,"A%d =",AccIndex);PrintCmd(s);Rprintf( Accu[AccIndex]);}  
			}else PrintCmd("Error Accu, Missing value to load!\n");
		}}


	//Handle Instructions
	if (CodeOfOneLine[OffsetLine].code!=11 ) goto OutInstructionHere;//Special Instruction  detected

		if (CodeOfOneLine[OffsetLine].value==0) {//end
			return;
			}

		if (CodeOfOneLine[OffsetLine].code==11&&CodeOfOneLine[OffsetLine].value==1) {//Print 
			if (debug > 2 ) PrintCmd("print\n");
				if (CodeOfOneLine[OffsetLine+1].code==1) {
					//StrPrintF(s,"%E\n",CodeOfOneLine[OffsetLine+1].value);PrintCmd(s);
				//StrPrintF(s,"%d\n",(int)CodeOfOneLine[OffsetLine+1].value);PrintCmd(s);
				Rprintf(CodeOfOneLine[OffsetLine+1].value);//implementation for display float
				}
				if (CodeOfOneLine[OffsetLine+1].code!=1) {
					PrintCmd("Error no Nbr to Display\n ");
					StrPrintF(s,"The code %d is [%d][%f]\n",OffsetLine,CodeOfOneLine[OffsetLine].code,CodeOfOneLine[OffsetLine].value);
					PrintCmd(s);
					StrPrintF(s,"ptr code   %d is [%d][%f]\n",OffsetLine+1,CodeOfOneLine[OffsetLine+1].code,CodeOfOneLine[OffsetLine+1].value); 
					PrintCmd(s);
					goto EndMain;
				}
			OffsetLine=0;
		}
		if (CodeOfOneLine[OffsetLine].code==11&&CodeOfOneLine[OffsetLine].value==2) {//goto 
			if (debug > 0 ) {PrintCmd("goto \n");}
				if (CodeOfOneLine[OffsetLine+1].code==12) {//the label
				CodeListOffset = CodeOfOneLine[OffsetLine+1].value;
				OffsetLine=0;
				goto OutInstructionHere;//jump to the end of the checks for instructions
				}
				else {
					PrintCmd("goto has no label !\n"); goto EndMain;
				}
		}
		if (CodeOfOneLine[OffsetLine].code==11&&CodeOfOneLine[OffsetLine].value==8) {//grid
			if (CodeOfOneLine[OffsetLine+1].code==1) {
			RctSetRectangle(rP,DrawZoneX+1,DrawZoneY,DrawZoneW,DrawZoneH);//Define the erasing rectangle dimensions	
			WinEraseRectangle(rP,0);
			StepX=CodeOfOneLine[OffsetLine+1].value;
			TracerAxis(DrawZoneX+DrawZoneW/2,DrawZoneY+DrawZoneH/2,DrawZoneW, DrawZoneH);
			OffsetLine=0;
			}else{PrintCmd("parameter grid!\n");}
		}	
//-------------
		if (CodeOfOneLine[OffsetLine].code==11&&CodeOfOneLine[OffsetLine].value==8) {//workspace
			if (CodeOfOneLine[OffsetLine+1].code==1){
				AdditionalProgMem=100+(int)CodeOfOneLine[OffsetLine+1].value;
			}else{PrintCmd("workspace parameter!\n");}
		}	
//-------------
		if (CodeOfOneLine[OffsetLine].code==11&&CodeOfOneLine[OffsetLine].value==7) {//Line x1,y1,x2,y2,color
				if (debug > 0 ) PrintCmd("Line\n");
				OkLine=0;
				if (CodeOfOneLine[OffsetLine+1].code==1 && CodeOfOneLine[OffsetLine+2].code==10) {
	 				if (CodeOfOneLine[OffsetLine+3].code==1 && CodeOfOneLine[OffsetLine+4].code==10) {
						if (CodeOfOneLine[OffsetLine+5].code==1 && CodeOfOneLine[OffsetLine+6].code==10) {
							if (CodeOfOneLine[OffsetLine+7].code==1 && CodeOfOneLine[OffsetLine+8].code==10){
								if (CodeOfOneLine[OffsetLine+9].code==1 ){
								x1=CodeOfOneLine[OffsetLine+1].value;
								y1=CodeOfOneLine[OffsetLine+3].value;
								x2=CodeOfOneLine[OffsetLine+5].value;
								y2=CodeOfOneLine[OffsetLine+7].value;
								color=CodeOfOneLine[OffsetLine+9].value;
								Line(x1,y1,x2,y2,color);
								//Rprintf(x1);
								// Rprintf(x2);
								OkLine=1;
								}
							}
						}
					}
				}
				
				if (OkLine!=1) {
					printf("Error Syntaxe Line:\n");
					goto EndMain;
				}
		}
		
		
//-------------
		if (CodeOfOneLine[OffsetLine].code==11&&CodeOfOneLine[OffsetLine].value==11) {//box3d x,y,z,d
				if (debug > 0 ) printf("box3d\n");
				OkLine=0;
				if (CodeOfOneLine[OffsetLine+1].code==1 && CodeOfOneLine[OffsetLine+2].code==10) {
	 				if (CodeOfOneLine[OffsetLine+3].code==1 && CodeOfOneLine[OffsetLine+4].code==10) {
						if (CodeOfOneLine[OffsetLine+5].code==1 && CodeOfOneLine[OffsetLine+6].code==10) {
							if (CodeOfOneLine[OffsetLine+7].code==1 ){
								Xmax3d=CodeOfOneLine[OffsetLine+1].value;
								Ymax3d=CodeOfOneLine[OffsetLine+3].value;
								Zmax3d=CodeOfOneLine[OffsetLine+5].value;
								Inc3D=CodeOfOneLine[OffsetLine+7].value;
								Zmin3d=-Zmax3d;
								Ymin3d=-Ymax3d;
								Xmin3d=-Xmax3d;								
								OkLine=1;
							}
						}
					}
				}
				if (OkLine!=1) {printf("Error Syntaxe gfxdim:\n");goto EndMain;}
		}
		
//-------------
		if (CodeOfOneLine[OffsetLine].code==11&&CodeOfOneLine[OffsetLine].value==9) {//gfxdim x1,x2,y1,y2
				if (debug > 0 ) printf("gfxdim\n");
				OkLine=0;
				if (CodeOfOneLine[OffsetLine+1].code==1 && CodeOfOneLine[OffsetLine+2].code==10) {
	 				if (CodeOfOneLine[OffsetLine+3].code==1 && CodeOfOneLine[OffsetLine+4].code==10) {
						if (CodeOfOneLine[OffsetLine+5].code==1 && CodeOfOneLine[OffsetLine+6].code==10) {
							if (CodeOfOneLine[OffsetLine+7].code==1 && CodeOfOneLine[OffsetLine+8].code==10) {
							if (CodeOfOneLine[OffsetLine+9].code==1 ){
								DimXmin=CodeOfOneLine[OffsetLine+1].value;
								DimXmax=CodeOfOneLine[OffsetLine+3].value;
								DimYmin=CodeOfOneLine[OffsetLine+5].value;
								DimYmax=CodeOfOneLine[OffsetLine+7].value;
								IncX=CodeOfOneLine[OffsetLine+9].value;
								OkLine=1;
							}}
						}
					}
				}
				if (OkLine!=1) {printf("Error Syntaxe gfxdim:\n");goto EndMain;}
		}
//---------------
	OutInstructionHere:// ----- here is the end of the check for instructions

	AlmostEndLoop:
		OffsetLine=0; //Important before going back from here to LoopCodeProgram	
		if  (CodeList[CodeListOffset].code!=0 && CodeListOffset<CodeListOffsetMax ) goto LoopCodeProgram;
	//-------------------------------------------------------EndLoop---------------------------------
	if (debug > 0 )printf("End\n");
EndMain:
	return (0);
 }

 
 static int TraceFunctionOneVariable(floactet *CodeOfOneLine, floactet *CodeList)
 {
	int N_AccuX;
	float x1,y1,x2,y2,color;
	float X0,X1,Y0,Y1,Y;
	int FunctionStart,Error;
			N_AccuX=(int)CodeOfOneLine[2].value;
			CodeListOffset=CodeListOffset+5;
			FillCodeOfOneLine(CodeList,CodeOfOneLine);//transfert one line 
			X0=Accu[N_AccuX];
			if (X0<DimXmin || X0>DimXmax) X0=DimXmin;
			FunctionStart=0;
			if (GridSet == 0){
			RctSetRectangle(rP,DrawZoneX,DrawZoneY,DrawZoneW,DrawZoneH);
			WinEraseRectangle(rP,0);
			TracerAxis(DrawZoneX+DrawZoneW/2,DrawZoneY+DrawZoneH/2,DrawZoneW, DrawZoneH);
			}
			
		LoopDrawFunction:
				
				ReplaceAccuByValue(CodeOfOneLine);
				Error = TreatParenthese(CodeOfOneLine);													//parenthese
				if (Error != 0) {StrPrintF (s,"Parenthese Error = %d \n", Error);PrintCmd(s);goto EndFunctionFX;}
				if(StopProgram == 1) goto EndFunctionFX;
				Error = CalculOneLine(CodeOfOneLine); //Calcul
				if(StopProgram == 1) goto EndFunctionFX;
				if (Error != 0) {StrPrintF (s,"Error = %d \n", Error);PrintCmd(s);goto EndFunctionFX;}
				if (CodeOfOneLine[0].code != 1) {PrintCmd("Error finishing function line\n");goto EndFunctionFX;}
				Y=CodeOfOneLine[0].value;
				FillCodeOfOneLine(CodeList,CodeOfOneLine);//transfert one line 
				if (FunctionStart==0) {Y0=Y; X1=X0+IncX;Accu[N_AccuX]=X1; FunctionStart=1;}
				else{Y1=Y;
					if (Y0>DimYmin && Y0 < DimYmax) Line(X0,Y0,X1,Y1,ColorGraph);
					X0=X1;Y0=Y1;X1=X0+IncX;Accu[N_AccuX]=X1;}
				if ( X1>DimXmax) goto EndFunctionFX;
				goto LoopDrawFunction;
		EndFunctionFX:
			return Error;
}

 
 static int TraceFunctionTwoVariable(floactet *CodeOfOneLine, floactet *CodeList)
 {
	int N_AccuX,N_AccuY;
	float x,y,z;
	float X0,X1,Y0,Y1;
	int FunctionStart,Error;
	float dx,dy;
			dx=Inc3D;
			dy=dx;
			N_AccuX=(int)CodeOfOneLine[2].value;
			N_AccuY=(int)CodeOfOneLine[4].value;
			CodeListOffset=CodeListOffset+7;
			FillCodeOfOneLine(CodeList,CodeOfOneLine);//transfert one line 
			x=Accu[N_AccuX];
			y=Accu[N_AccuY];
			if (X0<Xmin3d || X0>Xmax3d) X0=Xmax3d; //box3d
			if (Y0<Ymin3d || Y0>Ymax3d) Y0=Ymin3d;
			FunctionStart=0;
			if (GridSet == 0){
			RctSetRectangle(rP,DrawZoneX,DrawZoneY,DrawZoneW,DrawZoneH);
			WinEraseRectangle(rP,0);
			Tracer3DAxis();
			}
			
		LoopDrawFunction:
				
				ReplaceAccuByValue(CodeOfOneLine);
				Error = TreatParenthese(CodeOfOneLine);													//parenthese
				if (Error != 0) {StrPrintF (s,"Parenthese Error = %d \n", Error);PrintCmd(s);goto EndFunctionFX;}
				if(StopProgram == 1) goto EndFunctionFX;
				Error = CalculOneLine(CodeOfOneLine); //Calcul
				if(StopProgram == 1) goto EndFunctionFX;
				if (Error != 0) {StrPrintF (s,"Error = %d \n", Error);PrintCmd(s);goto EndFunctionFX;}
				if (CodeOfOneLine[0].code != 1) {PrintCmd("Error finishing function line\n");goto EndFunctionFX;}
				z=CodeOfOneLine[0].value;
				FillCodeOfOneLine(CodeList,CodeOfOneLine);//transfert one line 
				if (FunctionStart==0) {
					X0=y-x*(xp-y)/zp;
					Y0=z-x*(yp-z)/zp;
					FunctionStart=1;
				}else{
					X1=y-x*(xp-y)/zp;
					Y1=z-x*(yp-z)/zp;
					if (Y0>DimYmin && Y0 < DimYmax && Y1>DimYmin && Y1 < DimYmax) 
						if (X0>DimXmin && X0 < DimXmax && X1>DimXmin && X1 < DimXmax) 
							Line(X0,Y0,X1,Y1,ColorGraph);
							X0=X1;Y0=Y1;
						}
				y=y+dy;Accu[N_AccuY]=y;
				if (y < Ymax3d) goto LoopDrawFunction;
				y=Ymin3d;x=x-dx;Accu[N_AccuX]=x;Accu[N_AccuY]=y; FunctionStart =0; 
				if (x > Xmin3d) goto LoopDrawFunction;

		EndFunctionFX:
			return Error;
}

 
 static int CheckLabelDef(char *InstructionLine,int Size){
	int i=0;
	while (i<Size && InstructionLine[i] != 0x0A){
		//printf("InstructionLine[%d]=%c\n",i,InstructionLine[i]);
		if (InstructionLine[i] == Octet(":") ) {
			if (debug >0) {StrPrintF (s,"lbl:  %d letters\n",i);PrintCmd(s);} 
		return (i);}
		i++;
	}
	return 0;
 }
 
 static void RemoveSpace(char * InstructionLine, int Size){
	int i=0; int p=0;
		LoopRS:
	while (i<Size){
		if (InstructionLine[i+p] == Octet(" ") ) {p++;goto LoopRS;}
		InstructionLine[i]=InstructionLine[i+p];
		i++;
	}
}

	
static int ConvertMnemo(char *MnemoListLine, floactet * CodeList){//converts one line

/* Return value:
							Error=1 : Syntax error
							Error=2 : Error Accu number should be number
							Error=3 : Endline too early example: 2-9+
							Error=4 : Error Accu number too big
							Error=5 : Error Exceeding memory for Codes
							*/

int i=0; //put UInt32 for Palm
int Out=0 ;
char val=0 ;
char x ;
float Nbr=0 ;
int Codage=10; // Means Decimal (hexa =16, binary =2)
int iop=0;
int ErrorCode=0; // Tells if error, Set to 0 at start
int opposite=0; // for the -x number at start of the math sentence
//CodeListOffset Should be initialized outside this program
int InstructionNumber=0;
int FunctionNumber;
//int NbrTest=0;
signed int LabelNbrFound;
static char StringNbr[100];
int StrtStr,EndStr,p,a,BackFromAccu=0;

		opposite=0;//init
		if (MnemoListLine[i]==Octet("\n") )  {Out=1; goto EndConvertNoData;} //endLine at start or No data
	StartConvList:

		//--- Search for instructions --------
	InstructionNumber = 1; // Set to 1. If no more instruction takes 0 value.
	while(InstructionNumber !=0){
		Iindex=i;
		//printf("MnemoListLine[%d]=%c\n",i,MnemoListLine[i]);
		InstructionNumber=HandleInstructions(MnemoListLine,Iindex);
		i=Iindex;
		if (debug > 0) {StrPrintF(s,"Instruction Nbr = %d \n",InstructionNumber); PrintCmd(s);}
		if (InstructionNumber !=0) {CodeList[CodeListOffset].code=11;	//General Code Instruction
		CodeList[CodeListOffset].value=InstructionNumber;
		CodeListOffset++;
		}
	}
	
	//--- check label ---
		LabelNbrFound=TestForLabels(MnemoListLine,i);
		if (LabelNbrFound != -1 ) {
			if (debug >1) printf("label [%d] \n",LabelNbrFound);
			i=Iindex;
			CodeList[CodeListOffset].code=12; //Code for Labels
			CodeList[CodeListOffset].value=LabelNbrFound ;// Later put Labels[LabelNbrFound].n;
			CodeListOffset++;
			//if (debug >1) printf("pointer on character [%c]\n",MnemoListLine[i]);
			}	
		
		if (MnemoListLine[i]==Octet("-")&& BackFromAccu==0 ){opposite=1; i++; }//sign for the number 
	ComeBackFromAccu:
		BackFromAccu=0;
	while (Out==0){
		if (CodeListOffset > CodeListSize-10) {PrintCmd("Exceeding Memory size for Codes --- contact Regan B.S. by email for info");return 5;} 
		if (MnemoListLine[i]==Octet("\n") )  {Out=1; goto EndNbr;} //endLine or No data
		if (MnemoListLine[i]==OperatorList[4] ) { //"("
			i++; ///parenthese left is special, there can be a sign behind like in (-1+2)
			CodeList[CodeListOffset].code = 6; 
			CodeList[CodeListOffset].value = 0; // Floating point Value is set

			if (debug > 0) printf("Parenthese on\n");
			CodeListOffset++;
			goto StartConvList;
			}//End Coding Parenthese 
		
		
		//---- Handle Detection of Math Function ------
//	FunctionNumber=1;//Set to 1, if no more instructions functionNumber is taking 0 value
//	while (FunctionNumber!=0){
		Iindex=i;
		//printf("MnemoListLine[%d]=%c\n",i,MnemoListLine[i]);
		FunctionNumber=HandleMathFunctions(MnemoListLine,Iindex);
		i=Iindex;
		if (FunctionNumber == 4 ){
			if (debug > 0) printf("Spécial Trf(x) \n");
			CodeList[CodeListOffset].code=11;	//Change it to instruction for spécial use
			CodeList[CodeListOffset].value=-1; 	//Specific coding for display function f(x)= ...
			CodeListOffset++;
			goto StartConvList;
			//the function f(x)= is actually using the job of the math function for coding it becomes an instruction with coding [11] [-1]
		}//end coding search function

		if (FunctionNumber !=0) {
			if (opposite == 1) {CodeList[CodeListOffset].code = 1;CodeList[CodeListOffset].value = 0;
			CodeList[CodeListOffset+1].code = 3;CodeList[CodeListOffset+1].value = 0;
			CodeListOffset=CodeListOffset+2; opposite=0;} //we've put 0-function
			if (debug > 0) printf("Function coded\n");
			CodeList[CodeListOffset].code=13;	//General Code Math Instruction
			CodeList[CodeListOffset].value=FunctionNumber; 	//Specific code (example: exp=1, ln =2,...)
			CodeListOffset++;
			goto StartConvList;
		}//end coding search function

		if (MnemoListLine[i]==Octet("^") )  {
			if (debug > 0 ) printf("found ^");
				CodeList[CodeListOffset].code = 13; //It is a function
				CodeList[CodeListOffset].value = 9;  // Coding for "^"
				CodeListOffset++;
				i++; // then i points next number
				CodeListOffsetMax=CodeListOffset;
				goto StartConvList;
			}
		
		//----------------------------------------
		//It would be simple to detect F0(...)=... or F1(..)=..., F...(..)= for functions to define
		// and use then like Accu but it recalculates each time the value.
		
	//i,MnemoListLine,opposite,CodeList,CodeListOffset
				//detect Fonction FAccu
		if (MnemoListLine[i]=='F' ) {//detect FAccu 
			i++; //get the number now
			val = MnemoListLine[i]-Octet("0");
			if (val<0 && val>9)  {ErrorCode=2; goto EndConvert;}//missing number behind Accu
			Nbr=0;
			HandleNbrFAccu:
				val= MnemoListLine[i]-Octet("0");
				if (val >= 0 && val <=9) Nbr = Codage*Nbr + val ; 
				else {
					if (opposite == 1) {CodeList[CodeListOffset].code = 1;CodeList[CodeListOffset].value = 0;
					CodeList[CodeListOffset+1].code = 3;CodeList[CodeListOffset+1].value = 0;
					CodeListOffset=CodeListOffset+2; opposite=0;} //we've put 0-Accu
					CodeList[CodeListOffset].code = 14; //code of the FAccu
					CodeList[CodeListOffset].value = Nbr;
					//Recording the place in FAccu[Nbr]
					if ((int)Nbr < NbrMaxFAccu) FAccu[(int)Nbr]=CodeListOffset;//integer
					if (Nbr>NbrMaxFAccu) {printf("FAccu Nbr too big\n");ErrorCode=4; goto EndConvert;} 
					if (debug > 0) {StrPrintF(s,"FAccu F%d at place %d\n",(int) Nbr,CodeListOffset);PrintCmd(s);}
					CodeListOffset++;
					Nbr=0;// Reinitialise  Nbr for next value
					goto EndNbrFAccu;// i points on the operator
				}
				i++;
				goto HandleNbrFAccu;
			EndNbrFAccu:				
				if (MnemoListLine[i]==Octet("\n") )  {//Check Endline
				Out=1; 
				CodeList[CodeListOffset].code = 0xFF;       // EndLine coding
				CodeList[CodeListOffset].value = 0; 
				CodeListOffset++;
				CodeListOffsetMax=CodeListOffset;
				}
				BackFromAccu=1;
				goto StartConvList; //Return in the loop
			}//End Coding FAccu

		//detect Accu
		if (MnemoListLine[i]==OperatorList[7] ) {//detect A 
			i++; //get the number now
			val = MnemoListLine[i]-Octet("0");
			if (val<0 && val>9)  {ErrorCode=2; goto EndConvert;}//missing number behind Accu
			Nbr=0;
			HandleNbrAccu:
				val= MnemoListLine[i]-Octet("0");
				if (val >= 0 && val <=9) Nbr = Codage*Nbr + val ; 
				else {
					if (opposite == 1) {CodeList[CodeListOffset].code = 1;CodeList[CodeListOffset].value = 0;
					CodeList[CodeListOffset+1].code = 3;CodeList[CodeListOffset+1].value = 0;
					CodeListOffset=CodeListOffset+2; opposite=0;} //we've put 0-Accu
					CodeList[CodeListOffset].code = 9; //code of the Accu
					CodeList[CodeListOffset].value = Nbr; 
					if (Nbr>NbrMaxAccu) {printf("Accu number too big\n");ErrorCode=4; goto EndConvert;} 
					CodeListOffset++;
					if (debug > 0) {StrPrintF(s,"Accu A%d \n",(int) Nbr);PrintCmd(s);}
					Nbr=0;// Reinitialise  Nbr for next value
					goto EndNbrAccu;// i points on the operator
				}
				i++;
				goto HandleNbrAccu;
			EndNbrAccu:				
				if (MnemoListLine[i]==Octet("\n") )  {//Check Endline
				Out=1; 
				CodeList[CodeListOffset].code = 0xFF;       // EndLine coding
				CodeList[CodeListOffset].value = 0; 
				CodeListOffset++;
				CodeListOffsetMax=CodeListOffset;
				}
				BackFromAccu=1;
				goto StartConvList; //Return in the loop
				//goto ComeBackFromAccu;
			}//End Coding Accu

			if (MnemoListLine[i] == ')') {
			CodeList[CodeListOffset].code = 7;       // +2  so that "+"=2
			CodeList[CodeListOffset].value = 0; 			// No values for operators
			CodeListOffset++;
			i++; // then i points next number
			CodeListOffsetMax=CodeListOffset;
			BackFromAccu=1;//BackFromAccu should be changed into BackAvoidNeg to avoid "- "as a negative sign, future "-" will be taken as an operator
			goto StartConvList;
			}

	
		for (iop=0;iop<OpListSize;iop++){
			if (MnemoListLine[i] == OperatorList[iop]) {
			CodeList[CodeListOffset].code = iop+2;       // +2  so that "+"=2
			CodeList[CodeListOffset].value = 0; 			// No values for operators
			CodeListOffset++;
			i++; // then i points next number
			CodeListOffsetMax=CodeListOffset;
			goto StartConvList;
			}
		}
			
		//--- Number Handling here --- 
	HandleNbr:
	
// System with sscanf to get the floating point nbr but need implement a function for sscanf
		val= MnemoListLine[i]-Octet("0");
		if (val >= 0 && val <=9) StrtStr=i;
		i++;
		LoopGetEndNbrStr:
		val = MnemoListLine[i]-Octet("0");
		if (val >= 0 && val <=9) {i++; goto LoopGetEndNbrStr;}
		if (MnemoListLine[i]=='E' && MnemoListLine[i+1]=='+') {i=i+2; goto LoopGetEndNbrStr;}
		if (MnemoListLine[i]=='E' && MnemoListLine[i+1]=='-') {i=i+2; goto LoopGetEndNbrStr;}
		if (MnemoListLine[i]=='.' ) {i=i+1; goto LoopGetEndNbrStr;}
		EndStr=i; 
		for (p=0; p<(EndStr-StrtStr) ; p++){StringNbr[p]=MnemoListLine[StrtStr+p];}
		StringNbr[p]=0;
		Rsscanf (StringNbr,"%f",&Nbr); // personal implementation sscanf not on PalmOS unfortunately
		if (opposite==1)  {opposite=0; Nbr= -Nbr;} 
		if (debug>0) {StrPrintF(s,"StringNbr= [%s], Nbr = %d\n",StringNbr,(int)Nbr);PrintCmd(s);}
		CodeList[CodeListOffset].code = 1;       // It is a number: code=0
		CodeList[CodeListOffset].value = Nbr; // Floating point Value is set
		CodeListOffset++;
		Nbr=0;// Reinitialise  Nbr for next value

	EndNbr:
		if (MnemoListLine[i]==Octet("\n") )  {//Check Endline
			Out=1; 
			CodeList[CodeListOffset].code = 0xFF;       // EndLine coding
			CodeList[CodeListOffset].value = 0; 
			CodeListOffset++;
			CodeListOffsetMax=CodeListOffset;
			goto EndConvert; 
			}
	// --------- Put Here the special syntax to check after a number code

			//"=>" the 'then' sign
			if (MnemoListLine[i]==Octet("=") && MnemoListLine[i+1]==Octet(">"))  {
			goto StartConvList; //The code will be put by instruction found
			}

			if (MnemoListLine[i]==Octet(",") && MnemoListLine[i+1]==Octet("-"))  {
			CodeList[CodeListOffset].code =  10;       // "," coding
			CodeList[CodeListOffset].value = 0; 
			CodeListOffset++;
			CodeListOffsetMax=CodeListOffset;
			i++;
			goto StartConvList; //The code will be put by instruction found
			}

			if (MnemoListLine[i]==Octet("<") )  {
			goto StartConvList; //The code will be put by instruction found
			}
			if (MnemoListLine[i]==Octet(">") )  {
			goto StartConvList; //The code will be put by instruction found
			}

			
		if (MnemoListLine[i]==Octet("^") )  {
			if (debug > 0 ) printf("found ^");
				CodeList[CodeListOffset].code = 13; //It is a function
				CodeList[CodeListOffset].value = 9;  // Coding for "^"
				CodeListOffset++;
				i++; // then i points next number
				CodeListOffsetMax=CodeListOffset;
				goto StartConvList;
			}
			
		for (iop=0;iop<OpListSize;iop++){
			if (MnemoListLine[i] == OperatorList[iop]) goto OpCodeFound;		
		}
		ErrorCode=1;
		goto EndConvert;

	OpCodeFound:
			CodeList[CodeListOffset].code = iop+2;       // +2  so that "+"=2
			CodeList[CodeListOffset].value = 0; 			// No values for operators
			CodeListOffset++;
			i++; // then i points next number
			CodeListOffsetMax=CodeListOffset;
		//	if (MnemoListLine[i]==Octet("\n") )  {Out=1;goto EndNbr; }//endLine
	}
		
	EndConvert:
		CodeListOffsetMax=CodeListOffset;

/*		if (debug==1 && CodeListOffsetMax!=0 ){Out=0;
			for (i=0; i<CodeListOffsetMax; i++){
				printf( "Code %d  [%d] [%f]\n",i, CodeList[i].code,CodeList[i].value);
				}
			}
			*/
	EndConvertNoData:
		return ErrorCode;
 }
 
 
signed int TestForLabels(char *MnemoListLine, int index){
int k,j;
	j=0;
	for (k =0; k< NmaxLbl;k++){//Enumerates the labels
	j=0;
		while (WholeMnemoProg[Labels[k].adr+j] != Octet (":") ) {
			if ( WholeMnemoProg[Labels[k].adr+j] != MnemoListLine[index+j]) goto NotThatLabel;   
			if ( WholeMnemoProg[Labels[k].adr+j+1] == Octet (":") && MnemoListLine[index+j+1]!= 0x0A){
			//printf("[%c][%c]\n",WholeMnemoProg[Labels[k].adr+j],WholeMnemoProg[Labels[k].adr+j+1]);
			//printf("[%c][%c]\n",MnemoListLine[index+j],MnemoListLine[index+j+1]);
			goto NotThatLabel;
			}
			j++;
		}
	Iindex=index+j; //Save on the external Iindex 
	//printf("k:%d\n",k);
	return k; //the number of the label
	NotThatLabel:	
	j=j;	
	}
	return -1; //message of error
 }
//--------- Gives the Instruction code --------- 
 int HandleInstructions(char *MnemoListLine, int i){//return the number instruction starting on line
	int LengthInstructionMax = 30;// exemple: length "print" = 5
	int j=0; int k=0;
	int NInstr=0; int Ok=0;

	
	while( j < sizeof(InstructionList) ){
		//printf("InstructionList[%d] = %c\n",j,InstructionList[j]);
		if (MnemoListLine[i] == InstructionList[j]){
			k=0;
			LoopTestInstruction:
			if (debug >0 ){StrPrintF(s,"Instruction ? %c\n",MnemoListLine[i+k]);PrintCmd(s);}

				if (MnemoListLine[i+k] != InstructionList[j+k]) goto NotThisInstruction;
				if (k>LengthInstructionMax) goto NotThisInstruction;
				if (InstructionList[j+k+1]== Octet("_") ) {Ok=1; Iindex=i+k+1;goto InstructionFound;}
				k++;
				goto LoopTestInstruction;
		}
	NotThisInstruction:
	while (InstructionList[j+k]!=Octet("_") && k< LengthInstructionMax){k++;}
	NInstr++; j=j+k;k=0;	
	j++;
	}
InstructionFound:
	if (Ok == 0) NInstr=0; //If none instruction found then send 0 back 
	return NInstr; //Contain the number of the instruction
 }

//----------- Gives the maths function code ----------- 
	int HandleMathFunctions(char *MnemoListLine, int i){//return the number instruction starting on line
	int LengthInstructionMax = 30;// exemple: length "print" = 5
	int j=0; int k=0;
	int NInstr=0; int Ok=0;

	
	while( j < sizeof(MathFunctions) ){// the list
		if (MnemoListLine[i] == MathFunctions[j]){
			k=0;
			LoopTestFunction:
			if (debug > 0 ){StrPrintF(s,"Math Func ? %c\n",MnemoListLine[i+k]);PrintCmd(s);}

				if (MnemoListLine[i+k] != MathFunctions[j+k]) goto NotThisFunction;
				if (k>LengthInstructionMax) goto NotThisFunction;
				if (MathFunctions[j+k+1]== Octet("_") ) {
					Ok=1; Iindex=i+k+1;NInstr++;//NInstr is increased 'cause we start at 1 the list (exp=1)
					if (debug > 0 ){StrPrintF(s,"MathFunction Nbr %d\n",NInstr);PrintCmd(s);}
					goto FunctionFound;}
				k++;
				goto LoopTestFunction;
		}
	NotThisFunction:
	while (MathFunctions[j+k]!=Octet("_") && k< LengthInstructionMax){k++;}
	NInstr++; j=j+k;k=0;	
	j++;
	}
FunctionFound:
	if (Ok == 0) NInstr=0; //If none instruction found then send 0 back 
	return NInstr; //Contain the number of the instruction
 }

 
int FillCodeOfOneLine(floactet *CodeList,floactet *CodeOfOneLine){
	//CodeListOffset is set outside this subroutine
	int i=0;
StartFill:
		CodeOfOneLine[i]=CodeList[CodeListOffset+i];
		i++;
	if (CodeList[CodeListOffset+i].code==0xFF ) goto EndFill;
	if ((CodeListOffset+i)>CodeListOffsetMax) goto EndFill;
	if (CodeList[CodeListOffset+i].code==0 ) goto EndFill;
	goto StartFill;
EndFill:
	CodeOfOneLine[i].code=0xFF; CodeOfOneLine[i].value=0;
	return (i+1);
}

static void ReplaceAccuByValue(floactet *CodeOfOneLine){
int i=0;
int iptrstrt=0,Aind;

  while (i<NbrMaxOperationOnLine ){ 
	if (CodeOfOneLine[i].code==0xFF) goto StartOperate;
	if (CodeOfOneLine[i].code == 8) {iptrstrt=i+1; goto StartOperate;} //8 is the code of "="
	i++;
  }
  StartOperate:
	i=iptrstrt;
	while (i<NbrMaxOperationOnLine && CodeOfOneLine[i].code!=0xFF){ 
		if (CodeOfOneLine[i].code == 9){ //9 is the Code for Accu
			Aind=CodeOfOneLine[i].value;
			CodeOfOneLine[i].code =1;//change in a number 
			CodeOfOneLine[i].value = Accu[Aind];
			if (debug >0) {StrPrintF(s,"A%d = ",Aind);PrintCmd(s);Rprintf(Accu[Aind]);}
			}
	i++;
	} 
 }
 
 
 static int TreatParenthese(floactet * CodeList){
 /* ErrorCode = 1 Different number of left and right parenthese should be equal
     ErrorCode = 2 Error calculation in the  parenthese

	 1. detect deepest parenthese, 
        and place iptr on the deepest parenthese copy the part in a buffer floactet line code,
	 2. give that job to CalculOneLine(CodeInParenthese)
	 3. Copy the code result in the Codelist removing the concerned parenthese
	 4. Start again the detection parenthese for extra parenthese
	 */
 int i,iptr,iptrEnd;
 int CountLevelPar,LevelParMax;
 int ErrorCode=0;
 int Error=0;
//floactet BufferLineCode[NbrMaxOperationOnLine];//avoid to lighten the stack
 MemHandle BLCHdle = MemHandleNew(NbrMaxOperationOnLine*sizeof(struct floactet) );
 floactet * BufferLineCode=MemHandleLock(BLCHdle);
 
 //counts the max level parenthese (((1+1)+3)-2) := 3level
 //and get iptr and iptrEnd the pointer indexes left right on the deepest level parenthese

 StartParentheseJob:
	LevelParMax=0;
	CountLevelPar=0;
	iptr=0;
	iptrEnd=0;
    i=0;
	
 while (i<NbrMaxOperationOnLine){ 
	if (CodeList[i].code==6) {
		CountLevelPar++; //detection of "("
		//if(LevelParMax<CountLevelPar) {//this was not working for (1)=(1)
		if(LevelParMax<=CountLevelPar) {
			LevelParMax=CountLevelPar;// get the deepest level
			iptr=i; //get the pointer on the deepest opening parenthese
			}
		}
	if (CodeList[i].code==7) {
		if(LevelParMax==CountLevelPar) iptrEnd=i;
		CountLevelPar--; // detection of ")"
		}
		if (CodeList[i].code==0xFF) goto CountParDone;
	i++;
	}
 CountParDone:
	//At the end of the counting left right, CountLevelPar should be 0
	if (CountLevelPar !=0) {ErrorCode = 1; goto EndHandleParenthese;}
	if (LevelParMax == 0) goto EndHandleParenthese; //no or no more parenthese
	
	// Copy little line between parenthese in BufferLineCode
	for (i=0;i<(iptrEnd-iptr-1);i++){BufferLineCode[i]=CodeList[iptr+i+1];}
	BufferLineCode[i].code=0xFF;BufferLineCode[i].value=0; //add the "\n" in the buffer
	Error = CalculOneLine(BufferLineCode);//Go to calculate the BufferOneLine
	if(StopProgram == 1) {MemHandleFree(BLCHdle);return 0;}
	if (Error != 0) {printf ("Error in Parenthese = %d\n", Error); ErrorCode=2;} 
	if (debug >0) {printf("Parenthese Result BufferLineCode:\n");
							StrPrintF(s,"  [%d]   [%d]\n",BufferLineCode[0].code,(int)BufferLineCode[0].value);
							PrintCmd(s);
							StrPrintF(s,"  [%d]   [%d]\n",BufferLineCode[1].code,(int)BufferLineCode[1].value);
							PrintCmd(s);
	}
	CodeList[iptr]=BufferLineCode[0]; //copy the result
	i=iptr+1;
	while (i<NbrMaxOperationOnLine){
		CodeList[i]=CodeList[i+(iptrEnd-iptr)];//copy what is after iptrEnd in place of iptr+1
		if (CodeList[i].code == 0xFF) goto JobDone; //"\n" reached (coded by 0xFF)
		i++;
	}
	JobDone:
		if (i==NbrMaxOperationOnLine){ErrorCode=3;}//Error not found the proper end of line
		else goto StartParentheseJob;
	
	EndHandleParenthese:
	MemHandleFree(BLCHdle);
	return ErrorCode;
	
 }
 
int CalculFAccu(floactet *CodeOfOneLine, int NbrFAccu,float valX, float *ResultY){
	//ResultY is the final calculation result of F...(ValX)=ResultY
	//floactet BufferCodes[200]; ! Creates stack overflow 'cause multiple loops
	MemHandle MemHdle;
	floactet * BufferCodes;
	float SaveAccuVal;
	int N_AccuX,OfPtr,SaveCodeListOffset,Error;
	int SizeBufferCodes=200;
	floactet *CodeList;

		CodeList = CodeListAdr; //Transporte the full list adresse here

		MemHdle = MemHandleNew(SizeBufferCodes*sizeof (struct floactet) );
		if( MemHdle == 0){PrintCmd("Cannot allocate BufferCodes!");return;}
		BufferCodes = MemHandleLock(MemHdle);

		
		OfPtr = FAccu[NbrFAccu];
		
		if (CodeList[OfPtr+0].code != 14 || CodeList[OfPtr+0].value != NbrFAccu ) {
				PrintCmd("Error Wrong Function");return 1;}
		
		N_AccuX=(int)CodeList[OfPtr+2].value;
		SaveCodeListOffset=CodeListOffset;
		CodeListOffset=OfPtr+5;
		FillCodeOfOneLine(CodeList,BufferCodes);//transfert one line 
		SaveAccuVal=Accu[N_AccuX]; //We save Accu value to avoid interference with other variables calculations
		Accu[N_AccuX]=valX; //Set the value of X for F...(ValX)=
		ReplaceAccuByValue(BufferCodes);
		//StrPrintF(s,"Code F [%d] [%d]",BufferCodes[0].code,(int)BufferCodes[0].value);
		//PrintCmd(s);
		Error = TreatParenthese(BufferCodes);													//parenthese
		if (Error != 0) {StrPrintF (s,"In function FAccu%d, Error%d  \n", NbrFAccu,Error);PrintCmd(s);goto ErrorOut;}
		Error = CalculOneLine(BufferCodes); //Calcul
		if (Error != 0) {StrPrintF (s,"Calcul Error in FAccu %d \n", NbrFAccu);PrintCmd(s);goto ErrorOut;}
		if (BufferCodes[0].code != 1) {PrintCmd("Error finishing function line\n");goto ErrorOut;}
		*ResultY= BufferCodes[0].value;
		CodeListOffset=SaveCodeListOffset;
		Accu[N_AccuX]=SaveAccuVal; //Set the value of X for F...(ValX)=
		MemHandleFree(MemHdle);
		return 0;
ErrorOut:
		MemHandleFree(MemHdle);
		return Error;
	}

 
 
static int  CalculOneLine(floactet * CodeListLine){//Must be one line only
	/* Error Code return   0  Calculation Done
									   1  No  End in the Line
									   2  Syntaxe error should find numbers around the math operator
									   3  Division by zero prohibited
									   4 Should be no parenthese at the stage of this program
										  The treatment of the parenthese should be done previously.
									   5 Bad syntax for test condition
								
									   
	The calculation is done after the equal sign if there is one,
	the pointer  iptrEqualSignP points the code 	after equal sign code 8.
	We use here the same iptrEqualSign for the Instruction code 11... to perform the calculation
	behind the instruction (exemple: print 1*2)
	*/
									   
	//Search for priority *,/,(,  
	int i,k; int imaxLine=0;
	float val,x1,x2;
	int ErrorCode=0;
	int iptrEqualSignP; //Used to save the position of the equal sign in the line
	
	if (BreakActivated==1) if( CheckForBreak() == 1) return; //test for break test

	
	i=0;
	imaxLine=0; iptrEqualSignP=0;
	while (i<NbrMaxOperationOnLine){
	if (CodeListLine[i].code == 0xFF) {imaxLine=i; goto SearchPriority;}
	if (CodeListLine[i].code == 8) iptrEqualSignP=i+1; //detect and save equal code position+1 

	if (CodeListLine[i].code==14 &&CodeListLine[i+1].code==1) {//FAccu insertion
		if(debug > 0) {StrPrintF(s,"OffsetAdr FAccu =%d\n",FAccu[(int)CodeListLine[i].value]); PrintCmd(s);}
		if( CalculFAccu( CodeListLine, (int)CodeListLine[i].value, CodeListLine[i+1].value,&val)!=0) return 5;
		else {CodeListLine[i].code=1; CodeListLine[i].value=val;}
		k=i+1;
		while (CodeListLine[k].code !=0xFF && CodeListLine[k].code !=0) {
			CodeListLine[k]=CodeListLine[k+1];
			k++;
			}
	}	
	
	if (CodeListLine[i].code==13 && CodeListLine[i+1].code==1) {//Math Function
		if (CodeListLine[i].value==1 ) {val=RMath_exp(CodeListLine[i+1].value);}//exp
		if (CodeListLine[i].value==2 ) {val=RMath_ln(CodeListLine[i+1].value);}	//ln		
		if (CodeListLine[i].value==3 ) {val=RMath_sqrt(CodeListLine[i+1].value);}	//sqrt		
	  //if (CodeListLine[i].value==4 ) {AlreadyTaken for the function f(x);}	//function f(x)
		if (CodeListLine[i].value==5 ) {val=RMath_sin(CodeListLine[i+1].value);}	//sin
		if (CodeListLine[i].value==6 ) {val=RMath_cos(CodeListLine[i+1].value);}	//cos
		if (CodeListLine[i].value==7 ) {val=RMath_tan(CodeListLine[i+1].value);}	//tan
		if (CodeListLine[i].value==8 ) {val=fact((int)CodeListLine[i+1].value);}	//factoriel
		if (CodeListLine[i].value==9 ) {goto OutForPower;}	//Power
		
		if (MathError !=0 ) {StrPrintF(s,"Math Error = %d",MathError);PrintCmd(s);return 3;}
		if (ErrorCode == 0) {CodeListLine[i].code=1; CodeListLine[i].value=val;}
		k=i+1;
		while (CodeListLine[k].code !=0xFF && CodeListLine[k].code !=0) {
			CodeListLine[k]=CodeListLine[k+1];
			k++;
			}
/*			for (k=0;k<5;k++){
			StrPrintF(s," code %d [%d] [%d] \n",k,CodeListLine[k].code,(int)CodeListLine[k].value);
			PrintCmd(s);
			}*/
		}
	OutForPower:
		
	if (CodeListLine[i+1].code==11 &&CodeListLine[i+1].value==3) {//Test "<"
		if (CodeListLine[i].code !=1 && CodeListLine[i+2].code !=1){ErrorCode = 5; goto EndCalculOneLine;}
		else { if ( CodeListLine[i].value < CodeListLine[i+2].value) {
					TestCondition=1; }
					i=i+3;goto EndCalculOneLine;
					}
	}
		if (CodeListLine[i+1].code==11 &&CodeListLine[i+1].value==6) {//Test ">"
		if (CodeListLine[i].code !=1 && CodeListLine[i+2].code !=1){ErrorCode = 5; goto EndCalculOneLine;}
		else { if ( CodeListLine[i].value > CodeListLine[i+2].value) {
					TestCondition=1; }
					i=i+3;goto EndCalculOneLine;
					}
	}
	if (CodeListLine[i].code == 11) iptrEqualSignP=i+1; //detect instruction and save code position+1 

	
	if (debug > 0) {
	StrPrintF (s," Code %d  [%d]   [%d] \n",i,CodeListLine[i].code, (int) CodeListLine[i].value);
	PrintCmd(s);
	}
	i++;
	}
	if (i>0 && imaxLine==0) {ErrorCode =1; goto EndCalculOneLine;} //NoEnd
	SearchPriority:
	i=iptrEqualSignP; //We start the calculation from the equal sign +1	
	while (i<NbrMaxOperationOnLine){
		if (CodeListLine[i].code == 13 && CodeListLine[i].value == 9 ) { goto Power;}
		if (CodeListLine[i].code == 4 ) goto Multiplication;
		if (CodeListLine[i].code == 5 ) goto Division;
		if (CodeListLine[i].code == 6 ) {ErrorCode=4; goto EndCalculOneLine;}
		if (CodeListLine[i].code == 0xFF ) goto NoPriority;
		if (CodeListLine[i].code == 0 ) goto NoPriority;
		i++;
		}
		
	Power:
		//i points on the sign ^
		i-- ; //place on the left number
		if (CodeListLine[i].code!=1 || CodeListLine[i+2].code != 1) {ErrorCode=2; goto EndCalculOneLine; }
		val = RMath_Pow(CodeListLine[i].value,CodeListLine[i+2].value);
		CodeListLine[i].code=1; //number
		CodeListLine[i].value=val; // new value
		i++;
		if (debug > 0) {StrPrintF (s,"Power Result : %d\n",(int)val); PrintCmd(s);}	
		while (i+2<=imaxLine){
			CodeListLine[i].code=CodeListLine[i+2].code;
			CodeListLine[i].value=CodeListLine[i+2].value;
			i++;
			}
		i=iptrEqualSignP; //reinitialise position after the equal sign	
		goto SearchPriority; // Back to seek for operation priority

	Multiplication:
		//i points on the multiplie *
		i-- ; //place on the left number
		if (CodeListLine[i].code!=1 || CodeListLine[i+2].code != 1) {ErrorCode=2; goto EndCalculOneLine; }
		val = CodeListLine[i].value * CodeListLine[i+2].value;
		CodeListLine[i].code=1; //number
		CodeListLine[i].value=val; // new value
		i++;
		//if (debug ==1) PrintCmd ("multiplication result : %f \n",val);	
		while (i+2<=imaxLine){
			CodeListLine[i].code=CodeListLine[i+2].code;
			CodeListLine[i].value=CodeListLine[i+2].value;
			i++;
			}
		i=iptrEqualSignP; //reinitialise position after the equal sign	
		goto SearchPriority; // Back to seek for operation priority
		
	Division:
		//i points on the multiplie *
		i-- ; //place on the left number
		if (CodeListLine[i].code!=1 || CodeListLine[i+2].code != 1) {ErrorCode=2; goto EndCalculOneLine; }
		if (CodeListLine[i+2].value!=0) val = CodeListLine[i].value / CodeListLine[i+2].value;
		else {ErrorCode = 3; goto EndCalculOneLine;}//division by zero prohibited; 

		CodeListLine[i].code=1; //number
		CodeListLine[i].value=val; // new value
		i++;
		//if (debug ==1) printf ("division result : %f \n",val);	
		while (i+2<=imaxLine){
			CodeListLine[i].code=CodeListLine[i+2].code;
			CodeListLine[i].value=CodeListLine[i+2].value;
			i++;
			}
		i=iptrEqualSignP;	
		goto SearchPriority; // Back to seek for operation priority
	
	NoPriority:
		i=iptrEqualSignP; //start after the equal sign
		if (CodeListLine[i].code==1 && CodeListLine[i+1].code == 0xFF) {val=CodeListLine[i].value; goto EndCalculOneLine; }// Already done		
		val=0;
		if (CodeListLine[i].code != 1) {ErrorCode =2; goto EndCalculOneLine; }// Should be a number !		
		if (CodeListLine[i+2].code != 1) {ErrorCode =2; goto EndCalculOneLine; }		
		val = CodeListLine[i].value;
		while(i<imaxLine){
			x2 = CodeListLine[i+2].value;
			if (CodeListLine[i+1].code==2) val=x2+val;
			if (CodeListLine[i+1].code==3) val=val-x2;
			i=i+2;
			}
			//Put Final calculation results
			CodeListLine[iptrEqualSignP].code=1; //Put value after the equal sign
			CodeListLine[iptrEqualSignP].value=val;
 EndCalculOneLine:
			//if (CodeListLine[0].code==1 && CodeListLine[1].code==0xFF)Rprintf(val); //"print" if nothing else but the number
			LastValCalculated=val; //After all passages LastValCalculated keeps the value
			if (debug > 0) {
				StrPrintF (s, "Line value is %d \n",(int) val); PrintCmd(s);
				}
			if (val == 13061976) {CodageIdentity=1; PrintCmd("et...né...où?\n");}	
	return ErrorCode;
 }


	
static void HideKeyPad(){
		FormPtr 	Frm;
		FieldPtr 	FldPtr;
		UInt16 k;
		Frm = FrmGetFormPtr(frmadc16);
		for (k=btntan;k<=btnE;k++){
		FrmHideObject(Frm, (FrmGetObjectIndex(Frm, k)));
		Keypad=0;
		}
		if (GfxBigDisplay == 1) {
			FrmHideObject(Frm, (FrmGetObjectIndex(Frm, fld_prog))); 
			FrmHideObject(Frm, (FrmGetObjectIndex(Frm, fld_cmd))); 
		}

	}	
static void ShowKeyPad(){
		FormPtr 	Frm;
		FieldPtr 	FldPtr;
		UInt16 k;
		Frm = FrmGetFormPtr(frmadc16);
		for (k=btntan;k<=btnE;k++){
		FrmShowObject(Frm, (FrmGetObjectIndex(Frm, k)));
		}
		Keypad=1;
		//Redraw also the prog and cmd fields:
			if (GfxBigDisplay == 1) {
				FrmShowObject(Frm, (FrmGetObjectIndex(Frm, fld_prog))); 
				FrmShowObject(Frm, (FrmGetObjectIndex(Frm, fld_cmd))); 
				}
	}
	

	
static void Tracer3DAxis(){
	RGBColorType MyPenColor,OldPenColor,OldPenColor2;
	int k;
	float color=0;

	GridSet=1;
	HideKeyPad(); //hide the keypad
	
	MyPenColor.r = 0;
	MyPenColor.g = 0; 
	MyPenColor.b = 0;
	
	WinSetForeColorRGB(&MyPenColor,&OldPenColor);
	
//	Line(0,0,Dx(Xmax3d,0,0),Dy(Xmax3d,0,0),color);
//	Line(0,0,Dx(0,Ymax3d,0),Dy(0,Ymax3d,0),color);
//	Line(0,0,Dx(0,0,Zmax3d),Dy(0,0,Zmax3d),color);

	Line(Dx(Xmin3d,Ymin3d,0),Dy(Xmin3d,Ymin3d,0),Dx(Xmax3d,Ymin3d,0),Dy(Xmax3d,Ymin3d,0),color);
	Line(Dx(Xmin3d,Ymin3d,Zmax3d),Dy(Xmin3d,Ymin3d,Zmax3d),Dx(Xmax3d,Ymin3d,Zmax3d),Dy(Xmax3d,Ymin3d,Zmax3d),color);

	Line(Dx(Xmin3d,Ymin3d,0),Dy(Xmin3d,Ymin3d,0),Dx(Xmin3d,Ymax3d,0),Dy(Xmin3d,Ymax3d,0),color);
	Line(Dx(Xmin3d,Ymin3d,Zmax3d),Dy(Xmin3d,Ymin3d,Zmax3d),Dx(Xmin3d,Ymax3d,Zmax3d),Dy(Xmin3d,Ymax3d,Zmax3d),color);
	Line(Dx(Xmin3d,Ymin3d,0),Dy(Xmin3d,Ymin3d,0),Dx(Xmin3d,Ymin3d,Zmax3d),Dy(Xmin3d,Ymin3d,Zmax3d),color);
	Line(Dx(Xmin3d,Ymax3d,0),Dy(Xmin3d,Ymax3d,0),Dx(Xmin3d,Ymax3d,Zmax3d),Dy(Xmin3d,Ymax3d,Zmax3d),color);

	Line(Dx(Xmax3d,Ymin3d,0),Dy(Xmax3d,Ymin3d,0),Dx(Xmax3d,Ymax3d,0),Dy(Xmax3d,Ymax3d,0),color);
	//Line(Dx(Xmax3d,Ymin3d,Zmax3d),Dy(Xmax3d,Ymin3d,Zmax3d),Dx(Xmax3d,Ymax3d,Zmax3d),Dy(Xmax3d,Ymax3d,Zmax3d),color);
	Line(Dx(Xmax3d,Ymin3d,0),Dy(Xmax3d,Ymin3d,0),Dx(Xmax3d,Ymin3d,Zmax3d),Dy(Xmax3d,Ymin3d,Zmax3d),color);
	//Line(Dx(Xmax3d,Ymax3d,0),Dy(Xmax3d,Ymax3d,0),Dx(Xmax3d,Ymax3d,Zmax3d),Dy(Xmax3d,Ymax3d,Zmax3d),color);
	Line(Dx(Xmin3d,Ymax3d,0),Dy(Xmin3d,Ymax3d,0),Dx(Xmax3d,Ymax3d,0),Dy(Xmax3d,Ymax3d,0),color);

}
	
static float Dx(float x, float y, float z){
	return y-x*(xp-y)/zp;
}
static float Dy(float x, float y, float z){
	return 	z-x*(yp-z)/zp;
}	
	
static void TracerAxis(int centerx,int centery,int width, int height){
	RGBColorType MyPenColor,OldPenColor,OldPenColor2;
	FormPtr 	Frm;
	FieldPtr 	FldPtr;
	int k;
	float color=0;
	GridSet=1;
	HideKeyPad(); //hide the keypad
	
	MyPenColor.r = 0;
	MyPenColor.g = 0; 
	MyPenColor.b = 0;
	
	WinSetForeColorRGB(&MyPenColor,&OldPenColor);


	WinDrawLine(centerx-width/2,centery-height/2, centerx+width/2, centery-height/2);	
	WinDrawLine(centerx-width/2,centery+height/2, centerx+width/2, centery+height/2);	
	WinDrawLine(centerx-width/2,centery-height/2, centerx-width/2, centery+height/2);	
	WinDrawLine(centerx+width/2,centery-height/2, centerx+width/2, centery+height/2);	

	centerx=0;
	centery=0;
	height=(-DimYmin+DimYmax);
	width=(-DimXmin+DimXmax);
	Line(0,0,DimXmin,0,color);
	Line(0,0,DimXmax,0,color);
	Line(0,0,0,DimYmin,color);
	Line(0,0,0,DimYmax,color);
	if (StepX !=0){
		for (k=0;k<(DimXmax-DimXmin)/StepX;k++){
		Line(-k*StepX,0,-k*StepX,(DimYmax-DimYmin)/DrawZoneH,color);
		Line(k*StepX,0,k*StepX,(DimYmax-DimYmin)/DrawZoneH,color);
		}
		}
	if (StepX !=0){
		for (k=0;k<(DimYmax-DimYmin)/StepX;k++){
		Line(0,-k*StepX,(DimXmax-DimXmin)/DrawZoneW,-k*StepX,color);
		Line(0,k*StepX,(DimXmax-DimXmin)/DrawZoneW,k*StepX,color);
		}
		}
	}

static void Line(float x1, float y1, float x2, float y2,float color){
/*Drawingzone Square
#define DrawZoneX 90
#define DrawZoneY 90
#define DrawZoneH 70
#define DrawZoneW 70*/
	RGBColorType MyPenColor,OldPenColor,OldPenColor2;

	MyPenColor.r = 0;
	MyPenColor.g = 0; 
	MyPenColor.b = 0;

	if (color == 1){
	MyPenColor.r = 0xFF;//Red
	MyPenColor.g = 0x0A; 
	MyPenColor.b = 0x0A;
	}
	if (color == 2){
	MyPenColor.r = 0x0A;
	MyPenColor.g = 0xFF; //Green Only
	MyPenColor.b = 0x0A;
	}
	if (color == 3){
	MyPenColor.r = 0x0A;
	MyPenColor.g = 0x0A; 
	MyPenColor.b = 0xFF;//Blue
	}
	
	WinSetForeColorRGB(&MyPenColor,&OldPenColor);

	//x2-x1 =160
	//y2-y1=160
	//resizing according to gfxdim values
	if (x1<DimXmin ) {goto out;}
	if (x2 >DimXmax ){goto out;}
	if (y1<DimYmin ) {goto out;}
	if (y2 >DimYmax) {goto out;} // no drawing 
    
	x1=(x1-DimXmin)/(DimXmax-DimXmin)*160;
	y1=(y1-DimYmin)/(DimYmax-DimYmin)*160;
	x2=(x2-DimXmin)/(DimXmax-DimXmin)*160;
	y2=(y2-DimYmin)/(DimYmax-DimYmin)*160;
	//WinDrawLine(0,10,160,160);
	WinDrawLine ( x1*DrawZoneW/160+DrawZoneX,
									-y1*DrawZoneH/160+DrawZoneY+DrawZoneH,
										x2*DrawZoneW/160+DrawZoneX,
										-y2*DrawZoneH/160+DrawZoneY+DrawZoneH);

	out:
	
	WinSetForeColorRGB(&OldPenColor,&OldPenColor2); //Set OldColor
	}

	
