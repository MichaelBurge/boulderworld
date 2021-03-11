// yugioh.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "resource.h"
#include "cards.h"
#define MAX_LOADSTRING 100
#define ArraySize(x) (sizeof(x)/sizeof(x[0]))
// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text
CDraw drawing;
CInput input;
int state = 0;

int clickzones=-1;//Make -1, so when I increment the value, it'll be 0, the start of the array, and I can refer to the array based on this value
int width=64,height=64;

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
enum Tile {
		Grass,
		Blank,
		Boulder,
		Player,
		SpikeWall,
		Diamond,
		Fire,
		CollectableBomb,
		Bomb,
		Fungus,
		DarkFungus,
		RightTransport,
		LeftTransport,
		UpTransport,
		DownTransport,
		BrickWall,
		GrowingGrass,
		GrassGenerator,
		Ghost,
		Gold,
		Volcano,
		Lava,
		Robot,
		Water,
		WaterBoulder
};
Tile **field = NULL;
Tile **backup = NULL;
Tile **tilebelow = NULL;

Tile tool=Grass;

Tile objects[] = {
	Boulder,
	Player,
	SpikeWall,
	Diamond,
	Fire,
	CollectableBomb,
	Bomb,
	RightTransport,
	LeftTransport,
	UpTransport,
	DownTransport,
	BrickWall,
	GrassGenerator,
	Ghost,
	Gold,
	Robot
};

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int __stdcall Start(HWND hdlg,UINT message,WPARAM wParam,LPARAM lParam);
int __stdcall Options(HWND hdlg,UINT message,WPARAM wParam,LPARAM lParam);
int __stdcall SwitchTiles(HWND hdlg,UINT message,WPARAM wParam,LPARAM lParam);
int __stdcall savedialog(HWND hdlg,UINT message,WPARAM wParam,LPARAM lParam);
int __stdcall toolbar(HWND hdlg,UINT message,WPARAM wParam,LPARAM lParam);

void InitData(int width,int height);
void Process(void);
void RenderField(void);
void SwitchtoEditor(void);
void Load(char *file2);
void Fill(int x,int y);
void move(int key);
void DisplayLoadDialog(void);
void SwitchtoEditor(void);

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
		if (state==2) {
			Process();
			RenderField();
		}
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

void MoveObject(int x,int y,int x2,int y2) {
	//Check if tile to be moved is an object. If it is, continue. Otherwise, exit.
	bool check = false;
	Tile first = field[x][y];
	Tile second= field[x2][y2];
	for (int temp=0;temp<ArraySize(objects);temp++) {
		if (first==objects[temp]) {
			check = true;
			break;
		}
	}
	if (!check)
		goto exit;

	//Check if there are no objects in tile 2. If there aren't any, continue. Otherwise, exit.
	check = false;
	for (temp=0;temp<ArraySize(objects);temp++) {
		if (second==objects[temp]) {
			check = true;
			break;
		}
	}
	if (check)
		goto exit;
	
	//Set the object in Tile 1 to the terrain that is in memory.
	field[x][y] = tilebelow[x][y];
	//Remember the tile in Tile 2
	tilebelow[x2][y2] = second;
	//Set Tile 2 to Tile 1
	field[x2][y2] = first;

exit:;

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
			case Boulder:
				if (field[temp][temp2+1]==5&&rockskill) {
					field[temp][temp2+1]=Grass;
				}
				if (field[temp][temp2+1] == Blank||field[temp][temp2+1]==18) {
					field[temp][temp2]=Blank;
					field[temp][temp2+1]=Boulder;
					xpos.push_back(temp);
					ypos.push_back(temp2+1);
					size++;
					if (field[temp][temp2+2]==3&&rockskill) {
						RenderField();
						SwitchtoEditor();
						MessageBox(NULL,"The rock fell on you!","You lose!",NULL);
						return;
					}
					
				} else if (temp < width-1 &&field[temp+1][temp2+1] == Blank && field[temp+1][temp2]==Blank) {
					field[temp+1][temp2+1]=Boulder;
					field[temp][temp2]=Blank;
					xpos.push_back(temp+1);
					ypos.push_back(temp2+1);
					size++;
				}
				else if (temp > 0&& field[temp-1][temp2+1]==Blank && field[temp-1][temp2]==Blank) {
					field[temp-1][temp2+1]=Boulder;
					field[temp][temp2]=Blank;
					xpos.push_back(temp-1);
					ypos.push_back(temp2+1);
					size++;
				}
				
				break;
			case Player:
				if (gravity && field[temp][temp2+1]==Blank) {
					field[temp][temp2+1]=Player;
					field[temp][temp2]=Blank;
					xpos.push_back(temp);
					ypos.push_back(temp2+1);
					size++;
				}
				break;
			case Diamond:
				diamondcount++;
				break;
			case Fire:
				{
					bool spread=false;
				if (temp > 0 && (field[temp-1][temp2]==Grass||field[temp-1][temp2]==9||field[temp-1][temp2]==10||field[temp-1][temp2]==18)) {
					field[temp-1][temp2]=Fire;
					xpos.push_back(temp-1);
					ypos.push_back(temp2);
					size++;
					spread=true;
				}
				if (temp < width-1 && (field[temp+1][temp2]==Grass||field[temp+1][temp2]==9||field[temp+1][temp2]==10||field[temp+1][temp2]==18)) {
					field[temp+1][temp2]=Fire;
					xpos.push_back(temp+1);
					ypos.push_back(temp2);
					size++;
					spread=true;
				}
				if (temp2 > 0 && (field[temp][temp2-1]==Grass||field[temp][temp2-1]==9||field[temp][temp2-1]==10||field[temp][temp2-1]==18)) {
					field[temp][temp2-1]=Fire;
					xpos.push_back(temp);
					ypos.push_back(temp2-1);
					size++;
					spread=true;
				}
				if (temp2 < height && (field[temp][temp2+1]==Grass||field[temp][temp2+1]==9||field[temp][temp2+1]==10||field[temp][temp2+1]==18)) {
					field[temp][temp2+1]=Fire;
					xpos.push_back(temp);
					ypos.push_back(temp2+1);
					size++;
					spread=true;
				}
				bool lost=false;
				if (temp > 0 && field[temp-1][temp2]==Player) lost=true;
				if (temp < width-1 && field[temp+1][temp2]==Player) lost=true;
				if (temp2 > 0 && field[temp][temp2-1]==Player) lost=true;
				if (temp2 < height && field[temp][temp2+1]==Player) lost=true;
				if (!spread) field[temp][temp2]=Blank;
				if (lost) {
					SwitchtoEditor();
					MessageBox(drawing.GetWindow(),"You were burnt in the fire!","You lost!",0);
					return;
				}
				}
			break;
			case Bomb:
				if (field[temp][temp2+1]==Blank) {field[temp][temp2+1]=Bomb;field[temp][temp2]=Blank;}
				else { 
					field[temp][temp2]=Fire;
					field[temp-1][temp2-1]=Fire;
					field[temp][temp2-1]=Fire;
					field[temp+1][temp2-1]=Fire;
					field[temp-1][temp2]=Fire;
					field[temp+1][temp2]=Fire;
					field[temp-1][temp2+1]=Fire;
					field[temp][temp2+1]=Fire;
					field[temp+1][temp2+1]=Fire;
				}
				xpos.push_back(temp);
				ypos.push_back(temp2+1);
				size++;
				break;
			case Fungus:
				{
					bool spread=false;
				if (temp > 0 && (field[temp-1][temp2]==Grass||field[temp-1][temp2]==GrowingGrass)) {
					field[temp-1][temp2]=Fungus;
					xpos.push_back(temp-1);
					ypos.push_back(temp2);
					size++;
					spread=true;
				}
				if (temp < width-1 && (field[temp+1][temp2]==Grass||field[temp+1][temp2]==GrowingGrass)) {
					field[temp+1][temp2]=Fungus;
					xpos.push_back(temp+1);
					ypos.push_back(temp2);
					size++;
					spread=true;
				}
				if (temp2 > 0 && (field[temp][temp2-1]==Grass||field[temp][temp2-1]==GrowingGrass)) {
					field[temp][temp2-1]=Fungus;
					xpos.push_back(temp);
					ypos.push_back(temp2-1);
					size++;
					spread=true;
				}
				if (temp2 < height && (field[temp][temp2+1]==Grass||field[temp][temp2+1]==GrowingGrass)) {
					field[temp][temp2+1]=Fungus;
					xpos.push_back(temp);
					ypos.push_back(temp2+1);
					size++;
					spread=true;
				}
				bool lost=false;
				if (temp > 0 && field[temp-1][temp2]==Player) lost=true;
				if (temp < width-1 && field[temp+1][temp2]==Player) lost=true;
				if (temp2 > 0 && field[temp][temp2-1]==Player) lost=true;
				if (temp2 < height && field[temp][temp2+1]==Player) lost=true;
				if (lost) {
					SwitchtoEditor();
					MessageBox(drawing.GetWindow(),"The mutant fungus ate your body!","You lost!",0);
					return;
				}
				}
				break;
			case DarkFungus:
				{
					bool spread=false;
				if (temp > 0 && field[temp-1][temp2]==Blank) {
					field[temp-1][temp2]=DarkFungus;
					xpos.push_back(temp-1);
					ypos.push_back(temp2);
					size++;
					spread=true;
				}
				if (temp < width-1 && field[temp+1][temp2]==Blank) {
					field[temp+1][temp2]=DarkFungus;
					xpos.push_back(temp+1);
					ypos.push_back(temp2);
					size++;
					spread=true;
				}
				if (temp2 > 0 && field[temp][temp2-1]==Blank) {
					field[temp][temp2-1]=DarkFungus;
					xpos.push_back(temp);
					ypos.push_back(temp2-1);
					size++;
					spread=true;
				}
				if (temp2 < height && field[temp][temp2+1]==Blank) {
					field[temp][temp2+1]=DarkFungus;
					xpos.push_back(temp);
					ypos.push_back(temp2+1);
					size++;
					spread=true;
				}
				bool lost=false;
				if (temp > 0 && field[temp-1][temp2]==Player) lost=true;
				if (temp < width-1 && field[temp+1][temp2]==Player) lost=true;
				if (temp2 > 0 && field[temp][temp2-1]==Player) lost=true;
				if (temp2 < height && field[temp][temp2+1]==Player) lost=true;
				if (lost) {
					SwitchtoEditor();
					MessageBox(drawing.GetWindow(),"The mutant fungus ate your body!","You lost!",0);
					return;
				}
				}
				break;
			case RightTransport:
				if (field[temp+1][temp2]==Blank) {
					field[temp+1][temp2]=field[temp-1][temp2];
					field[temp-1][temp2]=Blank;
					xpos.push_back(temp+1);
					ypos.push_back(temp2);
					size++;
				}
				break;
			case LeftTransport:
				if (field[temp-1][temp2]==Blank) {
					field[temp-1][temp2]=field[temp+1][temp2];
					field[temp+1][temp2]=Blank;
					xpos.push_back(temp-1);
					ypos.push_back(temp2);
					size++;
				}
				break;
			case UpTransport:
				if (field[temp][temp2-1]==Blank) {
					field[temp][temp2-1]=field[temp][temp2+1];
					field[temp][temp2+1]=Blank;
					xpos.push_back(temp);
					ypos.push_back(temp2-1);
					size++;
				}
				break;
			case DownTransport:
				if (field[temp][temp2+1]==Blank) {
					field[temp][temp2+1]=field[temp][temp2-1];
					field[temp][temp2-1]=Blank;
					xpos.push_back(temp);
					ypos.push_back(temp2+1);
					size++;
				}
				break;
			case GrowingGrass:
				{
				bool spread=false;
				if (temp > 0 && field[temp-1][temp2]==Blank) {
					field[temp-1][temp2]=GrowingGrass;
					xpos.push_back(temp-1);
					ypos.push_back(temp2);
					size++;
					spread=true;
				}
				if (temp < width-1 &&field[temp+1][temp2]==Blank) {
					field[temp+1][temp2]=GrowingGrass;
					xpos.push_back(temp+1);
					ypos.push_back(temp2);
					size++;
					spread=true;
				}
				if (temp2 > 0 && field[temp][temp2-1]==Blank) {
					field[temp][temp2-1]=GrowingGrass;
					xpos.push_back(temp);
					ypos.push_back(temp2-1);
					size++;
					spread=true;
				}
				if (temp2 < height-1 && field[temp][temp2+1]==Blank) {
					field[temp][temp2+1]=GrowingGrass;
					xpos.push_back(temp);
					ypos.push_back(temp2+1);
					size++;
					spread=true;
				}
				if (!spread) field[temp][temp2]=Blank;
				break;
				}
			case GrassGenerator:

				if (temp>0) {if (field[temp-1][temp2]==Player) {
					MessageBox(NULL,"The Blank generator enveloped you in Blank!","You lost!",NULL);
					SwitchtoEditor();
					return;
				}field[temp-1][temp2]=GrowingGrass;}
				if (temp<width-1) {if (field[temp+1][temp2]==Player) {
					MessageBox(NULL,"The Blank generator enveloped you in Blank!","You lost!",NULL);
					SwitchtoEditor();
					return;
				}field[temp+1][temp2]=GrowingGrass;}
				if (temp2>0) {if (field[temp][temp2-1]==Player) {
					MessageBox(NULL,"The Blank generator enveloped you in Blank!","You lost!",NULL);
					SwitchtoEditor();
					return;
				}field[temp][temp2-1]=GrowingGrass;}
				if (temp2<height-1) {if (field[temp][temp2+1]==Player) {
					MessageBox(NULL,"The Blank generator enveloped you in Blank!","You lost!",NULL);
					SwitchtoEditor();
					return;
				}field[temp][temp2+1]=GrowingGrass;}
				break;
			case Ghost:
				{
				if (ghoststatus) {
				bool found=false;
				int x,y;
				for (x=0;x<width && !found;x++)
					for (y=0;y<height && !found;y++)
						if (field[x][y]==Player) found=true;
				if ((temp&&field[temp-1][temp2]==Player) || (temp<width-1&&field[temp+1][temp2]==Player) || 
					(temp2&&field[temp][temp2-1]==Player) || (temp2<height-1&&field[temp][temp2+1]==Player)) {
					SwitchtoEditor();
					MessageBox(NULL,"The ghost has eaten you!","You lose!",NULL);
				} else {
				if (temp > 0 &&temp>x-1 && (field[temp-1][temp2]==1||field[temp-1][temp2]==Blank)) {
					field[temp-1][temp2]=Ghost;
					field[temp][temp2]=Blank;
					xpos.push_back(temp-1);
					ypos.push_back(temp2);
					size++;
				} else if (temp < width-1 && temp<x-1 && (field[temp+1][temp2]==1||field[temp+1][temp2]==Blank)) {
					field[temp+1][temp2]=Ghost;
					field[temp][temp2]=Blank;
					xpos.push_back(temp+1);
					ypos.push_back(temp2);
					size++;
				}else if (temp2 > 0 && temp2>y && (field[temp][temp2-1]==1||field[temp][temp2-1]==Blank)) {
					field[temp][temp2-1]=Ghost;
					field[temp][temp2]=Blank;
					xpos.push_back(temp);
					ypos.push_back(temp2-1);
					size++;
				}else if (temp2 < height-1&&temp2<y && (field[temp][temp2+1]==1||field[temp][temp2+1]==Blank)) {
					field[temp][temp2+1]=Ghost;
					field[temp][temp2]=Blank;
					xpos.push_back(temp);
					ypos.push_back(temp2+1);
					size++;
				}
				}
				}
				ghoststatus=!ghoststatus;
				}
				break;
			case Gold:
				diamondcount++;
				break;
			case Volcano:
				if (lavatime)
				if (field[temp-1][temp2]==Player||
					field[temp+1][temp2]==Player||
					field[temp][temp2-1]==Player||
					field[temp][temp2+1]==Player) {
					SwitchtoEditor();
					MessageBox(NULL,"The volcano spat lava upon you","You lose!",NULL);
					state=1;
				} else if (temp>0&&temp<width-1&&temp2>0&&temp2<height-1) {
					if (field[temp-1][temp2]==Blank||field[temp-1][temp2]==Blank) field[temp-1][temp2]=Lava;
					if (field[temp+1][temp2]==Blank||field[temp+1][temp2]==Blank) field[temp+1][temp2]=Lava;
					if (field[temp][temp2-1]==Blank||field[temp][temp2-1]==Blank) field[temp][temp2-1]=Lava;
					if (field[temp][temp2+1]==Blank||field[temp][temp2+1]==Blank) field[temp][temp2+1]=Lava;
				}
				break;
			case Lava:
				{
					bool spread = false;
					if (!lavatime) field[temp][temp2]=Boulder;
					if (temp > 0 &&temp<width-1 && temp2>0 && temp2<height-1&&lavatime)
				if (field[temp-1][temp2]==Player||
					field[temp+1][temp2]==Player||
					field[temp][temp2-1]==Player||
					field[temp][temp2+1]==Player) {
					SwitchtoEditor();
					MessageBox(NULL,"The lava ate you!","You lose!",NULL);
					state=1;
				} else {
					
					if (temp>0 && (field[temp-1][temp2]==Blank||field[temp-1][temp2]==Blank)) {
						field[temp-1][temp2]=Lava;
						xpos.push_back(temp-1);
						ypos.push_back(temp2);
						size++;
						spread=true;
					}
					if (temp<width-1 && (field[temp+1][temp2]==Blank||field[temp+1][temp2]==Blank)) {
						field[temp+1][temp2]=Lava;
						xpos.push_back(temp+1);
						ypos.push_back(temp2);
						size++;
						spread=true;
					}
					if (temp2>0 && (field[temp][temp2-1]==Blank||field[temp][temp2-1]==Blank)) {
						field[temp][temp2-1]=Lava;
						xpos.push_back(temp);
						ypos.push_back(temp2-1);
						size++;
						spread=true;
					}
					if (temp2<height-1 && (field[temp][temp2+1]==Blank||field[temp][temp2+1]==Blank)) {
						field[temp][temp2+1]=Lava;
						xpos.push_back(temp);
						ypos.push_back(temp2+1);
						size++;
						spread=true;
					}
					
				}
				
				}
				break;
			case Robot:
				{
				char spread=0;
				char directions=0;
				if (processed) {
				if (direction[currentdirectioncheck]==0 && temp > 0 && field[temp-1][temp2]==Blank) {
					xpos.push_back(temp-1); 
					ypos.push_back(temp2);
					size++;
					field[temp-1][temp2]=Robot;
					spread=true;
				}
				else if (direction[currentdirectioncheck]==1 && temp < width-1 && field[temp+1][temp2]==Blank) {
					xpos.push_back(temp+1);
					ypos.push_back(temp2);
					size++;
					field[temp+1][temp2]=Robot;
					spread=true;
				}
				else if (direction[currentdirectioncheck]==2 && temp2 > 0 && field[temp][temp2-1]==Blank) {
					xpos.push_back(temp);
					ypos.push_back(temp2-1);
					size++;
					field[temp][temp2-1]=Robot;
					spread=true;
				}
				else if (direction[currentdirectioncheck]==3 && temp2 < height-1 && field[temp][temp2+1]==Blank) {
					xpos.push_back(temp);
					ypos.push_back(temp2+1);
					size++;
					field[temp][temp2+1]=Robot;
					spread=true;
				} 
				
				
				if (spread) field[temp][temp2]=Blank;
				else if (temp > 0 && field[temp-1][temp2]==Blank)
					direction[currentdirectioncheck]=0;
				else if (temp2>0 && field[temp][temp2-1]==Blank) 
					direction[currentdirectioncheck]=2;
				else if (temp<width-1 && field[temp+1][temp2]==Blank) 
					direction[currentdirectioncheck]=1;
				else if (temp2<height-1&&field[temp][temp2+1]==Blank) 
					direction[currentdirectioncheck]=3;
				currentdirectioncheck++;
				} else {
				if (temp > 0 && field[temp-1][temp2]==Blank)directions=0;
				else if (temp2>0 && field[temp][temp2-1]==Boulder) directions=2;
				else if (temp<width-1 && field[temp+1][temp2]==Blank) directions=1;
				else if (temp2<height-1&&field[temp][temp2+1]==Boulder) directions=3;
				if (!processed) 
					direction.push_back(directions);
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
template<class T> Init2DArray(T ***pointer,int oldwidth,int newwidth,int newheight,T Init) {
	T **&grid = *pointer;
	if (grid!=NULL) {
		for (int temp=0;temp<oldwidth;temp++)
			delete [] grid[temp];
		delete [] grid;
	}
	grid = new T*[newwidth];
	for (int temp=0;temp<newwidth;temp++)
		grid[temp] = new T[newheight];
	int temp2;
	for (temp=0;temp<newwidth;temp++)
		for (temp2=0;temp2<newheight;temp2++)
			grid[temp][temp2]=Init;
}
void InitData(int width,int height) {
	Init2DArray(&field,::width,width,height,Grass);
	Init2DArray(&tilebelow,::width,width,height,Blank);
	Init2DArray(&backup,::width,width,height,Grass);
	
	::width=width;
	::height=height;
	/*if (field!=NULL) {
		for (int temp=0;temp<::width;temp++)
			delete [] field[temp];
		delete [] field;
	}
	field = new Tile*[width];
	for (int temp=0;temp<width;temp++)
		field[temp] = new Tile[height];
	int temp2;
	for (temp=0;temp<width;temp++)
		for (temp2=0;temp2<height;temp2++)
			field[temp][temp2]=Grass;
	if (backup!=NULL) {
		for (int temp=0;temp<::width;temp++)
			delete [] backup[temp];
		delete [] backup;
	}
	backup = new Tile*[width];
	for (temp=0;temp<width;temp++)
		backup[temp] = new Tile[height];
	::width=width;
	::height=height;*/
}
void exit(int x,int y) {
	if (state==0) PostQuitMessage(0);
}
void RenderField(void) {
	int temp2;
	RECT rt;
	GetClientRect(drawing.GetWindow(),&rt);
	drawing.SetRenew(false);
	drawing.StartDraw();
	for (int temp=mapxpos;temp-1<rt.right/16&&temp<width;temp++)
		for (temp2=mapypos;temp2-1<rt.bottom/16 && temp2<height;temp2++)
			drawing.DrawPicture(field[temp][temp2],temp*16,temp2*16,16,16);
	drawing.EndDraw();
	drawing.SetRenew(true);
	if (seetoolbar) ShowWindow(toolhandle,1);
	else ShowWindow(toolhandle,0);
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
			if (tool==Player) {
				for(int temp=0;temp<width;temp++)
					for (int temp2=0;temp2<height;temp2++)
						if (field[temp][temp2]==Player) field[temp][temp2]=Blank;
				render=true;
			}
			field[x/16][y/16] = tool;
			if (render) RenderField(); 
			else drawing.DrawPicture(tool,x/16*16,y/16*16,16,16);
		} else if (placementstyle==Blank) {
			Fill(x/16,y/16);
			RenderField();
		}
	}
}
bool ValidSpot(int x,int y) {
	bool okay = true;
	if (x >= 0 && x < width-1 && y >= 0 && y < height-1)
	switch (field[x][y]) {
	case 4: okay=false;break;
	case Boulder: okay=false;break;
	case Water:okay=false;break;
	} else okay=false;
	return okay;

}
void move(int key) {
	if (state==1) {
		switch (key) {
		case VK_DOWN:
			if (mapypos<height-1)
				mapypos++;
			break;
		case VK_UP:
			if (mapypos)
				mapypos--;
			break;
		case VK_LEFT:
			if (mapxpos)
				mapxpos--;
			break;
		case VK_RIGHT:
			if (mapxpos<width-1)
				mapxpos++;
			break;
		}
		RenderField();
	}
	if (state==2) {
		int x=0,y=0;
		bool found=false;
		for (int temp=0;temp<width&&!found;temp++)
			for (int temp2=0;temp2<height&&!found;temp2++)
				if (field[temp][temp2] == Player) {
					x=temp;
					y=temp2;
					found = true;
					switch (key) {
					case VK_DOWN:
						field[x][y]=Blank;
						if (ValidSpot(x,y+1)) {
							if (field[x][y+1]==2||field[x][y+1]==15) y--;
							if (field[x][y+1]==7) bombs++;
							
							y++;
						}
						break;
					case VK_UP:
						field[x][y]=Blank;
						if (ValidSpot(x,y-1)) {
							if (field[x][y-1]==2||field[x][y-1]==15) y++;
							if (field[x][y-1]==7) bombs++;
							if (gravity) 
								if (field[x][y+1]!=1&&field[x][y-1]==1&&ValidSpot(x,y-1)) y-=2; 
								else;
								else y--;
						}
						break;
					case VK_LEFT:
						if (ValidSpot(x-1,y)) {
							if (field[x-1][y] == Boulder||field[x-1][y]==15) x++;
							if (field[x-1][y]==7) bombs++;
							field[x][y]=tilebelow[x][y];
							x--;
							tilebelow[x][y]=field[x][y];
						}
						else if (field[x-1][y] == Boulder) {
							switch (field[x-2][y]) {
							case Water:
								field[x][y] = Blank;
								field[x-2][y] = WaterBoulder;
								tilebelow[x][y] = Blank;
								tilebelow[x-1][y]=Blank;
								tilebelow[x-2][y]=WaterBoulder;
								x--;
								break;
							case WaterBoulder:
								
								//Move the boulder to the WaterBoulder
								MoveObject(x-1,y,x-2,y);
								//Move the player to the space previously occupied by the boulder
								MoveObject(x,y,x-1,y);
								//Update the player position
								x--;
								break;
							case Blank:
								field[x][y] = tilebelow[x][y];
								field[x-2][y] = Boulder;
								tilebelow[x-1][y]=Blank;
								tilebelow[x-2][y]=Blank;
								x--;
								break;
							}
							
						}
						break;
					case VK_RIGHT:
						if (ValidSpot(x+1,y)) {
							if (field[x+1][y] == Boulder||field[x+1][y]==15) x--;
							if (field[x+1][y]==7) bombs++;
							field[x][y]=tilebelow[x][y];
							x++;
							tilebelow[x][y]=field[x][y];
						}
						else if (field[x+1][y] == Boulder) {
							switch (field[x+2][y]) {
							case Water:
								field[x][y] = Blank;
								field[x+2][y] = WaterBoulder;
								x++;
								break;
							case WaterBoulder:
								//Move the boulder to the WaterBoulder
								MoveObject(x+1,y,x+2,y);
								//Move the player to the space previously occupied by the boulder
								MoveObject(x,y,x+1,y);
								//Update the player position
								x++;
								break;
							case Blank:
								field[x][y] = Blank;
								field[x+2][y] = Boulder;
								x++;
								break;
							}
							
						}
						break;
					case VK_SPACE:
						if (bombs) {field[x][y+1]=Bomb;bombs--;}
						break;
					}
					SwitchtoEditor();
				} else {
					field[x][y] = Player;
				}
	} 
}

void Save(char *file2) {
	FILE *file = fopen(file2,"w");
	fprintf(file,"%d\n%d\n",width,height);
	for (int temp=0;temp<width;temp++)
		for (int temp2=0;temp2<height;temp2++)
			fputc(field[temp][temp2],file);
	fprintf(file,"\n%s\n",next);
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
		for (int temp2=0;temp2<height;temp2++) {
			char data = fgetc(file);
			memcpy(&field[temp][temp2],&data,sizeof(char));
		}
	fgetc(file);
	memset(next,0,sizeof(next));
	fgets(next,255,file);
	next[strlen(next)-1]=0;
	gravity = fgetc(file);
	if (gravity==EOF) gravity=0;
	fscanf(file,"%d",&lavatime);
	backuplavatime=lavatime;
	fclose(file);
	} else MessageBox(NULL,"Couldn't open file","Error",NULL);
	RenderField();
}
void Fill(int x,int y) {
	unsigned char target = field[x][y];
	bool **check = new bool*[width];
	for (int temp=0;temp<width;temp++)
		check[temp] = new bool[height];
	for (temp=0;temp<width;temp++)
		for (int temp2=0;temp2<height;temp2++)
			check[temp][temp2]=false;
	check[x][y] = true;
	bool fine = false;
	while (!fine) {
		fine=true;
		for (temp=0;temp<width;temp++)
			for (int temp2=0;temp2<height;temp2++)
				if (check[temp][temp2]) {
					field[temp][temp2]=tool;
					if (temp&&!check[temp-1][temp2]&&field[temp-1][temp2]==target) {
						check[temp-1][temp2]=true;
						fine=false;
					}
					if (temp<width-1&&!check[temp+1][temp2]&&field[temp+1][temp2]==target) {
						check[temp+1][temp2]=true;
						fine=false;
					}
					if (temp2&&!check[temp][temp2-1]&&field[temp][temp2-1]==target) {
						check[temp][temp2-1]=true;
						fine=false;
					}
					if (temp2<height-1&&!check[temp][temp2+1]&&field[temp][temp2+1]==target) {
						check[temp][temp2+1]=true;
						fine=false;
					}

				}
	}
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
		drawing.LoadPicture("grass.bmp",true);//0
		drawing.LoadPicture("blank.bmp",true);//1
		drawing.LoadPicture("boulder.bmp",true);//2
		drawing.LoadPicture("player.bmp",true);//3
		drawing.LoadPicture("spikewall.bmp",true);//4
		drawing.LoadPicture("diamond.bmp",true);//5
		drawing.LoadPicture("fire.bmp",true);//6
		drawing.LoadPicture("bomb.bmp",true);//7
		drawing.LoadPicture("bomb.bmp",true);//Same picture, but represents different objects(1 is collectable, 1 is falling)
		drawing.LoadPicture("fungus.bmp",true);//9
		drawing.LoadPicture("darkfungus.bmp",true);//10
		drawing.LoadPicture("transportright.bmp",true);//11
		drawing.LoadPicture("transportleft.bmp",true);//12
		drawing.LoadPicture("transportup.bmp",true);
		drawing.LoadPicture("transportdown.bmp",true);
		drawing.LoadPicture("brickwall.bmp",true);
		drawing.LoadPicture("grass.bmp",true);//'growing' grass from the grass generator
		drawing.LoadPicture("grassgenerator.bmp",true);
		drawing.LoadPicture("ghost.bmp",true);
		drawing.LoadPicture("gold.bmp",true);
		drawing.LoadPicture("volcano.bmp",true);
		drawing.LoadPicture("lava.bmp",true);
		drawing.LoadPicture("robot.bmp",true);
		drawing.LoadPicture("water.bmp",true);
		drawing.LoadPicture("waterboulder.bmp",true);
		mapeditorbutton=drawing.LoadPicture("mapeditor.bmp",true);
		exitgame=drawing.LoadPicture("exit.bmp",true);
		toolhandle=CreateDialog(GetModuleHandle(0),MAKEINTRESOURCE(IDD_TOOLBAR),hWnd,toolbar);
		if (toolhandle==NULL) MessageBox(NULL,"Could not create toolbar","Error",0);
		else ShowWindow(toolhandle,0);
		DialogBox(GetModuleHandle(0),MAKEINTRESOURCE(IDD_START),hWnd,Start);
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
					tool=Grass;
					break;
				case IDM_EDITOR_BLANK:
					tool=Blank;
					break;
				case IDM_EDITOR_BOULDER:
					tool=Boulder;
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
					{
					char buf[513];
					OPENFILENAME filestruct;
					ZeroMemory(&filestruct,sizeof(filestruct));
					filestruct.lStructSize = sizeof(filestruct);
					filestruct.hwndOwner = hWnd;
					filestruct.hInstance = GetModuleHandle(0);
					filestruct.lpstrFilter=NULL;
					filestruct.lpstrCustomFilter=NULL; 
					filestruct.nMaxCustFilter=NULL;
					filestruct.nFilterIndex=NULL;
					filestruct.lpstrFile=buf;
					filestruct.lpstrFile[0]=0;
					filestruct.nMaxFile=512; 
					filestruct.lpstrFileTitle=NULL; 
					filestruct.nMaxFileTitle=NULL; 
					filestruct.lpstrInitialDir=""; 
					filestruct.lpstrTitle=NULL; 
					filestruct.Flags=OFN_CREATEPROMPT|OFN_HIDEREADONLY|OFN_LONGNAMES|OFN_NOCHANGEDIR; 
					
					filestruct.nFileOffset=NULL; 
					filestruct.nFileExtension=NULL; 
					filestruct.lpstrDefExt=NULL; 
					filestruct.lCustData=NULL; 
					filestruct.lpfnHook=NULL; 
					filestruct.lpTemplateName=NULL; 
					if (!GetSaveFileName(&filestruct)){
						switch (CommDlgExtendedError()) {
						case CDERR_DIALOGFAILURE:
							MessageBox(NULL,"Dialog Failure!",NULL,NULL);
							break;
						case CDERR_FINDRESFAILURE:
							MessageBox(NULL,"Couldn't find a resource",NULL,NULL);
							break;
						case CDERR_INITIALIZATION:
							MessageBox(NULL,"Dialog initialization failure!",NULL,NULL);
							break;
						case FNERR_BUFFERTOOSMALL:
							MessageBox(NULL,"Filename can only be 512 characters",NULL,NULL);
							break;
						default:
							break;
						}
					} else {
						Save(filestruct.lpstrFile);
						state=1;
					}
					
					}
					break;
				case IDM_LOAD:
					DisplayLoadDialog();
					state=1;
					break;
				case IDM_EDITOR_PLAYER:
					tool=Player;
					break;
				case IDM_RUN:
					if (state==1) {
						mapxpos=0;
						mapypos=0;
						state = 2;
						for (int temp=0;temp<width;temp++)
							for (int temp2=0;temp2<height;temp2++)
								backup[temp][temp2] = field[temp][temp2];
					}
					else 
						MessageBox(drawing.GetWindow(),"Must be in map editor to run map","Start map editor first",NULL);
					break;
				case IDM_EDITOR_SPIKEWALL:
					tool=SpikeWall;
					break;
				case IDM_OPTIONS:
					DialogBox(GetModuleHandle(0),MAKEINTRESOURCE(IDD_OPTIONS),drawing.GetWindow(),Options);
					RenderField();
					break;
				case IDM_EDITOR_SWITCHTILES:
					DialogBox(GetModuleHandle(0),MAKEINTRESOURCE(IDD_SWITCHTILES),drawing.GetWindow(),SwitchTiles);
					break;
				case IDM_EDITOR_DIAMOND:
					tool=Diamond;
					break;
				case IDM_EDITOR_FIRE:
					tool=Fire;
					break;
				case IDM_REDRAW:
					TotalRedraw();
					break;
				case IDM_EDITOR_BOMB:
					tool=CollectableBomb;
					break;
				case IDM_EDITOR_BOMBONFIRE:
					tool=Bomb;
					break;
				case IDM_EDITOR_FUNGUS:
					tool=Fungus;
					break;
				case IDM_EDITOR_DARKFUNGUS:
					tool=DarkFungus;
					break;
				case IDM_EDITOR_TRANSPORTRIGHT:
					tool=RightTransport;
					break;
				case IDM_EDITOR_TRANSPORTLEFT:
					tool=LeftTransport;
					break;
				case IDM_EDITOR_TRANSPORTUP:
					tool=UpTransport;
					break;
				case IDM_EDITOR_TRANSPORTDOWN:
					tool=DownTransport;
					break;
				case IDM_EDITOR_BRICKWALL:
					tool=BrickWall;
					break;
				case IDM_EDITOR_GRASSGENERATOR:
					tool=GrassGenerator;
					break;
				case IDM_EDITOR_GHOST:
					tool=Ghost;
					break;
				case IDM_EDITOR_GOLD:
					tool=Gold;
					break;
				case IDM_EDITOR_VOLCANO:
					tool=Volcano;
					break;
				case IDM_EDITOR_LAVA:
					tool=Lava;
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
					break;
				case IDM_VIEW_TOOLBARON:
					ShowWindow(toolhandle,1);
					seetoolbar=true;
					break;
				case IDM_VIEW_TOOLBAROFF:
					ShowWindow(toolhandle,0);
					seetoolbar=false;
					break;
				case IDM_EDITOR_ROBOT:
					tool=Robot;
					break;
				case IDM_EDITOR_WATER:
					tool=Water;
					break;
				case IDM_EDITOR_WATERBOULDER:
					tool=WaterBoulder;
					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			{
			//int blah = drawing.LoadPicture("font.bmp",true);
				hdc = BeginPaint(drawing.GetWindow(),&ps);
				drawing.SetTarget(&hdc);
				switch (state) {
				case 1: RenderField();break;
				}
				EndPaint(drawing.GetWindow(),&ps);
				hdc = NULL;
				drawing.SetTarget(&hdc);
			}
			break;
		case WM_MOUSEMOVE:
			{
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
	unsigned short len=0;
	switch (message) {
	case WM_INITDIALOG:
		SetDlgItemText(hdlg,IDC_NEXT,next);
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
			GetDlgItemText(hdlg,IDC_NEXT,next,256);
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
		/*
			This section adds strings to the dialog listboxes 'IDC_FROM' and 'IDC_TO' for selection
		*/
		//Adds a string to the listbox 'IDC_FROM'. 
		#define AddToFrom(string) SendMessage(GetDlgItem(hdlg,IDC_FROM),LB_ADDSTRING,0,(LPARAM) (LPCTSTR)##string##)
		//Adds a string to the listbox 'IDC_TO'
		#define AddToTo(string) SendMessage(GetDlgItem(hdlg,IDC_TO),LB_ADDSTRING,0,(LPARAM) (LPCTSTR)##string##)
		//Adds a string to both listboxes
		#define AddString(string) AddToFrom(string);AddToTo(string)
		//Add strings
		AddString("Grass");
		AddString("Blank");
		AddString("Boulder");
		AddString("Player");
		AddString("SpikeWall");
		AddString("Diamond");
		AddString("Fire");
		AddString("Bomb");
		AddString("CollectableBomb");
		AddString("Fungus");
		AddString("DarkFungus");
		AddString("RightTransport");
		AddString("LeftTransport");
		AddString("UpTransport");
		AddString("DownTransport");
		AddString("BrickWall");
		AddString("GrowingGrass");
		AddString("GrassGenerator");
		AddString("Ghost");
		AddString("Gold");
		AddString("Volcano");
		AddString("Lava");
		AddString("Robot");
		AddString("Water");
		AddString("WaterBoulder");
		#undef AddToFrom
		#undef AddToTo
		#undef Addstring

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
			for (int temp=0;temp<width-1;temp++)
				for (temp2=0;temp2<height-1;temp2++)
					if (field[temp][temp2]==from) 
						field[temp][temp2] = (Tile)to;
					
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


int __stdcall Start(HWND hdlg,UINT message,WPARAM wParam,LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDCANCEL:
			SendMessage(hdlg,WM_CLOSE,0,0);
			break;
		case IDOK:
			EndDialog(hdlg,0);
			state=1;
			SendMessage(drawing.GetWindow(),WM_PAINT,0,0);
			break;
		case IDC_LOAD:
			EndDialog(hdlg,0);
			DisplayLoadDialog();
			SendMessage(drawing.GetWindow(),WM_PAINT,0,0);
			break;
		}
		break;
		case WM_CLOSE:
			EndDialog(hdlg,0);
			SendMessage(drawing.GetWindow(),WM_CLOSE,0,0);
			break;
	}
	return FALSE;
}


void DisplayLoadDialog(void) {
	char buf[513];
	OPENFILENAME filestruct;
	ZeroMemory(&filestruct,sizeof(filestruct));
	filestruct.lStructSize = sizeof(filestruct);
	filestruct.hwndOwner = drawing.GetWindow();
	filestruct.hInstance = GetModuleHandle(0);
	filestruct.lpstrFilter=NULL;
	filestruct.lpstrCustomFilter=NULL; 
	filestruct.nMaxCustFilter=NULL;
	filestruct.nFilterIndex=NULL;
	filestruct.lpstrFile=buf;
	filestruct.lpstrFile[0]=0;
	filestruct.nMaxFile=512; 
	filestruct.lpstrFileTitle=NULL; 
	filestruct.nMaxFileTitle=NULL; 
	filestruct.lpstrInitialDir=""; 
	filestruct.lpstrTitle=NULL; 
	filestruct.Flags=OFN_CREATEPROMPT|OFN_HIDEREADONLY|OFN_LONGNAMES|OFN_NOCHANGEDIR; 
	
	filestruct.nFileOffset=NULL; 
	filestruct.nFileExtension=NULL; 
	filestruct.lpstrDefExt=NULL; 
	filestruct.lCustData=NULL; 
	filestruct.lpfnHook=NULL; 
	filestruct.lpTemplateName=NULL; 
	if (!GetOpenFileName(&filestruct)){
		switch (CommDlgExtendedError()) {
		case CDERR_DIALOGFAILURE:
			MessageBox(NULL,"Dialog Failure!",NULL,NULL);
			break;
		case CDERR_FINDRESFAILURE:
			MessageBox(NULL,"Couldn't find a resource",NULL,NULL);
			break;
		case CDERR_INITIALIZATION:
			MessageBox(NULL,"Dialog initialization failure!",NULL,NULL);
			break;
		case FNERR_BUFFERTOOSMALL:
			MessageBox(NULL,"Filename can only be 512 characters",NULL,NULL);
			break;
		default:
			break;
		}
	} else {
		Load(filestruct.lpstrFile);
		state=1;
	}
	
}
