/* This is the main Window handler
 *  Win API
 *  
 * BUSSY-SOCRATE REGAN
 *
 * ------------------------*/

#include "windows.h"

	extern void DrawLine(int x, int y);

HINSTANCE	hinst=NULL;
HWND		btn_close,btn_line,btn_rect,btn_ell,btn_cpen,btn_cbrush;
HDC			hDC;
RECT		drawrect,wndrect;
RECT		prect,brect;
HBRUSH		hbrush;
HPEN		hpen;
COLORREF	colpen,colbrush;
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
	drawrect.left	= wndrect.left + 120;
	drawrect.right	= wndrect.right - 10;
	drawrect.top	= wndrect.top + 10;

	FrameRect(hDC,&drawrect,CreateSolidBrush(0xff0000));

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
	if(SendMessage(btn_rect,BM_GETCHECK,0,0)==1) return 2;
	if(SendMessage(btn_ell,BM_GETCHECK,0,0)==1) return 3; 
	return 0;

}

void ObjectCreation(HWND hwnd)
{
	GetClientRect(hwnd,&wndrect);	
	btn_line = CreateWindow("BUTTON","Line",WS_CHILD|WS_VISIBLE|BS_PUSHLIKE|BS_AUTORADIOBUTTON,wndrect.left+10,wndrect.top+10,100,25,hwnd,0,hinst,NULL);			
	btn_rect = CreateWindow("BUTTON","Rectangles",WS_CHILD|WS_VISIBLE|BS_PUSHLIKE|BS_AUTORADIOBUTTON,wndrect.left+10,wndrect.top+40,100,25,hwnd,0,hinst,NULL);			
	btn_ell = CreateWindow("BUTTON","Ellipses",WS_CHILD|WS_VISIBLE|BS_PUSHLIKE|BS_AUTORADIOBUTTON,wndrect.left+10,wndrect.top+70,100,25,hwnd,0,hinst,NULL);			
	btn_cpen = CreateWindow("BUTTON","Pen",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+10,wndrect.top+110,70,25,hwnd,0,hinst,NULL);
	btn_cbrush = CreateWindow("BUTTON","Brush",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+10,wndrect.top+140,70,25,hwnd,0,hinst,NULL);

	btn_close = CreateWindow("BUTTON","Close",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,wndrect.left+10,wndrect.top+180,100,25,hwnd,0,hinst,NULL);
}

void MoveMouse(HWND hwnd,int x,int y)
{
	if(CheckBState(hwnd)==1)
	{
		//LineTo(hDC,x,y);
		DrawLine(x,y);
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
			break;

		case WM_SIZING:
			Paint(hwnd);
			break;

		case WM_ACTIVATE:
			Paint(hwnd);
			break;

		case WM_COMMAND:
			if(btn_close==(HWND)lParam) CloseProc(hwnd);
			if(btn_cpen==(HWND)lParam) cColor(hwnd,1);
			if(btn_cbrush==(HWND)lParam) cColor(hwnd,2);
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


