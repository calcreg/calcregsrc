-/*---------------------
- *
- * Window Display
- *
- * BUSSY-SOCRATE REGAN
- *
- * --------------------*/

#include "windows.h"
#include "CalcReg.h"
#include <stdio.h>


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

HINSTANCE	hinst=NULL;
HWND		btn_close,btn_line,btn_test,btn_ell,btn_cpen,btn_cbrush;
HWND		btn_start,btn_read;
HWND		hmywin;
HDC			hDC;
RECT		drawrect,wndrect;
RECT		prect,brect;
HBRUSH		hbrush;
HPEN		hpen;
COLORREF	colpen,colbrush;
	HFONT hfDefault;
	HWND hEditP,hEditC;//txt win

int			x,y;

void CloseProc(HWND hwnd)
{
	DeleteObject(hbrush);
	DeleteObject(hpen);
	ReleaseDC(hwnd,hDC);
	UnregisterClass("Calcreg",hinst);
	PostQuitMessage(0);	
}

void Paint(HWND hwnd)
{
	RECT wndrect;

	GetClientRect(hwnd,&wndrect);

	drawrect.bottom = wndrect.bottom - 10;
	drawrect.left	= wndrect.left + 310;
	drawrect.right	= wndrect.right - 10;
	drawrect.top	= wndrect.top + 10;

	FillRect(hDC,&drawrect,CreateSolidBrush(0xFFFFFF));
	FrameRect(hDC,&drawrect,CreateSolidBrush(0x0A0AFF));
	/*
	prect.left = wndrect.left + 85;
	prect.top  = wndrect.top + 110;
	prect.right = prect.left + 25;
	prect.bottom = prect.top + 25;

	brect.left = wndrect.left + 85;
	brect.top  = wndrect.top + 140;
	brect.right = brect.left + 25;
	brect.bottom = brect.top + 25;

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
	btn_line = CreateWindow("BUTTON","Line",WS_CHILD|WS_VISIBLE|BS_PUSHLIKE|BS_AUTORADIOBUTTON,wndrect.left+10,wndrect.top+10,100,25,hwnd,0,hinst,NULL);			
	btn_test = CreateWindow("BUTTON","Test",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+10,wndrect.top+40,100,25,hwnd,0,hinst,NULL);			
	btn_ell = CreateWindow("BUTTON","Ellipses",WS_CHILD|WS_VISIBLE|BS_PUSHLIKE|BS_AUTORADIOBUTTON,wndrect.left+10,wndrect.top+70,100,25,hwnd,0,hinst,NULL);			
	btn_cpen = CreateWindow("BUTTON","Pen",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+10,wndrect.top+110,70,25,hwnd,0,hinst,NULL);
	btn_cbrush = CreateWindow("BUTTON","Brush",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+10,wndrect.top+140,70,25,hwnd,0,hinst,NULL);

	btn_close = CreateWindow("BUTTON","Close",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+10,wndrect.top+180,100,25,hwnd,0,hinst,NULL);
	btn_start = CreateWindow("BUTTON","EXE",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+10,wndrect.top+220,100,25,hwnd,0,hinst,NULL);			
	//btn_read = CreateWindow("BUTTON","Read",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+10,wndrect.top+220,100,25,hwnd,0,hinst,NULL);			
	}

void MoveMouse(HWND hwnd,int x,int y)
{
	if(CheckBState(hwnd)==1)
	{
		//LineTo(hDC,x,y);
		MoveToEx(hDC,x,y,NULL);
	}
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
	switch(msg)
	{
		case WM_DESTROY:
			CloseProc(hwnd);
			break;

		case WM_CREATE:
			ObjectCreation(hwnd);
			
		//Creation txt window heditProg

			hEditP = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 
				120, 10, 180, 200, hwnd, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);
			if(hEditP == NULL)
				MessageBox(hwnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);

			hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			//Chg font Reg
			long lfHeight = -MulDiv(14, GetDeviceCaps(hDC, LOGPIXELSY), 72);
			hfDefault = CreateFont(lfHeight, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, 0, 0, "Times New Roman");

			SendMessage(hEditP, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
			//if(SetWindowText(hEdit, pszFileText))bSuccess = TRUE; // It worked!
			SetWindowText(hEditP, "prog ici\n");

		//Creation txt window heditCommand

			hEditC = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 
				120, 210, 180, 140, hwnd, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);
			if(hEditC == NULL)
				MessageBox(hwnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);

			hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			SendMessage(hEditC, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
			//if(SetWindowText(hEdit, pszFileText))bSuccess = TRUE; // It worked!
			SetWindowText(hEditC, "cmd:\nhello\n");
			
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
			AppendMenu(hSubMenu, MF_STRING, TGfxDspMenuId, "Toogle Gfx Display");
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

			SetMenu(hwnd, hMenu);

			hIcon = (HICON)LoadImage(NULL, "CalcRegIcon.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
			if(hIcon)
			{
				SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			}
			else
			{
				MessageBox(hwnd, "Could not load large icon! Is it in the current working directory?", "Error", MB_OK | MB_ICONERROR);
			}

			hIconSm = (HICON)LoadImage(NULL, "CalcRegIcon.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
			if(hIconSm)
			{
				SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSm);
			}
			else
			{
				MessageBox(hwnd, "Could not load small icon! Is it in the current working directory?", "Error", MB_OK | MB_ICONERROR);
			}
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
			if (LOWORD(wParam) == TBreakMenuId) doMainMenu(TBreakMenuId);
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
			if(btn_close==(HWND)lParam) CloseProc(hwnd);
			if(btn_cpen==(HWND)lParam) cColor(hwnd,1);
			if(btn_cbrush==(HWND)lParam) cColor(hwnd,2);
			if(btn_start==(HWND)lParam) MainFormHandleEvent(btnstart);
			if(btn_test==(HWND)lParam) MainFormHandleEvent(btntest);
			break;

		case WM_LBUTTONDOWN:
			x = LOWORD(lParam);
			y = HIWORD(lParam);
			if(x>drawrect.left & x<drawrect.right & y>drawrect.top & y<drawrect.bottom)
				MoveToEx(hDC,x,y,NULL);
			break;

		case WM_MOUSEMOVE:
			if(LOWORD(lParam)>drawrect.left & LOWORD(lParam)<drawrect.right &
			HIWORD(lParam)>drawrect.top & HIWORD(lParam)<drawrect.bottom)

				switch(wParam)
				{
					case MK_LBUTTON:					
						MoveMouse(hwnd,LOWORD(lParam),HIWORD(lParam));
						break;
				}
			break;

		case WM_LBUTTONUP:
			if(CheckBState(hwnd))
				if(LOWORD(lParam)>drawrect.left & LOWORD(lParam)<drawrect.right &
				HIWORD(lParam)>drawrect.top & HIWORD(lParam)<drawrect.bottom)
					DrawPrimitive(hwnd,x,y,LOWORD(lParam),HIWORD(lParam));
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
	wc.lpszClassName = "Calcreg";
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wc);

	hwnd = CreateWindow("Calcreg","CalcReg V1.9 GUI Win32 API",WS_OVERLAPPEDWINDOW|DS_3DLOOK,CW_USEDEFAULT,0,CW_USEDEFAULT,0,0,0,hInstance,0);

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
