// Draw.h: interface for the CDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAW_H__36F76551_80C2_49CC_AA15_94D6E6E44F25__INCLUDED_)
#define AFX_DRAW_H__36F76551_80C2_49CC_AA15_94D6E6E44F25__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//Structure containing font data
struct CFont {
	int main;
	vector<HANDLE> characters;
	int charactercount;
	CFont(void) {charactercount=-1;}
};
//Drawing class
class CDraw {
	COLORREF mColor;
	HWND mWindow;
	bool mRenew;
	HDC mHdc;
	HBRUSH mBrush;
	HPEN mPen;
	bool mBrushChanged, mColorChanged;
	void EnactChanges(void);
	bool mDrawing;
	vector <HANDLE> mPictures;
	vector <CFont> mFonts;
	int mPictureCount;
	int mFontCount;
	int mCurrentFont;
	vector<char> mRegisteredCharacters;
public:
	CDraw(void);
	~CDraw(void);
	void SetColor(COLORREF color);
	void SetColor(unsigned char r,unsigned char g,unsigned char b);
	void SetWindow(HWND hwnd);
	void SetRenew(bool value);
	void SetBrush(HBRUSH brush);
	bool SetBrush(UINT style,COLORREF color,LONG hatch);
	bool SetBrush(LOGBRUSH *brush);
	void SetPen(HPEN pen);
	bool SetPen(DWORD style, DWORD width);
	void SetTarget(HDC *target);
	bool StartDraw(void);
	bool EndDraw(void);
	bool DrawLine(int x,int y,int x2, int y2);
	bool DrawLine(int x,int y,int x2, int y2,COLORREF color,HWND hwnd);
	bool DrawSquare(int x,int y, int width,int height);
	bool DrawSquare(int x,int y, int width,int height, COLORREF color, HWND hwnd);
	bool DrawDot(int x,int y);
	bool DrawPicture(int picture,int x, int y, int width, int height);
	bool DrawPictureEffects(int picture,int x, int y, int width, int height,int style);
	void RepeatPicture(int picture,int x, int y, int width, int height);
	int  LoadPicture(char *picture,bool file);
	HANDLE GetPictureHandle(int picture);
	HWND GetWindow(void);
};

#endif // !defined(AFX_DRAW_H__36F76551_80C2_49CC_AA15_94D6E6E44F25__INCLUDED_)
