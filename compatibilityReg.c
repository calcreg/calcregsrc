
// Here are the compatibility PalmOs to Win32api 

#include <windows.h>

extern HDC hDC;
extern HWND hEditP,hEditC;

	void WinDrawLine(int x1, int y1, int x2, int y2){
		MoveToEx(hDC,x1,y1,NULL);//chgPOs2Win32
		LineTo(hDC,x2,y2);
		return;
	}
	
	void Print_In_Prog_Win(char *s){
				SetWindowText(hEditP, s);
	}
	void Print_In_Cmd_Win(char *s){
				SetWindowText(hEditC, s);
	}
	void WinEraseRectangleReg(int DX,int DY,int DW,int DH){
		RECT rP;
				rP.left		= DX;
				rP.right	= DX+DW;
				rP.top		= DY;
				rP.bottom = DY+DH;
		FillRect(hDC,&rP,CreateSolidBrush(0xFFFFFF));
	}
	void SelectionStylo(COLORREF colpen){
				HPEN hpen	= CreatePen(PS_SOLID,1,colpen);
				SelectObject(hDC,hpen);
	}

