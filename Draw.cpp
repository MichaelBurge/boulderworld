// Draw.cpp: implementation of the CDraw class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CDraw::CDraw() {
SetColor(128,128,128);//Set color to grey
mWindow = NULL;
mRenew = true;
mHdc = NULL;
mBrush = NULL;
mBrushChanged = false;
mColorChanged = false;
mDrawing = false;
mPictureCount = 0;
mFontCount = 0;
}

CDraw::~CDraw() {
if (mHdc != NULL) ReleaseDC(mWindow,mHdc);
}
//////////////////////////////////////////////////////////////////////
// Loading functions
//////////////////////////////////////////////////////////////////////
int CDraw::LoadPicture(char *picture,bool file) {
	mPictures.push_back(LoadImage(GetModuleHandle(0),picture,IMAGE_BITMAP,0,0,(file ?LR_LOADFROMFILE:0)|LR_CREATEDIBSECTION|LR_DEFAULTSIZE));
	if (!mPictures[mPictureCount]) {
		string error = "Failed to load image:";
		error += picture;
		error += "Error:";
		char buf[256];
		error += FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_FROM_HMODULE,
			GetModuleHandle(0),
			GetLastError(),
			0,
			buf,
			256,
			NULL);

		MessageBox(0,"Failed to load image.",NULL,NULL);
	}
	return mPictureCount++;
}
//////////////////////////////////////////////////////////////////////
// Setting functions
//////////////////////////////////////////////////////////////////////

void CDraw::SetColor(unsigned char r,unsigned char g, unsigned char b) {
	mColor = RGB(r,g,b);
	LOGBRUSH brush = {BS_SOLID,mColor,NULL};
	mBrush = CreateBrushIndirect(&brush);
}
void CDraw::SetColor(COLORREF color) {
	mColor = color;
	LOGBRUSH brush = {BS_SOLID,mColor,NULL};
	mBrush = CreateBrushIndirect(&brush);
}
void CDraw::SetWindow(HWND hwnd) {
	mWindow = hwnd;
	if (mHdc!=NULL) ReleaseDC(mWindow,mHdc);
	mHdc = GetDC(mWindow);
}

void CDraw::SetBrush(HBRUSH brush) {
	mBrush = brush;
	DeleteObject(SelectObject(mHdc,mBrush));
}
bool CDraw::SetBrush(LOGBRUSH *brush) {
	mBrush = CreateBrushIndirect(brush);
	DeleteObject(SelectObject(mHdc,mBrush));
	return mBrush != NULL;
}
bool CDraw::SetBrush(UINT style,COLORREF color,LONG hatch) {
	LOGBRUSH brush = { style,color,hatch };
	mBrush = CreateBrushIndirect(&brush);
	DeleteObject(SelectObject(mHdc,mBrush));
	return mBrush != NULL;
}
void CDraw::SetPen(HPEN pen) {
	mPen = pen;
	DeleteObject(SelectObject(mHdc,mPen));
}
bool CDraw::SetPen(DWORD style,DWORD width) {
	LOGBRUSH brush;
	GetObject(mBrush,sizeof(brush),&brush);
	mPen = ExtCreatePen(style,width,&brush,NULL,NULL);
	DeleteObject(SelectObject(mHdc,mPen));
	return mPen!=NULL;
}

void CDraw::SetRenew(bool value) {
	mRenew = value;
}
void CDraw::SetTarget(HDC *target) {
	if (mHdc != NULL) ReleaseDC(mWindow,mHdc);
	mHdc = *target;
}
void CDraw::EnactChanges() {
	DeleteObject(SelectObject(mHdc,mBrush));
	DeleteObject(SelectObject(mHdc,mPen));

}

//////////////////////////////////////////////////////////////////////
// Drawing functions
//////////////////////////////////////////////////////////////////////
bool CDraw::StartDraw() {
	mDrawing = true;//Set variable to say we are drawing
	bool success;//
	mBackHdc = GetDC(mWindow);//Obtain device context so we can draw in the window
	success = mBackHdc>0;//Set success to whether HDC is valid
	//mHdc = CreateCompatibleDC(mBackHdc);//Create backbuffer HDC
	RECT rt;
	GetClientRect(mWindow,&rt);//Get size of window
	//mBackBitmap = CreateCompatibleBitmap(mHdc,rt.right,rt.bottom);//Create bitmap to go in HDC
	//mBitmapHold = (HBITMAP)SelectObject(mHdc,mBackBitmap);//Put the bitmap in the HDC
	EnactChanges();//Call function to set everything tow ork on the new HDC settings
	return success;//Return whether successful.
}
bool CDraw::EndDraw() {
	if (!mDrawing) //If not drawing, exit
		return false;
	RECT rt;
	GetClientRect(mWindow,&rt);//Get size of window
	//BitBlt(mBackHdc,0,0,rt.right,rt.bottom,mHdc,0,0,SRCCOPY);//Blit from backbuffer to frontbuffer
	//SelectObject(mHdc,mBitmapHold);//Set previous bitmap into the HDC
	//DeleteObject(mBackBitmap);//Delete the backbuffer
	//DeleteDC(mHdc);//Delete the backbuffer HDC
	return ReleaseDC(mWindow,mHdc)!=NULL;//Release the window DC and return whether successful
}

bool CDraw::DrawLine(int x,int y,int x2, int y2,COLORREF color,HWND hwnd) {
	bool success = true;
	if (mRenew) {mHdc = GetDC(hwnd);SetColor(color);EnactChanges();}
	if (mHdc != NULL) {
	success = MoveToEx(mHdc,x,y,NULL) > 0;
	if (success) 
		success = LineTo(mHdc,x2,y2) > 0;
	if (mRenew && success)
		success = ReleaseDC(hwnd,mHdc) > 0;
	} else success = false;
	return success;
}

bool CDraw::DrawLine(int x,int y,int x2, int y2) {
	bool success = true;
	if (mRenew) {mHdc = GetDC(mWindow);	EnactChanges();}
	if (mHdc != NULL) {
	success = MoveToEx(mHdc,x,y,NULL) > 0;
	if (success) 
		success = LineTo(mHdc,x2,y2) > 0;
	if (mRenew && success)
		success = ReleaseDC(mWindow,mHdc) > 0;
	} else success = false;
	return success;
}

bool CDraw::DrawSquare(int x,int y, int width, int height, COLORREF color,HWND hwnd) {
	bool success = true;
	SetWindow(hwnd);
	SetColor(color);
	if (mRenew) {mHdc = GetDC(mWindow);	EnactChanges();}
	if (mHdc != NULL) {										
		success = Rectangle(mHdc,x,y,x+width,y+height) != NULL;
		if (mRenew && success) ReleaseDC(mWindow,mHdc);
	} else success = false;
	return success;
}
bool CDraw::DrawSquare(int x,int y, int width, int height) {
	bool success = true;
	if (mRenew) {mHdc = GetDC(mWindow);	EnactChanges();}
	if (mHdc != NULL) {
		success = Rectangle(mHdc,x,y,x+width,y+height) != NULL;
		if (mRenew && success) ReleaseDC(mWindow,mHdc);
	} else success = false;
	return success;
}

bool CDraw::DrawDot(int x,int y) {
	bool success = true;
	if (mRenew) {mHdc = GetDC(mWindow);	EnactChanges();}
	if (mHdc != NULL) {
		success = SetPixel(mHdc,x,y,mColor) != NULL;
		if (mRenew && success) ReleaseDC(mWindow,mHdc);
	} else success = false;
	return success;
}

bool CDraw::DrawPicture(int picture,int x, int y, int width, int height) {
	HDC hdc = CreateCompatibleDC(mHdc);
	bool success = (hdc != NULL);
	if (mRenew) {mHdc = GetDC(mWindow);	EnactChanges();}
	if (success) {
		DeleteObject(SelectObject(hdc,mPictures[picture]));
		success = (BitBlt(mHdc,x,y,x+width,y+height,hdc,0,0,SRCCOPY)!=NULL);
		DeleteDC(hdc);
	}
	return success;
}
bool CDraw::DrawPictureEffects(int picture,int x, int y, int width, int height,int style) {
	HDC hdc = CreateCompatibleDC(mHdc);
	bool success = (hdc != NULL);
	//if (mRenew) {mHdc = GetDC(mWindow);}
	if (success) {
		DeleteObject(SelectObject(hdc,mPictures[picture]));
		success = (BitBlt(mHdc,x,y,x+width,y+height,hdc,0,0,style)!=NULL);
		DeleteDC(hdc);
	}
	return success;
}
void CDraw::RepeatPicture(int picture,int x,int y,int width,int height) {
	BITMAP data;
	GetObject(mPictures[picture],sizeof(data),&data);
	int x_step=data.bmWidth;
	int y_step=data.bmHeight;
	int temp,temp2;
	for (temp2=x;temp2<width;temp2+=x_step)
		for (temp=y;temp<height;temp+=y_step)
			DrawPicture(picture,x+temp2,y+temp,x_step,y_step);
}

//////////////////////////////////////////////////////////////////////
// Retrieval functions
//////////////////////////////////////////////////////////////////////
HANDLE CDraw::GetPictureHandle(int picture) {
	return mPictures[picture];
}

HWND CDraw::GetWindow(void) {
	return mWindow;
}