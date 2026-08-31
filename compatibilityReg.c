
// Here are the compatibility PalmOs to Win32api 
#include<stdio.h>
#include <windows.h>

#define MaxTextInCmdWin 500

extern HDC hDC;
extern HWND hEditP,hEditC;

	void Print_In_Prog_Win(char *s); 
	void Print_In_Cmd_Win(char *s); 
	void Delete_Cmd_Win(void);
	void Delete_Prog_Win(void);

	void WinDrawLine(int x1, int y1, int x2, int y2){
		MoveToEx(hDC,x1,y1,NULL);//chgPOs2Win32
		LineTo(hDC,x2,y2);
		return;
	}
	
	void Print_In_Prog_Win(char *s){ 
	//for some reason the program is unable to make 
	//sprintf ("\0d0a...") so we have to transform \n into \0d0a by hand
		int i,k=0,size,nbrEnter;
			size=strlen(s);
			for (i=0;i<size;i++) {if (s[i]==0x0a)nbrEnter++;}
			char * p = malloc ( (size+nbrEnter+1)*sizeof(char));
			p[0]=s[0];
			for (i =1;i<(size+nbrEnter);i++){
				if (s[i]==0x0a && s[i-1]!=0x0d) {p[i+k]=0x0d;p[i+k+1]=0x0a;k++;}
				else p[i+k]=s[i];
				}
				p[size+nbrEnter]=0;
			SetWindowText(hEditP, p);
			free (p);
	}
	
	void Print_In_Cmd_Win(char *s){
//il faut voir si size =1 ou zero si ca fonctionne
	//for some reason the program is unable to make 
	//sprintf ("\0d0a...") so we have to transform \n into \0d0a by hand
		int i,k=0,size,nbrEnter=0;
			size=strlen(s);
			for (i=0;i<size;i++) {if (s[i]==0x0a)nbrEnter++;}
			char * p = malloc ( (size+nbrEnter+1)*sizeof(char));
			if (s[0]==0x0a) {p[0]=0x0d;p[1]=0x0a;k++;}
			else p[0]=s[0];
			for (i =1;i<(size+nbrEnter);i++){
				if (s[i]==0x0a && s[i-1]!=0x0d) {p[i+k]=0x0d;p[i+k+1]=0x0a;k++;}
				else p[i+k]=s[i];
				}
			p[size+nbrEnter]=0;
			
	DWORD dwTextLength = GetWindowTextLength(hEditC);
	//if (dwTextLength > MaxTextInCmdWin){free(p);return;}
	DWORD dwBufferSize = dwTextLength + 1+ size+nbrEnter;
	LPSTR	 pszText = (LPSTR)GlobalAlloc(GPTR, dwBufferSize);
			if(pszText != NULL)
			{
				GetWindowText(hEditC, pszText, dwTextLength+1);
				for (i=0;i<(size+nbrEnter);i++) pszText[i+dwTextLength]=p[i];
				pszText[dwTextLength+size+nbrEnter]=0;
				SetWindowText(hEditC, pszText);
				GlobalFree(pszText);
			}
			else	SetWindowText(hEditC, p);
			free (p);
			//SetWindowText(hEditC, s);
	}
	
void Delete_Prog_Win(void){
//SetWindowText(hEditP, 0);
}	
void Delete_Cmd_Win(void){
//SetWindowText(hEditC, "---");
SetWindowText(hEditC, 0);
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

