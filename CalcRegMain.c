/*---------------------
 *
 * Window Display
 *
 * BUSSY-SOCRATE REGAN
 *
 * --------------------*/
#include "windows.h"
#include "CalcReg.h"
#include <stdio.h>
//#include "icon.h"

#define IDC_MAIN_EDIT	101
#define ID_FILE_EXIT 9001
#define ID_STUFF_GO 9002
#define ID_FILE_Read 9003
#define ID_FILE_Save 9004

	//extern void DrawLine(int x, int y);
	extern void Execute(void);
	extern int doMainMenu(int);
	extern int MainFormHandleEvent(int event);
	
	BOOL SaveTextFileFromEdit(HWND hEdit, LPCTSTR pszFileName);
	BOOL LoadTextFileToEdit(HWND hEdit, LPCTSTR pszFileName);
	void DoFileOpen(HWND hwnd);
	void DoFileSave(HWND hwnd);
	char* DoLoadSound();

	extern int Rabs(int);
	extern Rprintf(float);
HINSTANCE	hinst=NULL;
HWND		btn_close,btn_line,btn_test,btn_ell,btn_brk;
HWND		btn_ClCmd,btn_ClProg,btn_gfxmove,btn_gfxwork,btn_start,btn_read,btn_gfxplus,btn_gfxmoins,btn_gfxderivate,btn_gfxpset;
HWND		btn_1,btn_2,btn_3,btn_4,btn_5,btn_6,btn_7,btn_8,btn_9,hmywin;
HDC			hDC;
RECT		drawrect,wndrect;
RECT		prect,brect;
COLORREF	colpen,colbrush;
	HFONT hfDefault;
	HWND hEditP,hEditC;//txt win

int			x,y;
extern Pset;
extern int XpenDown, YpenDown,XpenUp,YpenUp;
extern float DimXmin,DimXmax,DimYmin,DimYmax,IncX;
extern float DrawZoneX, DrawZoneY,DrawZoneW,DrawZoneH;
extern int PenMoved; //=1 when down and moved then up
extern int RedrawingGfx,GfxMove,GfxZoom;
extern int StopProgram, BreakActivated;
void CloseProc(HWND hwnd)
{
	ReleaseDC(hwnd,hDC);
	UnregisterClass("CalcReg",hinst);
	PostQuitMessage(0);	
}

void Paint(HWND hwnd)
{
	RECT wndrect, valrect; //valrect to get a mouse value between -10 and 10
	GetClientRect(hwnd,&wndrect);

	drawrect.left	= wndrect.left + 312;
	drawrect.right	= wndrect.right - 10;
	drawrect.top	= wndrect.top + 10;
	drawrect.bottom = wndrect.bottom - 10;

	//re-evaluate the graph size when changing the windows sizing
	//DrawZoneX=320;//125;
	//DrawZoneY=25;
	DrawZoneH=wndrect.bottom-50;//300;
	DrawZoneW=wndrect.right -340;//450-195;//450


	
	FillRect(hDC,&drawrect,CreateSolidBrush(0xFFFFFF));
	FrameRect(hDC,&drawrect,CreateSolidBrush(0x0A0AFF));
	
	valrect.left = wndrect.left + 10;
	valrect.right = wndrect.left + 60;
	valrect.top  = wndrect.top + 310;
	valrect.bottom = wndrect.top + 335;
	FillRect(hDC,&valrect,CreateSolidBrush(0xFFFFFF));
	FrameRect(hDC,&valrect,CreateSolidBrush(0x000000));

/*
	hbrush	= CreateSolidBrush(colbrush);
	hpen	= CreatePen(PS_SOLID,1,colpen);

	SelectObject(hDC,hbrush);
	SelectObject(hDC,hpen);

	FillRect(hDC,&prect,CreateSolidBrush(colpen));
	FrameRect(hDC,&prect,CreateSolidBrush(0x000000));
	FillRect(hDC,&brect,CreateSolidBrush(colbrush));
	FrameRect(hDC,&brect,CreateSolidBrush(0x000000));
	*/
}

void cColor(HWND hwnd, int opt)
{
CHOOSECOLOR ccolor;
COLORREF	chos_color=0x000000;
COLORREF	chosed1[16]={0x0000ff,0x0000ff,0x0000ff,0x0000ff,
						0x000000,0x000000,0x000000,0x000000,
						0x000000,0x000000,0x000000,0x000000,
						0xff0000,0xff0000,0xff0000,0xff0000};


	ZeroMemory(&ccolor,sizeof(CHOOSECOLOR));

	ccolor.lStructSize = sizeof(CHOOSECOLOR);
	ccolor.hwndOwner = hwnd;
	ccolor.rgbResult = chos_color;
	ccolor.lpCustColors = chosed1;
	ccolor.Flags = CC_RGBINIT|CC_FULLOPEN;
	ChooseColor(&ccolor);
	
	if(opt==1) colpen = ccolor.rgbResult;
	if(opt==2) colbrush = ccolor.rgbResult;
	Paint(hwnd);
}

int CheckBState(HWND hwnd)
{
	char txt[]="Choose Line, Rectangle or Ellipse!";

	if(SendMessage(btn_line,BM_GETCHECK,0,0)==1) return 1;
	//if(SendMessage(btn_rect,BM_GETCHECK,0,0)==1) return 2;
	if(SendMessage(btn_ell,BM_GETCHECK,0,0)==1) return 3; 
	return 0;

}

void ObjectCreation(HWND hwnd)
{
	GetClientRect(hwnd,&wndrect);	
	btn_gfxplus = CreateWindow("BUTTON","+",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+300,wndrect.top+10,12,14,hwnd,0,hinst,NULL);			
	btn_gfxmoins = CreateWindow("BUTTON","-",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+300,wndrect.top+25,12,14,hwnd,0,hinst,NULL);			
	btn_gfxderivate = CreateWindow("BUTTON","D",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+300,wndrect.top+40,12,14,hwnd,0,hinst,NULL);			
	btn_gfxpset = CreateWindow("BUTTON","*",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+300,wndrect.top+55,12,14,hwnd,0,hinst,NULL);			
	btn_gfxwork = CreateWindow("BUTTON","Z",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+300,wndrect.top+70,12,14,hwnd,0,hinst,NULL);			
	btn_gfxmove = CreateWindow("BUTTON","M",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+300,wndrect.top+85,12,14,hwnd,0,hinst,NULL);			

	//btn_brk = CreateWindow("BUTTON","Break",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+10,wndrect.top+45,50,25,hwnd,0,hinst,NULL);			
	//btn_ClProg = CreateWindow("BUTTON","clr prog",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+10,wndrect.top+15,50,25,hwnd,0,hinst,NULL);			
	btn_test = CreateWindow("BUTTON","Test",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+10,wndrect.top+75,50,25,hwnd,0,hinst,NULL);			
	btn_start = CreateWindow("BUTTON","EXE",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+10,wndrect.top+105,50,25,hwnd,0,hinst,NULL);			
	btn_close = CreateWindow("BUTTON","Close",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+10,wndrect.top+180,50,25,hwnd,0,hinst,NULL);
	btn_ClCmd = CreateWindow("BUTTON","Clear",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+10,wndrect.top+205,50,25,hwnd,0,hinst,NULL);			

	btn_1 = CreateWindow("BUTTON","1",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+10,wndrect.top+286,18,18,hwnd,0,hinst,NULL);			
	btn_2 = CreateWindow("BUTTON","2",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+28,wndrect.top+286,18,18,hwnd,0,hinst,NULL);			
	btn_3 = CreateWindow("BUTTON","3",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+46,wndrect.top+286,18,18,hwnd,0,hinst,NULL);			
	btn_4 = CreateWindow("BUTTON","4",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+10,wndrect.top+268,18,18,hwnd,0,hinst,NULL);			
	btn_5	= CreateWindow("BUTTON","5",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+28,wndrect.top+268,18,18,hwnd,0,hinst,NULL);			
	btn_6	= CreateWindow("BUTTON","6",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+46,wndrect.top+268,18,18,hwnd,0,hinst,NULL);			
	btn_7 = CreateWindow("BUTTON","7",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+10,wndrect.top+250,18,18,hwnd,0,hinst,NULL);			
	btn_8 = CreateWindow("BUTTON","8",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+28,wndrect.top+250,18,18,hwnd,0,hinst,NULL);			
	btn_9 = CreateWindow("BUTTON","9",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+46,wndrect.top+250,18,18,hwnd,0,hinst,NULL);			
	}

void DrawPrimitive(HWND hwnd,int x,int y,int x1,int y1)
{
	if(CheckBState(hwnd)==2)
	{
		Rectangle(hDC,x,y,x1,y1);
		return;
	}

	if(CheckBState(hwnd)==3)
	{
		Ellipse(hDC,x,y,x1,y1);
		return;
	}
	
		if(CheckBState(hwnd)==btnstart)
	{
		Execute();
		return;
	}
}


LRESULT CALLBACK WndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	float newDimXmin,newDimXmax,newDimYmin,newDimYmax;

	switch(msg)
	{
		case WM_DESTROY:
			CloseProc(hwnd);
			break;

		case WM_CREATE:
			ObjectCreation(hwnd);
			
		//Creation txt window heditProg

			hEditP = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
				WS_CHILD | WS_VISIBLE | WS_VSCROLL |WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL| ES_AUTOHSCROLL, 
				70, 10, 230, 200, hwnd, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);
			if(hEditP == NULL)
				MessageBox(hwnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);

			hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			//Chg font Reg
			long lfHeight = -MulDiv(14, GetDeviceCaps(hDC, LOGPIXELSY), 72);
			hfDefault = CreateFont(lfHeight, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, 0, 0, "Times New Roman");

			SendMessage(hEditP, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

		//Creation txt window heditCommand

			hEditC = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 
				70, 210, 230, 140, hwnd, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);
			if(hEditC == NULL)
				MessageBox(hwnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);

			hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			//Chg font Reg
			lfHeight = -MulDiv(12, GetDeviceCaps(hDC, LOGPIXELSY), 72);
			hfDefault = CreateFont(lfHeight, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, 0, 0, "Times New Roman");
			SendMessage(hEditC, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
			
		//Creation menuitem for window hwnd
			HMENU hMenu, hSubMenu;
			HICON hIcon, hIconSm;

			hMenu = CreateMenu();

			hSubMenu = CreatePopupMenu();
			AppendMenu(hSubMenu, MF_STRING, btnstart, "Execute Prog");
			AppendMenu(hSubMenu, MF_STRING, ID_FILE_Read, "Load");
			AppendMenu(hSubMenu, MF_STRING, ID_FILE_Save, "Save");
			//MenuSeparator(hSubMenu);
			AppendMenu(hSubMenu, MF_STRING, DebugSCMenuId, "Debug Show Codes");
			AppendMenu(hSubMenu, MF_STRING, DebugMenuId, "Debug");
			//AppendMenu(hSubMenu, MF_STRING, TGfxDspMenuId, "Toogle Gfx Display");
			AppendMenu(hSubMenu, MF_STRING, TBreakMenuId, "Toogle Break");
			AppendMenu(hSubMenu, MF_STRING, cmplxMenuId, "Toogle Allow Complexe");
			AppendMenu(hSubMenu, MF_STRING, PrecisionMenuId, "Toogle Precision");
			//MenuSeparator(hSubMenu);			
			AppendMenu(hSubMenu, MF_STRING, ID_FILE_EXIT, "Quit");
			AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "MODE");
			hSubMenu = CreatePopupMenu();
			AppendMenu(hSubMenu, MF_STRING, ZerofndMenuId, "Zero finding");
			AppendMenu(hSubMenu, MF_STRING, PolPltMenuId, "Polar Plot");
			AppendMenu(hSubMenu, MF_STRING, DerivateMenuId, "Numerical Derivation");
			AppendMenu(hSubMenu, MF_STRING, IntegMenuId, "Numerical Integration");
			AppendMenu(hSubMenu, MF_STRING, ThreeDpltMenuId, "f(x,y) 3D plot");
			AppendMenu(hSubMenu, MF_STRING, FFTMenuId, "Basic FFT Program");
			AppendMenu(hSubMenu, MF_STRING, SuitePlotMenuId, "Recurrence Suite Plot");

			AppendMenu(hSubMenu, MF_STRING, ID_STUFF_GO, "&Go");
			AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "MATHS");

			hSubMenu = CreatePopupMenu();
			AppendMenu(hSubMenu, MF_STRING, OscilloMenuId, "Acquire data");
			AppendMenu(hSubMenu, MF_STRING, OscilloProgMenuId, "Oscilloscope");
			AppendMenu(hSubMenu, MF_STRING, OscFFTMenuId, "OSC FFT");			
			AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "SERIAL");

			hSubMenu = CreatePopupMenu();
			AppendMenu(hSubMenu, MF_STRING, ManualMenuId, "Manual");
			AppendMenu(hSubMenu, MF_STRING, NewFMenuId, "New Functions");
			AppendMenu(hSubMenu, MF_STRING, PhilosophyMenuId, "Philosophy");
			AppendMenu(hSubMenu, MF_STRING, AboutMenuId, "About");
			
			AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "Info");

			SetMenu(hwnd, hMenu);

			hIcon = (HICON)LoadImage(NULL, "CalcRegIcon.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
/*			hIcon= (HICON)malloc( sizeof(IconBitmapReg));
			int i;
			for (i=0;i<sizeof(IconBitmapReg);i++)hIcon[i]=IconBitmapReg[i];*/
			if(hIcon){SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);}
			//else{MessageBox(hwnd, "Could not load large icon! Is it in the current working directory?", "Error", MB_OK | MB_ICONERROR);}

			hIconSm = (HICON)LoadImage(NULL, "CalcRegIcon.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
			if(hIconSm){SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSm);}
			//else{MessageBox(hwnd, "Could not load small icon! Is it in the current working directory?", "Error", MB_OK | MB_ICONERROR);}

			StartApplication(); //go and decode the text and displays it.
			break;

		case WM_SIZING:
			Paint(hwnd);
			break;

		case WM_ACTIVATE:
			Paint(hwnd);
			break;

		case WM_COMMAND:
			if (LOWORD(wParam) == ID_FILE_EXIT) CloseProc(hwnd); 
			if (LOWORD(wParam) == ID_FILE_Read) DoFileOpen(hwnd);
			if (LOWORD(wParam) == ID_FILE_Save) DoFileSave(hwnd);
			if (LOWORD(wParam) == btnstart) MainFormHandleEvent(btnstart);
			if (LOWORD(wParam) == DebugSCMenuId) doMainMenu(DebugSCMenuId);
			if (LOWORD(wParam) == DebugMenuId) doMainMenu(DebugMenuId); 
			if (LOWORD(wParam) == TGfxDspMenuId) doMainMenu(TGfxDspMenuId);
			if (LOWORD(wParam) == TBreakMenuId) 
				{doMainMenu(TBreakMenuId);
						if (BreakActivated==1) btn_brk = CreateWindow("BUTTON","Break",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+10,wndrect.top+45,50,25,hwnd,0,hinst,NULL);
						else DeleteObject(btn_brk);
		}
			if (LOWORD(wParam) == cmplxMenuId) doMainMenu(cmplxMenuId);
			if (LOWORD(wParam) == ZerofndMenuId) doMainMenu(ZerofndMenuId);
			if (LOWORD(wParam) == PolPltMenuId) doMainMenu(PolPltMenuId);
			if (LOWORD(wParam) == DerivateMenuId) doMainMenu(DerivateMenuId);
			if (LOWORD(wParam) == IntegMenuId) doMainMenu(IntegMenuId);
			if (LOWORD(wParam) == ThreeDpltMenuId) doMainMenu(ThreeDpltMenuId);
			if (LOWORD(wParam) == FFTMenuId) doMainMenu(FFTMenuId);
			if (LOWORD(wParam) == SuitePlotMenuId) doMainMenu(SuitePlotMenuId);
			if (LOWORD(wParam) == OscilloMenuId) doMainMenu(OscilloMenuId);
			if (LOWORD(wParam) == OscilloProgMenuId) doMainMenu(OscilloProgMenuId);
			if (LOWORD(wParam) == OscFFTMenuId) doMainMenu(OscFFTMenuId);
			if (LOWORD(wParam) == PrecisionMenuId) doMainMenu(PrecisionMenuId);
			if (LOWORD(wParam) == ManualMenuId) doMainMenu(ManualMenuId);
			if (LOWORD(wParam) == NewFMenuId) doMainMenu(NewFMenuId);
			if (LOWORD(wParam) == PhilosophyMenuId) doMainMenu(PhilosophyMenuId);
			if (LOWORD(wParam) == AboutMenuId) doMainMenu(AboutMenuId);
			if(btn_close==(HWND)lParam) CloseProc(hwnd);
			if(btn_brk==(HWND)lParam) StopProgram=1;
			if(btn_start==(HWND)lParam) MainFormHandleEvent(btnstart);
			if(btn_ClCmd==(HWND)lParam) MainFormHandleEvent(btnclear);
			if(btn_ClProg==(HWND)lParam) MainFormHandleEvent(btnC);
			if(btn_test==(HWND)lParam) MainFormHandleEvent(btntest);
			if(btn_gfxplus==(HWND)lParam) MainFormHandleEvent(btngfxplus);
			if(btn_gfxmoins==(HWND)lParam) MainFormHandleEvent(btngfxmoins);
			if(btn_gfxderivate==(HWND)lParam) MainFormHandleEvent(btngfxderivate);
			if(btn_gfxpset==(HWND)lParam) MainFormHandleEvent(btngfxpset);
			if(btn_gfxwork==(HWND)lParam) MainFormHandleEvent(btngfxwork);
			if(btn_gfxmove==(HWND)lParam) MainFormHandleEvent(btngfxmove);
			if(btn_1==(HWND)lParam) MainFormHandleEvent(btn1);
			if(btn_2==(HWND)lParam) MainFormHandleEvent(btn2);
			if(btn_3==(HWND)lParam) MainFormHandleEvent(btn3);
			if(btn_4==(HWND)lParam) MainFormHandleEvent(btn4);
			if(btn_5==(HWND)lParam) MainFormHandleEvent(btn5);
			if(btn_6==(HWND)lParam) MainFormHandleEvent(btn6);
			if(btn_7==(HWND)lParam) MainFormHandleEvent(btn7);
			if(btn_8==(HWND)lParam) MainFormHandleEvent(btn8);
			if(btn_9==(HWND)lParam) MainFormHandleEvent(btn9);
			
			break;

		case WM_LBUTTONDOWN:
			x = LOWORD(lParam);
			y = HIWORD(lParam);
			if(x>drawrect.left & x<drawrect.right & y>drawrect.top & y<drawrect.bottom){
			XpenDown=x; 
			YpenDown=y;
			if (Rabs(XpenDown-(DrawZoneX+DrawZoneW/2))<DrawZoneW/2 &&
				Rabs(YpenDown-(DrawZoneY+DrawZoneH/2))<DrawZoneH/2) {
				if (Pset ==1){
					PrintCmd("X=");
					newDimXmin= (XpenDown-DrawZoneX)/DrawZoneW*(DimXmax-DimXmin)+DimXmin;
					Rprintf(newDimXmin);
					PrintCmd("Y=");
					newDimYmin= (-YpenDown+DrawZoneY+DrawZoneH)/DrawZoneH*(DimYmax-DimYmin)+DimYmin;
					Rprintf(newDimYmin);
					Pset=0;//reinitialise
					}
				}			
			}
			break;

		case WM_MOUSEMOVE:
			if(LOWORD(lParam)>drawrect.left & LOWORD(lParam)<drawrect.right &
			HIWORD(lParam)>drawrect.top & HIWORD(lParam)<drawrect.bottom)

				switch(wParam)
				{
					case MK_LBUTTON:
						//MoveMouse(hwnd,LOWORD(lParam),HIWORD(lParam));
						if (XpenDown != 0 && YpenDown != 0) {	PenMoved=1;}
						break;
				}
			break;

		case WM_LBUTTONUP:
			x = LOWORD(lParam);
			y = HIWORD(lParam);
				if(LOWORD(lParam)>drawrect.left & LOWORD(lParam)<drawrect.right &
				HIWORD(lParam)>drawrect.top & HIWORD(lParam)<drawrect.bottom){
			XpenUp=x; 
			YpenUp=y;
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
			break;

		default:
			return DefWindowProc(hwnd,msg,wParam,lParam);
	}


	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrev,LPSTR cmdline,int CmdShow)
{
HWND		hwnd;
WNDCLASS	wc;
MSG			msg;

	hinst = hInstance;

	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);
	wc.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	wc.lpfnWndProc = (WNDPROC) WndProc;
	wc.lpszClassName = "CalcReg";
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wc);

	hwnd = CreateWindow("CalcReg","CalcReg V1.9 GUI Win32 API",WS_OVERLAPPEDWINDOW|DS_3DLOOK,CW_USEDEFAULT,0,CW_USEDEFAULT,0,0,0,hInstance,0);

	hmywin=hwnd; //for external purpose
	
	ShowWindow(hwnd,SW_SHOW);
	UpdateWindow(hwnd);

	hDC = GetDC(hwnd);
	colpen   = 0x000000;
	colbrush = 0xffffff;
	Paint(hwnd);

	while(GetMessage(&msg,0,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}



//----------------------------------- LoadTextFile----------------

BOOL LoadTextFileToEdit(HWND hEdit, LPCTSTR pszFileName)
{
	HANDLE hFile;
	BOOL bSuccess = FALSE;

	hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, 0, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwFileSize;

		dwFileSize = GetFileSize(hFile, NULL);
		if(dwFileSize != 0xFFFFFFFF)
		{
			LPSTR pszFileText;

			pszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);
			if(pszFileText != NULL)
			{
				DWORD dwRead;

				if(ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL))
				{
					pszFileText[dwFileSize] = 0; // Add null terminator
					if(SetWindowText(hEdit, pszFileText))
						bSuccess = TRUE; // It worked!
				}
				GlobalFree(pszFileText);
			}
		}
		CloseHandle(hFile);
	}
	return bSuccess;
}

void DoFileOpen(HWND hwnd)
{
	OPENFILENAME ofn;
	char szFileName[MAX_PATH] = "";

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "txt";

	if(GetOpenFileName(&ofn))
	{
		HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
		LoadTextFileToEdit(hEdit, szFileName);
	}
}

//------------------------------------ Save File ------------------
BOOL SaveTextFileFromEdit(HWND hEdit, LPCTSTR pszFileName)
{
	HANDLE hFile;
	BOOL bSuccess = FALSE;

	hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwTextLength;

		dwTextLength = GetWindowTextLength(hEdit);
		// No need to bother if there's no text.
		if(dwTextLength > 0)
		{
			LPSTR pszText;
			DWORD dwBufferSize = dwTextLength + 1;

			pszText = (LPSTR)GlobalAlloc(GPTR, dwBufferSize);
			if(pszText != NULL)
			{
				if(GetWindowText(hEdit, pszText, dwBufferSize))
				{
					DWORD dwWritten;

					if(WriteFile(hFile, pszText, dwTextLength, &dwWritten, NULL))
						bSuccess = TRUE;
				}
				GlobalFree(pszText);
			}
		}
		CloseHandle(hFile);
	}
	return bSuccess;
}
void DoFileSave(HWND hwnd)
{
	OPENFILENAME ofn;
	char szFileName[MAX_PATH] = "";

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "txt";
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

	if(GetSaveFileName(&ofn))
	{
		HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
		SaveTextFileFromEdit(hEdit, szFileName);
	}
}


//------------------------ sound load -----------------
char* DoLoadSound()
{
	OPENFILENAME ofn;
	char szFileName[MAX_PATH] = "";
	HWND hwnd=hmywin;
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "txt";

	if(GetOpenFileName(&ofn))
	{
		HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
		return szFileName;
	}
}
