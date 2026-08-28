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



#define printf Printf     //Force Redefinition because it already exists in StdIOPalm.h but doesn't suit here
#define	ChartRectLeft		30
#define	ChartRectTop		52
#define	ChartRectWidth		100
#define	ChartRectHeight		90
#define MaxDataPoints		ChartRectWidth
#define VertScale		ChartRectHeight / 100
#define ProgTextMaxChars 	100

static void StartApplication(void);
static void StopApplication(void);
static Boolean MainFormHandleEvent(EventPtr event);
static void EventLoop(void);
static void ShowNumResultFld (Word FieldID, ULong val);
static void TestVersionSerialMgr(void); // implémentation regan
static void EnableControl (FormPtr Frm, Word ControlID, Boolean State);
static void EnableField (FormPtr Frm, Word FieldID, Boolean State);
static void SerIOReceive(void);
static void PrintCmd(char * s);
static void DeleteCmd();
static void PrintProg(char * s);
static void Printf(const char * format, ...); //should be Printf and not printf otherwise it conflicts with StdIOPalm.h definition
//static float sscanf (char *StringNbr,char* s, float *Nbr);
static float Rsscanf (char *StringNbr,char *s ,float *Nbr);
static float TenPower (int exp);
static int strlen(char *s);
static void Rprintf(float x);

//MathFunctions
int MathError; //send back a code if error in the Math functions. MathError=1 out of range exponential
//-------exponential----------  MathError=1 out of range exponential
#define SizeMathExpTable 500
static void CreateMathExpTable();
static float RMath_exp(float x);//value on range: exp(-+SizeMathExpTable) 
float MathExpTable[SizeMathExpTable];

//-------logarithme neperien----------  MathError=2 out of range Ln
#define SizeMathLnTable 102 //Not below 100 !
static void CreateMathLnTable();
static float RMath_ln(float x);//value on range: x>0 
static void GetMantisseExponant(float x, int *exponant, float*mantisse);
float MathLnTable[SizeMathLnTable];

//-------logarithme Square root----------  MathError=2 out of range Ln
static float RMath_sqrt(float x);

char MathFunctions[]= "exp_ln_sqrt_";
//										1	  2     3



char	*strErrSerLibOpen = "Error opening serial library";
char	*StrBlank = " ";


#define MnemoListSize 100
#define CodeListSize 100 // nbr of Codes floactet = octet + float

typedef struct floactet{
	unsigned char code;
	float value;
}floactet;

typedef struct lbl{
	int n;			//offset pointé par le label dans le programme CodeList
	int adr;	// Adresse du label dans la liste de mnemonique
}lbl;


static int ConvertMnemo(char * MnemoList, floactet * CodeList); //convert Mnemo in codes
static char Octet(char * );
static int CalculOneLine(floactet *CodeList);
static int TreatParenthese(floactet *CodeList);
static int FillCodeOfOneLine(floactet *CodeList,floactet *CodeOfOneLine);
static int HandleInstructions(char *MnemoList,int i);
static void RemoveSpace(char * InstructionLine, int LineSize);
static void ReplaceAccuByValue(floactet *CodeOfOneLine);
static int CheckLabelDef(char *InstructionLine, int LineSize);
static signed int TestForLabels(char *WholeProg, int index);
int CalcMain(char *progtext, int ProgSize);
char	*strErrSerSetup = "Error setting up serial port";


static unsigned char OperatorList[] = "+-*/()=AF";

/*
Code 1 = number
Code 2 = +
Code 3 = -
...
Code 9  =  Accumulateur   [9] [nbr Accu]
Code 10 = Functions (not yet implemented)
Code 11 = Instruction        [11][Instruction nbr]
Code 12 = Code for Label [12][CodeOffsetList]
Code 13 = Maths Functions [13][Code Function]
														exp=1
														 ln = 2
*/



#define OpListSize 9 // add the size if add new instructions or special codes in OperatorList
#define LabelListSize 100
#define MnemoProgSize 100
#define NbrMaxAccu 100
static unsigned char InstructionList[]= "Start_print_goto_<_=>_==_>_";
//																0       1      2       3   4    5    6   
char blabli[]= {'3',0x1,'4',0x0};


lbl Labels[LabelListSize];
unsigned char WholeMnemoProg[MnemoProgSize];
 
int debug=0; // possible values 0=nodebug, 1, 2
int NbrMaxOperationOnLine=100;//in CodeList
int CodeListOffsetMax; //Gets the number of coding instructions
int CodeListOffset;
int CodeOneLineSizeMax=20;
int Iindex,NmaxLbl, TestCondition;

float Accu[NbrMaxAccu];

UInt16 SerIORef;


//------------------------------------------------
static void SerIOReceive(void) {
	ULong		RcvCount = 0;
	Err 		error;
	Byte    msg;
   char szvolts[6],szResult[50], limit[3] ;
	UInt i, value;
	float volts;
   SerSettingsType sstSetup;
	FormPtr 	Frm, frmP;
	FieldPtr	fptr;
   char ValueStr[100];
	char s[1000];
   MemHandle textH;
	MemPtr  progtext;    
	FieldPtr FieldProgTextPtr;
	Frm = FrmGetFormPtr(frmadc16);

	EnableControl(Frm, btnexit, true);
	EnableControl(Frm, btnstart, false);

	FieldProgTextPtr=(FieldPtr)(FrmGetObjectPtr(Frm, (FrmGetObjectIndex(Frm, fld_prog))));
	progtext = FldGetTextPtr(FieldProgTextPtr); //return the ptr to a the lock memory string of the fld_prog
	//this text cannot be modified from progtext, because the memory can be reallocated by the system
	//PrintCmd(progtext);// test ok, this prints the progtextstring to the cmd field
	//TestVersionSerialMgr();
	
	//Rprintf(RMath_ln(1)); //test

	CalcMain(progtext, StrLen(progtext));
	
	if (debug > 0) printf("End\n");
	//	PrintCmd(ValueStr);

}



static void TestVersionSerialMgr(void){         // implémentation regan
	Err error;
	UInt32 value; UInt16 val;UInt32 RomVersion;
    char ValueStr[50] ;
	char s[10];
	error = FtrGet(sysFileCSerialMgr,sysFtrNewSerialVersion,&value);
    val = value; // il s'agit en fait d'un cast UInt32 vers UInt16 pour StrPrintF
    StrPrintF(ValueStr, "SerialMgrLib=%d\n",val);//only  %d, %i, %u, %x, %s, %x
	PrintCmd(ValueStr);
	error = FtrGet(sysFtrCreator,sysFtrNumROMVersion,&RomVersion);
	StrPrintF(ValueStr, "RomVersion=%x\n",RomVersion);
	PrintCmd(ValueStr); 
 }


/* To stdout... */
static void Printf(const char * format, ...) //we use printf which is redefined as Printf at the top
{
	va_list args;
	static Char buf[500];

	va_start(args, format);
	StrPrintF(buf, format, args);
	va_end(args);

	PrintCmd(buf); // Prints in the cmd field
}


static void PrintCmd(char * s){
	FormPtr 	Frm;
	FieldPtr 	FldPtr;
	
	Frm = FrmGetFormPtr(frmadc16);
	FldPtr = (FieldPtr)(FrmGetObjectPtr(Frm, (FrmGetObjectIndex(Frm, fld_cmd))));
	FldInsert(FldPtr, s, StrLen(s));
}
static void PrintProg(char * s){
	FormPtr 	Frm;
	FieldPtr 	FldPtr;
	
	Frm = FrmGetFormPtr(frmadc16);
	FldPtr = (FieldPtr)(FrmGetObjectPtr(Frm, (FrmGetObjectIndex(Frm, fld_prog))));
	FldInsert(FldPtr, s, StrLen(s));
}

static void DeleteCmd(){
	FormPtr 	Frm;
	FieldPtr 	FldPtr;
	
	Frm = FrmGetFormPtr(frmadc16);
	FldPtr = (FieldPtr)(FrmGetObjectPtr(Frm, (FrmGetObjectIndex(Frm, fld_cmd))));
	FldDelete(FldPtr, 0, 5000);//Size of the cmd 5000 maxchars see CalcReg.rcp
	// FldDelete (FieldType *fldP, Uint 16 start , UInt16 end)

}



//--------------------------------------------------------------

 static void ShowNumResultFld (Word FieldID, ULong val) {
	FormPtr 	Frm;
	FieldPtr 	FldPtr;
	char		Buf[10];
	
	Frm = FrmGetFormPtr(frmadc16);
	FldPtr = (FieldPtr)(FrmGetObjectPtr(Frm, (FrmGetObjectIndex(Frm, FieldID))));
	StrIToA(Buf, val);
	FldDelete(FldPtr, 0, FldGetTextLength(FldPtr));
	FldInsert(FldPtr, Buf, StrLen(Buf));
//	EnableField (Frm, FieldID, true); 
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
}


/*
 * MainFormHandleEvent
 */
static Boolean MainFormHandleEvent(EventPtr event) {
	Boolean		handled = false;
	EventType	newEvent;
	char ValueStr[50];
	if (event->eType == ctlSelectEvent) {
		switch (event->data.ctlEnter.controlID) {
			case btnexit:
			   	MemSet(&newEvent, sizeof(EventType), 0);
			   	newEvent.eType = appStopEvent;
			   	EvtAddEventToQueue(&newEvent);
   				break;

   			case btnstart:	
				SerIOReceive();
   				break;

			case btndebug:
				debug=1;
				SerIOReceive();
				debug=0;
   				break;
   			case btntest:	
				StrPrintF(ValueStr, "A1=exp(ln1)\nprintA1\nA1=1\nloop:\nprintA1\nA1=A1+1\nA1<10=>goto loop\n\n");
				PrintProg(ValueStr);
   				break;
   			case btnclear:
				DeleteCmd();
				break;
   		}
		handled = true;
	}

	else if (event->eType == nilEvent) {
		//SerIOReceive ();
		handled = true;
	}
		
	return handled;
}


/*
 * EventLoop
 */
static void EventLoop(void) {
	EventType	event;
	//Word		error;
	
	do {
		EvtGetEvent(&event, evtWaitForever);
		if (! SysHandleEvent(&event))
				if (! MainFormHandleEvent(&event))
					FrmHandleEvent(FrmGetActiveForm(), &event);
	}
	while (event.eType != appStopEvent);
}


/*
 * StartApplication
 */
static void StartApplication(void) {
	Err			Error;
	FormPtr		Frm;
	char information[]="cmd:                                    ";
	char information1[]="creating math library";

	
	// Initialize and draw the main form.
	Frm = FrmInitForm(frmadc16);	
	FrmSetActiveForm(Frm);
	FrmDrawForm(Frm);

	EnableControl(Frm, btnexit, true);
	WinDrawLine(0,90,160,90);
	WinDrawChars(information1, StrLen(information1),0,92);
	CreateMathExpTable();
	CreateMathLnTable();
	WinDrawChars(information, StrLen(information),0,92);
	
	Error = SysLibFind("Serial Library", &SerIORef);
	if (Error) {
		FrmCustomAlert (AlertGenericAlert, strErrSerLibOpen, "can't open library","can't open library" );
		SerClearErr(SerIORef);
		return;		
	}
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





/* ----------------------  Below is the program for the calcReg ---------------------------- 
--------------------------------------------------------------------------------------------------



-----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------*/



// --------------------CalcMain--------------------
 int CalcMain(char *progtext, int ProgSize){
 
//char *InstructionLine = "1+(1+(-1+3*2)-1)*2\n";
int LineSize=30;
// int ProgSize;  //receives the size of the WholeMnemoProg
char InstructionLine[LineSize];
int Error,i,a,b,nbrLine,NbrCodesCopied,OffsetLine;

//char MnemoList[MnemoListSize]; // octet ascii for mnemo codes
floactet CodeList[CodeListSize]; //Whole List 
floactet CodeOfOneLine[CodeOneLineSizeMax]; //Code Of One Line

int AccIndex,lblptr, istrt,pos,offsetP,k,K;
char s[50];

		//fill WholeMnemoProg
		if (ProgSize!=0) for (i=0;i<ProgSize;i++){WholeMnemoProg[i]=progtext[i];}
//		StrPrintF(WholeMnemoProg, "A1=1\nprintA1\nloop:\nA1=A1+1\nA1<4=>goto loop\nprintA1\n");
		WholeMnemoProg[ProgSize]=0;
		ProgSize = StrLen(WholeMnemoProg);
		if (debug>0){StrPrintF(s,"Size=%d bytes\n",ProgSize); 
		PrintCmd(s);}
		//PrintCmd(WholeMnemoProg);

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

		if (debug >0) printf("pass1... ");
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
		
	if (debug>0) PrintCmd(InstructionLine);
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
		if (Error!=0) {printf("Error %d\n",Error); return(0);}
	}
	CodeList[CodeListOffsetMax].code=0;   //Signal for End of Program
	CodeList[CodeListOffsetMax].value=-1;


	if (debug >0) {	printf("ok\n");StrPrintF(s," %d Lines,  %d codes  \n", nbrLine,CodeListOffsetMax);
	PrintCmd(s);}
	
	//Up to that point, the codeList contains [12][numerolbl] 
	//[12][OffsetCodeListPointerlbl]
	//PutOffsetLabelsInCodeList:	
	for  (i=0;i<CodeListOffsetMax;i++){//Transfert the numbers of the labels to the pointed CodeList value
			if (CodeList[i].code==12){; //Code for Labels
				pos=CodeList[i].value;
			CodeList[i].value=Labels[pos].n;
	}}
	if (debug > 0) {
		printf ("---------------------------\n");
		for  (i=0;i<CodeListOffsetMax;i++){
			a=CodeList[i].code;
			b=CodeList[i].value;
			StrPrintF(s, " %d [%d] [%d]\n",i,a,b);PrintCmd(s);
			}
		printf ("---------------------------\n");
		}


	if (debug > 0) printf("LAUNCHING... \n");
	CodeListOffset=0;
	OffsetLine=0;
LoopCodeProgram: //-----------------------------------Loop-----------------------------------
	if (debug > 0 ) {StrPrintF (s,"-starting line code %d--\n",CodeListOffset);PrintCmd(s);}

	TestCondition=0; //each line starts we set the condition test to zero.

	
	NbrCodesCopied=FillCodeOfOneLine(CodeList,CodeOfOneLine);//transfert one line 

	CodeListOffset=CodeListOffset+NbrCodesCopied; 						//Prepare for next line
	ReplaceAccuByValue(CodeOfOneLine);

	Error = TreatParenthese(CodeOfOneLine);													//parenthese
	if (Error != 0) printf ("Treatment parenthese Error = %d \n", Error);

	Error = CalculOneLine(CodeOfOneLine); //Calcul
	if (Error != 0) {printf ("Error = %d \n", Error);goto EndMain;}

		// => Instruction
		if (debug > 1) printf("Test condition= %d\n",TestCondition);
//		printf("[%d][%f]\n",CodeOfOneLine[OffsetLine].code,CodeOfOneLine[OffsetLine].value);
//		printf("[%d][%f]\n",CodeOfOneLine[OffsetLine+1].code,CodeOfOneLine[OffsetLine+1].value);
//		printf("[%d][%f]\n",CodeOfOneLine[OffsetLine+2].code,CodeOfOneLine[OffsetLine+2].value);
//		printf("[%d][%f]\n",CodeOfOneLine[OffsetLine+3].code,CodeOfOneLine[OffsetLine+3].value);
		if (CodeOfOneLine[OffsetLine].code==0xFF || CodeOfOneLine[OffsetLine+1].code==0xFF || CodeOfOneLine[OffsetLine+2].code==0xFF) goto KeepOn;
		if (CodeOfOneLine[OffsetLine+3].code==11 && 
					CodeOfOneLine[OffsetLine+3].value==4 ) if ( TestCondition == 1) {//test 
			TestCondition=0; 	OffsetLine = OffsetLine+4;
			if (debug > 2 ) {printf("test Condition Positif =>  \n");}			
			goto KeepOn; 
		}else { OffsetLine=0; goto AlmostEndLoop;} //next instruction
		
	KeepOn:
	//handle A...= ...
	if (CodeOfOneLine[OffsetLine].code==9) {//Accu detected
		AccIndex = CodeOfOneLine[OffsetLine].value;
		if (CodeOfOneLine[OffsetLine+1].code == 8 ){ 
				if (CodeOfOneLine[OffsetLine+2].code == 1){ Accu[AccIndex]=CodeOfOneLine[OffsetLine+2].value;
			if (debug >1 ) printf("Accu[%d] = %f \n",AccIndex, Accu[AccIndex]);  
			}else printf("Error Accu no number present to load\n");
		}}


	//Handle Instructions
	if (CodeOfOneLine[OffsetLine].code==11 ) {//Special Instruction  detected
		if (CodeOfOneLine[OffsetLine].value==1) {//Print 
			if (debug > 2 ) printf("print\n");
				if (CodeOfOneLine[OffsetLine+1].code==1) {
					//StrPrintF(s,"%E\n",CodeOfOneLine[OffsetLine+1].value);PrintCmd(s);
				//StrPrintF(s,"%d\n",(int)CodeOfOneLine[OffsetLine+1].value);PrintCmd(s);
				Rprintf(CodeOfOneLine[OffsetLine+1].value);//implementation for display float
				}
				if (CodeOfOneLine[OffsetLine+1].code!=1) {
					printf("error no number to display\n ");
					printf ("The code %d is [%d][%f]\n",OffsetLine,CodeOfOneLine[OffsetLine].code,CodeOfOneLine[OffsetLine].value);
					printf ("ptr code   %d is [%d][%f]\n",OffsetLine+1,CodeOfOneLine[OffsetLine+1].code,CodeOfOneLine[OffsetLine+1].value); 
					goto EndMain;
				}
		}
		if (CodeOfOneLine[OffsetLine].value==2) {//goto 
			if (debug > 2 ) {printf("goto \n");}
				if (CodeOfOneLine[OffsetLine+1].code==12) {//the label
				CodeListOffset = CodeOfOneLine[OffsetLine+1].value;
				OffsetLine=0;
				}
				else {
					printf("error no label for goto !\n"); goto EndMain;
				}
		}
	}

	AlmostEndLoop:
		OffsetLine=0; //Important before going back from here to LoopCodeProgram	
		if  (CodeList[CodeListOffset].code!=0 && CodeListOffset<CodeListOffsetMax ) goto LoopCodeProgram;
	//-------------------------------------------------------EndLoop---------------------------------
	if (debug > 0 )printf("End\n");
EndMain:
	return (0);
 }

 static int CheckLabelDef(char *InstructionLine,int LineSize){
	int i=0; char s[50];
	while (i<LineSize && InstructionLine[i] != 0x0A){
		//printf("InstructionLine[%d]=%c\n",i,InstructionLine[i]);
		if (InstructionLine[i] == Octet(":") ) {
			if (debug >0) {StrPrintF (s,"lbl:  %dletters \n",i);PrintCmd(s);} 
		return (i);}
		i++;
	}
	return 0;
 }
 
 static void RemoveSpace(char * InstructionLine, int LineSize){
	int i=0; int p=0;
		LoopRS:
	while (i<LineSize){
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
char StringNbr[100];
char s[50];
int StrtStr,EndStr,p,a;

		if (MnemoListLine[i]==Octet("\n") )  {Out=1; goto EndConvertNoData;} //endLine at start or No data
	StartConvList:
	//Check for instructions Mnemoniques such as "print", "plot"
		//printf ("now pointer on %c\n",MnemoListLine[i]);

		//--- Search for instructions --------
	//for (NbrTest=0; NbrTest<2; NbrTest++){//Could be more than one instruction on one line
	InstructionNumber = 1; // Set to 1. If no more instruction takes 0 value.
	while(InstructionNumber !=0){
		Iindex=i;
		//printf("MnemoListLine[%d]=%c\n",i,MnemoListLine[i]);
		InstructionNumber=HandleInstructions(MnemoListLine,Iindex);
		i=Iindex;
		//if (debug > 1) printf("Instruction Number = %d \n",InstructionNumber);
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
		
		if (MnemoListLine[i]==Octet("-") ) {opposite=1; i++; }//sign for the number 
	while (Out==0){

		if (MnemoListLine[i]==Octet("\n") )  {Out=1; goto EndNbr;} //endLine or No data
		if (MnemoListLine[i]==OperatorList[4] ) { 
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
		if (FunctionNumber !=0) {
			if (debug > 0) printf("function coded\n");
			CodeList[CodeListOffset].code=13;	//General Code Math Instruction
			CodeList[CodeListOffset].value=FunctionNumber; 	//Specific code (example: exp=1, ln =2,...)
			CodeListOffset++;
			goto StartConvList;
		}//end coding search function
//	}
		
		
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
					CodeList[CodeListOffset].code = 9;       // It is a number: code=0
					CodeList[CodeListOffset].value = Nbr; // Floating point Value is set
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
				goto StartConvList; //Return in the loop
			}//End Coding Accu

			
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
		/*
		val= MnemoListLine[i]-Octet("0");
		if (debug > 0) {if (val >0 ) printf ("val = %d \n",(int) val); else printf ( "sign detected \n");}
		if (val >= 0 && val <=9) Nbr = Codage*Nbr + val ; 

		else {
			if (opposite==1)  {opposite=0; Nbr= -Nbr;} 
			CodeList[CodeListOffset].code = 1;       // It is a number: code=0
			CodeList[CodeListOffset].value = Nbr; // Floating point Value is set

			if (debug > 1) printf("Le nombre est %f \n",Nbr);
			Nbr=0;// Reinitialise  Nbr for next value
			CodeListOffset++;
			goto EndNbr;// i points on the operator
			}
		i++;
		goto HandleNbr;
		*/
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

			if (MnemoListLine[i]==Octet("<") )  {
			goto StartConvList; //The code will be put by instruction found
			}
			if (MnemoListLine[i]==Octet(">") )  {
			goto StartConvList; //The code will be put by instruction found
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
 
 
 static char Octet(char * StringPointer){
	return StringPointer[0];
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
	char s[50];
	
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
	char s[50];
	
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
int iptrstrt=0; int Aind;
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
			if (debug ==1) printf("accuvalue = %f \n",Accu[Aind]);
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
 floactet BufferLineCode[NbrMaxOperationOnLine];
	char s[50];

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
		if(LevelParMax<CountLevelPar) {
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
	if (Error != 0) {printf ("Error calculation in the parenthese = %d\n", Error); ErrorCode=2;} 
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
	return ErrorCode;
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
	char s[50];
	i=0;
	imaxLine=0; iptrEqualSignP=0;
	while (i<NbrMaxOperationOnLine){
	if (CodeListLine[i].code == 0xFF) {imaxLine=i; goto SearchPriority;}
	if (CodeListLine[i].code == 8) iptrEqualSignP=i+1; //detect and save equal code position+1 

	
	if (CodeListLine[i].code==13 && CodeListLine[i+1].code==1) {//Math Function
		if (CodeListLine[i].value==1 ) {val=RMath_exp(CodeListLine[i+1].value);}//exp
		if (CodeListLine[i].value==2 ) {val=RMath_ln(CodeListLine[i+1].value);}	//ln		
		if (CodeListLine[i].value==3 ) {val=RMath_sqrt(CodeListLine[i+1].value);}	//sqrt		
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
		if (CodeListLine[i].code == 4 ) goto Multiplication;
		if (CodeListLine[i].code == 5 ) goto Division;
		if (CodeListLine[i].code == 6 ) {ErrorCode=4; goto EndCalculOneLine;}
		if (CodeListLine[i].code == 0xFF ) goto NoPriority;
		if (CodeListLine[i].code == 0 ) goto NoPriority;
		i++;
		}
		
	Multiplication:
		//i points on the multiplie *
		i-- ; //place on the left number
		if (CodeListLine[i].code!=1 || CodeListLine[i+2].code != 1) {ErrorCode=2; goto EndCalculOneLine; }
		val = CodeListLine[i].value * CodeListLine[i+2].value;
		CodeListLine[i].code=1; //number
		CodeListLine[i].value=val; // new value
		i++;
		if (debug ==1) printf ("multiplication result : %f \n",val);	
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
		if (debug ==1) printf ("division result : %f \n",val);	
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
			if (debug > 0) {
				StrPrintF (s, "Line value is %d \n",(int) val); PrintCmd(s);
				//printf("Results : Code 1  [%d]   [%f]\n",	CodeListLine[iptrEqualSignP].code,CodeListLine[iptrEqualSignP].value);
				//printf("Results : Code 2  [%d]   [%f]\n",	CodeListLine[iptrEqualSignP+1].code,CodeListLine[iptrEqualSignP+1].value);
				}
	return ErrorCode;
 }


 static float Rsscanf (char *s,char *c,float *Nbr){
	//int size=StrLen(s);
	int size=strlen(s);
	//printf("size=%d",size);
	float val=0;
	float savenbr=0;
	float nbr=0; float number;
	int Exp=0;int i=0; int ie; int exposant;
	float neg=1;int ip=0;
	for (i=0;i<size;i++){
		if (s[i]==Octet("E")) {savenbr=nbr*neg; neg=1;Exp=1;nbr=0; ie=i; i++;}
		if (s[i]==Octet(".")) {ip=i;i++;}
		if (s[i]==Octet("+")) {i++;}
		if (s[i]==Octet(" ")) {i++;}
		if (s[i]==Octet("-")) {neg=-1;i++;}
		val = s[i]-Octet("0");
		if (val <=9 && val >=0 ) nbr=nbr*10+val; 
		}
		nbr=nbr*neg;

		//synthèse du nombre
	if (Exp==1){exposant = nbr ; number = savenbr;
						if(ip!=0) number = number / TenPower(ie-ip-1);
	}else{ number = nbr; if(ip!=0) number = nbr / TenPower (i-ip-1);}
	if (Exp==1) number = number *TenPower (exposant);
	Nbr[0]=number;
	return number;
	}

static void Rprintf(float x){
	int Pres=3; int i;
	char s[20],s2[20],*c;
	float X,alfa,precision;
	int n=0; int opp=0;
	if (x==0) {printf(" 0\n"); return;}
	precision=TenPower(-Pres);
	if (x<0) {x=-x;opp=1;}	
	X=x;
	if (x>=precision)while(X>=precision){X=X/10; n++;}
	if (x< precision){while(X<=precision){X=X*10; n--;}n++;}

	alfa=x*TenPower(2*Pres+1-n);
	if ( (alfa*10-10*(UInt32)alfa) >5) alfa=alfa+1; //if the truncate properly.
	
	StrPrintF(s,"%d",(int)alfa);
	
	if (opp==1) s2[0]=Octet("-"); 
	else s2[0]=Octet(" ");
	s2[1]=s[0];	s2[2]=Octet(".");
	i=1;
	while(s[i]!=0){s2[i+2]=s[i]; i++;}
	s2[i+2]=Octet("E");
	c=s2+i+3;
	StrPrintF(c,"%d\n",n-4);
	PrintCmd(s2);
	}
	
static int strlen(char *s){
	int i;
	for (i=0;i<20;i++){if (s[i]==0) return i;}
	return 0;//error
	}
	
static float TenPower (int exp){//10^exp, with exp pos or neg
		float n=1;int i; 
		if (exp >=0) {for (i=0;i<exp;i++) n=n*10;}
		else{ for (i=0;i<-exp;i++) n=n/10;}
		return n;
		}
 
 static float RMath_exp(float x){//value on range: exp(-+SizeMathExpTable) 
	//SizeMathExpTable = 100 most likely to set
	int a,Inv;
	float m,ea,eb,ec,A,b,c,Px ;
	Inv=0;

	//x=2*x;//to be consistant with e^(1/2)
	x=4*x;//to be consistant with e^(1/4)
	if (x<0) {x=-x;Inv=1;}//Invert for e(-x) 

	a=(int) (x);
	
	if (a+1>SizeMathExpTable) {MathError=1; return 0;}//The error will be collected at the loop program
	ea= MathExpTable[a];
	eb= MathExpTable[a+1];
	ec= MathExpTable[a+2];
	m=eb-ea;//coeff directeur approximation linéaire
	A=a; b=a+1;c=a+2;
	Px= (x-b)*(x-c)*ea/2-(x-A)*(x-c)*eb+(x-A)*(x-b)*ec/2;//Polynome  interpolateur Lagrange
	if (Inv==0) return Px;
	else return 1/Px;
		
	//if (Inv==0) return (m*(x-(float)a)+ea); //linear
	//else return 1/(m*(x-(float)a)+ea);//
	}
	
static void CreateMathExpTable(){
	//float e=2.718281828;
	//float e2=1.648721271; //e^(1/2)
	float e4=1.284025417; //e^(1/4)
	float x=1;//set x to 1
	int i;
	if (debug>0) printf("Create exp\n");
	for (i=0;i<SizeMathExpTable;i++){
		MathExpTable[i]=x;
		x=x*e4; //e^(1/2)
		}
	}
	
	
static void CreateMathLnTable(){
	int k,l,OK,bug=0;
	float dx,x,a,b,N,n,alfa=1,x0;
	char s[50];
	
	if (debug>0) printf("Create Ln\n");

	for (k=0;k<SizeMathLnTable;k++){MathLnTable[k]=0;}//clean to zero
	//fill base ln numbers.
	MathLnTable[0]=-1E20;//-inf
	MathLnTable[1]= 0;
	MathLnTable[2]= 0.693147181;
	MathLnTable[3]= 1.098612289;
	MathLnTable[4]= 1.386294361;
	MathLnTable[5]= 1.609437912;
	MathLnTable[6]= 1.791759469;
	MathLnTable[7]= 1.945910149;
	MathLnTable[8]= 2.079441542;
	MathLnTable[9]= 2.197224577;
	MathLnTable[10]= 2.302585093;
	MathLnTable[11]= 2.397895273;
	MathLnTable[13]= 2.564949357;
	MathLnTable[17]= 2.833213344;
	MathLnTable[19]= 2.944438979;
	MathLnTable[23]= 3.135494216;
	MathLnTable[29]= 3.367295830;
	MathLnTable[31]= 3.433987204;
	MathLnTable[37]= 3.610917913;
	MathLnTable[41]= 3.713572067;
	MathLnTable[43]= 3.761200116;
	MathLnTable[47]= 3.850147602;
	MathLnTable[53]= 3.970291914;
	MathLnTable[61]= 4.110873864;
	MathLnTable[67]= 4.204692619;
	MathLnTable[59]= 4.077537444;
	MathLnTable[71]= 4.262679877;
	MathLnTable[73]= 4.290459441;
	MathLnTable[79]= 4.369447852;
	MathLnTable[89]= 4.488636370;
	MathLnTable[94]= 4.543294782;
	MathLnTable[97]= 4.574710979;

	//by multiplication create the other multiples 
	//first littles for calculations of 14,15,..20
	for (k=2;k<4;k++){for (l=2;l<11;l++){if(k*l<SizeMathLnTable)MathLnTable[k*l]=MathLnTable[k]+MathLnTable[l];}
		}
	//full calculation of multiples
	for (k=2;k<11;k++){for (l=2;l<47;l++){if(k*l<SizeMathLnTable)MathLnTable[k*l]=MathLnTable[k]+MathLnTable[l];}
		}
	/* //add the nbr premier (non multiples like 17)
		for (k=2;k<SizeMathLnTable-1;k++){//No calculation after 10*20
			if (MathLnTable[k]==0){StrPrintF(s,"missing ln(%d)\n",k);
													PrintCmd(s);}
		}//check forgottens !  */
					
	}

static float RMath_ln(float x){//value on range: x>0 
	//SizeMathLnTable = 101 but we use 100 data
	int a,exponant;
	float m,lna,lnb,lnc,n,mantisse,A,b,c,Px;
	float ln10=2.302585093;
	n=10;// the ratio is 10 for use ln(0) to ln(100) table
	if (x<0) {MathError=2; return 0;} //The error will be collected at the loop program

	GetMantisseExponant(x, &exponant, &mantisse);
	a = (int) (10*mantisse); //we choose the technic
	
	if (a+1>SizeMathLnTable) {MathError=2; return 0;}//The error will be collected at the loop program
	lna= MathLnTable[a];
	//Rprintf(lna);
	lnb= MathLnTable[a+1];
	//Rprintf(lnb);
	m=lnb-lna;//coeff directeur approximation linéaire

	//lnc= MathLnTable[a+2];

	//A=a/10; b=a/10+1;c=a/10+2;
	//Px= (mantisse-b)*(mantisse-c)*lna/2-(mantisse-A)*(mantisse-c)*lnb+(mantisse-A)*(mantisse-b)*lnc/2+ ((float)exponant-1)* ln10;//Polynome  interpolateur Lagrange
	//return Px;

	 //! we don't take x, we take the mantisse.
	return (m*(mantisse-(float)a/10)+lna) + ((float)exponant-1)* ln10;   // ln(x=a10^b)= ln(a)+b ln10
	
	}

static void GetMantisseExponant(float x, int *exponant, float*mantisse){
	//works only for positive numbers x.
	int Pres=3; int i;
	float X,precision;
	int n=0; 
	char s[50];

	if (x==0) {exponant[0]=0; mantisse[0]=0; return;}
	precision=TenPower(-Pres);
	X=x;
	if (x>=precision)while(X>=precision){X=X/10; n++;}
	if (x< precision){while(X<=precision){X=X*10; n--;}n++;}
	
	mantisse[0]=x*TenPower(Pres+1-n);
	exponant[0]=n-4;
	//StrPrintF(s,"mantisse*1000= %d, exponant%d\n",(int)(1000*mantisse[0]),(int) n-4);
	//PrintCmd(s);
	}
	
static float RMath_sqrt(float x){//value on range: x>0
	return RMath_exp(0.5*RMath_ln(x));
	}
	
