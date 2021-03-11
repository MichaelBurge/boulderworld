// yugioh.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "cards.h"
#define MAX_LOADSTRING 100
//DirectX
#include <d3d9.h>
#include <ddraw.h>
// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text
CDraw drawing;
CInput input;
int state = 0;
char **field = NULL;
char **backup = NULL;
int clickzones=-1;//Make -1, so when I increment the value, it'll be 0, the start of the array, and I can refer to the array based on this value
int width=64,height=64;
int tool=0;
int mapeditorbutton=0,exitgame=0;
int bombs=0;
char *tempbuffer,*tempbuffer2, next[256];
char gravity=0;
bool ghoststatus=false;
int lavatime=0,backuplavatime=0;
char rockskill=0;
char placementstyle=0;
HWND toolhandle=NULL;
int mapxpos=0,mapypos=0;
bool seetoolbar=true;
bool processed=false;
int slowdown=0;
bool spacebar=false;//Whether the spacebar is pressed or not
// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
int __stdcall Options(HWND hdlg,UINT message,WPARAM wParam,LPARAM lParam);
int __stdcall SwitchTiles(HWND hdlg,UINT message,WPARAM wParam,LPARAM lParam);
int __stdcall savedialog(HWND hdlg,UINT message,WPARAM wParam,LPARAM lParam);
int __stdcall loaddialog(HWND hdlg,UINT message,WPARAM wParam,LPARAM lParam);
int __stdcall toolbar(HWND hdlg,UINT message,WPARAM wParam,LPARAM lParam);
void InitData(int width,int height);
void Process(void);
void RenderField(void);
void SwitchtoEditor(void);
void Load(char *file2);
void Fill(int thing,int x,int y);
void move(int key);
#include "time.h"

//Entry point into program
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow) {
	bool running = true;
	InitData(64,64);
	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_YUGIOH, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	
	InitInstance (hInstance, nCmdShow);
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_YUGIOH);
	memset(next,0,255);
	// Main message loop:
	while (PeekMessage(&msg,NULL,0,0,PM_REMOVE)>=0) {
		if (msg.message == WM_QUIT) break;
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (GetKeyState(VK_LEFT)&0x80) move(VK_LEFT);
		if (GetKeyState(VK_RIGHT)&0x80) move(VK_RIGHT);
		if (GetKeyState(VK_UP)&0x80) move(VK_UP);
		if (GetKeyState(VK_DOWN)&0x80) move(VK_DOWN);
		if (GetKeyState(VK_SPACE)&0x80) {
			if (!spacebar) move(VK_SPACE);
			spacebar = true;
		} else spacebar=false;
		if (state==2) Process();
	}

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_YUGIOH);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_YUGIOH;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd) {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
	
   return TRUE;
}
bool CheckArray(vector<int> *xpos,vector<int> *ypos,int x,int y,int size) {
	while (size--) if ((*xpos)[size]==x && (*ypos)[size]==y) return false;
	return true;
}

void Process(void) {
	int diamondcount=0;
	vector<int> xpos;
	vector<int> ypos;
	int size=0;
	static vector<char> direction;
	int currentdirectioncheck=0;
	
	for (int temp=0;temp<width;temp++)
		for (int temp2=0;temp2<height;temp2++)
			if (CheckArray(&xpos,&ypos,temp,temp2,size))
			switch (field[temp][temp2]) {
			case 2:
				if (field[temp][temp2+1]==5&&rockskill) {
					field[temp][temp2+1]=1;
				}
				if (field[temp][temp2+1] == 1||field[temp][temp2+1]==18) {
					field[temp][temp2]=1;
					field[temp][temp2+1]=2;
					xpos.push_back(temp);
					ypos.push_back(temp2+1);
					size++;
					if (field[temp][temp2+2]==3&&rockskill) {
						RenderField();
						SwitchtoEditor();
						MessageBox(NULL,"The rock fell on you!","You lose!",NULL);
						return;
					}
					
				} else if (temp < width-1 &&field[temp+1][temp2+1] == 1 && field[temp+1][temp2]==1) {
					field[temp+1][temp2+1]=2;
					field[temp][temp2]=1;
					xpos.push_back(temp+1);
					ypos.push_back(temp2+1);
					size++;
				}
				else if (temp > 0&& field[temp-1][temp2+1]==1 && field[temp-1][temp2]==1) {
					field[temp-1][temp2+1]=2;
					field[temp][temp2]=1;
					xpos.push_back(temp-1);
					ypos.push_back(temp2+1);
					size++;
				}
				
				break;
			case 3:
				if (gravity && field[temp][temp2+1]==1) {
					field[temp][temp2+1]=3;field[temp][temp2]=1;
					xpos.push_back(temp);
					ypos.push_back(temp2+1);
					size++;
				}
				break;
			case 5:
				diamondcount++;
				break;
			case 6:
				{
					bool spread=false;
				if (temp > 0 && (field[temp-1][temp2]==0||field[temp-1][temp2]==9||field[temp-1][temp2]==10||field[temp-1][temp2]==18)) {
					field[temp-1][temp2]=6;
					xpos.push_back(temp-1);
					ypos.push_back(temp2);
					size++;
					spread=true;
				}
				if (temp < width-1 && (field[temp+1][temp2]==0||field[temp+1][temp2]==9||field[temp+1][temp2]==10||field[temp+1][temp2]==18)) {
					field[temp+1][temp2]=6;
					xpos.push_back(temp+1);
					ypos.push_back(temp2);
					size++;
					spread=true;
				}
				if (temp2 > 0 && (field[temp][temp2-1]==0||field[temp][temp2-1]==9||field[temp][temp2-1]==10||field[temp][temp2-1]==18)) {
					field[temp][temp2-1]=6;
					xpos.push_back(temp);
					ypos.push_back(temp2-1);
					size++;
					spread=true;
				}
				if (temp2 < height && (field[temp][temp2+1]==0||field[temp][temp2+1]==9||field[temp][temp2+1]==10||field[temp][temp2+1]==18)) {
					field[temp][temp2+1]=6;
					xpos.push_back(temp);
					ypos.push_back(temp2+1);
					size++;
					spread=true;
				}
				bool lost=false;
				if (temp > 0 && field[temp-1][temp2]==3) lost=true;
				if (temp < width-1 && field[temp+1][temp2]==3) lost=true;
				if (temp2 > 0 && field[temp][temp2-1]==3) lost=true;
				if (temp2 < height && field[temp][temp2+1]==3) lost=true;
				if (!spread) field[temp][temp2]=1;
				if (lost) {
					SwitchtoEditor();
					MessageBox(drawing.GetWindow(),"You were burnt in the fire!","You lost!",0);
					return;
				}
				}
			break;
			case 8:
				if (field[temp][temp2+1]==1) {field[temp][temp2+1]=8;field[temp][temp2]=1;}
				else { field[temp][temp2]=6;field[temp-1][temp2-1]=6;field[temp][temp2-1]=6;field[temp+1][temp2-1]=6;
				field[temp-1][temp2]=6;field[temp+1][temp2]=6;field[temp-1][temp2+1]=6;field[temp][temp2+1]=6;field[temp+1][temp2+1];}
				xpos.push_back(temp);
				ypos.push_back(temp2+1);
				size++;
				break;
			case 9:
				{
					bool spread=false;
				if (temp > 0 && field[temp-1][temp2]==0) {
					field[temp-1][temp2]=9;
					xpos.push_back(temp-1);
					ypos.push_back(temp2);
					size++;
					spread=true;
				}
				if (temp < width-1 && field[temp+1][temp2]==0) {
					field[temp+1][temp2]=9;
					xpos.push_back(temp+1);
					ypos.push_back(temp2);
					size++;
					spread=true;
				}
				if (temp2 > 0 && field[temp][temp2-1]==0) {
					field[temp][temp2-1]=9;
					xpos.push_back(temp);
					ypos.push_back(temp2-1);
					size++;
					spread=true;
				}
				if (temp2 < height && field[temp][temp2+1]==0) {
					field[temp][temp2+1]=9;
					xpos.push_back(temp);
					ypos.push_back(temp2+1);
					size++;
					spread=true;
				}
				bool lost=false;
				if (temp > 0 && field[temp-1][temp2]==3) lost=true;
				if (temp < width-1 && field[temp+1][temp2]==3) lost=true;
				if (temp2 > 0 && field[temp][temp2-1]==3) lost=true;
				if (temp2 < height && field[temp][temp2+1]==3) lost=true;
				if (lost) {
					SwitchtoEditor();
					MessageBox(drawing.GetWindow(),"The mutant fungus ate your body!","You lost!",0);
					return;
				}
				}
			case 10:
				{
					bool spread=false;
				if (temp > 0 && field[temp-1][temp2]==1) {
					field[temp-1][temp2]=10;
					xpos.push_back(temp-1);
					ypos.push_back(temp2);
					size++;
					spread=true;
				}
				if (temp < width-1 && field[temp+1][temp2]==1) {
					field[temp+1][temp2]=10;
					xpos.push_back(temp+1);
					ypos.push_back(temp2);
					size++;
					spread=true;
				}
				if (temp2 > 0 && field[temp][temp2-1]==1) {
					field[temp][temp2-1]=10;
					xpos.push_back(temp);
					ypos.push_back(temp2-1);
					size++;
					spread=true;
				}
				if (temp2 < height && field[temp][temp2+1]==1) {
					field[temp][temp2+1]=10;
					xpos.push_back(temp);
					ypos.push_back(temp2+1);
					size++;
					spread=true;
				}
				bool lost=false;
				if (temp > 0 && field[temp-1][temp2]==3) lost=true;
				if (temp < width-1 && field[temp+1][temp2]==3) lost=true;
				if (temp2 > 0 && field[temp][temp2-1]==3) lost=true;
				if (temp2 < height && field[temp][temp2+1]==3) lost=true;
				if (lost) {
					SwitchtoEditor();
					MessageBox(drawing.GetWindow(),"The mutant fungus ate your body!","You lost!",0);
					return;
				}
				}
				break;
			case 11:
				if (field[temp+1][temp2]==1) {
					field[temp+1][temp2]=field[temp-1][temp2];
					field[temp-1][temp2]=1;
					xpos.push_back(temp+1);
					ypos.push_back(temp2);
					size++;
				}
				break;
			case 12:
				if (field[temp-1][temp2]==1) {
					field[temp-1][temp2]=field[temp+1][temp2];
					field[temp+1][temp2]=1;
					xpos.push_back(temp-1);
					ypos.push_back(temp2);
					size++;
				}
				break;
			case 13:
				if (field[temp][temp2-1]==1) {
					field[temp][temp2-1]=field[temp][temp2+1];
					field[temp][temp2+1]=1;
					xpos.push_back(temp);
					ypos.push_back(temp2-1);
					size++;
				}
				break;
			case 14:
				if (field[temp][temp2+1]==1) {
					field[temp][temp2+1]=field[temp][temp2-1];
					field[temp][temp2-1]=1;
					xpos.push_back(temp);
					ypos.push_back(temp2+1);
					size++;
				}
				break;
			case 16:
				{
				bool spread=false;
				if (temp > 0 && field[temp-1][temp2]==1) {
					field[temp-1][temp2]=16;
					xpos.push_back(temp-1);
					ypos.push_back(temp2);
					size++;
					spread=true;
				}
				if (temp < width-1 &&field[temp+1][temp2]==1) {
					field[temp+1][temp2]=16;
					xpos.push_back(temp+1);
					ypos.push_back(temp2);
					size++;
					spread=true;
				}
				if (temp2 > 0 && field[temp][temp2-1]==1) {
					field[temp][temp2-1]=16;
					xpos.push_back(temp);
					ypos.push_back(temp2-1);
					size++;
					spread=true;
				}
				if (temp2 < height-1 && field[temp][temp2+1]==1) {
					field[temp][temp2+1]=16;
					xpos.push_back(temp);
					ypos.push_back(temp2+1);
					size++;
					spread=true;
				}
				if (!spread) field[temp][temp2]=0;
				break;
				}
			case 17:

				if (temp>0) {if (field[temp-1][temp2]==3) {
					MessageBox(NULL,"The grass generator enveloped you in grass!","You lost!",NULL);
					SwitchtoEditor();
					return;
				}field[temp-1][temp2]=16;}
				if (temp<width-1) {if (field[temp+1][temp2]==3) {
					MessageBox(NULL,"The grass generator enveloped you in grass!","You lost!",NULL);
					SwitchtoEditor();
					return;
				}field[temp+1][temp2]=16;}
				if (temp2>0) {if (field[temp][temp2-1]==3) {
					MessageBox(NULL,"The grass generator enveloped you in grass!","You lost!",NULL);
					SwitchtoEditor();
					return;
				}field[temp][temp2-1]=16;}
				if (temp2<height-1) {if (field[temp][temp2+1]==3) {
					MessageBox(NULL,"The grass generator enveloped you in grass!","You lost!",NULL);
					SwitchtoEditor();
					return;
				}field[temp][temp2+1]=16;}
				break;
			case 18:
				{
				if (ghoststatus) {
				bool found=false;
				int x,y;
				for (x=0;x<width && !found;x++)
					for (y=0;y<height && !found;y++)
						if (field[x][y]==3) found=true;
				if (field[temp-1][temp2]==3 || field[temp+1][temp2]==3 || field[temp][temp2-1]==3 || field[temp][temp2+1]==3) {
					SwitchtoEditor();
					MessageBox(NULL,"The ghost has eaten you!","You lose!",NULL);
				} else {
				if (temp > 0 &&temp>x-1 && (field[temp-1][temp2]==1||field[temp-1][temp2]==0)) {
					field[temp-1][temp2]=18;
					field[temp][temp2]=1;
					xpos.push_back(temp-1);
					ypos.push_back(temp2);
					size++;
				} else if (temp < width-1 && temp<x-1 && (field[temp+1][temp2]==1||field[temp+1][temp2]==0)) {
					field[temp+1][temp2]=18;
					field[temp][temp2]=1;
					xpos.push_back(temp+1);
					ypos.push_back(temp2);
					size++;
				}else if (temp2 > 0 && temp2>y && (field[temp][temp2-1]==1||field[temp][temp2-1]==0)) {
					field[temp][temp2-1]=18;
					field[temp][temp2]=1;
					xpos.push_back(temp);
					ypos.push_back(temp2-1);
					size++;
				}else if (temp2 < height-1&&temp2<y && (field[temp][temp2+1]==1||field[temp][temp2+1]==0)) {
					field[temp][temp2+1]=18;
					field[temp][temp2]=1;
					xpos.push_back(temp);
					ypos.push_back(temp2+1);
					size++;
				}
				}
				}
				ghoststatus=!ghoststatus;
				}
				break;
			case 19:
				diamondcount++;
				break;
			case 20:
				if (lavatime)
				if (field[temp-1][temp2]==3||field[temp+1][temp2]==3||field[temp][temp2-1]==3||field[temp][temp2+1]==3) {
					SwitchtoEditor();
					MessageBox(NULL,"The volcano spat lava upon you","You lose!",NULL);
					state=1;
				} else if (temp>0&&temp<width-1&&temp2>0&&temp2<height-1) {
					if (field[temp-1][temp2]==1||field[temp-1][temp2]==0) field[temp-1][temp2]=21;
					if (field[temp+1][temp2]==1||field[temp+1][temp2]==0) field[temp+1][temp2]=21;
					if (field[temp][temp2-1]==1||field[temp][temp2-1]==0) field[temp][temp2-1]=21;
					if (field[temp][temp2+1]==1||field[temp][temp2+1]==0) field[temp][temp2+1]=21;
				}
				break;
			case 21:
				{
					bool spread = false;
					if (!lavatime) field[temp][temp2]=2;
					if (temp > 0 &&temp<width-1 && temp2>0 && temp2<height-1&&lavatime)
				if (field[temp-1][temp2]==3||field[temp+1][temp2]==3||field[temp][temp2-1]==3||field[temp][temp2+1]==3) {
					SwitchtoEditor();
					MessageBox(NULL,"The lava ate you!","You lose!",NULL);
					state=1;
				} else {
					
					if (temp>0 && (field[temp-1][temp2]==1||field[temp-1][temp2]==0)) {
						field[temp-1][temp2]=21;
						xpos.push_back(temp-1);
						ypos.push_back(temp2);
						size++;
						spread=true;
					}
					if (temp<width-1 && (field[temp+1][temp2]==1||field[temp+1][temp2]==0)) {
						field[temp+1][temp2]=21;
						xpos.push_back(temp+1);
						ypos.push_back(temp2);
						size++;
						spread=true;
					}
					if (temp2>0 && (field[temp][temp2-1]==1||field[temp][temp2-1]==0)) {
						field[temp][temp2-1]=21;
						xpos.push_back(temp);
						ypos.push_back(temp2-1);
						size++;
						spread=true;
					}
					if (temp2<height-1 && (field[temp][temp2+1]==1||field[temp][temp2+1]==0)) {
						field[temp][temp2+1]=21;
						xpos.push_back(temp);
						ypos.push_back(temp2+1);
						size++;
						spread=true;
					}
					
				}
				
				}
				break;
			case 22:
				{
				char spread=0;
				char directions=0;
				if (processed) {
				if (direction[currentdirectioncheck]==0 && temp > 0 && field[temp-1][temp2]==1) {
					xpos.push_back(temp-1); 
					ypos.push_back(temp2);
					size++;
					field[temp-1][temp2]=22;
					spread=true;
				}
				else if (direction[currentdirectioncheck]==1 && temp < width-1 && field[temp+1][temp2]==1) {
					xpos.push_back(temp+1);
					ypos.push_back(temp2);
					size++;
					field[temp+1][temp2]=22;
					spread=true;
				}
				else if (direction[currentdirectioncheck]==2 && temp2 > 0 && field[temp][temp2-1]==1) {
					xpos.push_back(temp);
					ypos.push_back(temp2-1);
					size++;
					field[temp][temp2-1]=22;
					spread=true;
				}
				else if (direction[currentdirectioncheck]==3 && temp2 < height-1 && field[temp][temp2+1]==1) {
					xpos.push_back(temp);
					ypos.push_back(temp2+1);
					size++;
					field[temp][temp2+1]=22;
					spread=true;
				} 
				
				
				if (spread) field[temp][temp2]=1;
				else if (temp > 0 && field[temp-1][temp2]==1)direction[currentdirectioncheck]=0;
				else if (temp2>0 && field[temp][temp2-1]==1) direction[currentdirectioncheck]=2;
				else if (temp<width-1 && field[temp+1][temp2]==1) direction[currentdirectioncheck]=1;
				else if (temp2<height-1&&field[temp][temp2+1]==1) direction[currentdirectioncheck]=3;
				currentdirectioncheck++;
				} else {
				if (temp > 0 && field[temp-1][temp2]==1)directions=0;
				else if (temp2>0 && field[temp][temp2-1]==2) directions=2;
				else if (temp<width-1 && field[temp+1][temp2]==1) directions=1;
				else if (temp2<height-1&&field[temp][temp2+1]==2) directions=3;
				if (!processed) direction.push_back(directions);
				}
				}
				break;
			}
		if (lavatime) lavatime--;
		if (diamondcount==0) {
			MessageBox(NULL,"You win!","",NULL);
			if (strcmp(next,"(null)")==0||next[0]==NULL) SwitchtoEditor();
			else {Load(next);state=2;}
		}
	RenderField();
	processed=true;
	if (slowdown) Sleep(slowdown);
	
}
void InitData(int width,int height) {
	if (field!=NULL) {
		for (int temp=0;temp<::width;temp++)
			delete [] field[temp];
		delete [] field;
	}
	field = new char*[width];
	for (int temp=0;temp<width;temp++)
		field[temp] = new char[height];
	int temp2;
	for (temp=0;temp<width;temp++)
		for (temp2=0;temp2<height;temp2++)
			field[temp][temp2]=0;
	if (backup!=NULL) {
		for (int temp=0;temp<::width;temp++)
			delete [] backup[temp];
		delete [] backup;
	}
	backup = new char*[width];
	for (temp=0;temp<width;temp++)
		backup[temp] = new char[height];
	::width=width;
	::height=height;
}
void exit(int x,int y) {
	if (state==0) PostQuitMessage(0);
}
void RenderMenu(void) {
	RECT temp;
	GetClientRect(drawing.GetWindow(),&temp);
	drawing.RepeatPicture(0,0,0,temp.right,temp.bottom);
	drawing.DrawPicture(mapeditorbutton,temp.right/2-256,temp.bottom/2-128,temp.right/2,temp.bottom/2);
	drawing.DrawPicture(exitgame,temp.right/2,temp.bottom/2,temp.right/2+256,temp.bottom/2+128);
}
void RenderField(void) {
	int temp2;
	RECT rt;
	GetClientRect(drawing.GetWindow(),&rt);
	for (int temp=mapxpos;temp<rt.right/16&&temp<width;temp++)
		for (temp2=mapypos;temp2<rt.bottom/16 && temp2<height;temp2++)
			drawing.DrawPicture(field[temp][temp2],temp*16,temp2*16,16,16);
	if (seetoolbar) ShowWindow(toolhandle,1);
}

void mapeditor(int x,int y) {
	if (state==0) {
		state=1;
		RenderField();
	}
}

void editor(int x,int y) {
	bool render=false;
	if (state==1) {
		if (placementstyle==0) {
			if (tool==3) {
				for(int temp=0;temp<width;temp++)
					for (int temp2=0;temp2<height;temp2++)
						if (field[temp][temp2]==3) field[temp][temp2]=1;
				render=true;
			}
			field[x/16][y/16] = tool;
			if (render) RenderField(); 
			else drawing.DrawPicture(tool,x/16*16,y/16*16,16,16);
		} else if (placementstyle==1) {
			Fill(field[x/16][y/16],x/16,y/16);
			RenderField();
		}
	}
}
bool ValidSpot(int x,int y) {
	bool okay = true;
	if (x >= 0 && x < width-1 && y >= 0 && y < height-1)
	switch (field[x][y]) {
	case 4: okay=false;break;
	} else okay=false;
	return okay;

}
void move(int key) {
	if (state==1) {
		switch (key) {
		case VK_DOWN:
			mapypos++;
			break;
		case VK_UP:
			mapypos--;
			break;
		case VK_LEFT:
			mapxpos--;
			break;
		case VK_RIGHT:
			mapxpos++;
			break;
		}
		RenderField();
	}
	if (state==2) {
		int x,y;
		for (int temp=0;temp<width;temp++)
			for (int temp2=0;temp2<height;temp2++)
				if (field[temp][temp2] == 3) {x=temp;y=temp2;}
				field[x][y] = 1;
				switch (key) {
				case VK_DOWN:
					if (ValidSpot(x,y+1)) {
						if (field[x][y+1]==2||field[x][y+1]==15) y--;
						if (field[x][y+1]==7) bombs++;
						y++;
					}
					break;
				case VK_UP:
					if (ValidSpot(x,y-1)) {
						if (field[x][y-1]==2||field[x][y-1]==15) y++;
						if (field[x][y-1]==7) bombs++;
						if (gravity) if (field[x][y+1]!=1&&field[x][y-1]==1&&ValidSpot(x,y-1)) y-=2; else;else y--;
					}
					break;
				case VK_LEFT:
					if (ValidSpot(x-1,y)) {
						if (field[x-1][y] == 2 && field[x-2][y] == 1) field[x-2][y] = 2; else if (field[x-1][y] == 2||field[x-1][y]==15) x++;
						if (field[x-1][y]==7) bombs++;
						x--;
					}
					
					break;
				case VK_RIGHT:
					if (ValidSpot(x+1,y)) {
						if (field[x+1][y] == 2 && field[x+2][y] == 1) field[x+2][y] = 2; else if (field[x+1][y] == 2||field[x+1][y]==15) x--;
						if (field[x+1][y]==7) bombs++;
						x++;
					}
					break;
				case VK_SPACE:
					if (bombs) {field[x][y+1]=8;bombs--;}
					break;
				}
				
				field[x][y] = 3;
	} 
}

void Save(char *file2) {
	FILE *file = fopen(file2,"w");
	fprintf(file,"%d\n%d\n",width,height);
	for (int temp=0;temp<width;temp++)
		for (int temp2=0;temp2<height;temp2++)
			fputc(field[temp][temp2],file);
	fprintf(file,"\n%s\n",tempbuffer2);
	fputc(gravity,file);
	fprintf(file,"\n%d",lavatime);
	fclose(file);
}
void SwitchtoEditor(void) {
	state=1;
	if (seetoolbar) ShowWindow(toolhandle,1);
	for (int temp=0;temp<width;temp++)
		for (int temp2=0;temp2<height;temp2++)
			field[temp][temp2]=backup[temp][temp2];
	bombs=0;
	lavatime=backuplavatime;
	RenderField();
}
void Load(char *file2) {
	FILE *file = fopen(file2,"r");
	if (file) {
	fscanf(file,"%d%d",&width,&height);
	InitData(width,height);
	fgetc(file);
	for (int temp=0;temp<width;temp++)
		for (int temp2=0;temp2<height;temp2++)
				field[temp][temp2]=fgetc(file);
	fgetc(file);
	if (!fgets(next,255,file)) memset(next,0,255);
	gravity = fgetc(file);
	if (gravity==EOF) gravity=0;
	fscanf(file,"%d",&lavatime);
	backuplavatime=lavatime;
	fclose(file);
	} else MessageBox(NULL,"Couldn't open file","Error",NULL);
	RenderField();
}
void Fill(int thing,int x,int y) {
	if (thing==tool) return;
	field[x][y]=tool;
	if (x > 0 && field[x-1][y]==thing) Fill(thing,x-1,y);
	if (x < width-1 && field[x+1][y]==thing) Fill(thing,x+1,y);
	if (y > 0 && field[x][y-1]==thing) Fill(thing,x,y-1);
	if (y < height-1 &&field[x][y+1]==thing) Fill(thing,x,y+1);
}
void TotalRedraw(void) {
	InvalidateRect(drawing.GetWindow(),NULL,false);
	RenderField();
}
//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
	input.UpdateInput(hWnd,message,wParam,lParam);
	switch (message) {
	case WM_CREATE:
		RECT rt;
		GetClientRect(hWnd, &rt);
		drawing.SetRenew(true);
		drawing.SetWindow(hWnd);
		drawing.SetBrush(BS_SOLID,RGB(200,200,15),NULL);
		drawing.SetPen(PS_COSMETIC | PS_SOLID,1);
		drawing.LoadPicture("grass.bmp",true);
		drawing.LoadPicture("blank.bmp",true);
		drawing.LoadPicture("boulder.bmp",true);
		drawing.LoadPicture("player.bmp",true);
		drawing.LoadPicture("spikewall.bmp",true);
		drawing.LoadPicture("diamond.bmp",true);
		drawing.LoadPicture("fire.bmp",true);
		drawing.LoadPicture("bomb.bmp",true);
		drawing.LoadPicture("bomb.bmp",true);//Same picture, but represents different objects(1 is collectable, 1 is falling)
		drawing.LoadPicture("fungus.bmp",true);
		drawing.LoadPicture("darkfungus.bmp",true);
		drawing.LoadPicture("transportright.bmp",true);
		drawing.LoadPicture("transportleft.bmp",true);
		drawing.LoadPicture("transportup.bmp",true);
		drawing.LoadPicture("transportdown.bmp",true);
		drawing.LoadPicture("brickwall.bmp",true);
		drawing.LoadPicture("grass.bmp",true);
		drawing.LoadPicture("grassgenerator.bmp",true);
		drawing.LoadPicture("ghost.bmp",true);
		drawing.LoadPicture("gold.bmp",true);
		drawing.LoadPicture("volcano.bmp",true);
		drawing.LoadPicture("lava.bmp",true);
		drawing.LoadPicture("robot.bmp",true);
		mapeditorbutton=drawing.LoadPicture("mapeditor.bmp",true);
		exitgame=drawing.LoadPicture("exit.bmp",true);
		input.AddClickZone(rt.right/2-256,rt.bottom/2-128,rt.right/2,rt.bottom/2,mapeditor);
		input.AddClickZone(rt.right/2,rt.bottom/2,rt.right/2+256,rt.bottom/2+128,exit);
		toolhandle=CreateDialog(GetModuleHandle(0),MAKEINTRESOURCE(IDD_TOOLBAR),hWnd,toolbar);
		if (toolhandle==NULL) MessageBox(NULL,"Could not create toolbar","Error",0);
		//CreateWindowEx(NULL,"BUTTON",NULL,WS_POPUP |BS_PUSHBUTTON|WS_VISIBLE,499,499,250,25,hWnd,NULL,GetModuleHandle(NULL),NULL);
		break;
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId) {
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
					DestroyWindow(hWnd);
				   break;
				case IDM_NEW:
					InitData(width,height);
					gravity=0;
					RenderField();
					break;
				case IDM_EDITOR_GRASS:
					tool=0;
					break;
				case IDM_EDITOR_BLANK:
					tool=1;
					break;
				case IDM_EDITOR_BOULDER:
					tool=2;
					break;
				case IDM_EDITOR_FLOOD:
					{
					for (int temp=0;temp<width;temp++)
						for (int temp2=0;temp2<height;temp2++)
							field[temp][temp2]=tool;
						RenderField();
					}
					break;
				case IDM_MAP:
					SwitchtoEditor();
					RenderField();
					break;
				case IDM_SAVE:
					if (state==1) {
						DialogBox(GetModuleHandle(0),MAKEINTRESOURCE(IDD_SAVE),drawing.GetWindow(),savedialog);
						if (tempbuffer) Save(tempbuffer); 
						if (tempbuffer) delete [] tempbuffer;
						if (tempbuffer2) delete [] tempbuffer2;
					}else MessageBox(NULL,"Not in map editor","Click map editor button",NULL);
					break;
				case IDM_LOAD:
					{
					
					DialogBox(GetModuleHandle(0),MAKEINTRESOURCE(IDD_LOAD),drawing.GetWindow(),loaddialog);
					if (tempbuffer) {Load(tempbuffer);state=1;}
					if (tempbuffer) delete [] tempbuffer;
					}
					break;
				case IDM_EDITOR_PLAYER:
					tool=3;
					break;
				case IDM_RUN:
					if (state==1) {
						state = 2;
						for (int temp=0;temp<width;temp++)
							for (int temp2=0;temp2<height;temp2++)
								backup[temp][temp2] = field[temp][temp2];
						}else MessageBox(drawing.GetWindow(),"Must be in map editor to run map","Start map editor first",NULL);
					break;
				case IDM_EDITOR_SPIKEWALL:
					tool=4;
					break;
				case IDM_OPTIONS:
					DialogBox(GetModuleHandle(0),MAKEINTRESOURCE(IDD_OPTIONS),drawing.GetWindow(),Options);
					RenderField();
					break;
				case IDM_EDITOR_SWITCHTILES:
					DialogBox(GetModuleHandle(0),MAKEINTRESOURCE(IDD_SWITCHTILES),drawing.GetWindow(),SwitchTiles);
					break;
				case IDM_EDITOR_DIAMOND:
					tool=5;
					break;
				case IDM_EDITOR_FIRE:
					tool=6;
					break;
				case IDM_REDRAW:
					TotalRedraw();
					break;
				case IDM_EDITOR_BOMB:
					tool=7;
					break;
				case IDM_EDITOR_BOMBONFIRE:
					tool=8;
					break;
				case IDM_EDITOR_FUNGUS:
					tool=9;
					break;
				case IDM_EDITOR_DARKFUNGUS:
					tool=10;
					break;
				case IDM_EDITOR_TRANSPORTRIGHT:
					tool=11;
					break;
				case IDM_EDITOR_TRANSPORTLEFT:
					tool=12;
					break;
				case IDM_EDITOR_TRANSPORTUP:
					tool=13;
					break;
				case IDM_EDITOR_TRANSPORTDOWN:
					tool=14;
					break;
				case IDM_EDITOR_BRICKWALL:
					tool=15;
					break;
				case IDM_EDITOR_GRASSGENERATOR:
					tool=17;
					break;
				case IDM_EDITOR_GHOST:
					tool=18;
					break;
				case IDM_EDITOR_GOLD:
					tool=19;
					break;
				case IDM_EDITOR_VOLCANO:
					tool=20;
					break;
				case IDM_EDITOR_LAVA:
					tool=21;
					break;
				case IDM_EDITOR_CUT:
					GetClientRect(drawing.GetWindow(),&rt);
					InitData(rt.right/16,rt.bottom/16);
					break;
				case IDM_EDITOR_BORDER:
					{
					for (int temp=0;temp<width;temp++)
						field[temp][0]=tool;
					for (temp=0;temp<width;temp++)
						field[temp][height-1]=tool;
					for (temp=0;temp<height;temp++)
						field[0][temp]=tool;
					for (temp=0;temp<height;temp++)
						field[width-1][temp]=tool;
					RenderField();
					}
					break;
				case IDM_EDITOR_FILL:
					placementstyle=1;
					break;
				case IDM_EDITOR_SINGLE:
					placementstyle=0;
				case IDM_VIEW_TOOLBARON:
					ShowWindow(toolhandle,1);
					seetoolbar=true;
					break;
				case IDM_VIEW_TOOLBAROFF:
					ShowWindow(toolhandle,0);
					seetoolbar=false;
					break;
				case IDM_EDITOR_ROBOT:
					tool=22;
					break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			{
			//int blah = drawing.LoadPicture("font.bmp",true);
			if (hdc!=NULL) EndPaint(hWnd,&ps);
			hdc = BeginPaint(hWnd,&ps);
			// TODO: Add any drawing code here...
			drawing.SetTarget(&hdc);
			GetClientRect(hWnd, &rt);
			/*for (int temp=0;temp<rt.bottom;temp++) {
				if ((temp & 1) == 0) drawing.DrawLine(0,temp,rt.right,temp,RGB(128,64,32),hWnd);
				else drawing.DrawLine(0,temp,rt.right,temp,RGB(0,0,255),hWnd);
			}*/
			switch (state) {
			case 0: RenderMenu();break;
			case 1: RenderField();break;
			}
			}
			break;
		case WM_MOUSEMOVE:
			{
			RECT rt;
			GetClientRect(hWnd, &rt);
			int X = LOWORD(lParam);
			int Y = HIWORD(lParam);
			if (wParam & MK_LBUTTON && X > 0 && X < width*16 && Y > 0 && Y < height*16) {
				drawing.DrawPicture(tool,X/16*16,Y/16*16,16,16);
				editor(X,Y);
			}
			}
			break;
		case WM_LBUTTONDOWN:
			{
			RECT rt;
			GetClientRect(hWnd, &rt);
			int X = LOWORD(lParam);
			int Y = HIWORD(lParam);
			if (wParam & MK_LBUTTON && X > 0 && X < width*16 && Y > 0 && Y < height*16) {
				drawing.DrawPicture(tool,X/16*16,Y/16*16,16,16);
				editor(X,Y);
			}
			}
			break;
		case WM_DESTROY:
			
			drawing.EndDraw();
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }

   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}


int __stdcall Options(HWND hdlg,UINT message,WPARAM wParam,LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			if (GetWindowTextLength(GetDlgItem(hdlg,IDC_WIDTH))&&GetWindowTextLength(GetDlgItem(hdlg,IDC_HEIGHT)))
			InitData(GetDlgItemInt(hdlg,IDC_WIDTH,NULL,NULL),GetDlgItemInt(hdlg,IDC_HEIGHT,NULL,NULL));
			if (GetWindowTextLength(GetDlgItem(hdlg,IDC_GRAVITY))) gravity = GetDlgItemInt(hdlg,IDC_GRAVITY,NULL,NULL);
			if (GetWindowTextLength(GetDlgItem(hdlg,IDC_LAVA))) {lavatime=GetDlgItemInt(hdlg,IDC_LAVA,NULL,NULL);backuplavatime=lavatime;}
			if (GetWindowTextLength(GetDlgItem(hdlg,IDC_ROCK))) rockskill=GetDlgItemInt(hdlg,IDC_ROCK,NULL,NULL);
			if (GetWindowTextLength(GetDlgItem(hdlg,IDC_SLOWDOWN))) slowdown=GetDlgItemInt(hdlg,IDC_SLOWDOWN,NULL,NULL);
		case IDCANCEL:
			EndDialog(hdlg,LOWORD(wParam));
			
			break;
		}
		return TRUE;
	}
	return FALSE;
}

int __stdcall SwitchTiles(HWND hdlg,UINT message,WPARAM wParam,LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG:
		SendMessage(GetDlgItem(hdlg,IDC_FROM),LB_ADDSTRING,0,(LPARAM) (LPCTSTR)"Grass");
		SendMessage(GetDlgItem(hdlg,IDC_FROM),LB_ADDSTRING,0,(LPARAM) (LPCTSTR)"Blank Spaces");
		SendMessage(GetDlgItem(hdlg,IDC_FROM),LB_ADDSTRING,0,(LPARAM) (LPCTSTR)"Boulders");
		SendMessage(GetDlgItem(hdlg,IDC_FROM),LB_ADDSTRING,0,(LPARAM) (LPCTSTR)"Player");
		SendMessage(GetDlgItem(hdlg,IDC_FROM),LB_ADDSTRING,0,(LPARAM) (LPCTSTR)"Spike Walls");
		SendMessage(GetDlgItem(hdlg,IDC_FROM),LB_ADDSTRING,0,(LPARAM) (LPCTSTR)"Diamonds");
		SendMessage(GetDlgItem(hdlg,IDC_FROM),LB_ADDSTRING,0,(LPARAM) (LPCTSTR)"Fire");
		SendMessage(GetDlgItem(hdlg,IDC_TO),LB_ADDSTRING,0,(LPARAM) (LPCTSTR)"Grass");
		SendMessage(GetDlgItem(hdlg,IDC_TO),LB_ADDSTRING,0,(LPARAM) (LPCTSTR)"Blank Spaces");
		SendMessage(GetDlgItem(hdlg,IDC_TO),LB_ADDSTRING,0,(LPARAM) (LPCTSTR)"Boulders");
		SendMessage(GetDlgItem(hdlg,IDC_TO),LB_ADDSTRING,0,(LPARAM) (LPCTSTR)"Player");
		SendMessage(GetDlgItem(hdlg,IDC_TO),LB_ADDSTRING,0,(LPARAM) (LPCTSTR)"Spike Walls");
		SendMessage(GetDlgItem(hdlg,IDC_TO),LB_ADDSTRING,0,(LPARAM) (LPCTSTR)"Diamonds");
		SendMessage(GetDlgItem(hdlg,IDC_TO),LB_ADDSTRING,0,(LPARAM) (LPCTSTR)"Fire");
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			{
				int from = SendMessage(GetDlgItem(hdlg,IDC_FROM),LB_GETCURSEL,0,0);
				int to   = SendMessage(GetDlgItem(hdlg,IDC_TO  ),LB_GETCURSEL,0,0);
				int temp2=0;
				char buffer[256];
				char buffer2[256];
				itoa(from,buffer,10);
				itoa(to,buffer2,10);
				MessageBox(NULL,buffer,buffer2,NULL);
			for (int temp=0;temp<width-1;temp++)
				for (temp2=0;temp2<height-1;temp2++)
					if (field[temp][temp2]==from) 
						field[temp][temp2]=to;
					
			}
		case IDCANCEL:
			EndDialog(hdlg,LOWORD(wParam));
			RenderField();
			break;
		}
		return TRUE;
	}
	return FALSE;
}

int __stdcall savedialog(HWND hdlg,UINT message,WPARAM wParam,LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			{
			tempbuffer=0;tempbuffer2=0;
			bool valid=false;
			int length = GetWindowTextLength(GetDlgItem(hdlg,IDC_FILE));
			if (length) {
				tempbuffer = new char[length+1];
				GetDlgItemText(hdlg,IDC_FILE,tempbuffer,length+1);
				valid=true;
			} else MessageBox(NULL,"Put a file name in","File name",NULL);
			length = GetWindowTextLength(GetDlgItem(hdlg,IDC_NEXT));
			if (length) {
				tempbuffer2 = new char[length+1];
				GetDlgItemText(hdlg,IDC_NEXT,tempbuffer2,length+1);
			}
			
			if (!valid)break;
			}
		case IDCANCEL:
			EndDialog(hdlg,LOWORD(wParam));
			return TRUE;
			break;
		}
	}
	return FALSE;
}

int __stdcall loaddialog(HWND hdlg,UINT message,WPARAM wParam,LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			{
			tempbuffer=0;
			bool valid=false;
			int length = GetWindowTextLength(GetDlgItem(hdlg,IDC_FILE));
			if (length) {
				tempbuffer = new char[length+1];
				GetDlgItemText(hdlg,IDC_FILE,tempbuffer,length+1);
				valid=true;
			} else MessageBox(NULL,"Put a file name in","File name",NULL);
			if (!valid) break;
			else EndDialog(hdlg,LOWORD(wParam));
			return TRUE;
			}
		case IDCANCEL:
			tempbuffer=NULL;
			EndDialog(hdlg,LOWORD(wParam));
			return TRUE;
			break;
		}
	}
	return FALSE;
}

int __stdcall toolbar(HWND hdlg,UINT message,WPARAM wParam,LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_FILL:
			placementstyle=1;
			break;
		case IDC_SINGLE:
			placementstyle=0;
			break;
		}
		return TRUE;
		break;
		case WM_CLOSE:
			DestroyWindow(hdlg);
			return TRUE;
			break;
	}
	return FALSE;
}