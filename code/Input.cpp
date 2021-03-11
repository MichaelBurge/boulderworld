// Input.cpp: implementation of the Input class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Input.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInput::CInput()
{
clickzones=-1;
dragzones=-1;
keys=0;
}

CInput::~CInput()
{

}
//////////////////////////////////////////////////////////////////////
// Data for input
//////////////////////////////////////////////////////////////////////
void CInput::AddClickZone(int x,int y,int x2,int y2,void (callback)(int mousex,int mousey)) {
	RECT temp = {x,y,x2,y2};
	ClickZone.push_back(temp);
	void (*blah)(int,int) = callback;
	ClickCall.push_back(blah);
	clickzones++;
}
void CInput::AddDragZone(int x,int y,int x2,int y2,void (callback)(int mousex,int mousey)) {
	RECT temp = {x,y,x2,y2};
	DragZone.push_back(temp);
	void (*blah)(int,int) = callback;
	DragCall.push_back(blah);
	dragzones++;
}
void CInput::ProcessClickZones(void) {
	RECT temp2;
	int x=mMouseX,y=mMouseY;
	for (int temp=0;temp<=clickzones;temp++) {
		temp2 = ClickZone[temp];
		if (x >= temp2.left && x <= temp2.right && y >= temp2.top && y <= temp2.bottom)
			ClickCall[temp](x,y);
	}
}
void CInput::ProcessDragZones(void) {
	RECT temp2;
	int x=mMouseX,y=mMouseY;
	for (int temp=0;temp<=dragzones;temp++) {
		temp2 = DragZone[temp];
		if (x >= temp2.left && x <= temp2.right && y >= temp2.top && y <= temp2.bottom)
			DragCall[temp](x,y);
	}
}
bool CInput::KeyPressed(int key) {
	return mKeyBuffer[key];
}
void CInput::UpdateInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
	case WM_MOUSEMOVE:
		mMouseX = LOWORD(lParam);
		mMouseY = HIWORD(lParam);
		mLeftDown = wParam & MK_LBUTTON;
		if (mLeftDown) ProcessDragZones();
		break;
	case WM_KEYDOWN:
		{
			
		for(int temp=0;temp<keys;temp++)
			if ((int)wParam==keyvalues[temp])
				keycall[temp]((int)wParam);
		}
		mKeyBuffer[(unsigned char)wParam] = true;
		break;
	case WM_KEYUP:
		mKeyBuffer[(unsigned char)wParam] = false;
		break;
	case WM_LBUTTONDOWN:
		mLeftDown=true;
		ProcessClickZones();
		break;
	case WM_LBUTTONUP:
		mLeftDown=false;
		break;
	case WM_RBUTTONDOWN:
		mRightDown=true;
		break;
	case WM_RBUTTONUP:
		mRightDown=false;
	}

}

void CInput::AddKey(int key,void (callback)(int key)) {
	keyvalues.push_back(key);
	void (*blah)(int) = callback;
	keycall.push_back(blah);
	keys++;
}
