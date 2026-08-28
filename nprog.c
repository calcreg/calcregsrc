/* -------------------------------------
 * BUSSY-SOCRATE REGAN
 * This is the first calculation base
 * Mathematical syntax analysis
 * ... and calculation
 * Syntax is trasnformed according to highest math priority
 *
 * Basic program instruction is designed as well
 * ------------------------------------
 *
#include <stdio.h>
#include<stdlib.h>

#define MnemoListSize 1000
#define CodeListSize 1000 // nbr of Codes floactet = octet + float

typedef struct floactet{
	unsigned char code;
	float value;
}floactet;

typedef struct lbl{
	int n;		//offset pointé par le label dans le programme CodeList
	int adr;	// Adresse du label dans la liste de mnemonique
}lbl;


int main(int argc, char *argv[]);
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

char	*strErrSerSetup = "Error setting up serial port";


static unsigned char OperatorList[] = "+-*/()=AF";

/*
Code 1 = number
Code 2 = +
Code 3 = -
...
Code 9  =  Accumulateur   [9] [nbr Accu]
Code 11 = Instruction        [11][Instruction nbr]
Code 12 = Code for Label [12][CodeOffsetList]

*/

#define OpListSize 9 // add the size if add new instructions or special codes in OperatorList
#define LabelListSize 100
#define MnemoProgSize 1000
#define NbrMaxAccu 100
static unsigned char InstructionList[]= "Start_print_goto_<_=>_==_>_";
//																0       1      2       3   4     5      6
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


// --------------------Main--------------------
 int main(int argc, char *argv[]){
//char *InstructionLine = "1+(1+(-1+3*2)-1)*2\n";
int LineSize=30;
int ProgSize;  //receives the size of the WholeMnemoProg
char InstructionLine[LineSize];
int Error,i,nbrLine,NbrCodesCopied,OffsetLine;
FILE *fp; //pointer on the source file

char MnemoList[MnemoListSize]; // octet ascii for mnemo codes
floactet CodeList[CodeListSize]; //Whole List 
floactet CodeOfOneLine[CodeOneLineSizeMax]; //Code Of One Line

int AccIndex,lblptr, istrt,pos;

/*	if(argc <3 ) {printf("too few argument: should be [debug] [progfile]\n"); goto EndMain;}
	printf("%s\n",argv[1]);
	if (argv[1]=="1") debug=1;
*/
	if(argc == 3)debug=3;
	printf("loading %s\n", argv[1]); //remind the name of the file loaded
	//open to build label list
	if ( (fp=fopen (argv[1],"r") )==NULL){printf("\n Error can't open file\n"); exit (1);}
	ProgSize=0;
		while (fgets (InstructionLine, LineSize,fp)!=NULL){
			i=0;
			while (i<LineSize && InstructionLine[i] !=0x0A){//code return est 0x0A0D
				WholeMnemoProg[ProgSize+i]=InstructionLine[i];
			i++;
			}
			WholeMnemoProg[ProgSize+i]=InstructionLine[i];i++;
			ProgSize=ProgSize+i;
		}
		printf("ProgSize %d octet\n",ProgSize); 
		istrt=0; i=0;
		lblptr=0;
		while (i<ProgSize){
			if(WholeMnemoProg[i] == 0x0A || WholeMnemoProg[i] ==0x0D) istrt=i+1;
				if (WholeMnemoProg[i] == Octet(":") ){
					Labels[lblptr].adr=istrt; //situation of the Label in WholeMnemoProg 
					if (debug > 1) printf("label found Labels[%d].adr= %d\n",lblptr,istrt);
					lblptr++;
				}
				//printf ("WholeMnemoProg[%d]= %c\n",i,WholeMnemoProg[i]);
				i++;
		}
		NmaxLbl=lblptr;
		printf ("Program\n %s\n",&WholeMnemoProg[0]);

	fclose(fp);

	//reopen
	if ( (fp=fopen (argv[1],"r") )==NULL){printf("\n Error can't open file\n"); exit (1);}

	printf("pass1... ");
	nbrLine=0; lblptr=0;
	CodeListOffset=0;
	CodeListOffsetMax=0; // Will Gives the Nbre of coding instructions in the coding program
	while (fgets (InstructionLine, LineSize,fp)!=NULL){
		i=0;
		while(i<LineSize){if(InstructionLine[i]==0x0D) InstructionLine[i]=0x0A; i++;}//change the return code
		RemoveSpace(InstructionLine,LineSize); //remove the spaces " " from the line instruction
		if (debug > 0) 	printf("\n------converting Line -------%s\n", InstructionLine);
		if (CheckLabelDef(InstructionLine,LineSize) == 0) { 
			//if (debug > 2) printf("No label \n");
			Error = ConvertMnemo(InstructionLine, CodeList); //InstructionLine = Mnemolist for the test
		}else {
				Labels[lblptr].n=CodeListOffset;
				if (debug > 2) printf("label found Labels[%d].n= %d\n",lblptr,CodeListOffset);
				lblptr++;
		}
		nbrLine++;
		if (Error!=0) {printf("Error %d\n",Error); fclose(fp);exit(1);}
	}
	CodeList[CodeListOffsetMax].code=0;   //Signal for End of Program
	CodeList[CodeListOffsetMax].value=-1;

	printf("ok");
	printf(" %d Mnemo Lines  %d coded floactet  \n", nbrLine,CodeListOffsetMax);

	fclose(fp);//close file

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
		for  (i=0;i<CodeListOffsetMax;i++){printf( "   %d [%3.d] [%f]\n",i,CodeList[i].code,CodeList[i].value);}
		printf ("---------------------------\n");
		}

	
	printf("LAUNCHING... \n");
	CodeListOffset=0;
	OffsetLine=0;
LoopCodeProgram: //-----------------------------------Loop-----------------------------------
	if (debug > 0 ) printf ("-------------starting line code %d-----------\n",CodeListOffset);

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
					printf("%E\n",CodeOfOneLine[OffsetLine+1].value);
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
	int i=0;
	while (i<LineSize && InstructionLine[i] != 0x0A){
		//printf("InstructionLine[%d]=%c\n",i,InstructionLine[i]);
		if (InstructionLine[i] == Octet(":") ) {printf ("labels word length:%d \n",i); return (i);}
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
int NbrTest=0;
signed int LabelNbrFound;
char StringNbr[100];
int StrtStr,EndStr,p;

		if (MnemoListLine[i]==Octet("\n") )  {Out=1; goto EndConvertNoData;} //endLine at start or No data
	StartConvList:
	//Check for instructions Mnemoniques such as "print", "plot"
		//printf ("now pointer on %c\n",MnemoListLine[i]);

	for (NbrTest=0; NbrTest<2; NbrTest++){
		Iindex=i;
		//printf("MnemoListLine[%d]=%c\n",i,MnemoListLine[i]);
		InstructionNumber=HandleInstructions(MnemoListLine,Iindex);
		i=Iindex;
		//if (debug > 1) printf("Instruction Number = %d \n",InstructionNumber);
		if (InstructionNumber !=0) {CodeList[CodeListOffset].code=11;//General Code Instruction
		CodeList[CodeListOffset].value=InstructionNumber;
		CodeListOffset++;
		}
	}
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
					if (debug > 0) printf("Accu A%f \n",Nbr);
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
			if (MnemoListLine[i] == OperatorList[iop]) goto OpCodeFound;		
		}

			
		//--- Number Handling here --- 
	HandleNbr:
		/*
		val= MnemoListLine[i]-Octet("0");
		if (debug > 0) {if (val >0 ) printf ("val = %d \n",val); else printf ( "sign detected \n");}
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
		sscanf (StringNbr,"%f",&Nbr);
		if (debug>0) printf("StringNbr= [%s], Nbr = %f \n",StringNbr,Nbr);
		if (opposite==1)  {opposite=0; Nbr= -Nbr;} 
		CodeList[CodeListOffset].code = 1;       // It is a number: code=0
		CodeList[CodeListOffset].value = Nbr; // Floating point Value is set
		CodeListOffset++;
		if (debug > 1) printf("Le nombre est %f \n",Nbr);
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
			if (MnemoListLine[i]==Octet("\n") )  {Out=1; }//endLine
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
 
 int HandleInstructions(char *MnemoListLine, int i){//return the number instruction starting on line
	int LengthInstructionMax = 30;// exemple: length "print" = 5
	int j=0; int k=0;
	int NInstr=0; int Ok=0;
	while( j < sizeof(InstructionList) ){
		//printf("InstructionList[%d] = %c\n",j,InstructionList[j]);
		if (MnemoListLine[i] == InstructionList[j]){
			if (debug==1)printf("j= %d, Mnemo[%d]=%c\n",j,i,MnemoListLine[i]);
			k=0;
			LoopTestInstruction:
			if (debug ==1 )printf("Mnemo[%d+%d]=%c\n",i,k,MnemoListLine[i+k]);

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
	if (debug == 1) {printf("Parenthese Result BufferLineCode:\n");
							printf("        [%d]   [%f]\n",BufferLineCode[0].code,BufferLineCode[0].value);
							printf("        [%d]   [%f]\n",BufferLineCode[1].code,BufferLineCode[1].value);
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
	int i; int imaxLine=0;
	float val,x1,x2;
	int ErrorCode=0;
	int iptrEqualSignP; //Used to save the position of the equal sign in the line
	
	i=0;
	imaxLine=0; iptrEqualSignP=0;
	while (i<NbrMaxOperationOnLine){
	if (CodeListLine[i].code == 0xFF) {imaxLine=i; goto SearchPriority;}
	if (CodeListLine[i].code == 8) iptrEqualSignP=i+1; //detect and save equal code position+1 

	if (CodeListLine[i+1].code==11 &&CodeListLine[i+1].value==3) {//Test "<"
		if (CodeListLine[i].code !=1 && CodeListLine[i+2].code !=1){ErrorCode = 5; goto EndCalculOneLine;}
		else { if ( CodeListLine[i].value < CodeListLine[i+2].value) {
					TestCondition=1; }
					i=i+3;goto EndCalculOneLine;
					}
	}
	if (CodeListLine[i].code == 11) iptrEqualSignP=i+1; //detect instruction and save code position+1 

	
	if (debug == 1) {
	printf ("            Code %d  [%d]   [%f] \n",i,CodeListLine[i].code,CodeListLine[i].value);
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
			if (debug == 1) {
				printf ("The line value is %f \n",val);
				printf("Results : Code 1  [%d]   [%f]\n",	CodeListLine[iptrEqualSignP].code,CodeListLine[iptrEqualSignP].value);
				printf("Results : Code 2  [%d]   [%f]\n",	CodeListLine[iptrEqualSignP+1].code,CodeListLine[iptrEqualSignP+1].value);
				}
	return ErrorCode;
 }
