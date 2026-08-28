/* --------------------------
 * 
 * Here are the compatibility PalmOs to Win32api 
 *
 *   BUSSY-SOCRATE REGAN
 *
 *------------------------*/

include <windows.h>

extern HDC hDC;
	
	void WinDrawLine(int x1, int y1, int x2, int y2){
		MoveToEx(hDC,x1,y1,NULL);//chgPOs2Win32
		LineTo(hDC,x2,y2);
		return;
	}

	
