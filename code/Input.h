// Input.h: interface for the Input class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUT_H__276A6C93_5955_4411_814E_9542E788F453__INCLUDED_)
#define AFX_INPUT_H__276A6C93_5955_4411_814E_9542E788F453__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CInput  
{
	vector<RECT> ClickZone;
	vector<void (*)(int,int)> ClickCall; //Vector of functions to call when mouse is pressed.
	vector<RECT> DragZone;
	vector<void (*)(int,int)> DragCall;
	vector<int> keyvalues;
	vector<void (*)(int)> keycall;
	int clickzones;//Make -1, so when I increment the value, it'll be 0, the start of the array, and I can refer to the array based on this value
	int dragzones;
	int keys;
	int mMouseX,mMouseY;
	bool mKeyBuffer[256];
	int mLeftDown,mRightDown;
	void ProcessClickZones(void);
	void ProcessDragZones(void);
public:
	CInput();
	virtual ~CInput();
	void AddClickZone(int x,int y,int x2,int y2,void (callback)(int mousex,int mousey));
	void AddDragZone(int x,int y,int x2,int y2,void (callback)(int mousex,int mousey));
	void AddKey(int key,void (callback)(int key));
	bool KeyPressed(int key);
	void UpdateInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif // !defined(AFX_INPUT_H__276A6C93_5955_4411_814E_9542E788F453__INCLUDED_)
