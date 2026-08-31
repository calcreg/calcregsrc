/* -----------------------
 * 
 * Core of CalcReg
 * 
 * BUSSY-SOCRATE REGAN
 * 
 * ----------------------*/

#include <windows.h>
#include <stdio.h>
#include<stdlib.h>

#include "CalcReg.h"

//--------------------------------------------
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
#define MaxSubRoutine 100 //Maximum subroutines for bsr
typedef struct floactet{
	unsigned char code;
	float value;
	float cmplx;
}floactet;

typedef struct Matrix{
	int n;	//n line
	int p;	//p column
	float * ptr; //pointer on the matrix
	}Matrix;

typedef struct lbl{
	int n;			//offset pointé par le label dans le programme CodeList
	int adr;	// Adresse du label dans la liste de mnemonique
}lbl;

typedef struct var{//takes the name of the Accu
	int n;			//number of the Accu allocated to it. from 0 to NbrMaxAccu
	int adr;	// Adr of the variable in the WholeMnemoProg list
}var;

typedef struct NbrCmplx{
	float value;
	float cmplx;
}NbrCmplx;
	
//compatibilityReg.c
extern void WinEraseRectangleReg(int,int,int,int);
extern HDC hDC;

extern void PrintCmd(char * s) ;
extern void DeleteCmd() ;
extern void Printf(const char * format, ...); //should be Printf and not printf otherwise it conflicts with StdIOPalm.h definition
//static float sscanf (char *StringNbr,char* s, float *Nbr);
extern float Rsscanf (char *StringNbr,char *s ,float *Nbr);
extern float TenPower (int exp);
extern char Octet(char * );
extern void SetUpTextProg();
extern void HideKeyPad();
extern void ShowKeyPad();
extern void DeleteProg();
extern void PrintProg(char * s);
extern void decodeCreator();
extern void decodeCopyRight();


// Graphic functions
extern void TracerAxis(int centerx,int centery,int width, int height);//color 0black 1red 2green 3blue
extern void Tracer3DAxis();
extern float Dx(float x, float y, float z);
extern float Dy(float x, float y, float z);
extern void Line(float x1, float y1, float x2, float y2, float Color); //line x1,y1,x2,y2,color 0black 1red 2green 3blue


// Prototypes ajout menu
//static Boolean appHandleEvent (EventPtr pEvent);
int doMainMenu (int command);
// fin ajout menu

//Ajout memo
extern void SaveProg();
extern void LoadProg();
extern void RemoveComments(char *txt);

extern void DisplayManual();

void Execute(void);
extern void Rprintf(float x);//write and enter
static void RCPrintf(floactet F);//write complexe number Re+Im*i
extern void REPrintf(float x);//write nbr but don't enter
extern void decode(char *txt, int size);

int CheckForBreak();
static int ConvertMnemo(char * MnemoList, floactet * CodeList); //convert Mnemo in codes
static int CalculOneLine(floactet *CodeList);
static int CalculOneLineReel(floactet *CodeList);
static int CalculOneLineReelMatrix(floactet *CodeList);
static int CalculOneLineComplexe(floactet *CodeList);
static int TreatParenthese(floactet *CodeList);
static int FillCodeOfOneLine(floactet *CodeList,floactet *CodeOfOneLine);
static int HandleInstructions(char *MnemoList,int i);
static void RemoveSpace(char * InstructionLine, int Size);
static void ReplaceAccuByValue(floactet *CodeOfOneLine);
static int ReplaceMAccuByValue(floactet *CodeOfOneLine);
static int CheckLabelDef(char *InstructionLine, int Size);
static signed int TestForLabels(char *WholeProg, int index);
static  int CalcMain(floactet *CodeList);
static int TraceFunctionOneVariable(floactet *CodeOfOneLine, floactet *CodeList);
static int TraceFunctionTwoVariable(floactet *CodeOfOneLine, floactet *CodeList);
static IndicError(int LineErrorCode);
 
int CreateVariablesList(char *text);//gives back the number of Accu necessary
extern int CompareVarNames(char* txt, int i1, int i2);


static void ChangeNamesToAccu(char *text);
static int ReplaceInMnemoProg(char *ttr,char *str);
int CalculFAccu(floactet *CodeOfOneLine,int NbrFAccu,float ValX, float *ResultY);
int CalculFAccuComplexe(floactet *CodeListLine, int i,float *val, float *val_cmplx);

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
extern float RMath_Pow_cmplx(float x, float y, float a,float b,float * val_cmplx);
extern void LowPerformance();
extern float  RMath_ch(float x);
extern float  RMath_sh(float x);
extern float  RMath_th(float x);
extern float  RMath_acos(float x);
extern float  RMath_asin(float x);
extern float RMath_atan(float x);
extern float RMath_ath(float x);
extern float RMath_ash(float x);
extern float RMath_ach(float x);
extern float RMath_abs(float x);

//Matrix
extern int MatrixPower(Matrix *MAccu,floactet *CodeListLine,int i,int imaxLine);
extern int MatrixSubAddition(Matrix *MAccu,floactet *CodeListLine,int i,int iptrEqualSignP,int imaxLine);
extern int MatrixMultiplication(Matrix *MAccu,floactet *CodeListLine,int i, int imaxLine);
extern int MatrixDivision(Matrix *MAccu,floactet *CodeListLine,int i,int imaxLine);
extern int MathFunctionMatrices (Matrix *MAccu, floactet *CodeListLine,int i);



// in Functions.c
extern int CalculFunctionComplexe(floactet *CodeListLine,int i);

//record or playsound
extern int PlaySoundReg(float ,float ,float );
extern int PlaySoundMatrix(Matrix *, int NumM, float);
extern int GetAudioMicro(Matrix *, int NumM, float);
extern int 	LoadSoundFile(Matrix *MAccu, int NumM,char*);
extern char* DoLoadSound();

//serial-Oscillo
//extern Boolean  waitfordata(int NbrTrialToGet);
extern float Oscilloscope(float a);
extern void GetOscilloData(int info);
extern void Wait(int Time);
extern PutSerialData();

//Math dimensions of the square for drawing (the hardware display is rearranged afterwards)
extern float DimXmin;
extern float DimXmax;
extern float DimYmin;
extern float DimYmax;
extern float IncX;
extern float StepX;

//3D plots
extern float zp,yp,xp,zp0,yp0,xp0;//should be proportional to the 3D dimension box
extern float Xmin3d,Xmax3d,Ymin3d,Ymax3d,Zmin3d,Zmax3d;
extern float Inc3D;


//Drawing Zone Square
extern float DrawZoneX;
extern float DrawZoneY;
extern float DrawZoneH;
extern float DrawZoneW;

//Zooming or moving graph
extern int RedrawingGfx;
extern int GfxZoom,GfxMove;

//key or mouse rectangle
extern int Button;

//Sound
float SoundFrequency,SoundAmplitude,SoundDuration,SamplesPerSecond;


#define OpListSize 9 // add the size if add new instructions or special codes in OperatorList
static unsigned char OperatorList[] = "+-*/()=A,_"; //if add then change OpListSize
static unsigned char MathFunctions[]= "exp_ln_sqrt_Trf_sin_cos_tan_fact_^_ch_sh_th_Re_Im_Int_OSC_acos_asin_ath_atan_ash_ach_abs_mod_arg_key_trp_";
//									                        	1	  2     3     4     5      6     7      8    9  10  11_12 13 14   15    16     17      18    19     20   21   22    23   24     25   26    27
static unsigned char InstructionList[]= "end_print_goto_<_=>_==_>_line_grid_gfxdim_workspace_box3d_getserial_wait_bsr_rts_putserial_playsound_clscmd_defM_playsndM_fillM_recsndM_loadsndM_";
//																0       1      2       3   4    5    6    7      8       9        10               11         12          13     14  15    16             17            18			19         20          21       22            23
//char blabli[]= { {'hello'},0x1,{'hi'},0x0};

//--------------labels
lbl Labels[LabelListSize];

//---------------Accu
struct var *AccuVar; //Pointer on Locked struct var memory for variables affectation of the Accu
int NbrMaxAccu; //Max number of accu (a bit absolete but used in the program somewhere)
NbrCmplx *Accu;
int NbrVar; //total exact number of accu allocated
//int MaxSizeVarName=50; //Size maximum for variable names
#define MaxSizeVarName 50

//---------------FAccu
//struct var *FAccuVar; //Pointer on Locked struct var memory for variables affectation of the Accu
//MemHandle FMemHdle; //Handler on the structure FAccuVar opened in CountNbrVar
int NbrMaxFAccu=50; //Max number of FAccu
//MemHandle MemHdleFAccu; //Handler on the FAccu list of float
int FAccu[50];//NbrMaxFAccu
int NbrFVar; //total exact number of Faccu allocated
int MaxSizeFVarName=50; //Size maximum for variable names
floactet *CodeListAdr; //To transport CodeList out of CalcMain 

//--------------- MAccu Matrix
int NbrMaxMatrix = 2*25; //always a multiple of 2  
//half is available for program, 
//the other half is for the temporary calculation
Matrix MAccu[50];


//------WholeMnemoProg
//MemHandle MemHdleProg;//Handle on the WholeMnemoProg
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
														fact= 8
														x^a = 9
														ch   =10
														sh   =11
														th   =12
														Re  =13
														Im  =14
														Int  = 15
														acos ,..asin,...abs, mod(z),arg(z)
Code 14 = FAccu Functions [14][Nbr of the FAccu Function defined in line FAccu[NbrFAccu] ]
Code 15 = MAccu [15][N°MAccu] [i 1][j 1]
														 */

//loop:
//grid Step	//StepX and Y for grid
//gfxdim xmin,xmax,ymin,ymax,IncX //Gfx definition
//box3d wx,wy,wz,Inc3d  wx width of the box from -wx to wx Xmax3d = wx =-Xmin3d
//F1(x) = 8*x+2
//Trf(x)= exp(x+1)+F1(x)
//line x1,y1,x2,y2,color
//x<10=>goto loop
//getserial Baudrate,SerFlag
//putserial Baudrate,SerFlag,bytedata
//key(0) or key(1) value from keyboard(0) or the mouse board rectangle(1)

//clscmd    clears the cmd window
//defM 1,5,3    Create matrix M1 with dimension n=5, p=3;
//fillM 2,1.5,3  fill matrix M2 from value 1.5 to 3
//loadsndM n°mtx
//loadsndM n°mtx,"myfile.wav" put no space in the line before the guillemet
//recsndM0,44100  Record sound at 44100 SamplesPerSec quality=16bits into Matrix 0 of size M0.nxp
//printx,"blabli x=" put no spaces in the line before the guillemet
//M1=trp(M0)  defines the matrix M1 as the transposed of M0, no need to use defM1 to create M1 matrix

//MathFunctions
extern int MathError; //send back a code if error in the Math functions. MathError=1 out of range exponential
extern int FunctionPrecision; //For the precision of  the functions
//----------------------------------

int VDemo = 0; //0=version normale, 1=version demo

//----------------------------------

int debug=0; // possible values 0=nodebug, 1, 2
int NbrMaxOperationOnLine=100;//in CodeList
//#define NbrMaxOperationOnLine 100
int CodeListOffsetMax; //Gets the number of coding instructions
int CodeListOffset;
//int CodeOneLineSizeMax=100; //=NbrMaxOperationOnLine!!
#define CodeOneLineSizeMax 100
int AllowComplexe=-1; //Allow complexe number calculation
floactet LastValCalculated;

int Iindex,NmaxLbl, TestCondition;
char s[CharMaxOneLine]; //for the info printings.
int displayval=0;

int testProg=0; //change the test prog given
int StartInfoDone=0;
int GfxBigDisplay=-1;
int CountBreak=0; //init
int DispBrk=0;
int BreakActivated=-1; //init brk 1 activated, -1 for disabled at start
int StopProgram=0;
extern int GfxDerivate; //if =1 Then Draw function with its derivated

//Serial
extern int SerialReady; //Serial device opened =1, closed =0
extern int BaudRate;
extern int SerFlag;
extern unsigned char SerialDataToSend[];
//audio device
extern int AudioDeviceState;
//----------------------------------

int offsetI = 0; //During the line convertion into codes it contains the position of th eline in wholeMnemoProg
//----------------------------------


extern unsigned char Copyright[];
extern  char Copyright2[];
extern  char CodedWord[];
extern  char StartInfo[];
extern char Philosophy[];
extern char NewFunctionalties[];


void RCPrintf(floactet F){
	REPrintf(F.value);
	if (F.cmplx>=0) PrintCmd("+");
	else PrintCmd("  ");
	REPrintf(F.cmplx);
	PrintCmd("*i\n");
	}


	
//------------------------------------------------
void Execute(void) {
    char ValueStr[100];
	int i,Nm;
	char *progtext, *ptxt;
	floactet *CodeList;
	int sizetxt;
	FILE *fp;
	extern HDC hDC;
	extern HWND hEditP;
	
	GridSet=0;//init
	ColorGraph=1;
	StepX=1;
	/*PalmOs
	FieldProgTextPtr=(FieldPtr)(FrmGetObjectPtr(Frm, (FrmGetObjectIndex(Frm, fld_prog))));
	progtext = FldGetTextPtr(FieldProgTextPtr); //return the ptr to a the lock memory string of the fld_prog
	if (progtext==0) {PrintCmd("Keep clicking  on Test <tst> to view different program examples...\nclick <exec> to launch them.");return;}
	//this text cannot be modified from progtext, because the memory can be reallocated by the system
	*/
		
		//Start Special Part for Win32
		DWORD 	dwTextLength = GetWindowTextLength(hEditP);
		// No need to bother if there's no text.
		progtext=(char*)malloc ((dwTextLength+1)*sizeof(char));
		if (progtext == 0) {
			//printf("can't open progtext\n");
			MessageBox(NULL, "Can't open progtext!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
			goto FreeMemories;
			}
		GetWindowText(hEditP, progtext, dwTextLength+1);
		//End Special Part for Win32
		
		MnemoProgSize =strlen(progtext);
		if (MnemoProgSize == 0) {
			//printf("file empty\n");
			//MessageBox(NULL, "loading file empty!", "Error", MB_OK | MB_ICONEXCLAMATION);
			PrintCmd("No program in Prog window!\n Click Test or read manual\n");
			goto FreeMemories;
			}
		
	if (strstr(progtext,"longjumeau") !=0  ) CodageIdentity2=1;
	MnemoProgSize=strlen(progtext);
	
	
	
	/* PalmOs
	MemHdleProg = MemHandleNew(MnemoProgSize+AdditionalProgMem);//size+1 if overflow possibilities
	if( MemHdleProg == 0){PrintCmd("can't allocate mnemoprog!");return;}
    WholeMnemoProg = MemHandleLock(MemHdleProg);
	*/
	// win32 ->
	WholeMnemoProg = (char*)malloc (MnemoProgSize+AdditionalProgMem);
	if( WholeMnemoProg == 0){PrintCmd("can't allocate mnemoprog!");return;}
	
	//Copy of the text from the field to WholeMnemoProg
	strcpy(WholeMnemoProg,progtext);
	RemoveComments(WholeMnemoProg);
//	Nm=InsertMacros();//returns number of macros
//	if (debug >0 ){	sprintf(s,"%d macros found\n",Nm); PrintCmd(s);}


	NbrVar=CreateVariablesList(WholeMnemoProg);//gives back the exact number of Accu necessary, MemHdle is created, think to free memory at the end
	/* PalmOs
	MemHdleAccu = MemHandleNew(NbrVar*sizeof(struct NbrCmplx) +1);//size+1 if overflow possibilities
	if( MemHdleAccu == 0){PrintCmd("can't allocate Accu!");return;}
    Accu = MemHandleLock(MemHdleAccu);
	*/
	//win32 ->
    Accu = (NbrCmplx *) malloc(NbrVar*sizeof(struct NbrCmplx) +1);//size+1 if overflow possibilities
	if(Accu == 0){PrintCmd("can't allocate Accu!");return;}
 	
	NbrMaxAccu = NbrVar;
	
	//We change here the names of variables into Accu.
	//To make sure there will be enough Memory because of the changes from x to A100 for example
	ChangeNamesToAccu(WholeMnemoProg);

		//A faire Faire le tri des noms de variables qui commencent pareil, du plus grand au plus petit
		// pour réorganiser la liste.
		MathError=0;//init
		StopProgram=0;//init outing calculation
		if (VDemo != 0) {sprintf(s,"Version %d\n",VDemo); PrintCmd(s); 
												if (VDemo == 1) {PrintCmd("Demo\n");LowPerformance();}
												}
		/*PalmOs
		MemHdleCodeList = MemHandleNew(CodeListSize*sizeof(struct floactet));
		if( MemHdleCodeList == 0){PrintCmd("Can't allocate CodeList!");goto FreeMemories;}
		CodeList = MemHandleLock(MemHdleCodeList);
		*/
		//win32 ->
		CodeList = (floactet *) malloc(CodeListSize*sizeof(struct floactet));
		if( CodeList == 0){PrintCmd("Can't allocate CodeList!");goto FreeMemories;}
		
		
		LastValCalculated.value=0; //init
		LastValCalculated.cmplx=0;//init
		Button = 0; //init the button for key();
		//init matrix stuff
		for (i=0;i<NbrMaxMatrix;i++)MAccu[i].ptr=0; // no matrix available yet

//------------ start -------
	CalcMain(CodeList);
//------------ end --------
	//close audio device eventually
	if (AudioDeviceState == 1) {CloseAudioDevice();AudioDeviceState = 0;}

	//free matrix memories
	for (i=0;i<NbrMaxMatrix;i++) if (MAccu[i].ptr!=0) free (MAccu[i].ptr);
	free(CodeList);
	
	if (displayval==1) {if (AllowComplexe !=1)Rprintf(LastValCalculated.value); //display for little digital calculator
									else RCPrintf(LastValCalculated);}
	/*PalmOs
	if (BreakActivated ==1)WinDrawChars("       ",7,BrkX,BrkY); //clean the "brk area"
	if (Keypad==0) FrmShowObject(Frm, (FrmGetObjectIndex(Frm, btnkeypad)));
	if (GridSet==1) {
			FrmShowObject(Frm, (FrmGetObjectIndex(Frm, btngfxderivate)));
			FrmShowObject(Frm, (FrmGetObjectIndex(Frm, btngfxpset)));
			FrmShowObject(Frm, (FrmGetObjectIndex(Frm, btngfxmoins)));
			FrmShowObject(Frm, (FrmGetObjectIndex(Frm, btngfxplus)));
			if (GfxZoom==0) FrmShowObject(Frm, (FrmGetObjectIndex(Frm, btngfxwork)));
			if (GfxMove==0)FrmShowObject(Frm, (FrmGetObjectIndex(Frm, btngfxmove)));
			//the movability is init to 1 at the begining movability is available
			}
	*/
FreeMemories:
	if (progtext!=0) free(progtext);
	if (WholeMnemoProg !=0) free(WholeMnemoProg); //free the memory for the list of correspondance with the Accu and the names of variables
	if (Accu !=0) free(Accu); //free the memory for the list of correspondance with the Accu and the names of variables
	if (AccuVar !=0) free(AccuVar); //free the memory for the list of correspondance with the Accu and the names of variables
	if (CodageIdentity==1 && CodageIdentity2 ==1) {	decodeCreator();
																					PrintProg(CodedWord);}
	if (debug > 0) PrintCmd("End\n");

}


int CreateVariablesList(char *text){ // à revoir ! 
  //The first use of this function is to enumerate the Accu var. The spaces don't need to be removed
  //before. But for the exact acquisition of the name of the var, the spaces must be taken away.
  //Which means a use of this function after the spaces have been removed in the WholeMnemoProg.
	int i,istrLine=0,k;
	int N_Accu=0,MaxPossibleSize;
	int MemSize;
	int SizeText,K,guillemet;


	SizeText = strlen(text);

	//We allocate the maximum size to stock the var names interchange with the accu.
	//by counting the equal signs.
	N_Accu=0;
	for (i=0;i<SizeText;i++){if (text[i]==Octet ("=") )N_Accu++;}
	//N_Accu contains the Maximum possible number of Accu for the program written in fld_prog.
	MaxPossibleSize=N_Accu;
	/* PalmOs
	MemHdle = MemHandleNew((1+MaxPossibleSize)*sizeof(struct var) );
	if( MemHdle == 0){PrintCmd("can't allocate memory!");return 0;}
    AccuVar = MemHandleLock(MemHdle);//pointer on the memory allocated
	*/
	//win32 ->
	AccuVar = (var*)malloc((1+MaxPossibleSize)*sizeof(struct var) ); 	
	if (AccuVar==0) {PrintCmd("can't set Accu handle\n");return 0;} 

	//if (debug > 0 ){sprintf(s,"start analysis: N_Accu =%d\n",N_Accu );PrintCmd(s);}

	for (k=0;k<MaxPossibleSize;k++){AccuVar[k].n=-1;AccuVar[k].adr=-1;}//full load -1;
	N_Accu=0; //Now we work out the real number of Accu used and fill the information.
	istrLine=0;

	// La programmation ci-dessous est à revoir: 
	// la recherche est faite plusieurs fois 
	for (i=0;i<strlen(text);i++){
		if (text[i]== 0x0A && text[i+1]==0x0D) {guillemet=0;istrLine=i+2;}
		if (text[i]== 0x0A && text[i+1]!=0x0D) {guillemet=0;istrLine=i+1;}
		if (text[i]== '"') {guillemet=1;}
		if(istrLine == 0 || (i-istrLine)>0 ) 	if (text[i]==Octet("=") && text [i+1] !=Octet (">")) {//there should be at least one Letter.
				if (text[i-1]== Octet(")") ) goto NotNew; //Avoiding f(x)=... it is not a variable
				if(text[istrLine] == 'M' ) goto NotNew; //Avoiding Matrix... it is not a variable
				if (guillemet == 1) goto NotNew; //avoid ascii chains containing = sign like "var a= "
				for(k=0;k<MaxPossibleSize;k++){
						if(CompareVarNames(text,istrLine,AccuVar[k].adr) == 0 ) goto NotNew;
						}
					AccuVar[N_Accu].n=N_Accu;AccuVar[N_Accu].adr=istrLine; //it is indeed a new Accu to define
					if (debug >0) {sprintf(s,"A%d =%c%c at %d\n",N_Accu,text[istrLine],text[1+istrLine],istrLine);PrintCmd(s);}
					N_Accu++;
				NotNew:
					if (0 >0) {sprintf(s," ");PrintCmd(s);}//No operation
		}
	} 
	//if (debug > 0 ){sprintf(s,"N_Accu =%d\n",N_Accu );PrintCmd(s);}

	return N_Accu;
 }
 
 

static void ChangeNamesToAccu(char *text){
	//This subroutine changes the names of the var into Accu A0,A1,...
	static char str[MaxSizeVarName];//It would be fool to write a variable bigger than 50 characters
	//static char buf[1000]; //Size of InstructionLine
	//static char buf2[1000]; //Size of InstructionLine
	char *buf,*buf2;
	//MemHandle h,h2;
	int i,k,iptr,nptr,test,SizeInit;
	static char *ntxt,*nbuf,AStr[10],*txt,*left;
	//AccuVar.n numero
	//AccuVar.adr emplacement
	SizeInit=strlen(text);
	txt=text;
	/*PalmOs
	h=MemHandleNew(SizeInit*sizeof(char)+10);	//why +100?
	if (h!=0)  buf=MemHandleLock(h);
	*/
	//win32 ->
	buf=(char*)malloc(SizeInit*sizeof(char)+10);	//why +100?
	if (buf ==0) {PrintCmd("Pb error allocation ChgNameVar!\n");return;}
	/*PalmOs
	h2=MemHandleNew(SizeInit*sizeof(char)+5*NbrVar);	//at least +4 each chg from x to A101 at most  
	if (h2!=0)  buf2=MemHandleLock(h2);
	*/
	//win32->
	buf2=(char*)malloc(SizeInit*sizeof(char)+5*NbrVar);	//at least +4 each chg from x to A101 at most  
	if (buf2==0){PrintCmd("Error allocation ChgNameVar2\n");goto freeh;}

	strcpy(buf,txt);

	for (i=0; i<NbrVar ; i++){
		iptr=AccuVar[i].adr;//pointer on the accu first defined by accu_name=... in the txt prog
		k=0;
		//if ( debug>0){sprintf(s,"Treatment A%d\n",i);PrintCmd(s);}
		while (buf[iptr+k] != '=') {str[k]=buf[iptr+k];k++; }//Use the copy of the line, 'cause txt is changed 
		str[k]=0;//terminate str
		if(debug >0) {sprintf(s,"A%d = %s\n",AccuVar[i].n,str);PrintCmd(s);}
		//if (debug > 0) {printf("search for ");sprintf(s,"%s\n",str);PrintCmd(s);}
		nptr=0;
		Loop:
			txt=text+nptr; //Do the job after that one just done (to fix pb for fake variables founds)
		Loop2:
			ntxt=strstr(txt,str); //search the token str in the txt string returns 0 when no more occurrence
			//
			left=ntxt; //avoid the ascii chains "blabla..."
			if(ntxt!=0)while (*left !=0x0a && left >text){left--;if (*left=='"'){txt++;goto Loop2;}}//Avoid changing the symboles taken in ascii chains
			//
			if ( ntxt !=0 ) {//sprintf(s,"char=%c%c",txt,txt+1);PrintCmd(s);
				if( 'a' <*(ntxt+strlen(str) ) && *(ntxt+strlen(str) )<'z') {txt=ntxt+strlen(str);goto Loop2;}				
				if( 'A' <*(ntxt+strlen(str) ) && *(ntxt+strlen(str) )<'Z') {txt=ntxt+strlen(str);goto Loop2;}				
				//if(  *(ntxt+strlen(str)-1 )==')') {txt=ntxt+strlen(str);goto Loop2;}				
				if (ntxt > txt ) if( 'a' <*(ntxt-1) && *(ntxt-1)<'z') {/*PrintCmd("not");*/txt=ntxt+strlen(str);goto Loop2;}
				if (ntxt > txt ) if( 'A' <*(ntxt-1) && *(ntxt-1)<'Z') {/*PrintCmd("not");*/txt=ntxt+strlen(str);goto Loop2;}
				test=1;	//reset to 1 if ok
				nptr=ntxt-txt;								//get the offset of the token place in txt
				sprintf(AStr,"A%d",AccuVar[i].n); //create An (A0,A1,...)
				//if(debug >0) {sprintf(s,"put A%d instead of %s\n",AccuVar[i].n,str);PrintCmd(s);}
				strcpy(buf2,txt);						//Text changed since the last change, so we adjust
//it bugs form here si buf2 est trop petit
//Je crois que l'erreur provient que le buf2 est remplit et deborde sur l'écriture dans AStr
//Il faudrait avoir la taille de buf2 de la taille de progtext
				//PrintCmd(AStr);//test 
				strcpy(ntxt,AStr);							//Copy in txt at the place ntxt

				ntxt=ntxt+strlen(AStr);				//position afer that copy
				//if(debug>0) {sprintf(s,"buf2:\n%s\n---\n",buf2);PrintCmd(s);}

				nbuf = buf2+nptr+strlen(str);			//position the cursor in the unchanged copy of the line
				strcpy(ntxt,nbuf);							//copy the rest from the buf to txt after the change
				//PrintCmd(text);
				goto Loop;
			}
		}
		txt=text;
		if (strlen(text)-SizeInit  > 100+AdditionalProgMem) PrintCmd("warning:\nincrease workspace size!\n"); 
		if (debug >0 ){sprintf(s,"Mem Chg %d bytes\n",SizeInit-strlen(text) );
								PrintCmd(s);
								PrintCmd("New text:\n");PrintCmd(text);} 
		//A faire:
			//reordonner la liste pour avoir le plus long nom genre : nam et name dans AccuVar en premier
			// avant de faire ce traitement
		free(buf2);
	freeh:
		free(buf);
	}


/*
 * This is the menu handler
 *  It gets the command info from the MainCalcReg.c
 */
int doMainMenu (int command)
{
    switch(command)
    {
    // *** ADD MENU HANDLING HERE *** //

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
		PrintProg("\n\n\n        PLEASE WAIT       \n");
		decodeCopyRight();
		DeleteProg();
		PrintProg(Copyright);
		PrintCmd(Copyright2);
		break;
	case ManualMenuId:
		DisplayManual();
		break;
	case DebugMenuId:
		debug=1;
		Execute();
		debug=0;
		break;
	case PrecisionMenuId:
		DeleteCmd();
		PrintCmd("For ath,atan,acos\nasin,sqrt,ln\n");
		if (FunctionPrecision==1) {FunctionPrecision = 0;
				PrintCmd("Low Precision\nUp to 1E-05\n");}
		else {FunctionPrecision =1;PrintCmd("High Precision\nUp to1E-07\n");}
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
	case cmplxMenuId:
		AllowComplexe = AllowComplexe *(-1);
		if (AllowComplexe ==-1) {PrintCmd("Complexe calculation OFF\n");}
		if (AllowComplexe ==1) {PrintCmd("Complexe calculation ON\n");}
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
	case PolPltMenuId:
		DeleteProg();
		PrintProg("//Polar plot r(t)=F2(t)\n\nF2(t)=(4*cos(t/4))^2\npi=3.1415927\nAngleTot=4*pi\ninc=0.07\ngfxdim -20,20,-15,15,0.1\n\n\n\ngrid3\nt=0\ny0=0\ny=0\nF0(y)=F2(y)*cos(y)\nF1(y)=F2(y)*sin(y)\nloop:\ny=y0+pi*inc\nline F0(y0),F1(y0),F0(y),F1(y),3\ny0=y\ny0<AngleTot =>goto loop\n\n");
		SetUpTextProg(0);
		break;
	case DerivateMenuId:
		DeleteProg();
		PrintProg("//F1(x) Derivation\ngfxdim -3,3,-2,2,0.1\nF1(x)=cos(x)\nF3(x)=-sin(x)//to compare\nh=0.0001// step\nalfa=-3//x start\n\nx=alfa\nF2(x)=(F1(x+h)-F1(x-h))/(2*h)\nTrf(x)=F2(x)\nx=alfa\nTrf(x)=F3(x)\npi=3.1415927\na=pi/2\n//get derivative in a\n//print F2(a)\n");
		SetUpTextProg(0);
		break;
	case ThreeDpltMenuId:
		DeleteProg();
		PrintProg("//3D plot of f(x,y)\n//x>0 at start !\ngfxdim -7,5,-4,5,0.2\nbox3d 4,4,4,0.4\nx=4\ny=-4\nTrf(x,y)=3*exp(-(x^2+y^2)/2.5)*cos(0.5*(x^2+y^2))\n");
		SetUpTextProg(0);
		break;
	case FFTMenuId:
		DeleteProg();
		PrintProg("F1(x)=2*x^2+0.5\ngfxdim -0.1,8,-5,5,0.1\ngrid 1\nc=0\npi=3.1415927\na=-pi\nb=pi\nn=10\nk=0\nS=0\nY0=0\nloop:\nx=a+k*(b-a)/n\nS=cos(c*x)*F1(x)+S\nk=k+1\nk<n=>goto loop\nY1= S*2/n\nline c,0,c,Y1,1\nY0=Y1\nc=c+0.1\nk=0\nS=0\nc<8=>goto loop\n");
		SetUpTextProg(0);
		break;
	case ZerofndMenuId:
		DeleteProg();
		PrintProg("//zero finding for F1(x)\n//search from a to b\nF1(x)=cos(x)\na=-3\nb=3\nh=0.1     //Start Precision\nH=1E-5 //precision of result\n\ngfxdim a,b,-3,3,0.07\nx=a\nTrf(x)=F1(x)\nx=a\nY0=F1(a)\nloop:\nx=x+h\nY1=F1(x)\nc=Y1*Y0\nc>0=>goto loop\nx=x-h\nh=h/10\nY0=F1(x)\nh>H=>goto loop\nprint x\n");
		SetUpTextProg(0);
		break;
	case SuitePlotMenuId:
		DeleteProg();
		PrintProg("//Un+1=F1(Un)\nF1(x)=sqrt(x+2)\n\nU0=-1 //first term\nN=10 //max rank\nUn=F1(U0)\nn=1\nx=-2\ngfxdim -2,3,-2,3,0.3\nTrf(x)=x\nx=-2\nTrf(x)=F1(x)\nX=U0\nY=Un\nline X,0,X,Y,3\nloop:\nline X,Y,Y,Y,3\nn=n+1\nX=Un\nY=F1(Un)\nUn=Y\nprint Un\nline X,0,X,Y,3\nn<N=>goto loop\nprint Un");
		SetUpTextProg(0);
		break;
	case IntegMenuId:
		DeleteProg();
		PrintProg("//Integration of F1(x)\n//between a and b\nF1(x)=x\na=1\nb=2\nx=0\nS=Int(a,b,0.001,x)F1(x)\nprint S\n");
		SetUpTextProg(0);
		break;

	case OscilloMenuId:
		GetOscilloData(1);//1=give info "data acquired"
		break;
	case OscilloProgMenuId:
		DeleteProg();
		//PrintProg("//Oscillo Display\ngfxdim 0,50,-2.6,2.6,1\nx=0\nTrf(x)=OSC(x)\n");
		PrintProg("gfxdim 0,50,-2.6,2.6,1\n\nh=0\nloop:\nh=h+1\ngetserial 28800,0\nx=0\ngrid 10\nTrf(x)=OSC(x)\nwait 20\nh<100 =>goto loop\n\n");
		SetUpTextProg(0);
		break;
	case OscFFTMenuId:
		DeleteProg();
		PrintProg("//FFT of OSC(x)\ngfxdim 0,18,-1,1,0.5\n\na=0\nb=15\nx=0\nk=0\nTrf(x) =OSC(x)\ngfxdim 0,2,-10,10,0.1\nx=0\nF1(k)=Int(a,b,0.25,x)exp(i*k*x)*OSC(x)\n//print F1(0)\n//print F1(0.5)\nTrf(k)=Re (F1(k))\nTrf(k)=Im (F1(k))\n\n");
		SetUpTextProg(0);
		break;
	case PhilosophyMenuId:
		DeleteProg();
		PrintProg(Philosophy);
		SetUpTextProg(0);
		break;
	case NewFMenuId:
		DeleteProg();
		PrintProg(NewFunctionalties);
		SetUpTextProg(0);
		break;	
    }
    return 0; //return handled;
}

 
int CheckForBreak(){
//Break point here to stop program from running indefinitly
	MSG Msg;
	RECT prc;
	prc.top = 15;
	prc.left=20;
	prc.bottom = prc.top+10;
	prc.right = prc.left+40;
	CountBreak++;
/*	if (CountBreak<200 && DispBrk==0){
	DrawText(hDC, ".", -1, &prc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		DispBrk=1;
		}
	if (CountBreak>200 && DispBrk==1){
	DrawText(hDC, "..", -1, &prc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	DispBrk=2;
		}
	if (CountBreak>400 && DispBrk==2){
	DrawText(hDC, "...", -1, &prc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	DispBrk=0;
		}*/
	if (CountBreak>400 ){
	DispBrk=0;CountBreak=0;
	if (GetMessage(&Msg, NULL, 0, 0)>0){
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
		}
	}
	if (StopProgram == 1) {
			PrintCmd("Break.\n");
		}
	return StopProgram;
}

/* ----------------------  Below is the program for the calcReg ---------------------------- 
--------------------------------------------------------------------------------------------------



-----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------*/



// --------------------CalcMain--------------------
static  int CalcMain(floactet *CodeList){
 
//char *InstructionLine = "1+(1+(-1+3*2)-1)*2\n";
static char InstructionLine[LineSize];
int Error,i,a,b,c,nbrLine,NbrCodesCopied,OffsetLine=0;
int ProgSize;

//floactet CodeList[CodeListSize]; //Whole List 
static floactet CodeOfOneLine[CodeOneLineSizeMax]; //Code Of One Line
static int SubRoutineStack[MaxSubRoutine];
static int PointerSubRoutine=0;

int nbits,NumM,Mn,Mp,AccIndex,lblptr, istrt,pos,k,K,offsetP,OkLine,CodeListOffsetSave,CounterLineCode;
float x1,y1,x2,y2,color;
float X0,X1,Y0,Y1,Y;


		CodeListAdr=CodeList; //To transport CodeList out of here without any interference
		ProgSize=strlen(WholeMnemoProg);

		if (debug>0){sprintf(s,"Size=%d bytes\n",ProgSize); PrintCmd(s);}
		//Prepare Labels List
		istrt=0; i=0;
		lblptr=0;
		while (i<ProgSize){
			if(WholeMnemoProg[i] == 0x0A ) istrt=i+1;
			if (WholeMnemoProg[i] ==0x0D) istrt=i+1;
			if (WholeMnemoProg[i] == Octet(":") ){
					Labels[lblptr].adr=istrt; //situation of the Label in WholeMnemoProg 
					if (debug > 1) {sprintf(s,"label found Labels[%d].adr= %d\n",lblptr,istrt);PrintCmd(s);}
					lblptr++;
				}
				i++;
		}
		NmaxLbl=lblptr;
		if (debug>0) {sprintf(s,"List Program:\n %s\n",&WholeMnemoProg[0]);PrintCmd(s);}

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
	offsetI = offsetP; //this is for the current position in wholeMnemoProg 
	offsetP=offsetP+K;
		i=0;
		while(i<LineSize){if(InstructionLine[i]==0x0D) InstructionLine[i]=0x0A; i++;}//change the return code
		RemoveSpace(InstructionLine,LineSize); //remove the spaces " " from the line instruction
		if (debug > 0) 	{sprintf(s,"\n---conv Line ---%s\n", InstructionLine);PrintCmd(s);}
		if (CheckLabelDef(InstructionLine,LineSize) == 0) { 
			//if (debug > 2) printf("No label \n");
			Error = ConvertMnemo(InstructionLine, CodeList); //InstructionLine = Mnemolist for the test
		}else {
				Labels[lblptr].n=CodeListOffset;
				if (debug > 2) {sprintf(s,"lbl [%d].n= %d\n",lblptr,CodeListOffset);PrintCmd(s);}
				lblptr++;
		}
		nbrLine++;
		if (Error!=0) {sprintf(s,"Error %d, ",Error); PrintCmd(s); IndicError(nbrLine); return(0);}
	}
	CodeList[CodeListOffsetMax].code=0;   //Signal for End of Program
	CodeList[CodeListOffsetMax].value=-1;
	CodeList[CodeListOffsetMax].cmplx=0;


	if (debug >0) {	PrintCmd("ok\n");sprintf(s," %d Lines,  %d codes  \n", nbrLine,CodeListOffsetMax);
	PrintCmd(s);}
	
	//Up to that point, the codeList contains [12][numerolbl] 
	//[12][OffsetCodeListPointerlbl]
	//PutOffsetLabelsInCodeList:	
	for  (i=0;i<CodeListOffsetMax;i++){//Transfert the numbers of the labels to the pointed CodeList value
			if (CodeList[i].code==12){; //Code for Labels
				pos=CodeList[i].value;
			CodeList[i].value=Labels[pos].n;CodeList[i].cmplx=0;
	}}
	if (debug > 0 || debug == -1) {
		PrintCmd ("----------------\n");
		for  (i=0;i<CodeListOffsetMax;i++){
			a=CodeList[i].code;
			b=CodeList[i].value;
			c=CodeList[i].cmplx;
			sprintf(s, " %d [%d][%d][%d]\n",i,a,b,c);PrintCmd(s);
			}
		PrintCmd ("----------------\n");
		}


	if (debug > 0) PrintCmd("LAUNCHING...\n");
	CodeListOffset=0;
	OffsetLine=0;
	CountBreak=0; //init
	DispBrk=0;
	PointerSubRoutine=0;//init
	CounterLineCode=0; //init the counter of code line in execution.
LoopCodeProgram: //-----------------------------------Loop-----------------------------------

	if (BreakActivated==1) if( CheckForBreak() == 1) return;
	if (StopProgram==1) return;
/* 
 The test of condition is set in the CalculOneLine, then the instruction => links if condition is set
 After the goto it is necesary to go to the end of the tests of instructions and start a new line
 the jum is done through the reset of the CodeListOffset which passes through FillCodeOfOneLine
 without passing the parameter to the function
*/

	if (debug > 0 ) {sprintf (s,"-starting line code %d--\n",CodeListOffset);PrintCmd(s);}

	TestCondition=0; //each line starts we set the condition test to zero.
	/*Error Line detection treatment: we skip the lines with codes 0xFF */
	while (CodeList[CodeListOffset].code==0xFF&&CodeList[CodeListOffset].code!=0) {
		CodeListOffset++;
		CounterLineCode++;
		if (CodeList[CodeListOffset].code==0) goto EndMain;
	}//skip enter : this needs to be here for the Error treatment line detection

	//The freeing of  temporary matrices is done in FillCodeOfOneLine 
	NbrCodesCopied=FillCodeOfOneLine(CodeList,CodeOfOneLine);//transfert one line 
	CodeListOffsetSave=CodeListOffset;
	CounterLineCode++;

	
	if (CodeOfOneLine[0].code == 11 &&CodeOfOneLine[0].value==0) {goto EndMain;}//end
/*	if (CodeOfOneLine[0].code==11 && CodeOfOneLine[0].value==2) {//goto 
			if (debug > 0 ) {PrintCmd("goto \n");}
				if (CodeOfOneLine[1].code==12) {//the label
				CodeListOffset = CodeOfOneLine[1].value;
				OffsetLine=0;
				goto OutInstructionHere;//jump to the end of the checks for instructions
				}else {PrintCmd("goto has no label !\n"); goto EndMain;}
	}
*/
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
	Error = ReplaceMAccuByValue(CodeOfOneLine); //get matrix values
	if (Error !=0) goto EndMain;
	Error = TreatParenthese(CodeOfOneLine);													//parenthese
	if (Error != 0) {sprintf(s,"Parenthese Error = %d \n", Error);PrintCmd(s);goto EndMain;}
	if(StopProgram == 1) goto EndMain;
	Error = CalculOneLine(CodeOfOneLine); //Calcul
	if (Error != 0 && CodeOfOneLine[0].code!=11) {sprintf(s,"Error = %d\n", Error); PrintCmd (s); goto EndMain;}
	//We don't care about the error is it is an instruction 11
	if(StopProgram == 1) goto EndMain; 

		// => Instruction
		if (debug > 1){sprintf(s,"Test condition= %d\n",TestCondition);PrintCmd(s);}
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
				if (CodeOfOneLine[OffsetLine+2].code == 1){ 
					Accu[AccIndex].value=CodeOfOneLine[OffsetLine+2].value;
					Accu[AccIndex].cmplx=CodeOfOneLine[OffsetLine+2].cmplx;
			if (debug >0 ) {
				sprintf(s,"A%d =",AccIndex);PrintCmd(s);
				Rprintf(Accu[AccIndex].value);PrintCmd("+i*");Rprintf(Accu[AccIndex].cmplx);}  
			}else PrintCmd("Error Accu, Missing value to load!\n");
		}}

	//handle M n°(n,p)= ...   Matrix and  M n°= ... M
	if (CodeOfOneLine[OffsetLine].code==15 ) {//MAccu detected
		NumM = (int)CodeOfOneLine[OffsetLine].value;
		//sprintf(s,"NumM = %d\n",NumM); PrintCmd(s);
		//for (i=0; i<7;i++) {sprintf(s,"%d  [%d][%d]\n",i,(int)CodeOfOneLine[OffsetLine+i].code,(int)CodeOfOneLine[OffsetLine+i].value);PrintCmd(s);}
		if (CodeOfOneLine[OffsetLine+1].code == 6 ){ 
				if (CodeOfOneLine[OffsetLine+2].code == 1 && 
					CodeOfOneLine[OffsetLine+3].code == 10 && 
						CodeOfOneLine[OffsetLine+4].code == 1 && 
							CodeOfOneLine[OffsetLine+5].code == 7 &&
								CodeOfOneLine[OffsetLine+6].code == 8 ){ 
				if (CodeOfOneLine[OffsetLine+7].code == 1){
					Mn= (int)CodeOfOneLine[OffsetLine+2].value;
					Mp= (int)CodeOfOneLine[OffsetLine+4].value;
					if (Mn<1 || Mn>MAccu[NumM].n) {PrintCmd("index Matrix Out of range");Error=6; goto EndMain;}
					if (Mp<1 || Mp>MAccu[NumM].p) {PrintCmd("index Matrix Out of range");Error=6; goto EndMain;}
					if (MAccu[NumM].ptr !=0){ MAccu[NumM].ptr[(Mn-1)*MAccu[NumM].p+Mp-1]=CodeOfOneLine[OffsetLine+7].value;}
						else {PrintCmd("Matrix not defined!\n");Error = 6; goto EndMain;}
			if (debug >0 ) {
				sprintf(s,"M%d (%d,%d)",NumM,Mn,Mp);PrintCmd(s);}
			}else PrintCmd("Error Accu, Missing value to load!\n");
			}else PrintCmd("Error Matrix definition !\n");
		}else{
				//treat the case of Mn°= .... M
					if (CodeOfOneLine[OffsetLine+1].code == 8 && 
						CodeOfOneLine[OffsetLine+2].code == 15){
						int NumM2= (int) CodeOfOneLine[OffsetLine+2].value;
						if (MAccu[NumM].ptr !=0){if (MAccu[NumM2].n != MAccu[NumM].n ||
								MAccu[NumM2].p != MAccu[NumM].p){
									PrintCmd("Matrices incompatible sizes!\n");
									Error = 6;goto EndMain;}
						}else{ 
								//allouer une nouvelle MatriceM pour ainsi la definir  
								//create matrix
								MAccu[NumM].n=MAccu[NumM2].n;MAccu[NumM].p=MAccu[NumM2].p;
								MAccu[NumM].ptr = (float*)malloc(MAccu[NumM].n*MAccu[NumM].p*sizeof(float) );
								if (MAccu[NumM].ptr == 0) {Error = 1; PrintCmd("Error Allocation for Matrix!\n");goto EndMain;}
								}
						int j;
						for (j=0;j<MAccu[NumM].n*MAccu[NumM].p;j++)
							MAccu[NumM].ptr[j]=MAccu[NumM2].ptr[j]; 
					}else{
							PrintCmd("Should be Matrix =Matrix\n not equal to scalar or else !\n");
							Error =6;
							goto EndMain;
							}
				}
	}
	
	//Handle Instructions
	if (CodeOfOneLine[OffsetLine].code!=11 ) goto OutInstructionHere;//Special Instruction  detected


	if (CodeOfOneLine[OffsetLine].code==11&&CodeOfOneLine[OffsetLine].value==1) {//Print 
		if (debug > 0 ) PrintCmd("print\n");

				if (CodeOfOneLine[OffsetLine+1].code!=1) {
					PrintCmd("Error no Nbr to Display\n ");
					sprintf(s,"The code %d is [%d][%f]\n",OffsetLine,CodeOfOneLine[OffsetLine].code,CodeOfOneLine[OffsetLine].value);
					PrintCmd(s);
					sprintf(s,"ptr code   %d is [%d][%f]\n",OffsetLine+1,CodeOfOneLine[OffsetLine+1].code,CodeOfOneLine[OffsetLine+1].value); 
					PrintCmd(s);
					goto EndMain;
				}
				
			int TextIndex=0;
			if (CodeOfOneLine[OffsetLine+2].code==10 &&
						CodeOfOneLine[OffsetLine+3].code==1){
						TextIndex=(int)CodeOfOneLine[OffsetLine+3].value;
						}
			if (TextIndex != 0){
			i=0;
				while (WholeMnemoProg[TextIndex+i]!='"'){
							s[i]=WholeMnemoProg[TextIndex+i];i++;}
				s[i]=0; //terminate the ascii chain
				PrintCmd(s);
			}
			//print number here
			if (CodeOfOneLine[OffsetLine+1].code==1) {
			if (AllowComplexe != 1 || CodeOfOneLine[OffsetLine+1].cmplx ==0)Rprintf(CodeOfOneLine[OffsetLine+1].value);//implementation for display float
			else RCPrintf(CodeOfOneLine[OffsetLine+1]);//print complexe number
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
			WinEraseRectangleReg(DrawZoneX+1,DrawZoneY,DrawZoneW,DrawZoneH);//Define the erasing rectangle dimensions	
			StepX=CodeOfOneLine[OffsetLine+1].value;
			TracerAxis(DrawZoneX+DrawZoneW/2,DrawZoneY+DrawZoneH/2,DrawZoneW, DrawZoneH);
			ColorGraph=1; //Set Color Graph to init
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
					PrintCmd("Error Syntaxe Line:\n");
					goto EndMain;
				}
		}
		
		
//-------------
		if (CodeOfOneLine[OffsetLine].code==11&&CodeOfOneLine[OffsetLine].value==11) {//box3d x,y,z,d
				if (debug > 0 ) PrintCmd("box3d\n");
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
								zp=Zmax3d/4*zp0;
								yp=Ymax3d/4*yp0;
								xp=Xmax3d/4*xp0;
								OkLine=1;
							}
						}
					}
				}
				if (OkLine!=1) {PrintCmd("Error Syntaxe box3d:\n");goto EndMain;}
		}
		
//-------------
		if (CodeOfOneLine[OffsetLine].code==11&&CodeOfOneLine[OffsetLine].value==12) {//getserial
			if (CodeOfOneLine[OffsetLine+1].code==1&& CodeOfOneLine[OffsetLine+2].code==10 &&
			CodeOfOneLine[OffsetLine+3].code==1) {
			BaudRate=CodeOfOneLine[OffsetLine+1].value;
			SerFlag=CodeOfOneLine[OffsetLine+3].value;
			GetOscilloData(0);//0=quiet
			OffsetLine=0;
			}else{PrintCmd("getserial!\n");goto EndMain;}
		}	
//-------------
		if (CodeOfOneLine[OffsetLine].code==11&&CodeOfOneLine[OffsetLine].value==16) {//putserial
			if (CodeOfOneLine[OffsetLine+1].code==1&& CodeOfOneLine[OffsetLine+2].code==10 &&
			CodeOfOneLine[OffsetLine+3].code==1&& CodeOfOneLine[OffsetLine+4].code==10 &&
			CodeOfOneLine[OffsetLine+5].code==1) {
			BaudRate=CodeOfOneLine[OffsetLine+1].value;
			SerFlag=CodeOfOneLine[OffsetLine+3].value;
			SerialDataToSend[0]=CodeOfOneLine[OffsetLine+5].value;
			PutSerialData();//0=quiet
			OffsetLine=0;
			}else{PrintCmd("putserial!\n");goto EndMain;}
		}	
//-------------
		if (CodeOfOneLine[OffsetLine].code==11&&CodeOfOneLine[OffsetLine].value==17) {//playsound
			if (CodeOfOneLine[OffsetLine+1].code==1&& CodeOfOneLine[OffsetLine+2].code==10 &&
			CodeOfOneLine[OffsetLine+3].code==1&& CodeOfOneLine[OffsetLine+4].code==10 &&
			CodeOfOneLine[OffsetLine+5].code==1) {
			SoundFrequency=CodeOfOneLine[OffsetLine+1].value;
			SoundAmplitude=CodeOfOneLine[OffsetLine+3].value;
			SoundDuration=CodeOfOneLine[OffsetLine+5].value;
			PlaySoundReg(SoundFrequency,SoundAmplitude,SoundDuration);

			OffsetLine=0;
			}else{PrintCmd("playsound!\n");goto EndMain;}
		}	
//-------------
		if (CodeOfOneLine[OffsetLine].code==11&&CodeOfOneLine[OffsetLine].value==20) {//playsndM
			if (CodeOfOneLine[OffsetLine+1].code==1&& CodeOfOneLine[OffsetLine+2].code==10 &&
			CodeOfOneLine[OffsetLine+3].code==1) {
			NumM=(int)CodeOfOneLine[OffsetLine+1].value;//n°Matrix MAccu to play
			SamplesPerSecond=CodeOfOneLine[OffsetLine+3].value;
			PlaySoundMatrix(MAccu, NumM,SamplesPerSecond);

			OffsetLine=0;
			}else{PrintCmd("playsndM!\n");goto EndMain;}
		}	
//------------
		if (CodeOfOneLine[OffsetLine].code==11&&CodeOfOneLine[OffsetLine].value==22) {//recsndM
			if (CodeOfOneLine[OffsetLine+1].code==1&& CodeOfOneLine[OffsetLine+2].code==10 &&
			CodeOfOneLine[OffsetLine+3].code==1) {
			NumM=(int)CodeOfOneLine[OffsetLine+1].value;//n°Matrix MAccu to play
			SamplesPerSecond=CodeOfOneLine[OffsetLine+3].value;
			if (NumM<0 || NumM > NbrMaxMatrix/2){sprintf(s,"unauthorized number for Matrix\n 0<Mtx n°<%d\n",NbrMaxMatrix/2); PrintCmd(s);goto EndMain;}

			GetAudioMicro(MAccu, NumM,SamplesPerSecond);

			OffsetLine=0;
			}else{PrintCmd("recsndM!\n");goto EndMain;}
		}	
//------------
		if (CodeOfOneLine[OffsetLine].code==11&&CodeOfOneLine[OffsetLine].value==23) {//loadsndM
			if ( CodeOfOneLine[OffsetLine+1].code==1){
			NumM=(int)CodeOfOneLine[OffsetLine+1].value;//n°Matrix MAccu to play
			if (NumM<0 || NumM > NbrMaxMatrix/2){sprintf(s,"unauthorized number for Matrix\n 0<Mtx n°<%d\n",NbrMaxMatrix/2); PrintCmd(s);goto EndMain;}
			char sndfilename[50];
			int TextIndex=0,i=0;
			if (CodeOfOneLine[OffsetLine+2].code==10 &&
						CodeOfOneLine[OffsetLine+3].code==1){
						TextIndex=(int)CodeOfOneLine[OffsetLine+3].value;
						//Rprintf((float)TextIndex);
						}
			if (TextIndex != 0){
				while (WholeMnemoProg[TextIndex+i]!='"'){
							sndfilename[i]=WholeMnemoProg[TextIndex+i];i++;}
				sndfilename[i]=0; //terminate the ascii chain
				//PrintCmd(sndfilename);
				if (LoadSoundFile(MAccu, NumM,sndfilename)==FALSE){
						Error=8; goto EndMain;}
			}else{
				if (LoadSoundFile(MAccu, NumM,DoLoadSound() )==FALSE){
						Error=8; goto EndMain;}
			}

			OffsetLine=0;
			}else{PrintCmd("loadsndM!\n");goto EndMain;}
		}	
//------------
		if (CodeOfOneLine[OffsetLine].code==11&&CodeOfOneLine[OffsetLine].value==18) {//clscmd
			DeleteCmd();
			OffsetLine=0;
			}
//------------
		if (CodeOfOneLine[OffsetLine].code==11&&CodeOfOneLine[OffsetLine].value==19) {//defM
			if (CodeOfOneLine[OffsetLine+1].code==1&& CodeOfOneLine[OffsetLine+2].code==10 &&
			CodeOfOneLine[OffsetLine+3].code==1&& CodeOfOneLine[OffsetLine+4].code==10 &&
			CodeOfOneLine[OffsetLine+5].code==1) {
			NumM=(int)CodeOfOneLine[OffsetLine+1].value;
			MAccu[NumM].n=(int)CodeOfOneLine[OffsetLine+3].value;
			MAccu[NumM].p=(int)CodeOfOneLine[OffsetLine+5].value;
			//create matrix
			MAccu[NumM].ptr = (float*)malloc(MAccu[NumM].n*MAccu[NumM].p*sizeof(float) );
			if (MAccu[NumM].ptr == 0) {Error = 1; PrintCmd("Error Allocation for Matrix!\n");goto EndMain;}
			OffsetLine=0;
			}else{PrintCmd("Matrix!\n");goto EndMain;}
		}	
		
//------------
		if (CodeOfOneLine[OffsetLine].code==11&&CodeOfOneLine[OffsetLine].value==21) {//fillM
			if (CodeOfOneLine[OffsetLine+1].code==1&& CodeOfOneLine[OffsetLine+2].code==10 &&
			CodeOfOneLine[OffsetLine+3].code==1&& CodeOfOneLine[OffsetLine+4].code==10 &&
			CodeOfOneLine[OffsetLine+5].code==1) {
			NumM = (int)CodeOfOneLine[OffsetLine+1].value;
			float strtM = CodeOfOneLine[OffsetLine+3].value;
			float endM = CodeOfOneLine[OffsetLine+5].value;
			int sizeM=MAccu[NumM].n*MAccu[NumM].p;
			//fill matrix
			if (MAccu[NumM].ptr == 0) {Error = 1; PrintCmd("Can't fill Matrix, not defined!\n");goto EndMain;}
			for (i=0;i<sizeM;i++) MAccu[NumM].ptr[i]= strtM+i*(endM-strtM)/(sizeM-1);
			OffsetLine=0;
			}else{PrintCmd("Fill Matrix!\n");goto EndMain;}
		}	
		
//------------
		if (CodeOfOneLine[OffsetLine].code==11&&CodeOfOneLine[OffsetLine].value==14) {//bsr 
			if (debug > 0 ) {PrintCmd("bsr \n");}
				if (CodeOfOneLine[OffsetLine+1].code==12) {//the label
				i = CodeOfOneLine[OffsetLine+1].value; //save offset to go
				//sprintf(s,"bsr = %d\n",i);PrintCmd(s);
				//sprintf(s,"save  = %d\n",CodeListOffset);PrintCmd(s);
				//remark:We just save CodeListOffset because it was changed above to the next line
				SubRoutineStack[PointerSubRoutine] = CodeListOffset;//we save the following offset Line
				PointerSubRoutine++;
				CodeListOffset = i; //retrieive offset to go 
				if (PointerSubRoutine >= MaxSubRoutine){PrintCmd("Max Subroutines reached!\n");goto EndMain;}
				OffsetLine=0;
				goto OutInstructionHere;//gosubroutine to the end of the checks for instructions
				}
				else {
					PrintCmd("bsr has no label !\n"); goto EndMain;
				}
		}
//-------------
		if (CodeOfOneLine[OffsetLine].code==11&&CodeOfOneLine[OffsetLine].value==15) {//rts
			if (debug > 0 ) {PrintCmd("rts \n");}
			if (PointerSubRoutine >0){
				PointerSubRoutine = PointerSubRoutine-1;
				CodeListOffset = SubRoutineStack[PointerSubRoutine];
				//sprintf(s,"Retreive CodeListOffset= %d\n",CodeListOffset);PrintCmd(s);
				OffsetLine=0;
				goto OutInstructionHere;//back from subroutine
			}else {PrintCmd("rts => end !\n"); goto EndMain;}
		}
//-------------

		if (CodeOfOneLine[OffsetLine].code==11&&CodeOfOneLine[OffsetLine].value==13) {//wait
			if (CodeOfOneLine[OffsetLine+1].code==1) {
			Wait((int)CodeOfOneLine[OffsetLine+1].value);//wait the amount of time
			OffsetLine=0;
			}else{PrintCmd("wait!\n");}
		}
//-------------
		if (CodeOfOneLine[OffsetLine].code==11&&CodeOfOneLine[OffsetLine].value==9) {//gfxdim x1,x2,y1,y2
				if (debug > 0 ) PrintCmd("gfxdim\n");
				OkLine=0;
				if (CodeOfOneLine[OffsetLine+1].code==1 && CodeOfOneLine[OffsetLine+2].code==10) {
	 				if (CodeOfOneLine[OffsetLine+3].code==1 && CodeOfOneLine[OffsetLine+4].code==10) {
						if (CodeOfOneLine[OffsetLine+5].code==1 && CodeOfOneLine[OffsetLine+6].code==10) {
							if (CodeOfOneLine[OffsetLine+7].code==1 && CodeOfOneLine[OffsetLine+8].code==10) {
							if (CodeOfOneLine[OffsetLine+9].code==1 ){
							if(RedrawingGfx==1) goto NoRedefinition;//Zooming or moving with pen 
								DimXmin=CodeOfOneLine[OffsetLine+1].value;
								DimXmax=CodeOfOneLine[OffsetLine+3].value;
								DimYmin=CodeOfOneLine[OffsetLine+5].value;
								DimYmax=CodeOfOneLine[OffsetLine+7].value;
								IncX=CodeOfOneLine[OffsetLine+9].value;
							NoRedefinition:
								RedrawingGfx=0; //Reset to 0 after that
								OkLine=1;
							}}
						}
					}
				}
				if (OkLine!=1) {PrintCmd("Error Syntaxe gfxdim:\n");goto EndMain;}
		}
//---------------
	OutInstructionHere:// ----- here is the end of the check for instructions

	AlmostEndLoop:
		OffsetLine=0; //Important before going back from here to LoopCodeProgram	
		if  (CodeList[CodeListOffset].code!=0 && CodeListOffset<CodeListOffsetMax ) goto LoopCodeProgram;
	//-------------------------------------------------------EndLoop---------------------------------
	if (debug > 0 )PrintCmd("End\n");
EndMain:
	if (Error!=0) IndicError(CounterLineCode);
	return (0);
 }

 static IndicError(int LineErrorCode){
/*PalmOs
	MemPtr  progtext;    
	FieldPtr FieldProgTextPtr;
	FormPtr Frm = FrmGetFormPtr(frmadc16);
	int i,k,is;
	i=0;k=0;
	sprintf (s,"Line %d\n",LineErrorCode);PrintCmd(s);
	if (LineErrorCode < 1) goto HighLight ;
	FieldProgTextPtr=(FieldPtr)(FrmGetObjectPtr(Frm, (FrmGetObjectIndex(Frm, fld_prog))));
	progtext = FldGetTextPtr(FieldProgTextPtr); //return the ptr to a the lock memory string of the fld_prog
	StrCopy (WholeMnemoProg,progtext);
	while (i<strlen(WholeMnemoProg)){
			if (WholeMnemoProg[i] == 0x0A ) k++;
			if (WholeMnemoProg[i] == 0x0D ) k++;
			i++;
			if (k==(LineErrorCode-2)) {is=i;}  //position at the line of error detected
			if (k==(LineErrorCode-1)) {SetUpTextProg(is); goto HighLight;}
		}
	HighLight:
		FldSetSelection (FieldProgTextPtr, (UInt16) i, (UInt16) i+1);
	*/
	printf("Error indication to be implemented\n");
}
 
 static int TraceFunctionOneVariable(floactet *CodeOfOneLine, floactet *CodeList)
 {
	int N_AccuX;
	float x1,y1,x2,y2,color;
	float X0,X1,Y0,Y1,Y,Ym; //Ym is used for derivation
	int FunctionStart,Error;
			N_AccuX=(int)CodeOfOneLine[2].value;
			CodeListOffset=CodeListOffset+5;
			FillCodeOfOneLine(CodeList,CodeOfOneLine);//transfert one line 
			X0=Accu[N_AccuX].value;
			if (X0<DimXmin || X0>=DimXmax) X0=DimXmin;
			FunctionStart=0;
			if (GridSet == 0){
			WinEraseRectangleReg(DrawZoneX,DrawZoneY,DrawZoneW,DrawZoneH);
			printf("TraceFunctionOneVariable rectangle to be set!\n");
			TracerAxis(DrawZoneX+DrawZoneW/2,DrawZoneY+DrawZoneH/2,DrawZoneW, DrawZoneH);
			}
			
		LoopDrawFunction:
				ReplaceAccuByValue(CodeOfOneLine);

				Error = ReplaceMAccuByValue(CodeOfOneLine); //get matrix values
				if (Error !=0) {sprintf (s,"While replacing Mtx n°(n,p) by Values Error= %d \n", Error);PrintCmd(s);goto EndFunctionFX;}

				Error = TreatParenthese(CodeOfOneLine);													//parenthese
				if (Error != 0) {sprintf (s,"Parenthese Error = %d \n", Error);PrintCmd(s);goto EndFunctionFX;}
				if(StopProgram == 1) goto EndFunctionFX;
				Error = CalculOneLine(CodeOfOneLine); //Calcul
				if(StopProgram == 1) goto EndFunctionFX;
				if (Error != 0) {sprintf (s,"Error = %d \n", Error);PrintCmd(s);goto EndFunctionFX;}
				if (CodeOfOneLine[0].code != 1) {PrintCmd("Error finishing function line\n");goto EndFunctionFX;}
				Y=CodeOfOneLine[0].value;
				FillCodeOfOneLine(CodeList,CodeOfOneLine);//transfert one line 
				if (FunctionStart==0) {Ym=Y0;Y0=Y; X1=X0+IncX;Accu[N_AccuX].value=X1; Accu[N_AccuX].cmplx=0;FunctionStart=1;}
				else{Y1=Y;
					if (Y0>DimYmin && Y0 < DimYmax) {
					Line(X0,Y0,X1,Y1,ColorGraph);
					if (GfxDerivate==1) {
						Line (X0,(Y0-Ym)/IncX,X1,(Y1-Y0)/IncX,ColorGraph+3);
						Ym=Y0;
						}
					}
					X0=X1;Y0=Y1;X1=X0+IncX;Accu[N_AccuX].value=X1;Accu[N_AccuX].cmplx=0;}
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
	typedef struct BandDrawn{
		float max;
		float min;
		}BandDrawn;
	//MemHandle hb;
	BandDrawn *band;
	int a,b,st,N,i,init;

		N=DrawZoneW/2;//(DimXmax-DimXmin)/Inc3D;
		/*PalmOs
		hb = MemHandleNew (N*sizeof(struct BandDrawn) );
		if (hb !=0) band=MemHandleLock(hb);
		else {PrintCmd("BandDrawn Allocation failed\n");goto OutFunction;}
		*/
		//win32 ->
		band = (BandDrawn*)malloc(N*sizeof(struct BandDrawn) );
		if (band ==0) {PrintCmd("BandDrawn Allocation failed\n");goto OutFunction;}

		init =0;
		dx=Inc3D;
			st=0;
			dy=dx;
			N_AccuX=(int)CodeOfOneLine[2].value;
			N_AccuY=(int)CodeOfOneLine[4].value;
			CodeListOffset=CodeListOffset+7;
			FillCodeOfOneLine(CodeList,CodeOfOneLine);//transfert one line 
			x=Accu[N_AccuX].value;
			y=Accu[N_AccuY].value;
			if (X0<Xmin3d || X0>Xmax3d) X0=Xmax3d; //box3d
			if (Y0<Ymin3d || Y0>Ymax3d) Y0=Ymin3d;
			FunctionStart=0;
			if (GridSet == 0){
			//RctSetRectangle(rP,DrawZoneX,DrawZoneY,DrawZoneW,DrawZoneH);
			//WinEraseRectangle(rP,0);
			WinEraseRectangleReg(DrawZoneX,DrawZoneY,DrawZoneW,DrawZoneH);
			Tracer3DAxis();
			}
			
		LoopDrawFunction:
				
				ReplaceAccuByValue(CodeOfOneLine);

				Error = ReplaceMAccuByValue(CodeOfOneLine); //get matrix values
				if (Error !=0) {sprintf (s,"While replacing Mtx n°(n,p) by Values Error= %d \n", Error);PrintCmd(s);goto EndFunctionFX;}

				Error = TreatParenthese(CodeOfOneLine);													//parenthese
				if (Error != 0) {sprintf (s,"Parenthese Error = %d \n", Error);PrintCmd(s);goto EndFunctionFX;}
				if(StopProgram == 1) goto EndFunctionFX;
				Error = CalculOneLine(CodeOfOneLine); //Calcul
				if(StopProgram == 1) goto EndFunctionFX;
				if (Error != 0) {sprintf (s,"Error = %d \n", Error);PrintCmd(s);goto EndFunctionFX;}
				if (CodeOfOneLine[0].code != 1) {PrintCmd("Error finishing function line\n");goto EndFunctionFX;}
				z=CodeOfOneLine[0].value;
				FillCodeOfOneLine(CodeList,CodeOfOneLine);//transfert one line 


				if (FunctionStart==0) {
					X0=y-x*(xp-y)/zp;
					Y0=z-x*(yp-z)/zp;
					FunctionStart=1;
					if (init==0)for (i=0;i<N;i++) {band[i].min =Y0;band[i].max =Y0;}
				}else{
					X1=y-x*(xp-y)/zp;
					Y1=z-x*(yp-z)/zp;
					if (Y0>DimYmin && Y0 < DimYmax && Y1>DimYmin && Y1 < DimYmax) 
						if (X0>DimXmin && X0 < DimXmax && X1>DimXmin && X1 < DimXmax) {

							a=(int)( (X1-DimXmin)*N /(DimXmax-DimXmin) );
							b=(int)( (X1+Inc3D-DimXmin)*N /(DimXmax-DimXmin) );
							if (init==0){Line(X0,Y0,X1,Y1,ColorGraph);
								for (i=a; i <= b; i++){band[i].min =Y0; band[i].max =Y0;}
							}else{
							if ( band[a].min>= Y1 || Y1>=band[a].max ){//outside band
							 Line(X0,Y0,X1,Y1,ColorGraph);
							if (band[a].min >Y1) band[a].min=Y1;
							if (band[a].max <Y1) band[a].max=Y1;
							//Line(X1,band[a].min,X1,band[a].max,ColorGraph+2);
							}else {
									if ( band[a].min>= Y0){
									Line( (band[a].min*(X1-X0)-Y0*X1+X0*Y1)/(Y1-Y0),band[a].min,X0,Y0,ColorGraph);
									}
									if ( band[a].max<= Y0){
									Line( (band[a].max*(X1-X0)-Y0*X1+X0*Y1)/(Y1-Y0),band[a].max,X0,Y0,ColorGraph);
									}
							}
						}
						X0=X1;Y0=Y1;
						}}
				y=y+dy;Accu[N_AccuY].value=y;
				if (y < Ymax3d) goto LoopDrawFunction;
				init=1; //first line done
				y=Ymin3d;x=x-dx;Accu[N_AccuX].value=x;Accu[N_AccuY].value=y; FunctionStart =0; 
				if (x > Xmin3d) goto LoopDrawFunction;

		EndFunctionFX:
			free(band);
		OutFunction:
			return Error;
}

 
 static int CheckLabelDef(char *InstructionLine,int Size){
	int i=0;
	while (i<Size && InstructionLine[i] != 0x0A){
		//printf("InstructionLine[%d]=%c\n",i,InstructionLine[i]);
		if (InstructionLine[i] == Octet(":") ) {
			if (debug >0) {sprintf (s,"lbl:  %d letters\n",i);PrintCmd(s);} 
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
		if (MnemoListLine[i]==Octet("\n") )  {Out=1; 
		CodeList[CodeListOffset].code = 0xFF; CodeList[CodeListOffset].value = 0; CodeList[CodeListOffset].cmplx = 0;
		CodeListOffset++; CodeListOffsetMax=CodeListOffset;
		goto EndConvertNoData;} //endLine coding

	StartConvList:

		//--- Search for instructions --------
	InstructionNumber = 0; // Set to 0. If no more instruction takes -1 value.
	while(InstructionNumber !=-1){
		Iindex=i;
		//printf("MnemoListLine[%d]=%c\n",i,MnemoListLine[i]);
		InstructionNumber=HandleInstructions(MnemoListLine,Iindex);
		i=Iindex;
		if (debug > 0) {sprintf(s,"Instruction Nbr = %d \n",InstructionNumber); PrintCmd(s);}
		if (InstructionNumber !=-1) {CodeList[CodeListOffset].code=11;	//General Code Instruction
		CodeList[CodeListOffset].value=InstructionNumber;
		CodeList[CodeListOffset].cmplx=0;
		CodeListOffset++;
		}
	}
	
	//--- check label ---
		LabelNbrFound=TestForLabels(MnemoListLine,i);
		if (LabelNbrFound != -1 ) {
			if (debug >1) {sprintf(s,"label [%d] \n",LabelNbrFound);PrintCmd(s);}
			i=Iindex;
			CodeList[CodeListOffset].code=12; //Code for Labels
			CodeList[CodeListOffset].value=LabelNbrFound ;// Later put Labels[LabelNbrFound].n;
			CodeList[CodeListOffset].cmplx=0 ;
			CodeListOffset++;
			//if (debug >1) printf("pointer on character [%c]\n",MnemoListLine[i]);
			}	
		
		if (MnemoListLine[i]==Octet("-")&& BackFromAccu==0 ){opposite=1; i++; }//sign for the number 
	ComeBackFromAccu:
		BackFromAccu=0;
	while (Out==0){
		if (CodeListOffset > CodeListSize-10) {PrintCmd("Exceeding Memory size for Codes --- contact Regan B.S. by email for info");return 5;} 
		if (MnemoListLine[i]=='"' )  {//PrintCmd("text detected\n");
			CodeList[CodeListOffset].code = 1;
			CodeList[CodeListOffset].value = (float)(offsetI+i+1);//OffsetI is the current position into WholeMnemoProg
			while (MnemoListLine[i]!=0 || MnemoListLine[i] != '"' || MnemoListLine[i]!=Octet("\n")){
				if (MnemoListLine[i] != '"') {		
					CodeListOffset++;				
					CodeListOffsetMax=CodeListOffset;
					goto EndConvert; //this line is finished
					}
				i++;
			}
			PrintCmd("Finish your text by guillemet \n");
			ErrorCode=1;
			goto EndConvert;
		} //set Text position "blabla"
		if (MnemoListLine[i]==Octet("\n") )  {Out=1; goto EndNbr;} //endLine or No data
		if (MnemoListLine[i]==OperatorList[4] ) { //"("
			if (opposite == 1) {
			CodeList[CodeListOffset].code = 1;
			CodeList[CodeListOffset].value = 0;
			CodeList[CodeListOffset].cmplx = 0;
			CodeList[CodeListOffset+1].code = 3;
			CodeList[CodeListOffset+1].value = 0;
			CodeList[CodeListOffset+1].cmplx = 0;
			CodeListOffset=CodeListOffset+2; opposite=0;} //we've put 0-(
			i++; ///parenthese left is special, there can be a sign behind like in (-1+2)
			CodeList[CodeListOffset].code = 6; 
			CodeList[CodeListOffset].value = 0; 
			CodeList[CodeListOffset].cmplx = 0;
		
			if (debug > 0) PrintCmd("Parenthese on\n");
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
			if (debug > 0) PrintCmd("Spécial Trf(x) \n");
			CodeList[CodeListOffset].code=11;	//Change it to instruction for spécial use
			CodeList[CodeListOffset].value=-1; 	//Specific coding for display function f(x)= ...
			CodeList[CodeListOffset].cmplx=0;
			CodeListOffset++;
			goto StartConvList;
			//the function f(x)= is actually using the job of the math function for coding it becomes an instruction with coding [11] [-1]
		}//end coding search function

		if (FunctionNumber !=0) {
			if (opposite == 1) {
			CodeList[CodeListOffset].code = 1;
			CodeList[CodeListOffset].value = 0;
			CodeList[CodeListOffset].cmplx = 0;
			CodeList[CodeListOffset+1].code = 3;
			CodeList[CodeListOffset+1].value = 0;
			CodeList[CodeListOffset+1].cmplx = 0;
			CodeListOffset=CodeListOffset+2; opposite=0;} //we've put 0-function
			if (debug > 0) PrintCmd("Function coded\n");
			CodeList[CodeListOffset].code=13;	//General Code Math Instruction
			CodeList[CodeListOffset].value=FunctionNumber; 	//Specific code (example: exp=1, ln =2,...)
			CodeList[CodeListOffset].cmplx=0;
			CodeListOffset++;
			goto StartConvList;
		}//end coding search function

		if (MnemoListLine[i]==Octet("^") )  {
			if (debug > 0 ) PrintCmd("found ^");
				CodeList[CodeListOffset].code = 13; //It is a function
				CodeList[CodeListOffset].value = 9;  // Coding for "^"
				CodeList[CodeListOffset].cmplx = 0;
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
					if (opposite == 1) {
					CodeList[CodeListOffset].code = 1;CodeList[CodeListOffset].value = 0;CodeList[CodeListOffset].cmplx = 0;
					CodeList[CodeListOffset+1].code = 3;CodeList[CodeListOffset+1].value = 0;CodeList[CodeListOffset+1].cmplx = 0;
					CodeListOffset=CodeListOffset+2; opposite=0;} //we've put 0-Accu
					CodeList[CodeListOffset].code = 14; //code of the FAccu
					CodeList[CodeListOffset].value = Nbr;
					CodeList[CodeListOffset].cmplx = 0;
					//Recording the place in FAccu[Nbr]
					if ((int)Nbr < NbrMaxFAccu) FAccu[(int)Nbr]=CodeListOffset;//integer
					if (Nbr>NbrMaxFAccu) {PrintCmd("FAccu Nbr too big\n");ErrorCode=4; goto EndConvert;} 
					if (debug > 0) {sprintf(s,"FAccu F%d at place %d\n",(int) Nbr,CodeListOffset);PrintCmd(s);}
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
					CodeList[CodeListOffset].cmplx = 0; 
					if (Nbr>NbrMaxAccu) {PrintCmd("Accu number too big\n");ErrorCode=4; goto EndConvert;} 
					CodeListOffset++;
					if (debug > 0) {sprintf(s,"Accu A%d \n",(int) Nbr);PrintCmd(s);}
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

		//detect MAccu
		if (MnemoListLine[i]=='M' ) {//detect Matrix 
			i++; //get the number now
			val = MnemoListLine[i]-Octet("0");
			if (val<0 && val>9)  {ErrorCode=2; goto EndConvert;}//missing number behind Accu
			Nbr=0;
			HandleNbrMAccu:
				val= MnemoListLine[i]-Octet("0");
				if (val >= 0 && val <=9) Nbr = Codage*Nbr + val ; 
				else {
					if (opposite == 1) {CodeList[CodeListOffset].code = 1;CodeList[CodeListOffset].value = 0;
					CodeList[CodeListOffset+1].code = 3;CodeList[CodeListOffset+1].value = 0;
					CodeListOffset=CodeListOffset+2; opposite=0;} //we've put 0-Accu
					CodeList[CodeListOffset].code = 15; //code of the MAccu
					CodeList[CodeListOffset].value = Nbr; 
					CodeList[CodeListOffset].cmplx = 0; 
					if (Nbr>NbrMaxMatrix) {PrintCmd("Matrix Number too big! 50 Max\n");ErrorCode=4; goto EndConvert;} 
					CodeListOffset++;
					if (debug > 0) {sprintf(s,"Accu A%d \n",(int) Nbr);PrintCmd(s);}
					Nbr=0;// Reinitialise  Nbr for next value
					goto EndNbrMAccu;// i points on the operator
				}
				i++;
				goto HandleNbrMAccu;
			EndNbrMAccu:				
				if (MnemoListLine[i]==Octet("\n") )  {//Check Endline
				Out=1; 
				CodeList[CodeListOffset].code = 0xFF;       // EndLine coding
				CodeList[CodeListOffset].value = 0; 
				CodeListOffset++;
				CodeListOffsetMax=CodeListOffset;
				}
				BackFromAccu=1;
				goto StartConvList; //Return in the loop
			}//End Coding MAccu


			if (MnemoListLine[i] == ')') {
			CodeList[CodeListOffset].code = 7;       // +2  so that "+"=2
			CodeList[CodeListOffset].value = 0; 			// No values for operators
			CodeList[CodeListOffset].cmplx = 0; 			// No values for operators
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
			CodeList[CodeListOffset].cmplx = 0; 			// No values for operators
			CodeListOffset++;
			i++; // then i points next number
			CodeListOffsetMax=CodeListOffset;
			goto StartConvList;
			}
		}
			
			
		// i Complexe handling here
		if (AllowComplexe == 1 && MnemoListLine[i]=='i'){
			CodeList[CodeListOffset].code = 1;       // It is a number: code=0
			CodeList[CodeListOffset].value = 0; 
			CodeList[CodeListOffset].cmplx = 1;// The initial point of complexe numbers! 			
			CodeListOffset++;i++;
			CodeListOffsetMax=CodeListOffset;
			goto StartConvList;
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
		if (debug>0) {sprintf(s,"StringNbr= [%s], Nbr = %d\n",StringNbr,(int)Nbr);PrintCmd(s);}
		CodeList[CodeListOffset].code = 1;       // It is a number: code=0
		CodeList[CodeListOffset].value = Nbr; // Floating point Value is set
		CodeList[CodeListOffset].cmplx = 0; // Floating point Value is set
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
			CodeList[CodeListOffset].cmplx = 0; 
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
			if (debug > 0 ) PrintCmd("found ^");
				CodeList[CodeListOffset].code = 13; //It is a function
				CodeList[CodeListOffset].value = 9;  // Coding for "^"
				CodeList[CodeListOffset].cmplx = 0;  // Coding for "^"
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
			CodeList[CodeListOffset].cmplx = 0; 			// No values for operators
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
			if (debug >0 ){sprintf(s,"Instruction ? %c\n",MnemoListLine[i+k]);PrintCmd(s);}

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
	if (Ok == 0) NInstr=-1; //If none instruction found then send 0 back 
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
			if (debug > 0 ){sprintf(s,"Math Func ? %c\n",MnemoListLine[i+k]);PrintCmd(s);}

				if (MnemoListLine[i+k] != MathFunctions[j+k]) goto NotThisFunction;
				if (k>LengthInstructionMax) goto NotThisFunction;
				if (MathFunctions[j+k+1]== Octet("_") ) {
					Ok=1; Iindex=i+k+1;NInstr++;//NInstr is increased 'cause we start at 1 the list (exp=1)
					if (debug > 0 ){sprintf(s,"MathFunction Nbr %d\n",NInstr);PrintCmd(s);}
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
	int i=0,j;
StartFill:
	if (CodeList[CodeListOffset+i].code==15 &&CodeList[CodeListOffset+i+1].code!=6){
	//Now we empty the temporary Matrices
	//free temporary matrix memories<=> NumM aboves NbrMaxMatrix/2
	for (j=NbrMaxMatrix/2;j<NbrMaxMatrix;j++) if (MAccu[j].ptr!=0) {free (MAccu[j].ptr);MAccu[j].ptr=0;}
	}

		CodeOfOneLine[i]=CodeList[CodeListOffset+i];
		i++;
	if (CodeList[CodeListOffset+i].code==0xFF ) goto EndFill;
	if ((CodeListOffset+i)>CodeListOffsetMax) goto EndFill;
	if (CodeList[CodeListOffset+i].code==0 ) goto EndFill;
	goto StartFill;
EndFill:
	CodeOfOneLine[i].code=0xFF; CodeOfOneLine[i].value=0;CodeOfOneLine[i].cmplx=0;
	return (i+1);
}

static void ReplaceAccuByValue(floactet *CodeOfOneLine){
int i=0;
int iptrstrt=0,Aind;
signed int Ainteg=-1;

	if (CodeOfOneLine[0].code == 15) goto StartOperate; //If Mx(i,j) then change accu i and j into numbers 

	while (i<NbrMaxOperationOnLine ){ 
		if (CodeOfOneLine[i].code==0xFF) goto StartOperate;
		if (CodeOfOneLine[i].code == 8) {iptrstrt=i+1; goto StartOperate;} //8 is the code of "="
		i++;
	}
  StartOperate:
	i=iptrstrt;
	while (i<NbrMaxOperationOnLine && CodeOfOneLine[i].code!=0xFF){ 
		if (CodeOfOneLine[i].code==13 && CodeOfOneLine[i].value==15) {Ainteg=CodeOfOneLine[i+8].value;}//Int(a,b,h,x) detection to avoid change of x in value we'll need the nbr of the accu, but change the others
		if (CodeOfOneLine[i].code == 9){ //9 is the Code for Accu
			Aind=CodeOfOneLine[i].value;
			if (Aind != Ainteg){
			CodeOfOneLine[i].code =1;//change in a number 
			CodeOfOneLine[i].value = Accu[Aind].value;
			CodeOfOneLine[i].cmplx=Accu[Aind].cmplx;
			}//else PrintCmd("One not done accu\n");
			if (debug >0) {
			sprintf(s,"A%d = ",Aind);PrintCmd(s);Rprintf(Accu[Aind].value);
			PrintCmd("+i*");Rprintf(Accu[Aind].cmplx);}
			}
	i++;
	} 
 }

static int ReplaceMAccuByValue(floactet *CodeOfOneLine){//Replace Matrix by values
int i=0,k;
int iptrstrt=0,Aind, Mn,Mp,NumM;

  while (i<NbrMaxOperationOnLine ){ 
	if (CodeOfOneLine[i].code==0xFF) goto StartOperate;
	if (CodeOfOneLine[i].code == 8) {iptrstrt=i+1; goto StartOperate;} //8 is the code of "="
	i++;
  }
  StartOperate:
	i=iptrstrt;
	while (i<NbrMaxOperationOnLine && CodeOfOneLine[i].code!=0xFF){ 
		if (CodeOfOneLine[i].code == 15 && CodeOfOneLine[i+1].code ==6){ //15 is the Code for MAccu Matrix
			if(debug > 0) {sprintf(s,"MAccu =%d\n",(int)CodeOfOneLine[i].value); PrintCmd(s);}
			if(CodeOfOneLine[i+2].code!=1 ||
				CodeOfOneLine[i+3].code!=10 ||CodeOfOneLine[i+4].code!=1 ||
				CodeOfOneLine[i+5].code!=7 ) {
				PrintCmd("Syntaxe Matrix!\n");
				sprintf(s,"[%d],[%d],[%d],[%d]\n",(int)CodeOfOneLine[i+2].code,(int)CodeOfOneLine[i+3].code,(int)CodeOfOneLine[i+4].code,(int)CodeOfOneLine[i+5].code);
				PrintCmd(s);
				return 9;
				}else {
				NumM = (int) CodeOfOneLine[i].value;
				if (MAccu[NumM].ptr == 0) {PrintCmd("Matrix not defined!\n");return 9;}
				Mn= (int) CodeOfOneLine[i+2].value;
				Mp= (int) CodeOfOneLine[i+4].value;
				if (Mn < 1 || Mn > MAccu[NumM].n ||Mp<1 || Mp >MAccu[NumM].p){ 
					PrintCmd("Out of Range Matrix index!\n"); return 9;}
				CodeOfOneLine[i].code = 1; 
				CodeOfOneLine[i].value = MAccu[NumM].ptr[(Mn-1)*MAccu[NumM].p+Mp-1];
				}
		k=i+1;
		while (CodeOfOneLine[k].code !=0xFF && CodeOfOneLine[k].code !=0) {
			CodeOfOneLine[k]=CodeOfOneLine[k+5];
			k++;}
		}	
		i++;
	}
	return 0;
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
 /* PalmOs
 MemHandle BLCHdle = MemHandleNew(NbrMaxOperationOnLine*sizeof(struct floactet) );
 floactet * BufferLineCode=MemHandleLock(BLCHdle);
 */
 //win32 ->
 floactet * BufferLineCode = (floactet*)malloc(NbrMaxOperationOnLine*sizeof(struct floactet) );
 if (BufferLineCode == 0) {printf("Pb in TreatParenthese allocation bufferLineCode!\n");return 0;} 

 //counts the max level parenthese (((1+1)+3)-2) := 3level
 //and get iptr and iptrEnd the pointer indexes left right on the deepest level parenthese

 StartParentheseJob:
	LevelParMax=0;
	CountLevelPar=0;
	iptr=0;
	iptrEnd=0;
    i=0;
	
 while (i<NbrMaxOperationOnLine){
	if (CodeList[i].code==13 && CodeList[i].value == 15) {goto EndHandleParenthese;}//for Int
	if (CodeList[i].code==15 &&CodeList[i+1].code==6) {i=i+6;} //to avoid handling Matrix Mn°(n,p)=
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
	if (CountLevelPar !=0) {PrintCmd("Parenthese CountLevelPar !=0 \n");ErrorCode = 1; goto EndHandleParenthese;}
	if (LevelParMax == 0) goto EndHandleParenthese; //no or no more parenthese
	
	// Copy little line between parenthese in BufferLineCode
	for (i=0;i<(iptrEnd-iptr-1);i++){BufferLineCode[i]=CodeList[iptr+i+1];}
	BufferLineCode[i].code=0xFF;BufferLineCode[i].value=0; //add the "\n" in the buffer
	Error = CalculOneLine(BufferLineCode);//Go to calculate the BufferOneLine
	if(StopProgram == 1) {free(BufferLineCode);return 0;}
	if (Error != 0) {sprintf (s,"Error in Parenthese = %d\n", Error);PrintCmd(s); ErrorCode=2;} 
	if (debug >0) {PrintCmd("Parenthese Result BufferLineCode:\n");
		int j;					for (j=0;j<6;j++){
							sprintf(s," %d [%d]   [%d]\n",j,(int)BufferLineCode[j].code,(int)BufferLineCode[j].value);
							PrintCmd(s);}
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
	free(BufferLineCode);
	return ErrorCode;
	
 }
CalculFAccuComplexe(floactet *CodeLine, int i,float *ResultY,float *ResultY_cmplx){
//old one: int CalculFAccu(floactet *CodeOfOneLine, int NbrFAccu,float valX, float *ResultY){
	//ResultY is the final calculation result of F...(ValX)=ResultY
	//floactet BufferCodes[200]; ! Creates stack overflow 'cause multiple loops
	floactet * BufferCodes;
	float SaveAccuVal,valX,SaveAccuVal_cmplx,valX_cmplx;
	int N_AccuX,OfPtr,SaveCodeListOffset,Error,NbrFAccu;
	int SizeBufferCodes=200;
	floactet *CodeList;

		NbrFAccu=(int)CodeLine[i].value;
		valX=CodeLine[i+1].value;
		if (AllowComplexe!=1) valX_cmplx=0;
		else 	valX_cmplx=CodeLine[i+1].cmplx;

		CodeList = CodeListAdr; //Transporte the full list adresse here
		/* PalmOs
		MemHdle = MemHandleNew(SizeBufferCodes*sizeof (struct floactet) );
		if( MemHdle == 0){PrintCmd("Cannot allocate BufferCodes!");return;}
		BufferCodes = MemHandleLock(MemHdle);
		*/
		//win32 ->
		BufferCodes = (floactet*) malloc(SizeBufferCodes*sizeof (struct floactet) );
		if( BufferCodes == 0){PrintCmd("Cannot allocate BufferCodes!");return;}
		
		
		OfPtr = FAccu[NbrFAccu];
		
		if (CodeList[OfPtr+0].code != 14 || CodeList[OfPtr+0].value != NbrFAccu ) {
				PrintCmd("Error Wrong Function");return 1;}
		
		N_AccuX=(int)CodeList[OfPtr+2].value;
		SaveCodeListOffset=CodeListOffset;
		CodeListOffset=OfPtr+5;
		FillCodeOfOneLine(CodeList,BufferCodes);//transfert one line 
		SaveAccuVal=Accu[N_AccuX].value; //We save Accu value to avoid interference with other variables calculations
		SaveAccuVal_cmplx=Accu[N_AccuX].cmplx; //We save Accu value to avoid interference with other variables calculations
		Accu[N_AccuX].value=valX; //Set the value of X for F...(ValX)=
		Accu[N_AccuX].cmplx=valX_cmplx; //Set the value of X for F...(ValX)=
		ReplaceAccuByValue(BufferCodes);
		//sprintf(s,"Code F [%d] [%d]",BufferCodes[0].code,(int)BufferCodes[0].value);
		//PrintCmd(s);
		Error = TreatParenthese(BufferCodes);													//parenthese
		if (Error != 0) {sprintf (s,"In function FAccu%d, Error%d  \n", NbrFAccu,Error);PrintCmd(s);goto ErrorOut;}
		Error = CalculOneLine(BufferCodes); //Calcul
		if (Error != 0) {sprintf (s,"Calcul Error in FAccu %d \n", NbrFAccu);PrintCmd(s);goto ErrorOut;}
		if (BufferCodes[0].code != 1) {PrintCmd("Error finishing function line\n");goto ErrorOut;}
		*ResultY= BufferCodes[0].value;
		*ResultY_cmplx=BufferCodes[0].cmplx;
		CodeListOffset=SaveCodeListOffset;
		Accu[N_AccuX].value=SaveAccuVal; //Set the value of X for F...(ValX)=
		Accu[N_AccuX].cmplx=SaveAccuVal_cmplx; //Set the value of X for F...(ValX)=
		free(BufferCodes);
		return 0;
ErrorOut:
		free(BufferCodes);
		return Error;
	}
 
/*
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
		SaveAccuVal=Accu[N_AccuX].value; //We save Accu value to avoid interference with other variables calculations
		Accu[N_AccuX].value=valX; //Set the value of X for F...(ValX)=
		ReplaceAccuByValue(BufferCodes);
		//sprintf(s,"Code F [%d] [%d]",BufferCodes[0].code,(int)BufferCodes[0].value);
		//PrintCmd(s);
		Error = TreatParenthese(BufferCodes);													//parenthese
		if (Error != 0) {sprintf (s,"In function FAccu%d, Error%d  \n", NbrFAccu,Error);PrintCmd(s);goto ErrorOut;}
		Error = CalculOneLine(BufferCodes); //Calcul
		if (Error != 0) {sprintf (s,"Calcul Error in FAccu %d \n", NbrFAccu);PrintCmd(s);goto ErrorOut;}
		if (BufferCodes[0].code != 1) {PrintCmd("Error finishing function line\n");goto ErrorOut;}
		*ResultY= BufferCodes[0].value;
		CodeListOffset=SaveCodeListOffset;
		Accu[N_AccuX].value=SaveAccuVal; //Set the value of X for F...(ValX)=
		MemHandleFree(MemHdle);
		return 0;
ErrorOut:
		MemHandleFree(MemHdle);
		return Error;
	}
*/

int Integration(floactet *CodeOfOneLine, int i,float *ResultY,float *ResultY_cmplx){
	//ResultY is the final calculation result of the integration
	floactet * BufferCodes,*CodeList;
	float SaveAccuVal,SaveAccuVal_cmplx,S,S_cmplx,a,h,b,X,Y;
	int N_AccuX,OfPtr,SaveCodeListOffset,Error,k;
	int SizeBufferCodes=200; //therefore maximum code for CodeOfOneLine is 200
	int iloop;
	
	CodeList = CodeListAdr; //We take the unchanged value of the Accu x Int(a,b,h,x)
	/*PalmOs
		MemHdle = MemHandleNew(SizeBufferCodes*sizeof (struct floactet) );
		if( MemHdle == 0){PrintCmd("Can't allocate BufferCodes!\n");return;}
		BufferCodes = MemHandleLock(MemHdle);
		*/
		BufferCodes = (floactet*)malloc(SizeBufferCodes*sizeof (struct floactet) );
		if( BufferCodes == 0){PrintCmd("Can't allocate BufferCodes!\n");return 0;}
		
		a = CodeOfOneLine[i+2].value; b= CodeOfOneLine[i+4].value;
		h= CodeOfOneLine[i+6].value; N_AccuX=(int)CodeOfOneLine[i+8].value; //We take from CodeList this one! otherwise we get the changed value into values instead of accu
		if(debug > 0) {sprintf(s,"Int(%d,%d,%d,A%d)\n",(int)a,(int)b,(int)h,N_AccuX); PrintCmd(s);}
		if (h<=0) {PrintCmd("Error Step of integration h should be > 0");Error =1;goto ErrorOut;} 
		SaveCodeListOffset=CodeListOffset;

		X=a;//init
		Accu[N_AccuX].value=X; 
		Accu[N_AccuX].cmplx=0; // init
		S=0;//init
		S_cmplx=0;
		SaveAccuVal=Accu[N_AccuX].value; //We save Accu value to avoid interference with other variables calculations
		SaveAccuVal_cmplx=Accu[N_AccuX].cmplx; //We save Accu value to avoid interference with other variables calculations

		iloop=0;
	LoopCalculateIntegral:
		k=0;
	//sprintf(s,"i=%d\n",i);PrintCmd(s);

		while (CodeOfOneLine[k+i+10].code !=0xFF &&CodeOfOneLine[k+i+10].code!=0){
												BufferCodes[k]=CodeOfOneLine[k+i+10];k++;}
		BufferCodes[k]=CodeOfOneLine[k+i+10];//copy the last code
		/*PrintCmd("-0-");
			for (k=0;k<5;k++){
			sprintf(s," code %d [%d] [%d] \n",k,BufferCodes[k].code,(int)BufferCodes[k].value);
			PrintCmd(s);
			}
			for (k=0;k<5;k++){
			sprintf(s," code %d [%d] [%d] \n",k,CodeOfOneLine[k].code,(int)CodeOfOneLine[k].value);
			PrintCmd(s);
			}*/

		ReplaceAccuByValue(BufferCodes);
		Error = TreatParenthese(BufferCodes);
		if (Error != 0) {sprintf (s,"In Integral Error%d\n", Error);PrintCmd(s);goto ErrorOut;}

		Error = CalculOneLine(BufferCodes); //Calcul
		if (Error != 0) {sprintf (s,"Calcul Error %d in Integral \n", Error);PrintCmd(s);goto ErrorOut;}
		if (BufferCodes[0].code != 1) {PrintCmd("Error finishing function line\n");goto ErrorOut;}

		if (iloop==0){//the first term
		S=BufferCodes[0].value/2.0;
		if (AllowComplexe==1) S_cmplx=BufferCodes[0].cmplx/2.0;
		}else{//the other terms with the last term where half of it will be removed below
		S=BufferCodes[0].value+S;
		if (AllowComplexe==1) S_cmplx=BufferCodes[0].cmplx+S_cmplx;
		}
		
		X=X+h;
		Accu[N_AccuX].value=X;
		iloop++;
		if (X<=b) goto LoopCalculateIntegral;
		if (X==b+h){
		S=S-BufferCodes[0].value/2.0; //remove half of last term for trapeze sum
		if (AllowComplexe==1) S_cmplx= S_cmplx-BufferCodes[0].cmplx/2.0;
		}
		*ResultY= S*h;
		*ResultY_cmplx= S_cmplx*h;
		CodeListOffset=SaveCodeListOffset;
		Accu[N_AccuX].value=SaveAccuVal; //Give back the value of X
		Accu[N_AccuX].cmplx=SaveAccuVal_cmplx; //Give back the value of X
		free(BufferCodes);
		return 0;
ErrorOut:
		free(BufferCodes);
		return Error;
	}
	
static int CalculOneLine(floactet *CodeListLine){
	int Error,i;
	//ToDo: for matrix detection on a line we have let an information at the
	//end of the line [0xFF][1] means there is a matrix in this line
	//the information is deposited in ConvertMnemo
	i=0;
	while (CodeListLine[i].code!=0xFF){
		if (CodeListLine[i].code==15) 
			if (CodeListLine[i+1].code!=6){
				//PrintCmd("Detection matrix\n");
				Error = CalculOneLineReelMatrix(CodeListLine);
				return Error;}
		i++;
	}
			
	if (AllowComplexe ==1) Error = CalculOneLineComplexe(CodeListLine);
	else Error = CalculOneLineReel(CodeListLine);
	return Error;
	}
	
//------------------------------  Calcul with Matrix -------------------------

 static int  CalculOneLineReelMatrix(floactet * CodeListLine){//Must be one line only
	/* Error Code return   0  Calculation Done
									   1  No  End in the Line
									   2  Syntaxe error should find numbers around the math operator
									   3  Division by zero prohibited
									   4 Should be no parenthese at the stage of this program
										  The treatment of the parenthese should be done previously.
									   5 Bad syntax for test condition
								       6 Syntaxe Error
									   7 Serial Device Not Opened (Oscilloscope)
									   8 Complexe only
									   9 Matrix Error
	The calculation is done after the equal sign if there is one,
	the pointer  iptrEqualSignP points the code 	after equal sign code 8.
	We use here the same iptrEqualSign for the Instruction code 11... to perform the calculation
	behind the instruction (exemple: print 1*2)
	*/
									   
	//Search for priority *,/,(,  
	int i,k; int imaxLine=0, NumM,Mn,Mp;
	float CodeVal,val,x1,x2,a,b,h,val_cmplx;
	int ErrorCode=0,NbrAccu;
	int iptrEqualSignP; //Used to save the position of the equal sign in the line
	
	if (BreakActivated==1) if( CheckForBreak() == 1) return; //test for break test

	
	i=0;
	imaxLine=0; iptrEqualSignP=0;
	while (i<NbrMaxOperationOnLine){
	if (CodeListLine[i].code == 0xFF) {imaxLine=i; goto SearchPriority;}
	if (CodeListLine[i].code == 8) iptrEqualSignP=i+1; //detect and save equal code position+1 

	//The way FAccu is handled is that in the LAUNCHING codes it only detects the definition Fi(x)=... then skips
	//here (just below we are not in that case, we only replace the calculated values by CalculFAccuComplexe 
	if (CodeListLine[i].code==14 &&CodeListLine[i+1].code==1) {//FAccu insertion
		if(debug > 0) {sprintf(s,"OffsetAdr FAccu =%d\n",FAccu[(int)CodeListLine[i].value]); PrintCmd(s);}
		if(CalculFAccuComplexe(CodeListLine, i,&val,&val_cmplx)!=0) return 6;
		//if(CalculFAccu( CodeListLine, (int)CodeListLine[i].value, CodeListLine[i+1].value,&val)!=0) return 5;
		else {CodeListLine[i].code=1; CodeListLine[i].value=val;}
		k=i+1;
		while (CodeListLine[k].code !=0xFF && CodeListLine[k].code !=0) {
			CodeListLine[k]=CodeListLine[k+1];
			k++;
			}
	}	

	if (CodeListLine[i].code==13 &&CodeListLine[i+1].code==15) {
		MathFunctionMatrices(MAccu, CodeListLine,i);//Math Function for Matrices
		}
		
	if (CodeListLine[i].code==13 &&CodeListLine[i+1].code==1) {//Math Function
		CodeVal=CodeListLine[i].value;
		if (CodeVal==1 ) {val=RMath_exp(CodeListLine[i+1].value);goto KeepOn;}//exp
		if (CodeVal==2 ) {val=RMath_ln(CodeListLine[i+1].value);goto KeepOn;}	//ln		
		if (CodeVal==3 ) {val=RMath_sqrt(CodeListLine[i+1].value);goto KeepOn;}	//sqrt		
	  //if (CodeVal==4 ) {AlreadyTaken for the function f(x);}	//function f(x)
		if (CodeVal==5 ) {val=RMath_sin(CodeListLine[i+1].value);goto KeepOn;}	//sin
		if (CodeVal==6 ) {val=RMath_cos(CodeListLine[i+1].value);goto KeepOn;}	//cos
		if (CodeVal==7 ) {val=RMath_tan(CodeListLine[i+1].value);goto KeepOn;}	//tan
		if (CodeVal==8 ) {val=fact((int)CodeListLine[i+1].value);goto KeepOn;}	//factoriel
		if (CodeVal==9 ) {goto OutForPower;}	//Power
		if (CodeVal==10 ) {val=RMath_ch(CodeListLine[i+1].value);goto KeepOn;}	//ch
		if (CodeVal==11 ) {val=RMath_sh(CodeListLine[i+1].value);goto KeepOn;}	//sh
		if (CodeVal==12 ) {val=RMath_th(CodeListLine[i+1].value);goto KeepOn;}	//th
		if (CodeVal==13 ) {val=CodeListLine[i+1].value;PrintCmd("Activate Complexe before use!\n");MathError=8;}
		if (CodeVal==14 ) {val=0;PrintCmd("Activate Complexe before use!\n");MathError=8;}	//Im
		//if (CodeVal==15 ) {//Already taken for integrals}
		if (CodeVal==16 ) {if (SerialReady ==0) MathError = 7; val=Oscilloscope(CodeListLine[i+1].value);goto KeepOn;}
		if (CodeVal==17 ) {val=RMath_acos(CodeListLine[i+1].value);goto KeepOn;}	//acos
		if (CodeVal==18 ) {val=RMath_asin(CodeListLine[i+1].value);goto KeepOn;}	//asin
		if (CodeVal==19 ) {val=RMath_ath(CodeListLine[i+1].value);goto KeepOn;}	//atanh
		if (CodeVal==20 ) {val=RMath_atan(CodeListLine[i+1].value);goto KeepOn;}	//atan
		if (CodeVal==21 ) {val=RMath_ash(CodeListLine[i+1].value);goto KeepOn;}	//ash
		if (CodeVal==22 ) {val=RMath_ach(CodeListLine[i+1].value);goto KeepOn;}	//ach
		if (CodeVal==23 ) {val=RMath_abs(CodeListLine[i+1].value);goto KeepOn;}	//abs
		if (CodeVal==24 ) {val=0;PrintCmd("Activate Complexe before use mod(z)\n");MathError=8;goto KeepOn;}	//mod(z)
		if (CodeVal==25 ) {val=0;PrintCmd("Activate Complexe before use arg(z)\n");MathError=8;goto KeepOn;}	//arg(z)
		if (CodeVal==26 ) {
								MSG Msg;
								GetMessage(&Msg, NULL, 0, 0);
								TranslateMessage(&Msg);
								DispatchMessage(&Msg);
								val=Button;Button=0;}	//key

	KeepOn:
		if (MathError !=0 ) {sprintf(s,"Math Error = %d\n",MathError);PrintCmd(s);return 3;}
		if (ErrorCode == 0) {CodeListLine[i].code=1; CodeListLine[i].value=val;}
		k=i+1;
		while (CodeListLine[k].code !=0xFF && CodeListLine[k].code !=0) {
			CodeListLine[k]=CodeListLine[k+1];
			k++;
			}
		}
OutForPower:
		
	if (CodeListLine[i+1].code==11 &&CodeListLine[i+1].value==5) {//Test "=="
		if (CodeListLine[i].code !=1 && CodeListLine[i+2].code !=1){ErrorCode = 5; goto EndCalculOneLine;}
		else { if ( CodeListLine[i].value == CodeListLine[i+2].value) {
					TestCondition=1; }
					i=i+3;goto EndCalculOneLine;
					}
	}
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
	sprintf (s," Code %d  [%d]   [%d] \n",i,CodeListLine[i].code, (int) CodeListLine[i].value);
	PrintCmd(s);
	}
	i++;
	}//end loop while(i<NbrMaxOperationOnLine)
	if (i>0 && imaxLine==0) {ErrorCode =1; goto EndCalculOneLine;} //NoEnd
	SearchPriority:
	i=iptrEqualSignP; //We start the calculation from the equal sign +1	
	while (i<NbrMaxOperationOnLine){//Strong Priority for Power
		if (CodeListLine[i].code == 13 && CodeListLine[i].value == 9 ) { goto Power;}
		if (CodeListLine[i].code == 13 && CodeListLine[i].value == 15 ) { goto Integral;}
		if (CodeListLine[i].code == 0xFF ) goto AfterPower;
		if (CodeListLine[i].code == 0 ) goto AfterPower;
		i++;
		}
	AfterPower:
	i=iptrEqualSignP; //We start the calculation from the equal sign +1	
	while (i<NbrMaxOperationOnLine){
		if (CodeListLine[i].code == 4 ) goto Multiplication;
		if (CodeListLine[i].code == 5 ) goto Division;
		if (CodeListLine[i].code == 6 ) {ErrorCode=4; goto EndCalculOneLine;}
		if (CodeListLine[i].code == 0xFF ) goto NoPriority;
		if (CodeListLine[i].code == 0 ) goto NoPriority;
		i++;
		}
	PrintCmd("Arriving end of line before Math sign\n");
	goto EndCalculOneLine;	

	Integral:
			if (CodeListLine[i].code==13 && CodeListLine[i].value==15) {//Integral
			if(CodeListLine[i+1].code!=6||CodeListLine[i+2].code!=1||CodeListLine[i+3].code!=10
			||CodeListLine[i+4].code!=1||CodeListLine[i+5].code!=10||CodeListLine[i+6].code!=1
			||CodeListLine[i+7].code!=10||CodeListLine[i+8].code!=9||CodeListLine[i+9].code!=7)
			{PrintCmd("Syntaxe Error Integral\n");
			//for (k=0;k<11;k++){
			//sprintf(s," code %d [%d] [%d] \n",k,CodeListLine[k].code,(int)CodeListLine[k].value);
			//PrintCmd(s);}
			return 6;}//for the accu to appear (code 9) at i+8 position we have to make a special treatment to avoid conversion of the accu for the variable
		if(Integration(CodeListLine,i,&val,&val_cmplx)!=0) return 5;
			//sprintf(s,"Integration =%d\n",(int)val);PrintCmd(s);
			CodeListLine[i].code=1; CodeListLine[i].value=val;
			CodeListLine[i+1].code =0xFF;CodeListLine[i+1].value =0;
			if (AllowComplexe == 0 && val_cmplx !=0) PrintCmd("Your function of complexe type: you need to Toogle Allow complexe\n");
		}
		goto SearchPriority;
//----------- start modify for matrices		
	Power:
		//i points on the sign ^
		i-- ; //place on the left number
		if (CodeListLine[i].code==15 || CodeListLine[i+2].code == 15) 
			{ErrorCode=MatrixPower(MAccu,CodeListLine,i,imaxLine);
					i=iptrEqualSignP; //reinitialise position after the equal sign	
					goto SearchPriority; // Back to seek for operation priority
			}
		if (CodeListLine[i].code!=1 || CodeListLine[i+2].code != 1) {PrintCmd("How possible ?? Power\n");ErrorCode=2; goto EndCalculOneLine; }
		val = RMath_Pow(CodeListLine[i].value,CodeListLine[i+2].value);
		CodeListLine[i].code=1; //number
		CodeListLine[i].value=val; // new value
		i++;
		if (debug > 0) {sprintf (s,"Power Result : %d\n",(int)val); PrintCmd(s);}	
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
		if (CodeListLine[i].code==15 || CodeListLine[i+2].code == 15) 
				{ErrorCode=MatrixMultiplication(MAccu,CodeListLine,i,imaxLine);
						i=iptrEqualSignP; //reinitialise position after the equal sign	
						goto SearchPriority; // Back to seek for operation priority
				}
		if (CodeListLine[i].code!=1 || CodeListLine[i+2].code != 1) {PrintCmd("How possible ?? multiplication\n");ErrorCode=2; goto EndCalculOneLine; }
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
		if (CodeListLine[i].code==15 || CodeListLine[i+2].code == 15) 
				{ErrorCode=MatrixDivision(MAccu,CodeListLine,i,imaxLine);
						i=iptrEqualSignP; //reinitialise position after the equal sign	
						goto SearchPriority; // Back to seek for operation priority
				}
		if (CodeListLine[i].code!=1 || CodeListLine[i+2].code != 1) {PrintCmd("How possible ?? division\n"); ErrorCode=2; goto EndCalculOneLine; }
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
		if (CodeListLine[i].code==15 && CodeListLine[i+1].code == 0xFF) {goto EndCalculOneLine; }// Matrix ready 
		val=0;
		//We handle now the addition of scalar with matrices
		ErrorCode=MatrixSubAddition(MAccu,CodeListLine,i,iptrEqualSignP,imaxLine);
		if (ErrorCode !=0) {PrintCmd("SubaddMatrix treatment  generated Error\n");}
 EndCalculOneLine:
			LastValCalculated.value=val; //After all passages LastValCalculated keeps the value
			if (debug > 0) {sprintf (s, "Matrix result number is %d \n",(int) CodeListLine[iptrEqualSignP].value); PrintCmd(s);} 
			if (val == 13061976) {CodageIdentity=1; PrintCmd("et...né...où?\n");}
	return ErrorCode;
 }



//--------------------------------End Calcul with Matrix -------------------	
	

	
	
	
	
	
 static int  CalculOneLineReel(floactet * CodeListLine){//Must be one line only
	/* Error Code return   0  Calculation Done
									   1  No  End in the Line
									   2  Syntaxe error should find numbers around the math operator
									   3  Division by zero prohibited
									   4 Should be no parenthese at the stage of this program
										  The treatment of the parenthese should be done previously.
									   5 Bad syntax for test condition
								       6 Syntaxe Error
									   7 Serial Device Not Opened (Oscilloscope)
									   8 Complexe only
									   9 Matrix Syntaxe
	The calculation is done after the equal sign if there is one,
	the pointer  iptrEqualSignP points the code 	after equal sign code 8.
	We use here the same iptrEqualSign for the Instruction code 11... to perform the calculation
	behind the instruction (exemple: print 1*2)
	*/
									   
	//Search for priority *,/,(,  
	int i,k; int imaxLine=0, NumM,Mn,Mp;
	float CodeVal,val,x1,x2,a,b,h,val_cmplx;
	int ErrorCode=0,NbrAccu;
	int iptrEqualSignP; //Used to save the position of the equal sign in the line
	
	if (BreakActivated==1) if( CheckForBreak() == 1) return; //test for break test

	
	i=0;
	imaxLine=0; iptrEqualSignP=0;
	while (i<NbrMaxOperationOnLine){
	if (CodeListLine[i].code == 0xFF) {imaxLine=i; goto SearchPriority;}
	if (CodeListLine[i].code == 8) iptrEqualSignP=i+1; //detect and save equal code position+1 

	if (CodeListLine[i].code==14 &&CodeListLine[i+1].code==1) {//FAccu insertion
		if(debug > 0) {sprintf(s,"OffsetAdr FAccu =%d\n",FAccu[(int)CodeListLine[i].value]); PrintCmd(s);}
		if(CalculFAccuComplexe(CodeListLine, i,&val,&val_cmplx)!=0) return 6;
		//if(CalculFAccu( CodeListLine, (int)CodeListLine[i].value, CodeListLine[i+1].value,&val)!=0) return 5;
		else {CodeListLine[i].code=1; CodeListLine[i].value=val;}
		k=i+1;
		while (CodeListLine[k].code !=0xFF && CodeListLine[k].code !=0) {
			CodeListLine[k]=CodeListLine[k+1];
			k++;
			}
	}	
	
	if (CodeListLine[i].code==13 &&CodeListLine[i+1].code==1) {//Math Function
		CodeVal=CodeListLine[i].value;
		if (CodeVal==1 ) {val=RMath_exp(CodeListLine[i+1].value);goto KeepOn;}//exp
		if (CodeVal==2 ) {val=RMath_ln(CodeListLine[i+1].value);goto KeepOn;}	//ln		
		if (CodeVal==3 ) {val=RMath_sqrt(CodeListLine[i+1].value);goto KeepOn;}	//sqrt		
	  //if (CodeVal==4 ) {AlreadyTaken for the function f(x);}	//function f(x)
		if (CodeVal==5 ) {val=RMath_sin(CodeListLine[i+1].value);goto KeepOn;}	//sin
		if (CodeVal==6 ) {val=RMath_cos(CodeListLine[i+1].value);goto KeepOn;}	//cos
		if (CodeVal==7 ) {val=RMath_tan(CodeListLine[i+1].value);goto KeepOn;}	//tan
		if (CodeVal==8 ) {val=fact((int)CodeListLine[i+1].value);goto KeepOn;}	//factoriel
		if (CodeVal==9 ) {goto OutForPower;}	//Power
		if (CodeVal==10 ) {val=RMath_ch(CodeListLine[i+1].value);goto KeepOn;}	//ch
		if (CodeVal==11 ) {val=RMath_sh(CodeListLine[i+1].value);goto KeepOn;}	//sh
		if (CodeVal==12 ) {val=RMath_th(CodeListLine[i+1].value);goto KeepOn;}	//th
		if (CodeVal==13 ) {val=CodeListLine[i+1].value;PrintCmd("Activate Complexe before use!\n");MathError=8;}
		if (CodeVal==14 ) {val=0;PrintCmd("Activate Complexe before use!\n");MathError=8;}	//Im
		//if (CodeVal==15 ) {//Already taken for integrals}
		if (CodeVal==16 ) {if (SerialReady ==0) MathError = 7; val=Oscilloscope(CodeListLine[i+1].value);goto KeepOn;}
		if (CodeVal==17 ) {val=RMath_acos(CodeListLine[i+1].value);goto KeepOn;}	//acos
		if (CodeVal==18 ) {val=RMath_asin(CodeListLine[i+1].value);goto KeepOn;}	//asin
		if (CodeVal==19 ) {val=RMath_ath(CodeListLine[i+1].value);goto KeepOn;}	//atanh
		if (CodeVal==20 ) {val=RMath_atan(CodeListLine[i+1].value);goto KeepOn;}	//atan
		if (CodeVal==21 ) {val=RMath_ash(CodeListLine[i+1].value);goto KeepOn;}	//ash
		if (CodeVal==22 ) {val=RMath_ach(CodeListLine[i+1].value);goto KeepOn;}	//ach
		if (CodeVal==23 ) {val=RMath_abs(CodeListLine[i+1].value);goto KeepOn;}	//abs
		if (CodeVal==24 ) {val=0;PrintCmd("Activate Complexe before use mod(z)\n");MathError=8;goto KeepOn;}	//mod(z)
		if (CodeVal==25 ) {val=0;PrintCmd("Activate Complexe before use arg(z)\n");MathError=8;goto KeepOn;}	//arg(z)
		if (CodeVal==26 ) {
								MSG Msg;
								GetMessage(&Msg, NULL, 0, 0);
								TranslateMessage(&Msg);
								DispatchMessage(&Msg);
								val=Button;Button=0;}	//key

	KeepOn:
		if (MathError !=0 ) {sprintf(s,"Math Error = %d\n",MathError);PrintCmd(s);return 3;}
		if (ErrorCode == 0) {CodeListLine[i].code=1; CodeListLine[i].value=val;}
		k=i+1;
		while (CodeListLine[k].code !=0xFF && CodeListLine[k].code !=0) {
			CodeListLine[k]=CodeListLine[k+1];
			k++;
			}
/*			for (k=0;k<5;k++){
			sprintf(s," code %d [%d] [%d] \n",k,CodeListLine[k].code,(int)CodeListLine[k].value);
			PrintCmd(s);
			}*/
		}
OutForPower:
		
	if (CodeListLine[i+1].code==11 &&CodeListLine[i+1].value==5) {//Test "=="
		if (CodeListLine[i].code !=1 && CodeListLine[i+2].code !=1){ErrorCode = 5; goto EndCalculOneLine;}
		else { if ( CodeListLine[i].value == CodeListLine[i+2].value) {
					TestCondition=1; }
					i=i+3;goto EndCalculOneLine;
					}
	}
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
	sprintf (s," Code %d  [%d]   [%d] \n",i,CodeListLine[i].code, (int) CodeListLine[i].value);
	PrintCmd(s);
	}
	i++;
	}//end loop while(i<NbrMaxOperationOnLine)
	if (i>0 && imaxLine==0) {ErrorCode =1; goto EndCalculOneLine;} //NoEnd
	SearchPriority:
	i=iptrEqualSignP; //We start the calculation from the equal sign +1	
	while (i<NbrMaxOperationOnLine){//Strong Priority for Power
		if (CodeListLine[i].code == 13 && CodeListLine[i].value == 9 ) { goto Power;}
		if (CodeListLine[i].code == 13 && CodeListLine[i].value == 15 ) { goto Integral;}
		if (CodeListLine[i].code == 0xFF ) goto AfterPower;
		if (CodeListLine[i].code == 0 ) goto AfterPower;
		i++;
		}
	AfterPower:
	i=iptrEqualSignP; //We start the calculation from the equal sign +1	
	while (i<NbrMaxOperationOnLine){
		if (CodeListLine[i].code == 4 ) goto Multiplication;
		if (CodeListLine[i].code == 5 ) goto Division;
		if (CodeListLine[i].code == 6 ) {ErrorCode=4; goto EndCalculOneLine;}
		if (CodeListLine[i].code == 0xFF ) goto NoPriority;
		if (CodeListLine[i].code == 0 ) goto NoPriority;
		i++;
		}
	PrintCmd("Arriving end of line before Math sign\n");
	goto EndCalculOneLine;	

	Integral:
			if (CodeListLine[i].code==13 && CodeListLine[i].value==15) {//Integral
			if(CodeListLine[i+1].code!=6||CodeListLine[i+2].code!=1||CodeListLine[i+3].code!=10
			||CodeListLine[i+4].code!=1||CodeListLine[i+5].code!=10||CodeListLine[i+6].code!=1
			||CodeListLine[i+7].code!=10||CodeListLine[i+8].code!=9||CodeListLine[i+9].code!=7)
			{PrintCmd("Syntaxe Error Integral\n");
			//for (k=0;k<11;k++){
			//sprintf(s," code %d [%d] [%d] \n",k,CodeListLine[k].code,(int)CodeListLine[k].value);
			//PrintCmd(s);}
			return 6;}//for the accu to appear (code 9) at i+8 position we have to make a special treatment to avoid conversion of the accu for the variable
		if(Integration(CodeListLine,i,&val,&val_cmplx)!=0) return 5;
			//sprintf(s,"Integration =%d\n",(int)val);PrintCmd(s);
			CodeListLine[i].code=1; CodeListLine[i].value=val;
			CodeListLine[i+1].code =0xFF;CodeListLine[i+1].value =0;
			if (AllowComplexe == 0 && val_cmplx !=0) PrintCmd("Your function of complexe type: you need to Toogle Allow complexe\n");
		/*k=i+1;
		while (CodeListLine[k].code !=0xFF && CodeListLine[k].code !=0) {
			CodeListLine[k]=CodeListLine[k+1];
			k++;
			}*/
		}
		goto SearchPriority;
		
	Power:
		//i points on the sign ^
		i-- ; //place on the left number
		if (CodeListLine[i].code!=1 || CodeListLine[i+2].code != 1) {ErrorCode=2; goto EndCalculOneLine; }
		val = RMath_Pow(CodeListLine[i].value,CodeListLine[i+2].value);
		CodeListLine[i].code=1; //number
		CodeListLine[i].value=val; // new value
		i++;
		if (debug > 0) {sprintf (s,"Power Result : %d\n",(int)val); PrintCmd(s);}	
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
			if (CodeListLine[i+1].code == 0xFF) goto FinalStep; // Line Finished		
			x2 = CodeListLine[i+2].value;
			if (CodeListLine[i+1].code==2) val=x2+val;
			if (CodeListLine[i+1].code==3) val=val-x2;
			i=i+2;
			}
			//Put Final calculation results
		FinalStep:
			CodeListLine[iptrEqualSignP].code=1; //Put value after the equal sign
			CodeListLine[iptrEqualSignP].value=val;
 EndCalculOneLine:
			//if (CodeListLine[0].code==1 && CodeListLine[1].code==0xFF)Rprintf(val); //"print" if nothing else but the number
			LastValCalculated.value=val; //After all passages LastValCalculated keeps the value
			if (debug > 0) {
				sprintf (s, "Line value is %d \n",(int) val); PrintCmd(s);
				}
			if (val == 13061976) {CodageIdentity=1; PrintCmd("et...né...où?\n");}	
	return ErrorCode;
 }


static int  CalculOneLineComplexe(floactet * CodeListLine){//Must be one line only
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
	float val,val_cmplx,x1,x2,x2_cmplx,a1,a2,b1,b2;
	int ErrorCode=0;
	int iptrEqualSignP; //Used to save the position of the equal sign in the line
	
	if (BreakActivated==1) if( CheckForBreak() == 1) return; //test for break test

	
	i=0;
	imaxLine=0; iptrEqualSignP=0;
	while (i<NbrMaxOperationOnLine){
	if (CodeListLine[i].code == 0xFF) {imaxLine=i; goto SearchPriority;}
	if (CodeListLine[i].code == 8) iptrEqualSignP=i+1; //detect and save equal code position+1 

	if (CodeListLine[i].code==14 &&CodeListLine[i+1].code==1) {//FAccu insertion
		if(debug > 0) {sprintf(s,"OffsetAdr FAccu =%d\n",FAccu[(int)CodeListLine[i].value]); PrintCmd(s);}
		if(CalculFAccuComplexe(CodeListLine, i,&val,&val_cmplx)!=0) {return 5;}
		else {CodeListLine[i].code=1; CodeListLine[i].value=val;CodeListLine[i].cmplx=val_cmplx;}
		k=i+1;
		while (CodeListLine[k].code !=0xFF && CodeListLine[k].code !=0) {
			CodeListLine[k]=CodeListLine[k+1];
			k++;
			}
	}	
	//--------------------  Here we go to calculate the different functions ------------------
	if (CodeListLine[i].code==13 && CodeListLine[i+1].code==1) {//Math Function
		MathError = CalculFunctionComplexe(CodeListLine,i);
		//Remark: val and val_cmplx are changed in the function
		if (MathError !=0 ) {sprintf(s,"Math Error = %d\n",MathError);PrintCmd(s);return 3;}
		}
	//----------------------------------------------------------------------------------------------
	//OutForPower:
		
	if (CodeListLine[i+1].code==11 &&CodeListLine[i+1].value==3) {//Test "<"
		if (CodeListLine[i].code !=1 && CodeListLine[i+2].code !=1){ErrorCode = 5; goto EndCalculOneLine;}
		else { if ( CodeListLine[i].value < CodeListLine[i+2].value) {
					TestCondition=1; }
					i=i+3;goto EndCalculOneLine;
					}
	}
	if (CodeListLine[i+1].code==11 &&CodeListLine[i+1].value==5) {//Test "=="
		if (CodeListLine[i].code !=1 && CodeListLine[i+2].code !=1){ErrorCode = 5; goto EndCalculOneLine;}
		else { if ( CodeListLine[i].value == CodeListLine[i+2].value && CodeListLine[i].cmplx == CodeListLine[i+2].cmplx) {
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
	sprintf (s," %d  [%d][%d][%d] \n",i,CodeListLine[i].code,(int) CodeListLine[i].value,(int) CodeListLine[i].cmplx);
	PrintCmd(s);
	}
	i++;
	}
	if (i>0 && imaxLine==0) {ErrorCode =1; goto EndCalculOneLine;} //NoEnd
	SearchPriority:
	i=iptrEqualSignP; //We start the calculation from the equal sign +1	
	while (i<NbrMaxOperationOnLine){//Strong Priority for Power
		if (CodeListLine[i].code == 13 && CodeListLine[i].value == 9 ) { goto Power;}
		if (CodeListLine[i].code == 13 && CodeListLine[i].value == 15 ) { goto Integral;}
		if (CodeListLine[i].code == 0xFF ) goto AfterPower;
		if (CodeListLine[i].code == 0 ) goto AfterPower;
		i++;
		}
	AfterPower:
	i=iptrEqualSignP; //We start the calculation from the equal sign +1	
	while (i<NbrMaxOperationOnLine){
		if (CodeListLine[i].code == 4 ) goto Multiplication;
		if (CodeListLine[i].code == 5 ) goto Division;
		if (CodeListLine[i].code == 6 ) {ErrorCode=4; goto EndCalculOneLine;}
		if (CodeListLine[i].code == 0xFF ) goto NoPriority;
		if (CodeListLine[i].code == 0 ) goto NoPriority;
		i++;
		}
	PrintCmd("Arriving end of line before Math sign\n");
	goto EndCalculOneLine;	

	Integral:
			if (CodeListLine[i].code==13 && CodeListLine[i].value==15) {//Integral
			if(CodeListLine[i+1].code!=6||CodeListLine[i+2].code!=1||CodeListLine[i+3].code!=10
			||CodeListLine[i+4].code!=1||CodeListLine[i+5].code!=10||CodeListLine[i+6].code!=1
			||CodeListLine[i+7].code!=10||CodeListLine[i+8].code!=9||CodeListLine[i+9].code!=7)
			{PrintCmd("Syntaxe Error Integral\n");
		//	for (k=0;k<11;k++){
		//	sprintf(s," code %d [%d] [%d] \n",k,CodeListLine[k].code,(int)CodeListLine[k].value);
		//	PrintCmd(s);}
			return 6;}//for the accu to appear (code 9) at i+8 position we have to make a special treatment to avoid conversion of the accu for the variable
		if(Integration(CodeListLine,i,&val,&val_cmplx)!=0) {return 5;}
		//else { sprintf(s,"Bla S_cmplx= %d\n",(int)val_cmplx);PrintCmd(s);}
			CodeListLine[i].code=1; CodeListLine[i].value=val;CodeListLine[i].cmplx=val_cmplx;
			CodeListLine[i+1].code =0xFF;CodeListLine[i+1].value =0;CodeListLine[i+1].cmplx =0;
		}
		goto SearchPriority;
		
	Power:
		//i points on the sign ^
		i-- ; //place on the left number
		if (CodeListLine[i].code!=1 || CodeListLine[i+2].code != 1) {ErrorCode=2; goto EndCalculOneLine; }
		val = RMath_Pow_cmplx(CodeListLine[i].value,CodeListLine[i].cmplx,CodeListLine[i+2].value,CodeListLine[i+2].cmplx,&val_cmplx);
		CodeListLine[i].code=1; //number
		CodeListLine[i].value=val; // new value
		CodeListLine[i].cmplx=val_cmplx;//to be changed later
		i++;
		if (debug > 0) {sprintf (s,"Power Result : %d\n",(int)val); PrintCmd(s);}	
		while (i+2<=imaxLine){CodeListLine[i]=CodeListLine[i+2]; i++; }
		i=iptrEqualSignP; //reinitialise position after the equal sign	
		goto SearchPriority; // Back to seek for operation priority

	Multiplication:
		//i points on the multiplie *
		i-- ; //place on the left number
		if (CodeListLine[i].code!=1 || CodeListLine[i+2].code != 1) {ErrorCode=2; goto EndCalculOneLine; }
		if(CodeListLine[i].cmplx==0 && CodeListLine[i+2].cmplx==0){
			val = CodeListLine[i].value * CodeListLine[i+2].value;
			val_cmplx= 0;
		}else {
			a1=CodeListLine[i].value; a2=CodeListLine[i+2].value;
			b1=CodeListLine[i].cmplx; b2=CodeListLine[i+2].cmplx;
			val = a1*a2-b1*b2;
			val_cmplx = a2*b1+a1*b2;			
			}
		CodeListLine[i].code=1; //number
		CodeListLine[i].value=val; // new value
		CodeListLine[i].cmplx=val_cmplx; // new value
		i++;
		while (i+2<=imaxLine){CodeListLine[i]=CodeListLine[i+2]; i++; }
		i=iptrEqualSignP; //reinitialise position after the equal sign	
		goto SearchPriority; // Back to seek for operation priority
		
	Division:
		//i points on the multiplie *
		i-- ; //place on the left number
		if (CodeListLine[i].code!=1 || CodeListLine[i+2].code != 1) {ErrorCode=2; goto EndCalculOneLine; }
		if(CodeListLine[i].cmplx == 0&& CodeListLine[i+2].cmplx == 0 && CodeListLine[i+2].value != 0) {
			val = CodeListLine[i].value / CodeListLine[i+2].value; val_cmplx=0;}
		else {
			if(CodeListLine[i+2].value==0 && CodeListLine[i+2].cmplx==0){ErrorCode = 3; goto EndCalculOneLine;}
			a1=CodeListLine[i].value; a2=CodeListLine[i+2].value;
			b1=CodeListLine[i].cmplx; b2=CodeListLine[i+2].cmplx;
			val = (a1*a2+b1*b2)/(a2*a2+b2*b2);
			val_cmplx = (a2*b1-a1*b2)/(a2*a2+b2*b2);			
			}

		CodeListLine[i].code=1; //number
		CodeListLine[i].value=val; // new value
		CodeListLine[i].cmplx=val_cmplx; // new value
		i++;
		//if (debug ==1) printf ("division result : %f \n",val);	
		while (i+2<=imaxLine){CodeListLine[i]=CodeListLine[i+2]; i++; }
		i=iptrEqualSignP;	
		goto SearchPriority; // Back to seek for operation priority
	
	NoPriority:
		i=iptrEqualSignP; //start after the equal sign
		val = CodeListLine[i].value; val_cmplx=CodeListLine[i].cmplx;
		if (CodeListLine[i].code==1 && CodeListLine[i+1].code == 0xFF) {goto EndCalculOneLine; }
		if (CodeListLine[i].code != 1) {ErrorCode =2; goto EndCalculOneLine; }// Should be a number !		
		if (CodeListLine[i+2].code != 1) {ErrorCode =2; goto EndCalculOneLine; }		
		while(i<imaxLine){
			if (CodeListLine[i+1].code == 0xFF) goto FinalStep; // Line Finished		
			x2 = CodeListLine[i+2].value; x2_cmplx = CodeListLine[i+2].cmplx;
			if (CodeListLine[i+1].code==2) {val=x2+val; val_cmplx=x2_cmplx+val_cmplx;}
			if (CodeListLine[i+1].code==3) {val=val-x2;  val_cmplx=val_cmplx-x2_cmplx;}
			i=i+2;
			}
			//Put Final calculation results
		FinalStep:
			CodeListLine[iptrEqualSignP].code=1; //Put value after the equal sign
			CodeListLine[iptrEqualSignP].value=val;
			CodeListLine[iptrEqualSignP].cmplx=val_cmplx;
 EndCalculOneLine:
			//if (CodeListLine[0].code==1 && CodeListLine[1].code==0xFF)Rprintf(val); //"print" if nothing else but the number
			LastValCalculated.value=val; //After all passages LastValCalculated keeps the value
			LastValCalculated.cmplx=val_cmplx; //After all passages LastValCalculated keeps the value
		
			if (debug > 0) {
				sprintf (s, "Line value is [%d][%d] \n",(int) val,(int)val_cmplx); PrintCmd(s);
				}
	return ErrorCode;
 }

