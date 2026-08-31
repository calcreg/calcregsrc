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
extern HBRUSH 	hbrush;


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

	typedef struct ListOrder{
		int pt;//postion index in the matrix in Memory convention (start from 0 not 1)
		int next; //what index in the list order is the next point in th chosen order
		float x; //the x value
		}ListOrder;


	typedef struct Points{
		float x;
		float y;
	}Points;


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
int FillSphere(Matrix *MAccu,int NumM,int PtLinkM,float Radius,float period);
int DisplayObjectMatrix(Matrix *MAccu,int NumM,int PtLinkM,int DrawingMode);
void FillQuadrilatere(float X1,float Y1,float X2,float Y2,float X3,float Y3,float X4,float Y4,int ColorGraph);
int FunctToObjMatrix(Matrix *MAccu,int NumM,int M2,int PtLinkM);
int GetClosestPoint(Matrix *MAccu,int NumM,int VectP,int ResM3,int mask);
int ModifObject(Matrix* MAccu,int NumM,int PtLinkM,int IndexP,int Vect,int mod);

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

float lastcolor; //pencolor


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
	int FillSphere(Matrix *MAccu,int NumM,int PtLinkM,float Radius,float period){
			float teta=0, pi=3.1415927,phi;
			int k,j,NbrLatitude;
			int Error=0;
			int Mp=MAccu[NumM].p;
			if (MAccu[NumM].n !=3 && MAccu[NumM].n !=4) {PrintCmd("fillsphM: Mtx Size should be Mn=3 or 4\n");Error=1;goto EndMain;} 
			if (period==0 || MAccu[NumM].p / period != (float) (int)(MAccu[NumM].p / period) ) {PrintCmd("fillsphM n°,radius,period_phi:\nInvalid period or Mp should be a multiple of the period_phi \n");Error=1;goto EndMain;}
			//fill matrix with sphere
			if (MAccu[NumM].ptr == 0) {Error = 1; PrintCmd("Can't fill Matrix sphere, not defined!\n");goto EndMain;}
			if (MAccu[PtLinkM].ptr == 0) {Error = 1; PrintCmd("To fill Matrix sphere, PtLinkMatrix must be defined!\n");goto EndMain;}
			if (MAccu[PtLinkM].p!=Mp) {PrintCmd("fillsphM: M.p should be same as PtLinkM.p\n");Error=1;goto EndMain;} 
			if (MAccu[PtLinkM].n!=4) {PrintCmd("fillsphM: PtLinkM.n should be equal to 4\n");Error=1;goto EndMain;} 
			//This line below if for eventual translations
			if (MAccu[NumM].n==4)for (k=0;k<Mp;k++)MAccu[NumM].ptr[3*Mp+k]=1;
			
			NbrLatitude=Mp/(int)period;
			for (j=0;j<NbrLatitude;j++){
				for (k=0;k<(int)period;k++){
					phi= (float)k*2*pi/period;
					teta=(float)j*pi/NbrLatitude;
					//PrintCmd("phi=");Rprintf(phi);
					MAccu[NumM].ptr[k+j*(int)period]= Radius*RMath_sin(teta)*RMath_cos(phi); //x
					MAccu[NumM].ptr[Mp+k+j*(int)period]= Radius*RMath_sin(teta)*RMath_sin(phi);//y
					MAccu[NumM].ptr[Mp*2+k+j*(int)period]= Radius*RMath_cos(teta);//z
					}
			//PrintCmd("teta=");Rprintf(teta);
			}
			//fillPtLinkM CAREFUL: the index on mtx are from 1 to p, but here in memory the tables start from 0, we thus add 1
	//            Table of link
	//				  P1 |
	//			  P4 --   -- P2  
	//					   |  P3
	
			for (j=0;j<NbrLatitude;j++){
				for (k=0;k<(int)period;k++){
					if (j>0){
					MAccu[PtLinkM].ptr[k+j*(int)period]=(j-1)*(int)period+k+1;// j-1,k
					}else{//condition au limites
						MAccu[PtLinkM].ptr[k+j*(int)period]=0;// no link point index
						}	
					if (k<(int)period-1){
					MAccu[PtLinkM].ptr[Mp+k+j*(int)period]=j*(int)period+k+1+1;//j,k+1
					}else{MAccu[PtLinkM].ptr[Mp+k+j*(int)period]=j*(int)period+1;//j,k=0
						}
					if (j<NbrLatitude-1){
					MAccu[PtLinkM].ptr[2*Mp+k+j*(int)period]=(j+1)*(int)period+k+1;//j+1,k
					}else{MAccu[PtLinkM].ptr[2*Mp+k+j*(int)period]=0;//no link point index
						}
					if (k>0){
					MAccu[PtLinkM].ptr[3*Mp+k+j*(int)period]=j*(int)period+k-1+1;//j,k-1
					}else{MAccu[PtLinkM].ptr[3*Mp+k+j*(int)period]=j*(int)period+(int)period-1+1;//j,period
						}
				}
			}
		EndMain:
	return Error;
	}

int DisplayObjectMatrix(Matrix *MAccu,int NumM,int PtLinkM,int DrawingMode){
	int Error,i,k,L,foundPlace,pt,pt1,pt2,NoDraw,m,mmax,minPt,maxPt;
	float xmax,xmin,x,x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4,X1,Y1,X2,Y2,X3,Y3,X4,Y4;

	SelectObject(hDC,hbrush);

	int Mp = MAccu[NumM].p;
	int Mn = MAccu[NumM].n;
	int PtLinkMn = MAccu[PtLinkM].n;
	int PtLinkMp = MAccu[PtLinkM].p;
	if (MAccu[NumM].ptr == 0) {Error = 1; PrintCmd("dispobjM: objM not defined!\n");goto OutDrawObject;}
	if (MAccu[PtLinkM].ptr == 0) {Error = 1; PrintCmd("disobjM: PtLinkMatrix not defined!\n");goto OutDrawObject;}
	if (Mn!=3 && Mn!=4) {PrintCmd("dispobjM: objMn should be 3 or 4 \n");Error =1;goto OutDrawObject;}
	if (PtLinkMp!=Mp) {PrintCmd("dispobjM: objMp should be same as PtLinkM.p\n");Error=1;goto OutDrawObject;} 
	if (PtLinkMn!=4) {PrintCmd("dispobjM: PtLinkM.n should be equal to 4\n");Error=1;goto OutDrawObject;} 

	
			//	x3,y3,z3 (PtLink pt1)			x4,y4,z4
			//     ---------------------------------
			//     |             Square               	   |
			//     |           definition           	   |
			//     ---------------------------------
			//  x1,y1,z1            x2,y2,z2 (PtLink pt2)

	//						---- DrawingMode 0  ----
	if (DrawingMode == 0){ //iron wires
		for (k=0;k<Mp;k++){
	
	
			x1=MAccu[NumM].ptr[ 0*Mp + k];
			y1=MAccu[NumM].ptr[ 1*Mp + k];
			z1=MAccu[NumM].ptr[ 2*Mp + k];
		NoDraw=0;
			pt1 = MAccu[PtLinkM].ptr[0*Mp+k]; //get ptlink 1
			if (pt1 == 0) NoDraw=1;
			else {x3=MAccu[NumM].ptr[ 0*Mp + pt1-1]; //pt-1 because index is in Matrix math convention starting from 1 and not 0
			y3=MAccu[NumM].ptr[ 1*Mp + pt1-1];
			z3=MAccu[NumM].ptr[ 2*Mp + pt1-1];
			}
			
			pt2 = MAccu[PtLinkM].ptr[1*Mp+k];//get ptlink 2
			if (pt2 == 0) NoDraw=1;
			else{x2=MAccu[NumM].ptr[ 0*Mp + pt2-1];
			y2=MAccu[NumM].ptr[ 1*Mp + pt2-1];
			z2=MAccu[NumM].ptr[ 2*Mp + pt2-1];
			}
			//Remarque: We just draw line 2 by 2    x1<->x2, x1<->x3
			//It is enough to draw everything in the other points handling
		if (NoDraw==0){
			X1=y1-x1*(xp-y1)/zp;
			Y1=z1-x1*(yp-z1)/zp;
			X2=y2-x2*(xp-y2)/zp;
			Y2=z2-x2*(yp-z2)/zp;
			X3=y3-x3*(xp-y3)/zp;
			Y3=z3-x3*(yp-z3)/zp;
			if (x1>0){
				Line(X1,Y1,X2,Y2,ColorGraph);
				Line(X1,Y1,X3,Y3,ColorGraph);
			}else{
				Line(X1,Y1,X2,Y2,0);
				Line(X1,Y1,X3,Y3,0);
			}
			
			
			}//if NoDraw
		}//for k
	}//if DrawingMode 0

	//						---- DrawingMode 1  ----
	if (DrawingMode == 1 || DrawingMode == 2){ //only visible faces
	//DrawingMode 1=3D display, 2=Projection on 2D display
	//Here we need to find the points order to display in x from back to front
	//therefore for x decreasing order.
	ListOrder *list = (ListOrder*) malloc((Mp+1)*sizeof (struct ListOrder));
	if (list==0) {PrintCmd("dispobjM: Can't allocate memory for ListOrder\n"); goto OutDrawObject;}

	//Get the xmin of the whole object
	xmin=MAccu[NumM].ptr[0*Mp+0];
	minPt=0;//init
	for (k=0;k<Mp;k++) if(MAccu[NumM].ptr[0*Mp+k] < xmin){
										xmin = MAccu[NumM].ptr[0*Mp+k];
										minPt=k;
										}

	//Get the xmax of the whole object
	xmax=MAccu[NumM].ptr[0*Mp+0];
	maxPt=0;
	for (k=0;k<Mp;k++) if(MAccu[NumM].ptr[0*Mp+k] >= xmax){
										xmax = MAccu[NumM].ptr[0*Mp+k];
										maxPt=k;
										}
	//Set first value
	list[0].pt = minPt; //point on the first value in the matrix
	list[0].x = xmin;
	list[0].next=1;
	
	list[1].pt = maxPt; //point on the first value in the matrix
	list[1].x = xmax;
	list[1].next=-1;//The end code of next is -1 it should be never read if below is done fine
	mmax=2; //Therefore there are now two values in list[]

	//fill list[]
	for(k=0;k<Mp;k++){
		x=MAccu[NumM].ptr[0*Mp+k];
		if(k != minPt && k != maxPt){
			foundPlace=0;m=0;
			while (m<mmax && foundPlace==0){ //loop over the already stocked info in list to check where to put the new x data
				if(m!=1)if(list[m].x <= x && x <= list[list[m].next].x){
					//place this new point in between
					if (mmax>=Mp) {PrintCmd("Error mmax>Mp\n");for(i=0;i<Mp;i++){sprintf(s,"list[%3d]:\n.x=%d\n.pt=%d\n.next=%d\n",i,(int)list[i].x,list[i].pt,list[i].next);PrintCmd(s);}goto FREELIST;}	
					list[mmax].x=x;
					list[mmax].pt=k;
					list[mmax].next=list[m].next;
					list[m].next=mmax;
					mmax++;
					foundPlace=1;
					}//if list[m]
				m++;	
			}//while m
		}
	}//for k fill list
	if(list[1].next!=-1) {PrintCmd("list[1].next==-1!");goto FREELIST;}
	if (mmax !=Mp) {PrintCmd("xmin=");Rprintf((float)xmin);PrintCmd("xmax=");Rprintf((float)xmax);
				PrintCmd("Error in processing list Ordering:\nmmax=");Rprintf((float)mmax);goto FREELIST;}
	
	//The list is now filled

OUTLIST:
	
	//Below we draw the object Matrix from x min to xmax
	//               -----  Drawing Object with hidden faces -----
	L=0; //init at L=0, 
			//L will varie from one value to the other and will end up at L=1
			//because it is where is the ending xmin in the list.
	while (L!=-1){
			k=list[L].pt;
			L=list[L].next; //loading the next L for next round in the while
			x1=MAccu[NumM].ptr[ 0*Mp + k];
			y1=MAccu[NumM].ptr[ 1*Mp + k];
			z1=MAccu[NumM].ptr[ 2*Mp + k];
		NoDraw=0;
			pt1 = MAccu[PtLinkM].ptr[0*Mp+k]; //get ptlink 1
			if (pt1 == 0) NoDraw=1; //do not draw
			else {x3=MAccu[NumM].ptr[ 0*Mp + pt1-1]; //pt-1 because index is in Matrix math convention starting from 1 and not 0
			y3=MAccu[NumM].ptr[ 1*Mp + pt1-1];
			z3=MAccu[NumM].ptr[ 2*Mp + pt1-1];
			}
			
			pt2 = MAccu[PtLinkM].ptr[1*Mp+k];//get ptlink 2
			if (pt2 == 0) NoDraw=1;
			else{x2=MAccu[NumM].ptr[ 0*Mp + pt2-1];
			y2=MAccu[NumM].ptr[ 1*Mp + pt2-1];
			z2=MAccu[NumM].ptr[ 2*Mp + pt2-1];
			}
			//pt2 is used now to get the point P4(x4,y4,z4)
			pt = MAccu[PtLinkM].ptr[0*Mp+pt2-1];//get ptlink 1 of previous point pt2
			if (pt == 0) NoDraw=1;
			else{x4=MAccu[NumM].ptr[ 0*Mp + pt-1];
			y4=MAccu[NumM].ptr[ 1*Mp + pt-1];
			z4=MAccu[NumM].ptr[ 2*Mp + pt-1];
			}
			//Remarque: We just draw line 2 by 2    x1<->x2, x1<->x3
			//It is enough to draw everything in the other points handling
		if (NoDraw==0){//ok to draw
		
			//Here we have to make the test of over riding and hiding lines
			if (DrawingMode == 1){
			X1=y1-x1*(xp-y1)/zp;
			Y1=z1-x1*(yp-z1)/zp;
			X2=y2-x2*(xp-y2)/zp;
			Y2=z2-x2*(yp-z2)/zp;
			X3=y3-x3*(xp-y3)/zp;
			Y3=z3-x3*(yp-z3)/zp;
			X4=y4-x4*(xp-y4)/zp;
			Y4=z4-x4*(yp-z4)/zp;
			}else{//DrawingMode==2
			X1=y1;X2=y2;X3=y3;X4=y4; //we projecte with similarity with the 3D but on 2D none perspective
			Y1=z1;Y2=z2;Y3=z3;Y4=z4;
			}
			FillQuadrilatere(X1,Y1,X2,Y2,X3,Y3,X4,Y4,ColorGraph);
			
			/*	Line(X1,Y1,X2,Y2,0);
				Line(X1,Y1,X3,Y3,0);
				Line(X2,Y2,X4,Y4,0);
				Line(X3,Y3,X4,Y4,0);*/
		//	goto FREELIST;
			
			}//if NoDraw

		}//while

	FREELIST:
		free(list);
	}//if DrawingMode 1

	OutDrawObject:
		return Error;
}


void FillQuadrilatere(float X1,float Y1,float X2,float Y2,float X3,float Y3,float X4,float Y4,int ColorGraph){
	Points X[4];
	POINT quadpoints[4];
	int k;
	float x1,x2,y1,y2;
	SetPolyFillMode(hDC,WINDING);
	X[0].x=X1;X[0].y=Y1;
	X[1].x=X2;X[1].y=Y2;
	X[2].x=X4;X[2].y=Y4;//P3 and P4 inverted to turn properly
	X[3].x=X3;X[3].y=Y3;
	//resizing according to gfxdim values
	for (k=0;k<4;k++)if (X[k].x<DimXmin ) {goto out;}
	for (k=0;k<4;k++)if (X[k].x >DimXmax ){goto out;}
	for (k=0;k<4;k++)if (X[k].y<DimYmin ) {goto out;}
	for (k=0;k<4;k++)if (X[k].y >DimYmax ){goto out;}
	for (k=0;k<4;k++) X[k].x= (X[k].x-DimXmin)*DrawZoneW/(DimXmax-DimXmin)+DrawZoneX;
	for (k=0;k<4;k++) X[k].y= -(X[k].y-DimYmin)*DrawZoneH/(DimYmax-DimYmin)+DrawZoneY+DrawZoneH;
	for (k=0;k<4;k++) {quadpoints[k].x=(long) X[k].x;quadpoints[k].y=(long) X[k].y;}
	Polygon (hDC,quadpoints,4);
	//for (k=0;k<3;k++)WinDrawLine(X[k].x,X[k].y,X[k+1].x,X[k+1].y);
	out:
		return;
}

int FunctToObjMatrix(Matrix *MAccu,int NumM,int M2,int PtLinkM){
	//NumM is a meshgrid
	//M2 and PtLink will form the object
	int Error,j,k;
	int Mp = MAccu[NumM].p;
	int Mn = MAccu[NumM].n;
	int M2p = MAccu[M2].p;
	int M2n = MAccu[M2].n;
	int PtLinkMn = MAccu[PtLinkM].n;
	int PtLinkMp = MAccu[PtLinkM].p;
	if (MAccu[NumM].ptr == 0) {Error = 1; PrintCmd("fctobjM: objM not defined!\n");goto OutDrawObject;}
	if (MAccu[M2].ptr == 0) {Error = 1; PrintCmd("fctobjM: objM not defined!\n");goto OutDrawObject;}
	if (MAccu[PtLinkM].ptr == 0) {Error = 1; PrintCmd("fctobjM: PtLinkMatrix not defined!\n");goto OutDrawObject;}
	if (M2n!=3 && M2n!=4) {PrintCmd("fctobjM: objM2n should be 3 or 4 \n");Error =1;goto OutDrawObject;}
	if (PtLinkMp!=M2p) {PrintCmd("fctobjM: objMp should be same as PtLinkM.p\n");Error=1;goto OutDrawObject;} 
	if (M2p!=Mp*Mn) {PrintCmd("fctobjM: objMp must be equal to M2n*M2p\n");Error=1;goto OutDrawObject;} 
	if (PtLinkMn!=4) {PrintCmd("fctobjM: PtLinkM.n should be equal to 4\n");Error=1;goto OutDrawObject;} 

	int period=Mn;			//x
	int NbrLatitude=Mp;	//y
			for (j=0;j<Mp;j++){
				for (k=0;k<Mn;k++){
					MAccu[M2].ptr[k+j*Mn]= k+1; //x
					MAccu[M2].ptr[M2p+k+j*Mn]=j+1 ;//y
					MAccu[M2].ptr[M2p*2+k+j*Mn]= MAccu[NumM].ptr[k*Mp+j];//z
					}
			}
	//fillPtLinkM CAREFUL: the index on mtx are from 1 to p, but here in memory the tables start from 0, we thus add 1
	//            Table of link
	//				  P1 |
	//			  P4 --   -- P2  
	//					   |  P3
	
			for (j=0;j<NbrLatitude;j++){//index over y
				for (k=0;k<period;k++){//index over x
					if (j>0){
					MAccu[PtLinkM].ptr[k+j*period]=(j-1)*period+k+1;// j-1,k
					}else{//condition au limites
						MAccu[PtLinkM].ptr[k+j*period]=0;// no link point index
						}	
					if (k<period-1){
					MAccu[PtLinkM].ptr[M2p+k+j*period]=j*period+k+1+1;//j,k+1
					}else{MAccu[PtLinkM].ptr[M2p+k+j*period]=0;//j*period+1;//j,k=0
						}
					if (j<NbrLatitude-1){
					MAccu[PtLinkM].ptr[2*M2p+k+j*period]=(j+1)*period+k+1;//j+1,k
					}else{MAccu[PtLinkM].ptr[2*M2p+k+j*period]=0;//no link point index
						}
					if (k>0){
					MAccu[PtLinkM].ptr[3*M2p+k+j*period]=j*period+k-1+1;//j,k-1
					}else{MAccu[PtLinkM].ptr[3*M2p+k+j*period]=0;//j*period+period-1+1;//j,period
						}
				}
			}
		EndMain:
		OutDrawObject:
	return Error;
}


int GetClosestPoint(Matrix *MAccu,int NumM,int VectP,int ResM3,int mask){

//Matrix VectP is a point line or column dim =3*1 or 1*3 
//it is the point to match

//Matrix ResM3 is the matrix which will contain the result found, we give the closest and another 
//close point in case that one is not the good one, the user will chose
int k,kmin,kmin2,kmin3;
float hmin,hmin2,dx,dy,dz,h,m1,m2,m3;
int Mn=MAccu[NumM].n;
int Mp=MAccu[NumM].p;
int VectMn=MAccu[VectP].n;
int VectMp=MAccu[VectP].p;
int ResM3n=MAccu[ResM3].n;
int ResM3p=MAccu[ResM3].p;
if (mask>7){PrintCmd("Mask should be <7, read manual\n");return 1;}
if (Mn!=3 && Mn !=4){PrintCmd("Mn should be 3 or 4\n"); return 1;}
if (VectMn*VectMp !=3){PrintCmd("VectP should be 3*1 or 1*3 size\n"); return 1;}
if (ResM3n*ResM3p<3){PrintCmd("ResM3 shoud be at least size 3\n");return 1;}
//bit4=0, bit 3 is x, bit 2 is y, bit 0 is z   mask=%0xyz
 if(mask&0x01==1)m3=1;else m3=0;//to considere mask
 if(mask&0x02==2)m2=1;else m2=0;
 if(mask&0x04==4)m1=1;else m1=0;
float Ptx=MAccu[VectP].ptr[0];
float Pty=MAccu[VectP].ptr[1];
float Ptz=MAccu[VectP].ptr[2];
		dx=MAccu[NumM].ptr[0*Mp+0]-Ptx;
		dy=MAccu[NumM].ptr[1*Mp+0]-Pty;
		dz=MAccu[NumM].ptr[2*Mp+0]-Ptz;
		hmin=m1*dx*dx+m2*dy*dy+m3*dz*dz;
		kmin=0;kmin2=0;kmin3=0;hmin2=hmin;
	for (k=1;k<Mp;k++){
		dx=MAccu[NumM].ptr[0*Mp+k]-Ptx;
		dy=MAccu[NumM].ptr[1*Mp+k]-Pty;
		dz=MAccu[NumM].ptr[2*Mp+k]-Ptz;
		h=m1*dx*dx+m2*dy*dy+m3*dz*dz;
		//if(hmin2==h){kmin3=k+1;}
		//if (hmin==h){kmin2=k+1;hmin2=h;}
		if (hmin>=h){kmin3=kmin2,kmin2=kmin;hmin2=hmin;kmin=k;hmin=h;}
	}
	
	//the result is the index in math convention strating from 1
	//if an index is 0 for kmin2, kmin3: it means that there was no findings
	MAccu[ResM3].ptr[0]=(float)(kmin+1);
	MAccu[ResM3].ptr[1]=(float)(kmin2+1);
	MAccu[ResM3].ptr[2]=(float)(kmin3+1);
	return 0;
}

int ModifObject(Matrix* MAccu,int NumM,int PtLinkM,int IndexP,int Vect,int mod){

//modobj NumM,PtLinkM,IndexP,Vect2,mod
//NumM is the object matrix PtLink its link matrix
//VectP is the Point Index in the matrix NumM where the modif is localised
//Vect2 is a vector to assist the change, direction and intensity
//mod is the mode: 
//				0 flatten with plane orthogonal to Vect2 starting at point VectP
//				1 exponential large change along Vect2 with an exponantial centered in VectP 
//				2 exponential narrow change along Vect2 with an exponantial centered in VectP 
//				3
//P0
float alfa,d,d2,x,y,z,x0,y0,z0;
int k;
	int Mp = MAccu[NumM].p;
	int Mn = MAccu[NumM].n;
	int PtLinkMn = MAccu[PtLinkM].n;
	int PtLinkMp = MAccu[PtLinkM].p;

	if (MAccu[NumM].ptr == 0) {PrintCmd("modobjM: objM not defined!\n");return 1;}
	if (MAccu[PtLinkM].ptr == 0) {PrintCmd("modobjM: PtLinkMatrix not defined!\n");return 1;}
	if (MAccu[Vect].ptr == 0) {PrintCmd("modobjM: Vect not defined!\n");return 1;}
	if (PtLinkMp!=Mp) {PrintCmd("modobjM: objMp should be same as PtLinkM.p\n");return 1;} 
	if (PtLinkMn!=4) {PrintCmd("modobjM: PtLinkM.n should be equal to 4\n");return 1;} 
	if (MAccu[Vect].n*MAccu[Vect].p!=3) {PrintCmd("modobjM: Vect n*p should equal 3\n");return 1;} 

	float a=MAccu[Vect].ptr[0];
	float b=MAccu[Vect].ptr[1];
	float c=MAccu[Vect].ptr[2];
	if (a==0&&b==0&&c==0){PrintCmd("modobjM: Vect is null!\n");return 1;}

switch (mod){
	case 0:
	x0=MAccu[NumM].ptr[0*Mp+IndexP-1]+a; //point xA c'est le point du plan d'aplatissage
	y0=MAccu[NumM].ptr[1*Mp+IndexP-1]+b; //point yA
	z0=MAccu[NumM].ptr[2*Mp+IndexP-1]+c; //point  zA
	d=a*x0+b*y0+c*z0; //Plan Equation: ax+by+cz=d
			for (k=0;k<Mp;k++){
				x=MAccu[NumM].ptr[0*Mp+k];
				y=MAccu[NumM].ptr[1*Mp+k];
				z=MAccu[NumM].ptr[2*Mp+k];
				d2=a*x+b*y+c*z;
				if (d2-d<0){ //( a*(x-x0)+b*(y-y0)+c*(z-z0) <0 ){ scalar vector AM1.n<0
					//we project the point on the plan of 
					//normal vector Vect and point P
					alfa=(d-d2)/(a*a+b*b+c*c);
					//proj. on plan coordinates:
					MAccu[NumM].ptr[0*Mp+k]=x+alfa*a;
					MAccu[NumM].ptr[1*Mp+k]=y+alfa*b;
					MAccu[NumM].ptr[2*Mp+k]=z+alfa*c;
					}
		}
	break;
	case 1:
	x0=MAccu[NumM].ptr[0*Mp+IndexP-1]; //point xA c'est le point sur l'objet
	y0=MAccu[NumM].ptr[1*Mp+IndexP-1]; //point yA
	z0=MAccu[NumM].ptr[2*Mp+IndexP-1]; //point  zA
		for (k=0;k<Mp;k++){
				x=MAccu[NumM].ptr[0*Mp+k];
				y=MAccu[NumM].ptr[1*Mp+k];
				z=MAccu[NumM].ptr[2*Mp+k];
				d2=(x-x0)*(x-x0)+(y-y0)*(y-y0)+(z-z0)*(z-z0); //distance Pt-P0
				alfa=RMath_exp(-d2*(a*a+b*b+c*c));//vect grand, base serrée et hauteur grande
				MAccu[NumM].ptr[0*Mp+k]=x+alfa*a;
				MAccu[NumM].ptr[1*Mp+k]=y+alfa*b;
				MAccu[NumM].ptr[2*Mp+k]=z+alfa*c;			
		}
	break;
	case 2:
	x0=MAccu[NumM].ptr[0*Mp+IndexP-1]; //point xA c'est le point sur l'objet
	y0=MAccu[NumM].ptr[1*Mp+IndexP-1]; //point yA
	z0=MAccu[NumM].ptr[2*Mp+IndexP-1]; //point  zA
		for (k=0;k<Mp;k++){
				x=MAccu[NumM].ptr[0*Mp+k];
				y=MAccu[NumM].ptr[1*Mp+k];
				z=MAccu[NumM].ptr[2*Mp+k];
				d2=(x-x0)*(x-x0)+(y-y0)*(y-y0)+(z-z0)*(z-z0); //distance Pt-P0
				alfa=RMath_exp(-4*d2*(a*a+b*b+c*c));//vect grand, base serrée et hauteur grande
				MAccu[NumM].ptr[0*Mp+k]=x+alfa*a;
				MAccu[NumM].ptr[1*Mp+k]=y+alfa*b;
				MAccu[NumM].ptr[2*Mp+k]=z+alfa*c;			
		}
	break;
	case 3:
	x0=MAccu[NumM].ptr[0*Mp+IndexP-1]; //point xA c'est le point sur l'objet
	y0=MAccu[NumM].ptr[1*Mp+IndexP-1]; //point yA
	z0=MAccu[NumM].ptr[2*Mp+IndexP-1]; //point  zA
		for (k=0;k<Mp;k++){
				x=MAccu[NumM].ptr[0*Mp+k];
				y=MAccu[NumM].ptr[1*Mp+k];
				z=MAccu[NumM].ptr[2*Mp+k];
				d2=(x-x0)*(x-x0)+(y-y0)*(y-y0)+(z-z0)*(z-z0); //distance Pt-P0
				alfa=RMath_exp(-8*d2*(a*a+b*b+c*c));//vect grand, base serrée et hauteur grande
				MAccu[NumM].ptr[0*Mp+k]=x+alfa*a;
				MAccu[NumM].ptr[1*Mp+k]=y+alfa*b;
				MAccu[NumM].ptr[2*Mp+k]=z+alfa*c;			
		}
	break;

}
	return 0;
}



//------------------- Matrix Operations Analysis ------------------------------------
int MatrixPower(Matrix *MAccu,floactet *CodeListLine,int i,int imaxLine){
	int Error=1;
	PrintCmd("Function Power of matrix is not yet implemented\n");
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
			if (CodeListLine[j].code == 0xFF) {return 0;}// No matrix because it could have been M[1,1] =>scalar PrintCmd("no matrix???\n");return 9;}
			if (CodeListLine[j].code == 1&&CodeListLine[j+1].code == 0xFF) {return 0;}// This is used to let the function mtxn(Mn°) work properly as they return a scalar and not a matrix 
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

			if (MAccu[NumM1].p == MAccu[NumM2].n) {//conventional M1xM2
			//M1*M2 just start below
			MAccu[NewM].ptr = (float *) malloc (MAccu[NumM1].n*MAccu[NumM2].p*sizeof(float) );
			if (MAccu[NewM].ptr == 0 ) {PrintCmd ("Error couldn't allocate memory for temporary matrix\n"); return 9;}
			MAccu[NewM].n=MAccu[NumM1].n;
			MAccu[NewM].p=MAccu[NumM2].p;
			int M1n=MAccu[NumM1].n;
			int M1p=MAccu[NumM1].p;
			int M2n=MAccu[NumM2].n;
			int M2p=MAccu[NumM2].p;
			float *mat1=MAccu[NumM1].ptr;
			float *mat2=MAccu[NumM2].ptr;
			float Sum=0;
				for (j=0;j<MAccu[NumM1].n;j++){
					for (k=0;k<MAccu[NumM2].p;k++){
						Sum=0;
						//for (l=0;l<MAccu[NumM1].p;l++)Sum=Sum+MAccu[NumM1].ptr[j*MAccu[NumM1].p+l]*MAccu[NumM2].ptr[l*MAccu[NumM2].p+k];
						for (l=0;l<MAccu[NumM1].p;l++)Sum=Sum+mat1[j*M1p+l]*mat2[l*M2p+k];
						MAccu[NewM].ptr[j*MAccu[NewM].p+k]=Sum;
					}
				}
			}else{if (MAccu[NumM1].n == 1 && MAccu[NumM2].n==1 &&
							MAccu[NumM1].p == MAccu[NumM2].p){
							//We make a scalar style product M1*M2
								MAccu[NewM].ptr = (float *) malloc (MAccu[NumM1].n*MAccu[NumM1].p*sizeof(float) );
								if (MAccu[NewM].ptr == 0 ) {PrintCmd ("Error couldn't allocate memory for temporary matrix\n"); return 9;}
								MAccu[NewM].n=MAccu[NumM1].n;
								MAccu[NewM].p=MAccu[NumM1].p;

							for (j=0;j<MAccu[NumM1].n*MAccu[NumM1].p;j++){
								MAccu[NewM].ptr[j]=MAccu[NumM1].ptr[j]*MAccu[NumM2].ptr[j];
							}
				}else{PrintCmd("Incompatible matrix size Mtx1.p must equal Mtx2.n\n");
						return 9;
						}
			}

		//done result is in NewM
		//Now we put the matrix result in the CodeListLine
		if(MAccu[NewM].n==1 &&MAccu[NewM].p==1){
		//it is in fact a scalar sizeM =[1,1]... therefore we put a scalar
		CodeListLine[i].code=1; //mtx
		CodeListLine[i].value=MAccu[NewM].ptr[0]; // n°mtx
		}else{
		CodeListLine[i].code=15; //mtx
		CodeListLine[i].value=NewM; // n°mtx
		}
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
	//it shouldn't arrive here so if it is the case, generate error
	return 9;
}
int MatrixDivision(Matrix *MAccu,floactet *CodeListLine,int i,int imaxLine){
	int Error=0;
	return Error;
}

	
int MathFunctionMatrices (Matrix *MAccu, floactet *CodeListLine,int i){
		extern int NbrMaxMatrix; 
		int m,NewM,Mn,Mp;
		int j,k;
		float val;
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
			case 27:	//trp  transposition of the matrix
				Mn=MAccu[NumM].n;
				Mp=MAccu[NumM].p;
				MAccu[NewM].p=Mn; //inversion for transposition
				MAccu[NewM].n=Mp;
				if (NumM!=NewM){
					for (j=0;j<Mn*Mp; j++)MAccu[NewM].ptr[j] = MAccu[NumM].ptr[j];
				}
			break;
			case 28:	//mtxn
				val=(float)MAccu[NumM].n;
				goto PlaceVal;
			break;
			case 29:	//mtxp
				val=(float)MAccu[NumM].p;
				goto PlaceVal;
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
	
	PlaceVal:
		if (MathError !=0 ) {sprintf(s,"Math Error = %d\n",MathError);PrintCmd(s);return 3;}
		if (MathError == 0) {//Place scalar Result
				CodeListLine[i].code=1; 
				CodeListLine[i].value=val;
				}
		k=i+1;
		while (CodeListLine[k].code !=0xFF && CodeListLine[k].code !=0) {
			CodeListLine[k]=CodeListLine[k+1];
			k++;
			}
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
	HPEN	hpen;
	if (lastcolor != color ){
	colpen   = 0x000000;
	hpen	= CreatePen(PS_SOLID,1,colpen);
	SelectObject(hDC,hpen);
	if (color == 1)SelectionStylo(0x0A0AFF);//red
	if (color == 2)SelectionStylo(0x0AFF0A);//green
	if (color == 3)SelectionStylo(0xFF0A0A);//blue
	if (color == 4)SelectionStylo(0x0A0AFF);//yellow
	if (color == 5)SelectionStylo(0xFF0FFF);//
	if (color == 6)SelectionStylo(0x0FFFFF);//
	if (color >6) SelectionStylo(0x0111111*(color-6));
	if (color>22) SelectionStylo(0x0001111*(color-22));
	if (color>38) SelectionStylo(0x0111100*(color-38));
	if (color>54) SelectionStylo(0x0110011*(color-54));
	if (color>70) SelectionStylo(0x000000);
	lastcolor = color;
	}
	
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

	WinDrawLine ( x1*DrawZoneW/160+DrawZoneX,
									-y1*DrawZoneH/160+DrawZoneY+DrawZoneH,
										x2*DrawZoneW/160+DrawZoneX,
										-y2*DrawZoneH/160+DrawZoneY+DrawZoneH);
	

	out:
		return;
	}


	