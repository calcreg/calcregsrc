
#define NAME1_SECTION __attribute__ ( (section("Seg1")))


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
#include <PalmOSGlue.h>

//#define printf Printf     //Force Redefinition because it already exists in StdIOPalm.h but doesn't suit here




extern int debug;
extern int StartInfoDone;



//-------exponential----------  MathError=1 out of range exponential
#define SizeMathExpTable 800
static float MathExpTable[SizeMathExpTable];

//-------logarithme neperien----------  MathError=2 out of range Ln
//#define SizeMathLnTable 102 //Not below 100 !
//float MathLnTable[SizeMathLnTable];
//------- x^y ------Power -------------- MathError =3,4,5 undefined 0^0, 1/0, impossible sign (-1)^(1/2)
void PrintCmd(char * s) NAME1_SECTION;
void  Printf(const char * format, ...) NAME1_SECTION; //should be Printf and not printf otherwise it conflicts with StdIOPalm.h definition
void SetUpTextProg(int position) NAME1_SECTION;
void HideKeyPad() NAME1_SECTION;
void ShowKeyPad() NAME1_SECTION;

char Octet(char * StringPointer) NAME1_SECTION; 
float Rsscanf (char *StringNbr,char *s ,float *Nbr) NAME1_SECTION;
float TenPower (int exp) NAME1_SECTION;
void DeleteCmd() NAME1_SECTION;
void SaveProg()NAME1_SECTION;
void LoadProg()NAME1_SECTION;
void DeleteProg()NAME1_SECTION;
void PrintProg(char * s)NAME1_SECTION;
int CompareVarNames(char* txt, int i1, int i2)NAME1_SECTION;
void RemoveComments(char *txt)NAME1_SECTION;

void Rprintf(float x);//write and enter
void REPrintf(float x);//write nbr but don't enter
void decode(char *txt, int size);


//Math functions
void CreateMathExpTable()NAME1_SECTION;
float RMath_exp(float x)NAME1_SECTION;//value on range: exp(-+SizeMathExpTable) 
float RMath_ln(float x)NAME1_SECTION;//value on range: x>0 
void GetMantisseExponant(float x, int *exponant, float*mantisse);
float RMath_sqrt(float x)NAME1_SECTION;
float RMath_sin(float x)NAME1_SECTION;
float RMath_cos(float x)NAME1_SECTION;
float  RMath_tan(float x)NAME1_SECTION;
float  pow(float x, int a)NAME1_SECTION;//x^a, a dans N
float fact(int k)NAME1_SECTION;
float RMath_Pow(float x, float a)NAME1_SECTION;
float RMath_ch(float x)NAME1_SECTION;
float RMath_sh(float x)NAME1_SECTION;
float RMath_th(float x)NAME1_SECTION;

void LowPerformance()NAME1_SECTION;

void TracerAxis(int centerx,int centery,int width, int height)NAME1_SECTION;//color 0black 1red 2green 3blue
void Tracer3DAxis()NAME1_SECTION;
float Dx(float x, float y, float z)NAME1_SECTION;
float Dy(float x, float y, float z)NAME1_SECTION;
void Line(float x1, float y1, float x2, float y2, float Color)NAME1_SECTION; //line x1,y1,x2,y2,color 0black 1red 2green 3blue


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


//Drawing Zone Square
float DrawZoneX=75;
float DrawZoneY=90;
float DrawZoneH=70;
float DrawZoneW=85;


extern int GridSet; //for the grid =0 grid off, =1 grid on color Red
extern int ColorGraph; //the color of the graph
extern int Keypad;
extern int GfxBigDisplay;

int MathError;
float pi= 3.141592654;
int Nsin=7,Ncos=7,Nln=50; //Order to calculate the functions

//beyond Nsincos=7 the precision is 1E-04
//The ln needs high N beyond 20 at least for precision better than 1E-3

 
void PrintProg(char * s){
	FormPtr 	Frm;
	FieldPtr 	FldPtr;
	
	Frm = FrmGetFormPtr(frmadc16);
	FldPtr = (FieldPtr)(FrmGetObjectPtr(Frm, (FrmGetObjectIndex(Frm, fld_prog))));
	FldInsert(FldPtr, s, StrLen(s));
}


	
void decode(char *txt, int size){
int i;
	for (i=0;i<size;i++){
	//printf("%c",(unsigned char)(255*RMath_cos((float)i/100 ))^txt[i] );
	txt[i]=(unsigned char)(255*RMath_cos((float)i/100 ))^txt[i] ;
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
	StrPrintF(c,"%d",n-(Pres+1));
	PrintCmd(s2);
	}




int CompareVarNames(char* txt, int i1, int i2){ //returns 0 if same
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
 


void RemoveComments(char *txt){
	int i,k;
	static char *p,str[]="//";
	RCstart:
	p=StrStr(txt,str);
	if (p!=0){
		i=0;
		while (p[i] !=0 && p[i]!=0x0A) i++;
		for (k=0;k<StrLen(p+i);k++) p[k]=p[i+k];
		p[k]=0;
	}else{return;}
	goto RCstart;
	}


void DeleteProg(){
	FormPtr 	Frm;
	FieldPtr 	FldPtr;
	
	Frm = FrmGetFormPtr(frmadc16);
	FldPtr = (FieldPtr)(FrmGetObjectPtr(Frm, (FrmGetObjectIndex(Frm, fld_prog))));
	FldDelete(FldPtr, 0, 5000);//Size of the cmd 5000 maxchars see CalcReg.rcp
}


void SaveProg()
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
		

void LoadProg()
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
	SetUpTextProg(0);

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


/* To stdout... */
void Printf(const char * format, ...) //we use printf which is redefined as Printf at the top
{
	va_list args;
	static Char buf[200];

	va_start(args, format);
	StrPrintF(buf, format, args);
	va_end(args);
	PrintCmd(buf); // Prints in the cmd field
}

void LowPerformance(){
	Nsin=4;Ncos=4;Nln=6;
}

void PrintCmd(char * s) {
	FormPtr 	Frm;
	FieldPtr 	FldPtr;
	
	if (StartInfoDone==1) {DeleteCmd();StartInfoDone=0;}
	Frm = FrmGetFormPtr(frmadc16);
	FldPtr = (FieldPtr)(FrmGetObjectPtr(Frm, (FrmGetObjectIndex(Frm, fld_cmd))));
	FldInsert(FldPtr, s, StrLen(s));
}

void SetUpTextProg(int position){
	FormPtr 	Frm;
	FieldPtr 	FldPtr;
	ScrollBarType *scrollBarPtr;
	UInt16 valueP,minP,maxP,pageSizeP;
	UInt16 pos,textHeight,fieldHeight,maxValue;

	pos =(UInt16) position;
	Frm = FrmGetFormPtr(frmadc16);
	FldPtr = (FieldPtr)(FrmGetObjectPtr(Frm, (FrmGetObjectIndex(Frm, fld_prog))));
	FldSetScrollPosition (FldPtr,pos);
	//change and Set maxValue for the scrollbar
	scrollBarPtr = FrmGetObjectPtr(Frm,FrmGetObjectIndex(Frm, ProgScrollbarId));
	FldGetScrollValues (FldPtr, &pos,&textHeight, &fieldHeight);//from field

	if (textHeight > fieldHeight) maxValue = textHeight - fieldHeight;
	else if (pos) maxValue = pos;
	else maxValue = 0;
	SclSetScrollBar (scrollBarPtr,pos, 0, maxValue,fieldHeight-1);

}

 char Octet(char * StringPointer){
	return StringPointer[0];
 }

void DeleteCmd(){
	FormPtr 	Frm;
	FieldPtr 	FldPtr;
	
	Frm = FrmGetFormPtr(frmadc16);
	FldPtr = (FieldPtr)(FrmGetObjectPtr(Frm, (FrmGetObjectIndex(Frm, fld_cmd))));
	FldDelete(FldPtr, 0, 50000);//Size of the cmd 5000 maxchars see CalcReg.rcp
	// FldDelete (FieldType *fldP, Uint 16 start , UInt16 end)
}
 
 
float RMath_sin(float x){
	int N,k;
	float b,val;
	N=Nsin; //Order Precision
	while (x>pi) x=x - 2*pi;
	while (x<-pi)x=x+2*pi;
	//if(x==0) return 0;
	val =0; b=1.0;
	for (k=0;k<N;k++){val = val+b*pow(x,2*k+1)/fact(2*k+1); b=b*(-1.0);}
	return val;
	}
float RMath_cos(float x){
	int N,k;
	float b,val;
	N=Ncos; //Order Precision
	while (x>pi) x=x - 2*pi;
	while (x<-pi)x=x+2*pi;
	//if (x==0) return 1;
	val =0; b=-1.0;
	for (k=1;k<N;k++){val = val+b*pow(x,2*k)/fact(2*k); b=b*(-1.0);}
	return 1+val;
	}
float RMath_tan(float x){
	if (RMath_cos(x) !=0){return RMath_sin(x)/RMath_cos(x);}
	else {MathError=3; return 0;} //tan (pi/2 [pi])
	}
	
float fact(int k){
	int i;
	float val=1.0;
	for (i=2;i<=k;i++)val=val*(float)i;
	return val;
	}
float RMath_sqrt(float x){//value on range: x>0
	return RMath_exp(0.5*RMath_ln(x));
	}

 float Rsscanf (char *s,char *c,float *Nbr){
	int size=StrLen(s);
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
		if (exp >=0) {for (i=0;i<exp;i++) n=n*10;}
		else{ for (i=0;i<-exp;i++) n=n/10;}
		return n;
		}
 
float RMath_exp(float x){//value on range: exp(-+SizeMathExpTable) 
	//SizeMathExpTable = 100 most likely to set
	int a,Inv;
	float m,ea,eb,ec,A,b,c,Px ;
	Inv=0;

	//x=2*x;//to be consistant with e^(1/2)
	//x=4*x;//to be consistant with e^(1/4)
	x=8*x;//to be consistant with e^(1/8)
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
	
void CreateMathExpTable(){
	//float e=2.718281828;
	//float e2=1.648721271; //e^(1/2)
	//float e4=1.284025417; //e^(1/4)
	float e8=1.133148453; //e^(1/8)
	float x=1;//set x to 1
	int i;
	if (debug>0) PrintCmd("Create exp\n");
	for (i=0;i<SizeMathExpTable;i++){//max e(x) is e(Size/8)
		MathExpTable[i]=x;
		//x=x*e4; //e^(1/4)
		x=x*e8; //e^(1/8)
		}
	}
	

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
		for (k=1;k<N;k++){c=k; val = val+b*pow(x,k)/c; b=b*(-1.0);}
		return val+((float)exponant+1.0)*ln10;   // ln(x=a10^b)= ln(a)+b ln10
	}
	
float RMath_Pow(float x, float a){
	if (x==0 && a== 0) {MathError = 3; return 0;}//undefined form
	if (x==0 && a< 0) {MathError = 4; return 0;}//division by zero
	if (a != (int) a && x>0) return RMath_exp(a*RMath_ln(x));
	if (a > 0 && a== (float )((int)a) ) return pow(x,(int) a );
	if (a > 0) return pow(x,(int) a );
	if (a < 0 && a== (float) ((int)a) ) return 1/pow(x, -(int)a);
	if ( x<0 && a/2 == (float) ((int)(a/2)) ) {MathError=5; return 0;}//impossible sign
	if ( x<0 && a/2 != (float) ((int)(a/2)) ) {return -RMath_exp(a*RMath_ln(-x));}//sort of handle impairs	
	MathError=3;return 0;
	}

float pow(float x, int a){
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


	
	
	//------------------------- End math -----------------

	
	
	
		
void HideKeyPad(){
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
void ShowKeyPad(){
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
	

	
void Tracer3DAxis(){
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
	
float Dx(float x, float y, float z){
	return y-x*(xp-y)/zp;
}
float Dy(float x, float y, float z){
	return 	z-x*(yp-z)/zp;
}	
	
void TracerAxis(int centerx,int centery,int width, int height){
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

void Line(float x1, float y1, float x2, float y2,float color){
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
	
	WinSetForeColorRGB(&OldPenColor,&OldPenColor2); //Set OldColor
	}

	