
/* Compatibility math Layer
 *
 * BUSSY-SOCRATE REGAN
 *
 * ------------------------*/


#include "windows.h"

	void DrawLine(int x, int y);
	extern HDC hDC;
	
	void DrawLine(int x, int y){
	
	LineTo(hDC,x,y);
	LineTo(hDC,x-5,y);
	LineTo(hDC,x-5,y-5);
	LineTo(hDC,x,y-5);
	return ;
	}
