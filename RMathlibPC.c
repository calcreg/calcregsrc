/* ---------------
 *  Custom Math Lib
 *
 * BUSSY-SOCRATE REGAN
 *
 *  --------------*/


/* Includes */
#include <windows.h>			// system
#include <Stdio.h>

//compatibilityReg.c
extern void WinDrawLine(int , int, int,int);
extern void SelectionStylo(COLORREF);

//windows stuff
extern HWND hmywin;
extern HDC hDC; 


//#include "CalcReg.h"		// app
//matrix
typedef struct Matrix{
	int n;	//n line
	int p;	//p column
	float * ptr; //pointer on the matrix
	}Matrix;


typedef struct floactet{
	unsigned char code;
	float value;
	float cmplx;
}floactet;




extern int debug;
extern int StartInfoDone;
extern int SerialReady; //Serial device opened =1, closed =0

extern void SetUpTextProg(int position);
extern void DeleteProg();
extern void PrintProg(char * s);
extern float Oscilloscope(float a);
extern void Delete_Cmd_Win();

//-------exponential----------  MathError=1 out of range exponential
//#define SizeMathExpTable 1600
//static float MathExpTable[SizeMathExpTable];

//-------logarithme neperien----------  MathError=2 out of range Ln

//#define SizeMathLnTable 102 //Not below 100 !
//float MathLnTable[SizeMathLnTable];
//------- x^y ------Power -------------- MathError =3,4,5 undefined 0^0, 1/0, impossible sign (-1)^(1/2)

//------- acos ----------  MathError=6 out of range acos, asin

void PrintCmd(char * s);
void  Printf(const char * format, ...) ; //should be Printf and not printf otherwise it conflicts with StdIOPalm.h definition
void HideKeyPad() ;
void ShowKeyPad() ;

char Octet(char * StringPointer) ; 
float Rsscanf (char *StringNbr,char *s ,float *Nbr) ;
float TenPower (int exp) ;
void DeleteCmd() ;
void SaveProg();
void LoadProg();
int CompareVarNames(char* txt, int i1, int i2);
void RemoveComments(char *txt);

void Rprintf(float x);//write and enter
void REPrintf(float x);//write nbr but don't enter
void decode(char *txt, int size);



int CalculFunctionComplexe(floactet *CodeListLine,int i);

//Math functions
void CreateMathExpTable();
float RMath_exp(float x);//value on range: exp(-+SizeMathExpTable) 
float RMath_ln(float x);//value on range: x>0 
void GetMantisseExponant(float x, int *exponant, float*mantisse);
float RMath_sqrt(float x);
float RMath_sin(float x);
float RMath_cos(float x);
float  RMath_tan(float x);
float  Rpow(float x, int a);//x^a, a dans N
float fact(int k);
float RMath_Pow(float x, float a);
float RMath_Pow_cmplx(float x, float y, float a,float b,float * val_cmplx);
float RMath_ch(float x);
float RMath_sh(float x);
float RMath_th(float x);
float logarithm(float x);
float RMath_asin(float x);
float RMath_acos(float x);
float RMath_atan(float x);
float RMath_ath(float x);
float RMath_ash(float x);
float RMath_ach(float x);
float RMath_abs(float x);
float OldRMath_cos(float x);
static cos_sin(float x );
static float Rabs(float x);

void LowPerformance();

void TracerAxis(int centerx,int centery,int width, int height);//color 0black 1red 2green 3blue
void Tracer3DAxis();
float Dx(float x, float y, float z);
float Dy(float x, float y, float z);
void Line(float x1, float y1, float x2, float y2, float Color); //line x1,y1,x2,y2,color 0black 1red 2green 3blue
void FloatToString(float value, char *buffer, int Rounding);

//Matrix

int MatrixPower(Matrix *MAccu,floactet *CodeListLine,int i,int imaxLine);
int MatrixSubAddition(Matrix *MAccu,floactet *CodeListLine,int i,int iptrEqualSignP,int imaxLine);
int MatrixMultiplication(Matrix *MAccu,floactet *CodeListLine,int i, int imaxLine);
int MatrixDivision(Matrix *MAccu,floactet *CodeListLine,int i,int imaxLine);
int MathFunctionMatrices (Matrix *MAccu, floactet *CodeListLine,int i);



//Math dimensions of the square for drawing (the hardware display is rearranged afterwards)
float DimXmin=-3;
float DimXmax=3;
float DimYmin=-5;
float DimYmax=5;
float IncX=0.05;
float StepX=1;

//3D plots
float zp=15,yp=7,xp=6,zp0=15,yp0=7,xp0=6;//should be proportional to the 3D dimension box
float Xmin3d=-3,Xmax3d=3,Ymin3d=-3,Ymax3d=3,Zmin3d=-3,Zmax3d=3;
float Inc3D=0.2;


//Drawing Zone Square -------- Be careful it is redefined in CalcRegMain for window-resizing
float DrawZoneX=320;//125;
float DrawZoneY=25;
float DrawZoneH=300;
float DrawZoneW=450-195;//450


extern int GridSet; //for the grid =0 grid off, =1 grid on color Red
extern int ColorGraph; //the color of the graph
extern int Keypad;
extern int GfxBigDisplay;


//math
	float e_1024=1.000977039;//exp(1/1024)
	float	e001=1.0019550359;//exp(1/512)
	float	e002=1.003913889;
	float	e004=1.007843097;
	float	e008=1.015747709;
	float	e010=1.031743407;
	float	e020=1.064494459;
	float	e040=1.133148453;
	float	e080=1.284025417;
	float	e100=1.648721271;
	
	float e200=2.718281828;

	float	e400=7.389056099;
	float	e800=5.459815003E1;
	float	e1000=2.980957987E3;
	float	e2000=8.886110521E6;	
	float	e4000=7.896296018E13;
	float	e8000=6.235149081E27;


//Cosine Sine
	float SaveCosSin_x, SaveCos_y, SaveSin_y;

int MathError;
int FunctionPrecision=1; //High Precision
float pi= 3.141592654,Infinite=1E+40;
int Nsin=9,Ncos=9,Natan=15,Nln=70,Nsqrt=10; //Order to calculate the functions
float HlowPrecision=0.00001; //Low precision 1E-05;
//beyond Nsincos=7 the precision is 1E-04
//The ln needs high N beyond 20 at least for precision better than 1E-3


	char s[50];


//------------------------------------------------------------------------------------- 
//-------------------------------------------------------------------------------------
//-------------------------   Coding For matrices --------------------------

int MatrixPower(Matrix *MAccu,floactet *CodeListLine,int i,int imaxLine){
	int Error=0;
	return Error;
}
int MatrixSubAddition(Matrix *MAccu,floactet *CodeListLine,int i,int iptrEqualSignP,int imaxLine){
	//Sum or substract 2 following matrix separated by + - sign.
	//We end up by saving the resulting matrix in the CodeListLine
	//Remember to reactivate the changing VarNames to Accu because
	//of the problem M1= is interpreted as an accumulateur definition
	//and correct that. simply by checking if it is M n° 0<n°<9
	int Error=0,m,j,NumM,l,ResM;
	float val=0;
	extern int NbrMaxMatrix;
	//We create the resulting matrix anyway
	//As result demanded is a matrix
		for (m=NbrMaxMatrix/2; m<NbrMaxMatrix; m++)if (MAccu[m].ptr == 0) goto FoundFreeMatrix;
		PrintCmd("Not enough available matrices for solving!\n");
		return 9; //Matrix error
		FoundFreeMatrix:
		ResM=m;//this will receive the final matrix calculation
		//init ResM : We search in the CodeListLine One matrix to get the size
		//Matrices should appear only in addition as the multiplication 
		//and division should be already done 
		for (j=i;j<imaxLine;j++){
			if (CodeListLine[j].code == 0xFF) {PrintCmd("no matrix???\n");return 9;}
			if (CodeListLine[j].code == 15) {
				MAccu[ResM].n=MAccu[(int)CodeListLine[j].value].n;
				MAccu[ResM].p=MAccu[(int)CodeListLine[j].value].p;
				MAccu[ResM].ptr = (float *) malloc (MAccu[ResM].n*MAccu[ResM].p*sizeof(float) );
				if (MAccu[ResM].ptr == 0 ) {PrintCmd ("Error couldn't allocate memory for temporary matrix\n"); return 9;}
				goto ResMtxReady;
				}
		}
		ResMtxReady: //at this point ResM exists and contains the right size
		
		if (CodeListLine[i].code == 1){//load first data as matrix
			val = CodeListLine[i].value;
			for (j=0;j<MAccu[ResM].n*MAccu[ResM].p;j++)MAccu[ResM].ptr[j]=val;
			}
		if (CodeListLine[i].code == 15){
			NumM = CodeListLine[i].value;
			//copy NumM to ResM
			for (j=0;j<MAccu[ResM].n*MAccu[ResM].p;j++)MAccu[ResM].ptr[j]=MAccu[NumM].ptr[j];
		}
		//at this point ResM contains the first data
		if (CodeListLine[i+1].code==0xFF) {
			//This case is important because we get the scalar from upper 
			//treatment into matrix ResM 
			goto FinalStep; //no more calculation is necessary , returning no Error
		}
		while(i<imaxLine){
			if (CodeListLine[i+1].code == 0xFF) {goto FinalStep;}//finished

			if (CodeListLine[i+2].code==1) {
				if (CodeListLine[i+1].code==2) {//ResM+scalar
					val=CodeListLine[i+2].value;
					for (j=0;j<MAccu[ResM].n*MAccu[ResM].p;j++)MAccu[ResM].ptr[j]=MAccu[ResM].ptr[j]+val;
					}
				if (CodeListLine[i+1].code==3) {//ResM-scalar
					val=CodeListLine[i+2].value;
					for (j=0;j<MAccu[ResM].n*MAccu[ResM].p;j++)MAccu[ResM].ptr[j]=MAccu[ResM].ptr[j]-val;	
					}
			i=i+2;
			}

			if (CodeListLine[i+2].code == 15){
				if (CodeListLine[i+1].code==2) {//ResM+numM
					NumM=CodeListLine[i+2].value;
					for (j=0;j<MAccu[ResM].n*MAccu[ResM].p;j++)MAccu[ResM].ptr[j]=MAccu[ResM].ptr[j]+MAccu[NumM].ptr[j];
					}
				if (CodeListLine[i+1].code==3) {//ResM-numM
					NumM=CodeListLine[i+2].value;
					for (j=0;j<MAccu[ResM].n*MAccu[ResM].p;j++)MAccu[ResM].ptr[j]=MAccu[ResM].ptr[j]-MAccu[NumM].ptr[j];
					}
			i=i+2;
			}
		}//end while
		
		//Put Final calculation results
		FinalStep:
			CodeListLine[iptrEqualSignP].code=15; //Put ResM result after the equal sign
			CodeListLine[iptrEqualSignP].value=ResM;
			//Apparently it is not needed to finish the line by 0xFF
			//as the first data will be considered for the result
	return Error;
}

int MatrixMultiplication(Matrix *MAccu,floactet *CodeListLine,int i, int imaxLine){
	extern int NbrMaxMatrix; 
	int m,k,NewM,NumM,j,l;
	float scalar;
	//3 cas possibles  (there is at least one matrix in n°1 or n°2
	//  CASE1      n°1  is scalar       |     n°2 is matrix
	//  CASE2                 matrix      |               scalar
	//  CASE3                 matrix      |               matrix
	if (CodeListLine[i].code == 1 && CodeListLine[i+2].code == 15 ||
			CodeListLine[i].code == 15 && CodeListLine[i+2].code == 1){
		//CASE1 or CASE2
		if (CodeListLine[i].code == 1 && CodeListLine[i+2].code == 15){
			NumM=(int)CodeListLine[i+2].value; //mtx n°
			scalar = CodeListLine[i].value; }
		if (CodeListLine[i].code == 15 && CodeListLine[i+2].code == 1){
			NumM=(int)CodeListLine[i].value; //mtx n°
			scalar = CodeListLine[i+2].value;}
			
			if (NumM < NbrMaxMatrix) {//Get new temp mtx or same NumM
			//create a new matrix
			for (m=NbrMaxMatrix/2; m<NbrMaxMatrix; m++)if (MAccu[m].ptr == 0) goto FoundFreeMatrix;
			PrintCmd("Not enough available matrices for solving!\n");
			return 9; //Matrix error
			FoundFreeMatrix:
			NewM=m;
			MAccu[NewM].ptr = (float *) malloc (MAccu[NumM].n*MAccu[NumM].p*sizeof(float) );
			if (MAccu[NewM].ptr == 0 ) {PrintCmd ("Error couldn't allocate memory for temporary matrix\n"); return 9;}
			MAccu[NewM].n=MAccu[NumM].n;
			MAccu[NewM].p=MAccu[NumM].p;
		}else{NewM=NumM;}
		//Here is the multiplication:
		for (j=0; j<MAccu[NumM].n*MAccu[NumM].p; j++)
			MAccu[NewM].ptr[j] = MAccu[NumM].ptr[j] * scalar;
		//done result is in NewM
		//Now we put the matrix result in the CodeListLine
		CodeListLine[i].code=15; //mtx
		CodeListLine[i].value=NewM; // n°mtx
		i++;
		//sprintf(s,"Matrix Size nxp=%dx%d\n",MAccu[NewM].n,MAccu[NewM].p);
		//PrintCmd (s);	
		while (i+2<=imaxLine){
			CodeListLine[i].code=CodeListLine[i+2].code;
			CodeListLine[i].value=CodeListLine[i+2].value;
			i++;
			}
		return 0; //no errors
	}
	//CASE3
	if (CodeListLine[i].code == 15 && CodeListLine[i+2].code == 15){
		int NumM1 = (int)CodeListLine[i].value;
		int NumM2 = (int)CodeListLine[i+2].value;
		//we need a new matrix
		//create a new matrix
		for (m=NbrMaxMatrix/2; m<NbrMaxMatrix; m++)if (MAccu[m].ptr == 0) goto FoundFreeMatrix2;
			PrintCmd("Not enough available matrices for solving!\n");
			return 9; //Matrix error
		FoundFreeMatrix2:
			NewM=m;
			if (MAccu[NumM1].p != MAccu[NumM2].n) {
				PrintCmd("Incompatible matrix size Mtx1.p must equal Mtx2.n\n");
				return 9;
				}
			//M1*M2 just start below
			for (j=0;j<MAccu[NumM1].n;j++){
				for (k=0;k<MAccu[NumM2].p;k++){
					float Sum=0;
					for (l=0;l<MAccu[NumM1].p;l++)Sum=Sum+MAccu[NumM1].ptr[j*MAccu[NumM1].p+l]*MAccu[NumM2].ptr[l*MAccu[NumM2].p+k];
					MAccu[NewM].ptr[j*MAccu[NewM].p+k]=Sum;					
				}
			}
	}
	//it shouldn't arrive here so if it is the case, generate error
	return 9;
}
int MatrixDivision(Matrix *MAccu,floactet *CodeListLine,int i,int imaxLine){
	int Error=0;
	return Error;
}

	
int MathFunctionMatrices (Matrix *MAccu, floactet *CodeListLine,int i){
		extern int NbrMaxMatrix; 
		int m,NewM;
		int j,k;
		//we are in the case function(matrix):   [13][CodeVal],[15][n°]
		//NumM > NbrMaxMatrix/2 for the temporary matrices for calculation
		//These are free at the end of the line calculation.
		//If NumM >NbrMaxMatrix/2 We don't need to create a new matrix
		//If NumM <NbrMaxMatrix/2 We need to create a new matrix
		int NumM = (int) CodeListLine[i+1].value;
		if (NumM < NbrMaxMatrix) {
			//create a new matrix
			for (m=NbrMaxMatrix/2; m<NbrMaxMatrix; m++)if (MAccu[m].ptr == 0) goto FoundFreeMatrix;
			PrintCmd("Not enough available matrices for solving!\n");
			return 9; //Matrix error
			FoundFreeMatrix:
			NewM=m;
			MAccu[NewM].ptr = (float *) malloc (MAccu[NumM].n*MAccu[NumM].p*sizeof(float) );
			if (MAccu[NewM].ptr == 0 ) {PrintCmd ("Error couldn't allocate memory for temporary matrix\n"); return 9;}
			MAccu[NewM].n=MAccu[NumM].n;
			MAccu[NewM].p=MAccu[NumM].p;
		}else{NewM=NumM;} //we use temporary matrix as target and source
		//In the following we use NewM and NumM considering the possibility 
		// that numM could be temporary or program defined 
		//if NumM is temporary then NewM = NumM
		//if NumM is defined in the MnemoProgram then NewM != NumM
		
		switch ((int)CodeListLine[i].value){ 
			case 1:	//exp
				for (j=0;j<MAccu[NumM].n*MAccu[NumM].p; j++)
					MAccu[NewM].ptr[j] = RMath_exp(MAccu[NumM].ptr[j]);
			break; 
			case 2:	//ln
				for (j=0;j<MAccu[NumM].n*MAccu[NumM].p; j++)
					MAccu[NewM].ptr[j] = RMath_ln(MAccu[NumM].ptr[j]);
			break;
			case 3:	//sqrt
				for (j=0;j<MAccu[NumM].n*MAccu[NumM].p; j++)
					MAccu[NewM].ptr[j] = RMath_sqrt(MAccu[NumM].ptr[j]);
			break;

			//if (CodeVal==4 ) {AlreadyTaken for the function f(x);}	//function f(x)

			case 5:	//sin
				for (j=0;j<MAccu[NumM].n*MAccu[NumM].p; j++)
					MAccu[NewM].ptr[j] = RMath_sin(MAccu[NumM].ptr[j]);
			break;
			case 6:	//cos
				for (j=0;j<MAccu[NumM].n*MAccu[NumM].p; j++)
					//MAccu[NewM].ptr[j] = RMath_cos(MAccu[NumM].ptr[j]);
					MAccu[NewM].ptr[j] = RMath_cos(MAccu[NumM].ptr[j]);
			break;
			case 7:	//tan
				for (j=0;j<MAccu[NumM].n*MAccu[NumM].p; j++)
					MAccu[NewM].ptr[j] = RMath_tan(MAccu[NumM].ptr[j]);
			break;
			case 8:	//fact
				for (j=0;j<MAccu[NumM].n*MAccu[NumM].p; j++)
					MAccu[NewM].ptr[j] = fact((int)MAccu[NumM].ptr[j]);
			break;
			case 9:	//Power
				goto OutForPower;
			break;
			case 10:	//ch
				for (j=0;j<MAccu[NumM].n*MAccu[NumM].p; j++)
					MAccu[NewM].ptr[j] = RMath_ch(MAccu[NumM].ptr[j]);
			break;
			case 11:	//sh
				for (j=0;j<MAccu[NumM].n*MAccu[NumM].p; j++)
					MAccu[NewM].ptr[j] = RMath_sh(MAccu[NumM].ptr[j]);
			break;
			case 12:	//th
				for (j=0;j<MAccu[NumM].n*MAccu[NumM].p; j++)
					MAccu[NewM].ptr[j] = RMath_th(MAccu[NumM].ptr[j]);
			break;
			case 13:		//Re
				PrintCmd("Activate Complexe before use!\n");
				MathError=8;
			break;
			case 14:		//Im
				PrintCmd("Activate Complexe before use!\n");
				MathError=8;
			break;

			//if (CodeVal==15 ) {//Already taken for integrals}

			case 16:	//Oscilloscope
				if (SerialReady ==0) MathError = 7;
				for (j=0;j<MAccu[NumM].n*MAccu[NumM].p; j++)
					MAccu[NewM].ptr[j] = Oscilloscope(MAccu[NumM].ptr[j]);
			break;
			case 17:	//acos
				for (j=0;j<MAccu[NumM].n*MAccu[NumM].p; j++)
					MAccu[NewM].ptr[j] = RMath_acos(MAccu[NumM].ptr[j]);
			break;
			case 18:	//asin
				for (j=0;j<MAccu[NumM].n*MAccu[NumM].p; j++)
					MAccu[NewM].ptr[j] = RMath_asin(MAccu[NumM].ptr[j]);
			break;
			case 19:	//ath
				for (j=0;j<MAccu[NumM].n*MAccu[NumM].p; j++)
					MAccu[NewM].ptr[j] = RMath_ath(MAccu[NumM].ptr[j]);
			break;
			case 20:	//atan
				for (j=0;j<MAccu[NumM].n*MAccu[NumM].p; j++)
					MAccu[NewM].ptr[j] = RMath_atan(MAccu[NumM].ptr[j]);
			break;
			case 21:	//ash
				for (j=0;j<MAccu[NumM].n*MAccu[NumM].p; j++)
					MAccu[NewM].ptr[j] = RMath_ash(MAccu[NumM].ptr[j]);
			break;
			case 22:	//ach
				for (j=0;j<MAccu[NumM].n*MAccu[NumM].p; j++)
					MAccu[NewM].ptr[j] = RMath_ach(MAccu[NumM].ptr[j]);
			break;
			case 23:	//abs
				for (j=0;j<MAccu[NumM].n*MAccu[NumM].p; j++)
					MAccu[NewM].ptr[j] = RMath_abs(MAccu[NumM].ptr[j]);
			break;
			case 24:	//module(z)
				PrintCmd("Activate Complexe before use!\n");
				MathError=8;
			break;
			case 25:	//arg(z)
				PrintCmd("Activate Complexe before use!\n");
				MathError=8;
			break;
			case 26:	//key
				PrintCmd("key(matrix) What for!??\n");
				MathError=8;
			break;
		}
	KeepOn:
		if (MathError !=0 ) {sprintf(s,"Math Error = %d\n",MathError);PrintCmd(s);return 3;}
		if (MathError == 0) {//Place Matrix Result
				CodeListLine[i].code=15; 
				CodeListLine[i].value=NewM;
				}
		k=i+1;
		while (CodeListLine[k].code !=0xFF && CodeListLine[k].code !=0) {
			CodeListLine[k]=CodeListLine[k+1];
			k++;
			}
OutForPower:
	return;
}
//-------------------------- End Coding Matrices --------------------------

//-------------------------------------------------------------------------------


int CalculFunctionComplexe (floactet *CodeListLine,int i){
		int k,MathError=0;
		float hyp,a,b,a1,a2,ea,CodeVal,val,val_cmplx;
		
		a=CodeListLine[i+1].value; 
		b=CodeListLine[i+1].cmplx;
		CodeVal=CodeListLine[i].value;
		if (CodeVal==1 ) {ea=RMath_exp(a);val=ea*RMath_cos(b);val_cmplx=ea*RMath_sin(b);goto KeepOn;}//exp
		if (CodeVal==2 ) {val=0.5*RMath_ln(a*a+b*b);
												if ((a*a+b*b) !=0) {
													if (b>0 )val_cmplx=RMath_acos(a/RMath_sqrt(a*a+b*b));
													if (b<0 ) val_cmplx=-RMath_acos(a/RMath_sqrt(a*a+b*b));
													if (b==0) {if (a>0) val_cmplx=0; else val_cmplx=pi;}
												}else {PrintCmd("Undefined Arg(z) in ln(z)\n");MathError=2;}
												goto KeepOn;}	//ln		
		if (CodeVal==3 ) {val=RMath_sqrt(a);val_cmplx=0;goto KeepOn;}	//sqrt		
		//if (CodeListLine[i].value==4 ) {AlreadyTaken for the function f(x);}	//function f(x)
		if (CodeVal==5 ) {
			if (b == 0) {val=RMath_sin(a);val_cmplx=0;} 
			else {val = RMath_sin(a)*RMath_ch(b); val_cmplx=RMath_cos(a)*RMath_sh(b);}goto KeepOn;}	//sin
		if (CodeVal==6 ) {
			if (b == 0) {val=RMath_cos(a);val_cmplx=0;} 
			else {val = RMath_cos(a)*RMath_ch(b); val_cmplx=-RMath_sin(a)*RMath_sh(b);}goto KeepOn;}	//cos
		if (CodeVal==7 ) {val=RMath_tan(a);val_cmplx=0;goto KeepOn;}	//tan
		if (CodeVal==8 ) {val=fact((int)CodeListLine[i+1].value);val_cmplx=0;goto KeepOn;}	//factoriel
		if (CodeVal==9 ) {val_cmplx=0;goto OutForPower;goto KeepOn;}	//Power
		if (CodeVal==10 ) {val=RMath_cos(b)*RMath_ch(a);val_cmplx=RMath_sin(b)*RMath_sh(a);goto KeepOn;}	//ch
		if (CodeVal==11 ) {val=RMath_sh(a);val_cmplx=0;goto KeepOn;}	//sh
		if (CodeVal==12 ) {val=RMath_th(a);val_cmplx=0;goto KeepOn;}	//th
		if (CodeVal==13 ) {val=a;val_cmplx=0;goto KeepOn;}	//Re
		if (CodeVal==14 ) {val=b;val_cmplx=0;goto KeepOn;}	//Im
		//if (CodeListLine[i].value==15 ) {//Already taken for integrals}
		if (CodeVal==16 ) {if (SerialReady ==0) MathError = 7;val=Oscilloscope(CodeListLine[i+1].value);val_cmplx=0;goto KeepOn;}
		if (CodeVal==17 ) {val=RMath_acos(CodeListLine[i+1].value);val_cmplx=0;goto KeepOn;}	//acos
		if (CodeVal==18 ) {val=RMath_asin(CodeListLine[i+1].value);val_cmplx=0;goto KeepOn;}	//asin
		if (CodeVal==19 ) {val=RMath_ath(CodeListLine[i+1].value);val_cmplx=0;goto KeepOn;}	//atan
		if (CodeVal==20 ) {val=RMath_atan(CodeListLine[i+1].value);val_cmplx=0;goto KeepOn;}	//atanh
		if (CodeVal==21 ) {val=RMath_ash(CodeListLine[i+1].value);val_cmplx=0;goto KeepOn;}	//ash
		if (CodeVal==22 ) {val=RMath_ach(CodeListLine[i+1].value);val_cmplx=0;goto KeepOn;}	//ach
		if (CodeVal==23 ) {val=RMath_abs(CodeListLine[i+1].value);val_cmplx=0;goto KeepOn;}	//abs
		if (CodeVal==24 ) {val=RMath_sqrt(a*a+b*b);val_cmplx=0;goto KeepOn;}	//module(z)
		if (CodeVal==25 ) {
			hyp=RMath_sqrt(a*a+b*b);
			if (hyp !=0) {a1=RMath_acos(a/hyp);
								 a2=RMath_asin(b/hyp);
								 //PrintCmd("b=");Rprintf(b);
								 //PrintCmd("a2=");Rprintf(a2);
								if (a1 >=0 && a2 >=0) {val = a1;}
								if (a1 >0 && a2 <0) {val = -a1;}
								if (a1 <0 && a2 >=0) {val = pi-a2;}
								if (a1 <0 && a2 <0) {val = pi+a2;}
			}else{	PrintCmd("arg(0)!\n"); val = 0; // arg(0) set to  0 
			}
			val_cmplx=0;goto KeepOn;
			}	//arg(z)

		KeepOn:
		if (MathError == 0) {CodeListLine[i].code=1; CodeListLine[i].value=val;CodeListLine[i].cmplx=val_cmplx;}
		k=i+1;
		while (CodeListLine[k].code !=0xFF && CodeListLine[k].code !=0) {
			CodeListLine[k]=CodeListLine[k+1];
			k++;
			}
		OutForPower:
		return MathError; 
	}












	
void decode(char *txt, int size){
int i;
	for (i=0;i<size;i++){
	//printf("%c",(unsigned char)(255*RMath_cos((float)i/100 ))^txt[i] );
	txt[i]=(unsigned char)(255*OldRMath_cos((float)i/100 ))^txt[i] ;
	}
}

	
void Rprintf(float x){
	REPrintf(x);
	PrintCmd("\n");
}

void REPrintf(float x){
	int Pres=3; int i;
	char s2[20],*c,s[20];
	float X,alfa,precision;
	int n=0; int opp=0;
	int Ndigit=3;
	if (x==0) {PrintCmd(" 0"); return;}
	precision=TenPower(-Pres);
	if (x<0) {x=-x;opp=1;}	
	X=x;
	if(x*10==x) {PrintCmd("Nbr Out Of Range\n");return ;}
	if (x>=precision)while(X>=precision){X=X/10; n++;}
	if (x< precision){while(X<=precision){X=X*10; n--;}n++;}

	//alfa=x*TenPower(Ndigit+Pres+1-n);
	//if ( (alfa*10-10*(int)alfa) >5) alfa=alfa+1; //if ... to truncate properly.
	//sprintf(s,"%d%d",(int)alfa/1000,1000*alfa-1000*(int)alfa);

	alfa=x*TenPower(Ndigit+1-n);//rajout
	if (FunctionPrecision == 1) FloatToString(alfa,s,7);					//rajout
	else FloatToString(alfa,s,5);
	//sprintf(s,"%d",(UInt)alfa);
	
	if (opp==1) s2[0]=Octet("-"); 
	else s2[0]=Octet(" ");
	//s2[1]=s[0];	
	//s2[2]=Octet(".");
	//i=1;
	i=0;//rajout test
	//while(s[i]!=0){s2[i+2]=s[i]; i++;}
	while(s[i]!=0){s2[i+1]=s[i]; i++;}
	//s2[i+2]=Octet("E");
	s2[i+1]=Octet("E"); //rajout
	//c=s2+i+3;
	c=s2+i+2;
	if(alfa>=10000) n++;
	sprintf(c,"%d",n-(Pres+1));
	PrintCmd(s2);
	}


void FloatToString(float value, char *buffer, int Rounding)
{

	sprintf(buffer,"%f",value);
/*	
long iValue;
float dDecimal;
long iDecValue;
int i;
char sResult[50];
char sDecimal[50];
char sTemp[50];
 
// Get integer and decimal portions
iValue = value;
dDecimal = value - iValue;
//if (dDecimal < 0.000000001)
if (dDecimal < 0.00000001)
  dDecimal = 0;
 
// Convert integer portion to string
StrIToA(sResult, iValue);
if (strlen(sResult) < 1)
  StrCopy(sResult, "0");
StrCat(sResult, ".");
 
// Round decimal portion
for (i = 1; i <= Rounding; i++)
  dDecimal = dDecimal * 10;
 
iDecValue = dDecimal;
if (dDecimal - iDecValue >= 0.5)
  iDecValue++;
 
// Add decimal portion
StrIToA(sDecimal, iDecValue);
 
// Add leading zeros if neccessary
if (strlen(sDecimal) < Rounding)
  for (i = 1; i <= Rounding - strlen(sDecimal); i++)
  StrCat(sResult, "0");
StrCat(sResult, sDecimal);
 
// Copy into return string
StrCopy(buffer, sResult);
 
return;
*/
}



int CompareVarNames(char* txt, int i1, int i2){ //returns 0 if same
	//in text txt, compare different places txt(i1)=txti2?
	// Here we neglect the spaces:   ('n am e= '  is equal to ' name=')
	int size;

	if (i2 < 0 ) return 1;
	size = strlen(txt); //chgPOs2Win32
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
 


void RemoveComments(char *txt){
	int i,k;
	static char *p,str[]="//",s[10];
	RCstart:
	p=(char*)strstr(txt,str);//StrStr(txt,str):chgPOs2Win32
	if (p!=0){
		i=0;
		while (p[i] !=0 && p[i]!=0x0A) i++;
		for (k=0;k<strlen(p+i);k++) p[k]=p[i+k];//chgPOs2Win32
		p[k]=0;
	}else{return;}
	goto RCstart;
	}


void SaveProg()
{
/*	Err Error;
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
		h = DmNewRecord (dbP, &index, strlen(progtext) );
		if( h !=0 )Mem = MemHandleLock(h);
		else {PrintCmd("Can't save prog");return;}

		DmWrite(Mem,0,progtext,strlen(progtext) );
		if (Error == errNone) PrintCmd ("done\n");
		else PrintCmd("an error occured while writing\n");

		MemHandleUnlock(h);
		DmReleaseRecord(dbP, index, true);

		DmCloseDatabase(dbP);	*/
	PrintCmd("Command not yet implemented");
}
		

void LoadProg()
{
/*
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
	SetUpTextProg(0);

	h2=MemHandleNew(strlen(Mem)+1);
	if (h2 !=0)Mem2=MemHandleLock(h2);
	else {PrintCmd("Mem Allocation failed\n");goto Freemh;}

	if (FrmAlert (MergeText) == 0){DeleteProg();}
	sprintf(Mem2,"%s",Mem);//to insert the 0 at the end
	Mem2[(int)strlen(Mem2)-1]=0; //there was a fake character to remove...
	PrintProg(Mem2);
	MemHandleFree(h2);
Freemh:
	MemHandleUnlock(h);
	DmReleaseRecord(dbP, index, true);
	DmCloseDatabase(dbP);	
	*/
	printf("Command not yet implemented");
}


/* To stdout... */
/*
void Printf(const char * format, ...) //we use printf which is redefined as Printf at the top
{
	va_list args;
	static char buf[200];

	va_start(args, format);
	sprintf(buf, format, args);
	va_end(args);
	PrintCmd(buf); // Prints in the cmd field
}
*/
void LowPerformance(){
	Nsin=7;Ncos=7;Nln=25;
}

void PrintCmd(char * s) {
	if (StartInfoDone==1) {DeleteCmd();StartInfoDone=0;}
	Print_In_Cmd_Win(s); //See compatibilityReg.c
}

 char Octet(char * StringPointer){
	return StringPointer[0];
 }

void DeleteCmd(){
/*
	FormPtr 	Frm;
	FieldPtr 	FldPtr;
	
	Frm = FrmGetFormPtr(frmadc16);
	FldPtr = (FieldPtr)(FrmGetObjectPtr(Frm, (FrmGetObjectIndex(Frm, fld_cmd))));
	FldDelete(FldPtr, 0, 50000);//Size of the cmd 5000 maxchars see CalcReg.rcp
	// FldDelete (FieldType *fldP, Uint 16 start , UInt16 end)
	*/
	//printf("delete cmd");
	Delete_Cmd_Win();
}
 
float RMath_abs(float x){
	if (x<0) return -x;
	else return x;
}
 
 /*
float RMath_sin(float x){
	int N,k;
	float b,val;
	N=Nsin; //Order Precision
	if (x==0) return 0; //sin(0)=0
	while (x>pi) x=x - 2*pi;
	while (x<-pi)x=x+2*pi;
	//if(x==0) return 0;
	val =0; b=1.0;
	for (k=0;k<N;k++){val = val+b*Rpow(x,2*k+1)/fact(2*k+1); b=b*(-1.0);}
	return val;
	}
*/

float OldRMath_cos(float x){
	int N,k;
	float b,val;
	N=Ncos; //Order Precision
	if (x == 0 ) return 1; //cos(0)=1
	while (x>pi) x=x - 2*pi;
	while (x<-pi)x=x+2*pi;
	//if (x==0) return 1;
	val =0; b=-1.0;
	for (k=1;k<N;k++){val = val+b*Rpow(x,2*k)/fact(2*k); b=b*(-1.0);}
	return 1+val;
	}

float RMath_cos(float x){
	if (x>0)x=x-2*pi*(int)(x/(2*pi));
	if (x<0)x=x+2*pi*(1-(int)(x/(2*pi)) );
	//while (x>2*pi) x=x - 2*pi;
	//while (x<0)x=x+2*pi;
	if (x == 0 ) return 1; //cos(0)=1
	if (x<0) x=-x;
	if (x == SaveCosSin_x) {return SaveCos_y;}
	cos_sin(x);
	return SaveCos_y;
}
float RMath_sin(float x){
	if (x>0)x=x-2*pi*(int)(x/(2*pi));
	if (x<0)x=x+2*pi*(1-(int)(x/(2*pi)) );
	//while (x>2*pi) x=x - 2*pi;
	//while (x<0)x=x+2*pi;
	if (x==0) return 0; //sin(0)=0
	if (x == SaveCosSin_x) {return SaveSin_y;}
	cos_sin(x);
	return SaveSin_y;
}

static cos_sin(float x ){
	static int i,xint,m;
	static float cos1,sin1,cos2,sin2,xlim;
	//a[]=cos(1/2048),cos(1/1024),..., cos(1)
	//b[]=sin(1/2048),...						sin(1)
	static float a[]={9.999998808E-1, 9.999995232E-1, 9.999980927E-1, 9.999923706E-1,
					9.999694826E-1, 9.998779322E-1, 9.995117585E-1, 9.980475107E-1,
					9.921976672E-1, 9.689124217E-1, 8.775825619E-1, 5.403023059E-1,
					-4.161468365E-1,-6.536436209E-1};
	static float b[]={4.882812306E-4, 9.765623448E-4, 1.953123758E-3, 3.906240066E-3,
					7.812420527E-3, 1.562436422E-2, 3.124491399E-2, 6.245931784E-2,
					1.246747334E-1, 2.474039593E-1, 4.794255386E-1, 8.414709848E-1,
					9.092974268E-1, -7.568024953E-1};
	//Here we are in the interval x € [0,pi/2]
	//cos1 =1; sin1=0;//init to angle = 0
	m=1;
	//xint=(int)(x*2048); //for i =0    it starts at i=0
	xint=(int)(x*64);
	xlim = x - ((float) xint)/64.0;
	cos1 = 1.0-xlim*xlim/2.0;//+xlim*xlim*xlim*xlim/24.0; //up to order 4 at cos(1/16) précision 1E-7
	sin1 = xlim-xlim*xlim*xlim/6.0;
	for (i=5;i<14;i++){
		if( (xint&m) !=0) {cos2=cos1*a[i]-sin1*b[i]; sin2=sin1*a[i]+cos1*b[i];	cos1=cos2;sin1=sin2;}
	m=m*2;// m=2^i
	}
	SaveCosSin_x=x;
	SaveCos_y=cos1;
	SaveSin_y=sin1;
}

	
float RMath_tan(float x){
	if (RMath_cos(x) !=0){return RMath_sin(x)/RMath_cos(x);}
	else {MathError=3; return 0;} //tan (pi/2 [pi])
	}
	
float RMath_atan(float x){
	int N,k;
	float b,c,val;
	float a,h,H,Y0,Y1,t;
	
	N=Natan; //Order Precision
	if (x==0) return 0; //atan(0)=0

		if (x<0) a=-pi/2;//start
		else a=0;//start
		b=pi/2;//stop
		h=0.1;
		if (FunctionPrecision == 1) 	H=0.0000001;//precision of the result
		else H=HlowPrecision; //Low precision;
		//H=0.0000001;//precision of the result
		t=a;
		//Y0=RMath_tan(a)-x;
		Y0 = -Infinite;
	Loop:
		t=t+h;
		Y1=RMath_tan(t)-x;
		if (t>b) goto ErrorOut;
		if(Y0*Y1>0) goto Loop;
		t=t-h;
		h=h/10;
		if (h>H) goto Loop;
		return t;
	ErrorOut:
		//PrintCmd("Chg Calculation\n");
		//here we change calculation type
	val =0; b=1.0;
	for (k=0;k<N;k++){val = val+b*Rpow(x,2*k+1)/(2*k+1); b=b*(-1.0);}
	return val;
	}

float RMath_ath(float x){
	if (x <=-1 || x>=1) {PrintCmd("Out of Range ath\n"); MathError = 9; return 0;}
	return 0.5*RMath_ln( (1+x)/(1-x) );
	}


float RMath_ash(float x){
	return RMath_ln( x+RMath_sqrt(x*x+1) );
}
float RMath_ach(float x){
	if (x<1) {PrintCmd("Out of range ach\n"); MathError=9; return 0;}
	return RMath_ln( x + RMath_sqrt(x*x-1) );
}
		
float fact(int k){
	int i;
	float val=1.0;
	for (i=2;i<=k;i++)val=val*(float)i;
	return val;
	}
	
float RMath_sqrt(float x){
	int N,k,exponant=0;
	float b,c,val,mantisse;
	float a,h,H,Y0,Y1,t;
	float sqrt10=3.16227766;
	char s[50];
	if (x==0) return 0; //sqrt(0)=0
	if (x<0) {PrintCmd("sqrt(<0)!\n");MathError=2; return 0;} //The error will be collected at the loop program	
	if (FunctionPrecision==1) goto HighPrecision;
	GetMantisseExponant(x, &exponant, &mantisse);
	if (mantisse==1) {t=1; goto Result;}//'cause pblem finding sqrt(1)

	a=0;//start
	b=3.32; //stop sqrt(11)
		h=0.1;
		H=HlowPrecision;//precision of the result
		t=a;
		Y0 =a*a -mantisse;
	Loop:
		t=t+h;
		Y1=t*t-mantisse;
		if (t>b) goto HighPrecision;
		if(Y0*Y1>0) goto Loop;
		t=t-h;
		h=h/10;
		if (h>H) goto Loop;
	//result sqrt(mantisse)*TenPower(exponant/2), but exponant/2 can be 1.5 or 2.5...
		//sprintf(s,"exponant= %d\n",exponant); PrintCmd(s);
		//sprintf(s,"mantisse : ");PrintCmd(s);Rprintf(mantisse);
		//sprintf(s,"sqrt(mantisse) : ");PrintCmd(s);Rprintf(t);
	Result:
		if (exponant > 0 ) {
		if (exponant-2*(int)(exponant/2) == 0) return t*TenPower((int) exponant/2 );
		else return t*TenPower ((int)exponant/2)*sqrt10;
		}else{
		if (exponant-2*(int)(exponant/2) == 0) return t*TenPower((int) exponant/2 );
		else {
		return t*TenPower ((int)exponant/2) / sqrt10;}
		}
	HighPrecision:
		//PrintCmd("Chg Calculation sqrt\n");
		//here we change calculation type
		return RMath_exp(0.5*RMath_ln(x)); 
	}


 float Rsscanf (char *s,char *c,float *Nbr){
	int size=strlen(s);
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

float TenPower (int exp){//10^exp, with exp pos or neg
		float n=1;int i;
		//if (exp == 0) return 1;
		if (exp > 0) {for (i=0;i<exp;i++) n=n*10;}
		else{ for (i=0;i<-exp;i++) n=n/10;}
		return n;
		}
 
 
 /*
float RMath_exp(float x){//value on range: exp(-+SizeMathExpTable) 
	//SizeMathExpTable = 100 most likely to set
	int a,Inv;
	float m,ea,eb,ec,A,b,c,Px;
	Inv=0;

	//x=2*x;//to be consistant with e^(1/2)
	//x=4*x;//to be consistant with e^(1/4)
	//x=8*x;//to be consistant with e^(1/8)
	x=16*x;//to be consistant with e^(1/16)
	if (x<0) {x=-x;Inv=1;}//Invert for e(-x) 

	a=(int) (x);
	
	if (a+2>SizeMathExpTable) {MathError=1; PrintCmd("OverFlow exp()\n");return 0;}//The error will be collected at the loop program
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
*/


float RMath_exp(float x){
	int Inv;
	float A,b,c,Px,ea,eb,ec,m;
	float e=2.718281828;
	float b001,b002,b004,b008,b010,b020,b040,b080,b100,b200,b400,b800,b1000,b2000,b4000,b8000;
	int a;
	//unsigned long int a;
	float exp;
/*	float e_1024=1.000977039;//exp(1/1024)
	float	e001=1.0019550359;//exp(1/512)
	float	e002=1.003913889;
	float	e004=1.007843097;
	float	e008=1.015747709;
	float	e010=1.031743407;
	float	e020=1.064494459;
	float	e040=1.133148453;
	float	e080=1.284025417;
	float	e100=1.648721271;
	
	float e200=2.718281828;

	float	e400=7.389056099;
	float	e800=5.459815003E1;
	float	e1000=2.980957987E3;
	float	e2000=8.886110521E6;	
	float	e4000=7.896296018E13;
	float	e8000=6.235149081E27;
*/
	if (x == 0) return 1; //e(0)= 1
	Inv=0;
	if (x<0) {x=-x;Inv=1;}//Invert for e(-x) 

	a=(int)(x*0x200);
	//a=(unsigned long int)(x * 0x200);
	
	if ((a&0x001)!=0) b001  =  e001;   else b001=1;
	if ((a&0x002)!=0) b002  =  e002;   else b002=1;
	if ((a&0x004)!=0) b004  =  e004;   else b004=1;
	if ((a&0x008)!=0) b008  =  e008;   else b008=1;
	if ((a&0x010)!=0) b010  =  e010;   else b010=1;
	if ((a&0x020)!=0) b020  =  e020;   else b020=1;
	if ((a&0x040)!=0) b040  =  e040;   else b040=1;
	if ((a&0x080)!=0) b080  =  e080;   else b080=1;
	if ((a&0x100)!=0) b100  =  e100;   else b100=1;
	if ((a&0x200)!=0) b200  =  e200;   else b200=1;
	if ((a&0x400)!=0) b400  =  e400;   else b400=1;
	if ((a&0x800)!=0) b800  =  e800;   else b800=1;
	if ((a&0x1000)!=0) b1000  =  e1000;   else b1000=1;
	if ((a&0x2000)!=0) b2000  =  e2000;   else b2000=1;
	if ((a&0x4000)!=0) b4000  =  e4000;   else b4000=1;
	if ((a&0x8000)!=0) b8000  =  e8000;   else b8000=1;	

	exp= b001*b002*b004*b008*b010*b020*b040*b080*b100*b200*b400*b800*b1000*b2000*b4000*b8000;
	
	ea= exp;
	eb= ea*e_1024;
	ec= eb*e_1024;
	A=(float)a/0x200; b=A+1/1024.0;c=A+2/1024.0;
	m=512.0*1024.0;
	Px= (x-b)*(x-c)*ea*m-(x-A)*(x-c)*eb*2*m+(x-A)*(x-b)*ec*m;//Polynome  interpolateur Lagrange
	if (Inv==0) return Px;
	else return 1/Px;
}

/*	
void CreateMathExpTable(){
	//float e=2.718281828;
	//float e2=1.648721271; //e^(1/2)
	//float e4=1.284025417; //e^(1/4)
	//float e8=1.133148453; //e^(1/8)
	float e16=1.064494459; //e^(1/16)
	float x=1;//set x to 1
	int i;
	if (debug>0) PrintCmd("Create exp\n");
	for (i=0;i<SizeMathExpTable;i++){//max e(x) is e(Size/8)
		MathExpTable[i]=x;
		//x=x*e4; //e^(1/4)
		//x=x*e8; //e^(1/8)
		x=x*e16; //e^(1/16)
		}
	}
*/	
/*
float RMath_ln(float x){//value on range: x>0 
	//SizeMathLnTable = 101 but we use 100 data
	int exponant=0,k,N=Nln;
	float m,val,mantisse,b,c;
	float ln10=2.302585093;
	if (x==1)return 0; //ln1=0;
	if (x<0) {MathError=2; return 0;} //The error will be collected at the loop program	
		GetMantisseExponant(x, &exponant, &mantisse);
		x= (float)mantisse/10-1;  //We devide by 10 for precision of the limited development
		val =0; b=1.0;
		for (k=1;k<N;k++){c=k; val = val+b*Rpow(x,k)/c; b=b*(-1.0);}
		return val+((float)exponant+1.0)*ln10;   // ln(x=a10^b)= ln(a)+b ln10
	}
*/

float RMath_ln(float x){//value on range: x>0 
	//SizeMathLnTable = 101 but we use 100 data
	int exponant=0,k,N=Nln;
	float m,val,mantisse,b,c;
	float ln10=2.302585093;
	if (x==1)return 0; //ln1=0;
	if (x<0) {MathError=2; return 0;} //The error will be collected at the loop program	
		GetMantisseExponant(x, &exponant, &mantisse);
		return logarithm(mantisse)+((float)exponant)*ln10;   // ln(x=a10^b)= ln(a)+b ln10
	}
	
float logarithm(float x){
		float x0,lnx0, alpha,SumAlpha,H;
		//the algorithm is ln x = lnx0 + alpha0 + alpha1 + ... + alphaN
		//where alpha N = x/(x0*exp(alpha0 + alpha1+... +alpha(N-1) )) - 1
		
		if (x == 1) return 0;

		x0=1;
		lnx0=0;

		H=0.0000001;//precision of the result
		if (FunctionPrecision == 1) 	H=0.0000002;//high precision of the result
		else H=HlowPrecision; //Low precision;
		
		alpha=x/x0-1; //alpha0
		SumAlpha=0;
		while (Rabs(alpha) >H ){
			SumAlpha= SumAlpha + alpha;
			alpha = x/(x0*RMath_exp(SumAlpha))-1;
			//Rprintf(alpha);
		}
		return lnx0 + SumAlpha + alpha; //+ alpha to get the last term
	}

float Rabs(float x){ if (x<0) return -x; return x;}

/*	
float logarithm(float x){
		float a,b,h,H,Y0,Y1,t;
	int exponant=0,k,N=Nln;
	float c,val;
	//if (x == 1) return 0;
		a=0;//start
		b=2.39;//stop at ln11
		h=0.1;
		H=0.0000001;//precision of the result
		if (FunctionPrecision == 1) 	H=0.0000001;//high precision of the result
		else H=HlowPrecision; //Low precision;

		t=a;
		Y0=RMath_exp(a)-x;
	Loop:
		t=t+h;
		Y1=RMath_exp(t)-x;
		//printf("e(t= %f)=   %f\n",t,Y1);
		if (t>b) goto ErrorOut;
		if(Y0*Y1>0) goto Loop;
		t=t-h;
		h=h/10;
		//Y0=exponentiel(t)-x;
		if (h>H) goto Loop;
		return t;
	ErrorOut:
		//PrintCmd("Out of Range Ln\n");
		//PrintCmd("Chg Calculation\n");
		//here we change calculation type
		val =0; b=1.0;
		for (k=1;k<N;k++){c=k; val = val+b*Rpow(x,k)/c; b=b*(-1.0);}
		return val;
		//return 0;
	}
*/


float RMath_Pow_cmplx(float x, float y, float a,float b,float * val_cmplx){
	float xp,yp,re,im;
	int k;
	if (x==0 && a == 0) {MathError = 3; return 0;}//undefined form
	if (x==0 && a < 0) {MathError = 4; return 0;}//division by zero
	if (a != (int) a && x>0) return RMath_exp(a*RMath_ln(x));
	if (a > 0 && a== (float )((int)a) && b ==0){
		xp=x;yp=y;
		for (k=0;k<a-1;k++){ 
		re=x*xp-y*yp; im=x*yp+xp*y;
		xp=re;yp=im;
		}
		*val_cmplx=im;
		return re; //return val
		}
	//if (a > 0) return Rpow(x,(int) a );
	if (a < 0 && a== (float )((int)a) && b ==0){
		xp=x;yp=y;
		for (k=0;k<-a-1;k++){ 
		re=x*xp-y*yp; im=x*yp+xp*y;
		xp=re;yp=im;
		}
		*val_cmplx=-im/(re*re+im*im);
		return re/(re*re+im*im); //return val
		}
	if ( x<0 && a/2 == (float) ((int)(a/2)) ) {MathError=5; return 0;}//impossible sign
	if ( x<0 && a/2 != (float) ((int)(a/2)) ) {return -RMath_exp(a*RMath_ln(-x));}//sort of handle impairs	
	MathError=3;return 0;
	}
	
	
float RMath_Pow(float x, float a){
	if (x==0 && a== 0) {MathError = 3; return 0;}//undefined form
	if (x==0 && a< 0) {MathError = 4; return 0;}//division by zero
	if (a != (int) a && x>0) return RMath_exp(a*RMath_ln(x));
	if (a > 0 && a== (float )((int)a) ) return Rpow(x,(int) a );
	if (a > 0) return Rpow(x,(int) a );
	if (a < 0 && a== (float) ((int)a) ) return 1/Rpow(x, -(int)a);
	if ( x<0 && a/2 == (float) ((int)(a/2)) ) {MathError=5; return 0;}//impossible sign
	if ( x<0 && a/2 != (float) ((int)(a/2)) ) {return -RMath_exp(a*RMath_ln(-x));}//sort of handle impairs	
	MathError=3;return 0;
	}

float Rpow(float x, int a){
	int k; 
	float val;
	val =1.0;
	if (x==0 && a!=0 ) return 0;
	for (k=0;k<a;k++){ val=val*x;}
	return val;
	}
	
void GetMantisseExponant(float x, int *exponant, float*mantisse){
	//works only for positive numbers x.
	int Pres=3; int i;
	float X,precision;
	int n=0; 


	if (x==0) {exponant[0]=0; mantisse[0]=0; return;}
	precision=TenPower(-Pres);
	X=x;
	if (x>=precision)while(X>=precision){X=X/10; n++;}
	if (x< precision){while(X<=precision){X=X*10; n--;}n++;}
	
	mantisse[0]=x*TenPower(Pres+1-n);
	exponant[0]=n-4;
	}

float RMath_ch(float x){return ( RMath_exp(x)+RMath_exp(-x) )/2;}
float RMath_sh(float x){return ( RMath_exp(x)-RMath_exp(-x) )/2;}
float RMath_th(float x){return ( RMath_sh(x)/RMath_ch(x) );}

float RMath_acos(float x){ return pi/2 - RMath_asin(x);}

float RMath_asin(float x){
	float a,b,h,H,Y0,Y1,t;
	int exponant=0,k,N=Nln;
	float c,val;
		//Rprintf(x);
		
		if (x<-1.000001 || x>1.000001){MathError = 6; PrintCmd("Out of Range asin\n"); goto ErrorOut;}

		if (x<=-1) return -pi/2;
		if (x>=1) return pi/2;
		if (x<0) a=-pi/2;//start
		else a=0;
		b=pi/2;//stop at ln11
		h=0.1;
		if (FunctionPrecision == 1) 	H=0.0000001;//precision of the result
		else H=HlowPrecision; //Low precision;
		//H=0.0000001;//precision of the result
		t=a;
		//Y0=RMath_sin(a)-x;
		Y0=-1-x;
		if (Y0 == 0)return a;
	Loop:
		t=t+h;
		Y1=RMath_sin(t)-x;
		//printf("e(t= %f)=   %f\n",t,Y1);
		if (t>b) {PrintCmd("acos not found\n");goto ErrorOut;}
		if(Y0*Y1>0) goto Loop;
		t=t-h;
		h=h/10;
		if (h>H) {	goto Loop;}
		return t;
	ErrorOut:
		return 0;
	}
	
	
	//------------------------- End math -----------------

	
	
	
		
void HideKeyPad(){
/*
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
*/
	printf("hide keypad\n");
	}	
void ShowKeyPad(){
/*
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
				*/
	printf("show keypad\n");
	}
	

	
void Tracer3DAxis(){
	//RGBColorType MyPenColor,OldPenColor,OldPenColor2;
	int k;
	float color=0;

	GridSet=1;
	//HideKeyPad(); //hide the keypad
	SelectionStylo(0x000000);//noir
		
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
	
float Dx(float x, float y, float z){
	return y-x*(xp-y)/zp;
}
float Dy(float x, float y, float z){
	return 	z-x*(yp-z)/zp;
}	
	
void TracerAxis(int centerx,int centery,int width, int height){
	int k;
	float color=0;
	GridSet=1;
	//HideKeyPad(); //hide the keypad
	SelectionStylo(0x000000);//noir

	WinDrawLine(centerx-width/2,centery-height/2, centerx+width/2, centery-height/2);	
	WinDrawLine(centerx-width/2,centery+height/2, centerx+width/2, centery+height/2);	
	WinDrawLine(centerx-width/2,centery-height/2, centerx-width/2, centery+height/2);	
	WinDrawLine(centerx+width/2,centery-height/2, centerx+width/2, centery+height/2);	

	centerx=0;
	centery=0;
	height=(-DimYmin+DimYmax);
	width=(-DimXmin+DimXmax);
	//Line(0,0,DimXmin,0,color);
	Line(DimXmin,0,DimXmax,0,color);
	//Line(0,0,0,DimYmin,color);
	Line(0,DimYmin,0,DimYmax,color);
	if (StepX !=0){
		for (k=0;k<(DimXmax-DimXmin)/StepX;k++){
		Line(-k*StepX,0,-k*StepX,5*(DimYmax-DimYmin)/DrawZoneH,color);
		Line(k*StepX,0,k*StepX,5*(DimYmax-DimYmin)/DrawZoneH,color);
		}
		}
	if (StepX !=0){
		for (k=0;k<(DimYmax-DimYmin)/StepX;k++){
		Line(0,-k*StepX,5*(DimXmax-DimXmin)/DrawZoneW,-k*StepX,color);
		Line(0,k*StepX,5*(DimXmax-DimXmin)/DrawZoneW,k*StepX,color);
		}
		}
	}

void Line(float x1, float y1, float x2, float y2,float color){
	COLORREF	colpen  ;
	HPEN	hpen	;

/*Drawingzone Square
#define DrawZoneX 90
#define DrawZoneY 90
#define DrawZoneH 70
#define DrawZoneW 70*/

	colpen   = 0x000000;
	hpen	= CreatePen(PS_SOLID,1,colpen);
	SelectObject(hDC,hpen);

	if (color == 1)SelectionStylo(0x0A0AFF);//red
	if (color == 2)SelectionStylo(0x0AFF0A);//green
	if (color == 3)SelectionStylo(0xFF0A0A);//blue
	if (color == 4)SelectionStylo(0x0A0AFF);//yellow
	if (color == 5)SelectionStylo(0xFF0FFF);//
	if (color == 6)SelectionStylo(0x0FFFFF);//

	
	//x2-x1 =160
	//y2-y1=160
	//resizing according to gfxdim values
	if (x1<DimXmin ) {goto out;}
	if (x1 >DimXmax ){goto out;}
	if (y1<DimYmin ) {goto out;}
	if (y1 >DimYmax) {goto out;} // no drawing 
	if (x2<DimXmin ) {goto out;}
	if (x2 >DimXmax ){goto out;}
	if (y2<DimYmin ) {goto out;}
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
	//WinSetForeColorRGB(&OldPenColor,&OldPenColor2); //Set OldColor
		return;
	}


	
