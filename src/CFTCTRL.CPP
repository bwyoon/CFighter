#include "cftctrl.h"
#include <string.h>

YButton::YButton(PTWindowsObject AParent, int AnId, LPSTR ATitle, int X, int Y, int W, int H,
		 PTModule AModule)
: TControl(AParent, AnId, ATitle, X, Y, W, H, AModule)
{
      _pressed = _btnstate = _capturestate = 0;
}

YButton::~YButton()
{
}

LPSTR YButton::GetClassName()
{
     return "YButton";
}

void YButton::GetWindowClass(WNDCLASS& AWndClass)
{
  TControl::GetWindowClass(AWndClass);
}

void YButton::Paint(HDC DC, PAINTSTRUCT& PS)
{
     HBRUSH OldBrush = (_btnstate == 0) ? (HBRUSH)SelectObject(DC, GetStockObject(WHITE_BRUSH))
				        : (HBRUSH)SelectObject(DC, GetStockObject(BLACK_BRUSH));
     HPEN   OldPen   = (HPEN)SelectObject(DC, GetStockObject(BLACK_PEN));

     Rectangle(DC, 0, 0, Attr.W, Attr.H);

     HFONT  OldFont  = (HFONT)SelectObject(DC, GetStockObject(SYSTEM_FONT));
     COLORREF OldTC = (_btnstate == 0) ? SetTextColor(DC, RGB(0,0,0))
				     : SetTextColor(DC, RGB(255,255,255));
     UINT OldTA = SetTextAlign(DC, TA_CENTER | TA_TOP);
     int OldBK = SetBkMode(DC, TRANSPARENT);

     int X = (PS.rcPaint.left+PS.rcPaint.right)/2;
     int Y = (PS.rcPaint.top+PS.rcPaint.bottom)/2-8;

     TextOut(DC, X, Y, Title, strlen(Title));

     SetBkMode(DC, OldBK);
     SetTextAlign(DC, OldTA);
     SetTextColor(DC, OldTC);

     SelectObject(DC, OldFont);
     SelectObject(DC, OldBrush);
     SelectObject(DC, OldPen);
}

void YButton::WMLButtonDown(RTMessage)
{
     _btnstate = _pressed = 1;
     Repaint(FALSE);
     if (_capturestate == 0) {
	SetCapture(HWindow);
        _capturestate = 1;
     }
}

void YButton::WMLButtonUp(RTMessage Message)
{
     _pressed = 0;
     if (_btnstate != 0) {
	_btnstate = 0;
        Repaint(FALSE);
     }
     if (_capturestate == 1) {
	ReleaseCapture();
	_capturestate = 0;
     }
     int X = Message.LP.Lo;
     int Y = Message.LP.Hi;
     if ((X >= 0) && (X < Attr.W) && (Y >= 0) && (Y < Attr.H))
	SendMessage(Parent->HWindow, WM_COMMAND, Attr.Id, 0);
}

void YButton::WMMouseMove(RTMessage Message)
{
     if (_pressed != 0) {
        int X = Message.LP.Lo;
        int Y = Message.LP.Hi;
        if ((X >= 0) && (X < Attr.W) && (Y >= 0) && (Y < Attr.H)) {
	   if (_btnstate == 0) {
	      _btnstate = 1;
              Repaint(FALSE);
	   }
        } else {
	   if (_btnstate != 0) {
	      _btnstate = 0;
              Repaint(FALSE); 
	   }
	}
     }
}

void YButton::Repaint(BOOL OX)
{
     InvalidateRect(HWindow, NULL, OX);
     PAINTSTRUCT ps;
     HDC hdc = BeginPaint(HWindow, &ps);
     SaveDC(hdc);
     Paint(hdc, ps);
     RestoreDC(hdc, -1);
     EndPaint(HWindow, &ps);
}

YCheckBox::YCheckBox(PTWindowsObject AParent, int AnId, LPSTR ATitle, int X, int Y, int W, int H, COLORREF AColor,
		     BOOL Checked, PTModule AModule)
: YButton(AParent, AnId, ATitle, X, Y, W, H, AModule)
{
      _ischecked = Checked;
      Color = AColor;
}

YCheckBox::~YCheckBox()
{
}

LPSTR YCheckBox::GetClassName()
{
     return "YCheckBox";
}

void YCheckBox::GetWindowClass(WNDCLASS& AWndClass)
{
     YButton::GetWindowClass(AWndClass);
}

void YCheckBox::Paint(HDC DC, PAINTSTRUCT&)
{
     int      Xo = 0;
     int      Yc = Attr.H/2;

     HPEN OldPen   =  (HPEN)SelectObject(DC, GetStockObject(NULL_PEN));
     HBRUSH OldBrush = (HBRUSH)SelectObject(DC, GetStockObject(LTGRAY_BRUSH));
     Rectangle(DC, 0, 0, Attr.W+1, Attr.H+1);
     SelectObject(DC, GetStockObject(BLACK_PEN));
     if (!_ischecked) SelectObject(DC, GetStockObject(LTGRAY_BRUSH));
     else SelectObject(DC, CreateSolidBrush(Color)); //RGB(0, 255, 0)));
     Rectangle(DC, Xo, Yc-4, Xo+13, Yc+5);
     if (!_ischecked) SelectObject(DC, OldBrush);
     else DeleteObject(SelectObject(DC, OldBrush));
     if (!_pressed) {
        if (!_ischecked) SelectObject(DC, CreatePen(PS_SOLID, 1, RGB(255, 255, 255)));
        else SelectObject(DC, CreatePen(PS_SOLID, 1, RGB(128, 128, 128)));
        MoveTo(DC, Xo+12, Yc-4);
        LineTo(DC, Xo,    Yc-4);
        LineTo(DC, Xo,    Yc+5);
	if (!_ischecked) DeleteObject(SelectObject(DC, CreatePen(PS_SOLID, 1, RGB(128, 128, 128))));
	else  DeleteObject(SelectObject(DC, CreatePen(PS_SOLID, 1, RGB(128, 128, 128))));
        MoveTo(DC, Xo+1,  Yc+4);
        LineTo(DC, Xo+12, Yc+4);
        LineTo(DC, Xo+12, Yc-4);
        DeleteObject(SelectObject(DC, OldPen));
     } else SelectObject(DC, OldPen);
     HFONT OldFont = (HFONT)SelectObject(DC, GetStockObject(SYSTEM_FONT));
     COLORREF OldTC =  SetTextColor(DC, RGB(0,0,0));
     UINT OldTA = SetTextAlign(DC, TA_LEFT | TA_TOP);
     int OldBK = SetBkMode(DC, TRANSPARENT);

     TextOut(DC, Xo+18, Yc-8,  Title, strlen(Title));

     SetBkMode(DC, OldBK);
     SetTextAlign(DC, OldTA);
     SetTextColor(DC, OldTC);
     SelectObject(DC, OldFont);
}

void YCheckBox::WMLButtonDown(RTMessage)
{
     _pressed = 1;
     Repaint(FALSE);
     if (_capturestate == 0) {
	SetCapture(HWindow);
        _capturestate = 1;
     }
}

void YCheckBox::WMLButtonUp(RTMessage Message)
{
     _pressed = 0;
     if (_capturestate == 1) {
	ReleaseCapture();
	_capturestate = 0;
     }
     int X = Message.LP.Lo;
     int Y = Message.LP.Hi;
     if ((X >= 0) && (X < Attr.W) && (Y >= 0) && (Y < Attr.H)) {
	if (_ischecked) _ischecked = FALSE;
	else  _ischecked = TRUE;
	Repaint(FALSE);
	SendMessage(Parent->HWindow, WM_COMMAND, Attr.Id, 0);
     } else Repaint(FALSE);
}

BOOL YCheckBox::IsChecked()
{
     return _ischecked;
}

void YCheckBox::Check()
{
     _ischecked = TRUE;
     Repaint(FALSE);
}

void YCheckBox::Uncheck()
{
     _ischecked = FALSE;
     Repaint(FALSE);
}

/*
YCheckBox1::YCheckBox1(PTWindowsObject AParent, int AnId, LPSTR ATitle, int X, int Y, int W, int H,
		     BOOL Checked, PTModule AModule)
: YCheckBox(AParent, AnId, ATitle, X, Y, W, H, Checked, AModule)
{
}

YCheckBox1::~YCheckBox1()
{
}

LPSTR YCheckBox1::GetClassName()
{
     return "YCheckBox1";
}

void YCheckBox1::GetWindowClass(WNDCLASS& AWndClass)
{
     YCheckBox::GetWindowClass(AWndClass);
}

void YCheckBox1::Paint(HDC DC, PAINTSTRUCT&)
{
     int      Xo = 0;
     int      Yc = Attr.H/2;

     HPEN OldPen   =  (HPEN)SelectObject(DC, GetStockObject(NULL_PEN));
     HBRUSH OldBrush = (HBRUSH)SelectObject(DC, GetStockObject(LTGRAY_BRUSH));
     Rectangle(DC, 0, 0, Attr.W+1, Attr.H+1);
     SelectObject(DC, GetStockObject(BLACK_PEN));
     if (!_ischecked) SelectObject(DC, GetStockObject(LTGRAY_BRUSH));
     else SelectObject(DC, CreateSolidBrush(RGB(255, 0, 255)));
     Rectangle(DC, Xo, Yc-4, Xo+13, Yc+5);
     if (!_ischecked) SelectObject(DC, OldBrush);
     else DeleteObject(SelectObject(DC, OldBrush));
     if (!_pressed) {
        if (!_ischecked) SelectObject(DC, CreatePen(PS_SOLID, 1, RGB(255, 255, 255)));
        else SelectObject(DC, CreatePen(PS_SOLID, 1, RGB(128, 128, 128)));
        MoveTo(DC, Xo+12, Yc-4);
        LineTo(DC, Xo,    Yc-4);
        LineTo(DC, Xo,    Yc+5);
	if (!_ischecked) DeleteObject(SelectObject(DC, CreatePen(PS_SOLID, 1, RGB(128, 128, 128))));
	else  DeleteObject(SelectObject(DC, CreatePen(PS_SOLID, 1, RGB(128, 128, 128))));
        MoveTo(DC, Xo+1,  Yc+4);
        LineTo(DC, Xo+12, Yc+4);
        LineTo(DC, Xo+12, Yc-4);
        DeleteObject(SelectObject(DC, OldPen));
     } else SelectObject(DC, OldPen);
     HFONT OldFont = (HFONT)SelectObject(DC, GetStockObject(SYSTEM_FONT));
     COLORREF OldTC =  SetTextColor(DC, RGB(0,0,0));
     UINT OldTA = SetTextAlign(DC, TA_LEFT | TA_TOP);
     int OldBK = SetBkMode(DC, TRANSPARENT);

     TextOut(DC, Xo+18, Yc-8,  Title, strlen(Title));

     SetBkMode(DC, OldBK);
     SetTextAlign(DC, OldTA);
     SetTextColor(DC, OldTC);
     SelectObject(DC, OldFont);
} */

YStatic::YStatic(PTWindowsObject AParent, int AnId, LPSTR ATitle, int X, int Y, int W, int H,
		 WORD TextLen, PTModule AModule)
: TControl(AParent, AnId, ATitle, X, Y, W, H, AModule)
{
     _len = TextLen;
     _str = new char[_len+1];
     strncpy(_str, ATitle, _len);
     _str[_len] = '\0';
     
}

YStatic::~YStatic()
{
     delete _str;
}

LPSTR YStatic::GetClassName()
{
     return "YStatic";
}

void YStatic::GetWindowClass(WNDCLASS& AWndClass)
{
     TControl::GetWindowClass(AWndClass);
}

void YStatic::Paint(HDC DC, PAINTSTRUCT&)
{
     int      Xo = 0;
     int      Yc = Attr.H/2;

     HPEN   OldPen   =   (HPEN)SelectObject(DC, GetStockObject(NULL_PEN));
     HBRUSH OldBrush = (HBRUSH)SelectObject(DC, GetStockObject(LTGRAY_BRUSH));
     Rectangle(DC, 0, 0, Attr.W+1, Attr.H+1);
     HFONT  OldFont = (HFONT)SelectObject(DC, GetStockObject(SYSTEM_FONT));
     COLORREF OldTC =  SetTextColor(DC, RGB(0,0,0));
     int OldTA = SetTextAlign(DC, TA_LEFT | TA_TOP);
     int OldBK = SetBkMode(DC, TRANSPARENT);

     TextOut(DC, Xo+2, Yc-8, _str, strlen(_str));

     SetBkMode(DC, OldBK);
     SetTextAlign(DC, OldTA);
     SetTextColor(DC, OldTC);
     SelectObject(DC, OldFont);
     SelectObject(DC, OldBrush);
     SelectObject(DC, OldPen);
}

void YStatic::Repaint(BOOL OX)
{
     InvalidateRect(HWindow, NULL, OX);
     PAINTSTRUCT ps;
     HDC hdc = BeginPaint(HWindow, &ps);
     SaveDC(hdc);
     Paint(hdc, ps);
     RestoreDC(hdc, -1);
     EndPaint(HWindow, &ps);
}

void YStatic::SetText(char *str)
{
     strncpy(_str, str, _len);
     _str[_len] = '\0';
     Repaint(FALSE);
}

void YStatic::GetText(char *str)
{
     strncpy(str, _str, _len);
     str[_len] = '\0';
}

