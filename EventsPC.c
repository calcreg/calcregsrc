/* -----------------------
 *
 * Windows Events
 * Mouse, clicks, ...
 * 
 * BUSSY-SOCRATE REGAN
 *
 * ----------------------*/


/* Includes */
#include <windows.h>			// system
#include <stdio.h>
#include "CalcReg.h"

int MainFormHandleEvent(int event);

static int Rmin (int x1,int x2);
static int Rmax (int x1,int x2);
int Rabs(int x1);

extern void Execute(void);

extern void PrintCmd(char * s) ;
extern void DeleteCmd() ;
extern void Printf(const char * format, ...); //should be Printf and not printf otherwise it conflicts with StdIOPalm.h definition
//static float sscanf (char *StringNbr,char* s, float *Nbr);
extern void SetUpTextProg();
extern void HideKeyPad();
extern void ShowKeyPad();
extern void DeleteProg();
extern void PrintProg(char * s);
extern void Rprintf(float x);

extern int testProg;
extern int displayval;

/* for the Handling of the Pen Zooming and moving */
int XpenDown, YpenDown,XpenUp,YpenUp;
int PenMoved=0; //=1 when down and moved then up
int RedrawingGfx=0;
extern float DimXmin,DimXmax,DimYmin,DimYmax,IncX;
extern float DrawZoneX, DrawZoneY,DrawZoneW,DrawZoneH;
int GfxMove=1,GfxZoom=0,Pset=0; //at the begining the movability is set
int GfxDerivate=0; // If =1 then Draw the function with its derivated
int Button=0; //for the keyboard in CalcRegPC
int ProgInExecution=0; //=1 while program in execution
/*
 * MainFormHandleEvent
 */
int MainFormHandleEvent(int event) {
	char ValueStr[300];
	int size, handled;
	extern HWND hEditP;

	float newDimXmin,newDimXmax,newDimYmin,newDimYmax;
	handled=0; //false=0 true =1
	if (event !=0) {
		switch (event) {
	
   			case btnstart:	
				if (ProgExecution == 1){
					PrintCmd ("program didn't finish yet\n");
					return true;
				}else{
					GfxMove=1;	GfxDerivate=0;GfxZoom=0; //Re-init each time we launch with EXE 
					SetUpTextProg(0);
					ProgInExecution=1;
					Execute();
					ProgInExecution=0;
				}
   				break;
				
			case btnCP:
				//if (FrmAlert (SureDeleteProg) == 0){DeleteProg();}
				printf("Alert Sure?\n");
   				break;
			case btnkeypad:
				//ShowKeyPad();FrmHideObject(Frm, (FrmGetObjectIndex(Frm, btnkeypad)));				
				break;
			case btngfxwork:
				//FrmHideObject(Frm, (FrmGetObjectIndex(Frm, btngfxwork)));
				//FrmShowObject(Frm, (FrmGetObjectIndex(Frm, btngfxmove)));
				GfxZoom=1;
				GfxMove=0;
				break;
			case btngfxderivate:
				GfxDerivate=1;
				RedrawingGfx=1;
				Execute();
				break;
			case btngfxmove:
				GfxZoom=0;
				GfxMove=1;
				//FrmHideObject(Frm, (FrmGetObjectIndex(Frm, btngfxmove)));
				//FrmShowObject(Frm, (FrmGetObjectIndex(Frm, btngfxwork)));
				break;

			case btngfxmoins:
					newDimXmin=DimXmin-(DimXmax-DimXmin)/5;
					newDimXmax=DimXmax+(DimXmax-DimXmin)/5;
					newDimYmin=DimYmin-(DimYmax-DimYmin)/5;
					newDimYmax=DimYmax+(DimYmax-DimYmin)/5;
					if (newDimXmin!=DimXmin) IncX=(newDimXmax-newDimXmin)*IncX/(DimXmax-DimXmin);//proportionnal change
					DimXmin=newDimXmin;
					DimXmax=newDimXmax;
					DimYmin=newDimYmin;
					DimYmax=newDimYmax;
					//IncX=IncX*4/3;
					RedrawingGfx=1;
					Execute();
					XpenDown = 0;	XpenUp = 0; PenMoved=0;
					break;
					
			case btngfxplus:
					newDimXmin=DimXmin+(DimXmax-DimXmin)/5;
					newDimXmax=DimXmax-(DimXmax-DimXmin)/5;
					newDimYmin=DimYmin+(DimYmax-DimYmin)/5;
					newDimYmax=DimYmax-(DimYmax-DimYmin)/5;
					if (newDimXmin!=DimXmin) IncX=(newDimXmax-newDimXmin)*IncX/(DimXmax-DimXmin);//proportionnal change
					DimXmin=newDimXmin;
					DimXmax=newDimXmax;
					DimYmin=newDimYmin;
					DimYmax=newDimYmax;
					//IncX=IncX*1/2;
					RedrawingGfx=1;
					Execute();
					XpenDown = 0;	XpenUp = 0; PenMoved=0;
					break;
					
			case btngfxpset:
					Pset=1;
					break;
				
			case btnconst:
				sprintf(ValueStr, "c=3E+8\ng=9.81\nG=6.67E-11\nNa=6.022E-23\nh=6.63E-34\ne=1.6E-19\npi=3.1415927\n");
				PrintProg(ValueStr);
				SetUpTextProg(0);
   				break;
				
				
   			case btntest:
				DeleteProg();
				if (testProg == 0 ) PrintProg("defM0,1,10\ndefM1,1,10\nfillM0,0,2\nM1=M0\nprint M1(1,5)\nM1=cos(M0)\nprint M1(1,1)\n");
				//if( testProg == 0 ) PrintProg( "gfxdim -3,3,-10,3,0.1\nx=-3\nTrf(x)= -x + 1\nx=-3\nTrf(x)= -x^2 + 1\n");
				if( testProg == 1 ) PrintProg( "gfxdim -6,6,-1.2,1.2,0.1\nx=-6\nTrf(x)=sin (x)\nTrf(x)= cos (x) \n");
				if( testProg == 2 ) PrintProg( "gfxdim -7,5,-4,5,0.2\nbox3d 4,4,4,0.2\nx=4\ny=-4\nTrf(x,y)=3-3*exp(-(x^2+y^2)/2.5)*cos(0.9*(x^2+y^2))\n");
				if( testProg == 3 ) PrintProg( "//Test function Precision1\n//Should be near zero\ngfxdim -3,3,-0.0015,0.0015,0.07\nx=-3\nTrf(x)=-0.001\nTrf(x)=0.001\nTrf(x)=x - ln ( exp (x) ) \n");
				if( testProg == 4 ) PrintProg( "//Test function Precision2\n//should be near y=x-1\ngfxdim -3,3,-5,5,0.1\nx=-3\nTrf(x)=ln ( exp (x) )-1 \n");
				if( testProg == 5 ) PrintProg( "//Test function Precision3\n//should be  near y=1\ngfxdim -15,15,0.9998,1.0001,0.1\nx=-15\nTrf(x)=0.9999\nTrf(x)=1.00001\nTrf(x)= (cos(x))^2+(sin(x))^2\n");
				if( testProg == 6 ) PrintProg( "gfxdim -2,2,-1.1,1.1,0.05\nx=-3\nTrf(x)= exp(-x^2)*cos(10*x)\n");
				if( testProg == 7 ) PrintProg( "gfxdim -10,10,-10,10,0.05\ngrid3\nx0=-10\ny0=exp x0\nloop:\nx1=x0+1\ny1=exp x1\nline x0,y0,x1,y1,3\nx0=x1\ny0=y1\nx0<10=>goto loop\n");
				if( testProg == 8 ) PrintProg( "gfxdim -3,3,-1,3,0.1\nx=-3\nTrf(x)=x*exp (x)\nTrf(x)= x + 1\nTrf(x)= 1- 2*x\n");
				if( testProg == 9 ) PrintProg( "x=-3+i\nF1(x)=x\nprint Re(F1(x))\nprint Im(F1(x))\nF1(x)\n");
				if( testProg == 10 ) PrintProg("x=0\nbsr prog //branch subroutine\nend\n\nprog:\nx=x+1\nprint x\nx<10 =>goto prog\nrts //return from subroutine\n");

				testProg++;
				if (testProg >10) testProg=0;
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
				Button=9;
   				break;
   			case btn6:	
				Button=6;
   				break;
   			case btn3:	
				Button=3;
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
				Button=8;
   				break;
	   		case btn5:	
				Button=5;
   				break;
	   		case btn2:	
				Button=2;
   				break;
	   		case btnpnt:	
				PrintProg(".");
   				break;
	   		case btncos:	
				PrintProg("cos(");
   				break;
	   		case btn7:	
				Button=7;
   				break;
	   		case btn4:	
				Button=4;
   				break;
	   		case btn1:	
				Button=1;
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
		handled = 1;
	}
/*PalmOs Handling Pen

		if (event->eType == penUpEvent) {
			XpenUp=event->screenX; 
			YpenUp=event->screenY;
			if (Rabs(XpenUp-(DrawZoneX+DrawZoneW/2))<DrawZoneW/2 &&
				Rabs(YpenUp-(DrawZoneY+DrawZoneH/2))<DrawZoneH/2) {

				if (PenMoved == 1 &&(Rabs(XpenUp-XpenDown)+ 
				Rabs(YpenUp-YpenDown) )>5 ){
				PenMoved=0;
				if(GfxZoom==1){//Zoom Window definition
					RedrawingGfx=1;
					newDimXmin= (min(XpenUp,XpenDown)-DrawZoneX)/DrawZoneW*(DimXmax-DimXmin)+DimXmin;
					newDimXmax= (max(XpenUp,XpenDown)-DrawZoneX)/DrawZoneW*(DimXmax-DimXmin)+DimXmin;
					newDimYmin= (-max(YpenUp,YpenDown)+DrawZoneY+DrawZoneH)/DrawZoneH*(DimYmax-DimYmin)+DimYmin;
					newDimYmax= (-min(YpenUp,YpenDown)+DrawZoneY+DrawZoneH)/DrawZoneH*(DimYmax-DimYmin)+DimYmin;
					if (newDimXmin!=DimXmin) IncX=(newDimXmax-newDimXmin)*IncX/(DimXmax-DimXmin);//proportionnal change
					DimXmin=newDimXmin;
					DimXmax=newDimXmax;
					DimYmin=newDimYmin;
					DimYmax=newDimYmax;
					Execute();
					//return true; //done					
					}
				if(GfxMove==1){//Moving deplacement
					RedrawingGfx=1;
					newDimXmin= (XpenUp-DrawZoneX)/DrawZoneW*(DimXmax-DimXmin)+DimXmin;
					newDimXmax= (XpenDown-DrawZoneX)/DrawZoneW*(DimXmax-DimXmin)+DimXmin;
					newDimYmin= (-YpenUp+DrawZoneY+DrawZoneH)/DrawZoneH*(DimYmax-DimYmin)+DimYmin;
					newDimYmax= (-YpenDown+DrawZoneY+DrawZoneH)/DrawZoneH*(DimYmax-DimYmin)+DimYmin;
					DimXmin=DimXmin+newDimXmax-newDimXmin;
					DimXmax=DimXmax+newDimXmax-newDimXmin;
					DimYmin=DimYmin+newDimYmax-newDimYmin;
					DimYmax=DimYmax+newDimYmax-newDimYmin;
					Execute();
					//return true; //done

					}
				}
				}
			}
		if (event->eType == penMoveEvent) {
			//PrintCmd("PenMoved\n");
			if (XpenDown != 0 && YpenDown != 0) {	PenMoved=1;}
			}
		*/
	return handled;
}


static int Rmin(int x1,int x2){
	if (x1>x2) return x2;
	else return x1;
	}
static int Rmax(int x1,int x2){
	if (x1<x2) return x2;
	else return x1;
	}
int Rabs(int x1){
		if (x1<0) return -x1;
		else return x1;
		}
