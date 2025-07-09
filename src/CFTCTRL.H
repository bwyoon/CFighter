#ifndef __CFTCTRL_H
#define __CFTCTRL_H

#include <control.h>
#include <groupbox.h>

_CLASSDEF(YButton)
class YButton : public TControl
{
public:

  YButton(PTWindowsObject, int, LPSTR, int, int, int, int, PTModule AModule = NULL);
  ~YButton();

protected:
  int _pressed;
  int _btnstate;
  int _capturestate;
  virtual LPSTR GetClassName();
  virtual void GetWindowClass(WNDCLASS &);
  virtual void Paint(HDC, PAINTSTRUCT &);
  virtual void WMLButtonDown(RTMessage)  = [WM_FIRST + WM_LBUTTONDOWN];
  virtual void WMLButtonUp(RTMessage)    = [WM_FIRST + WM_LBUTTONUP];
  virtual void WMMouseMove(RTMessage)    = [WM_FIRST + WM_MOUSEMOVE];
  virtual void Repaint(BOOL);
};

_CLASSDEF(YCheckBox)
class YCheckBox : public YButton
{
public:
  COLORREF Color;

  YCheckBox(PTWindowsObject, int, LPSTR, int, int, int, int, COLORREF, BOOL, PTModule AModule = NULL);
  ~YCheckBox();

protected:
  BOOL _ischecked;
  virtual LPSTR GetClassName();
  virtual void GetWindowClass(WNDCLASS &);
  virtual void Paint(HDC, PAINTSTRUCT &);
  virtual void WMLButtonDown(RTMessage)  = [WM_FIRST + WM_LBUTTONDOWN];
  virtual void WMLButtonUp(RTMessage)    = [WM_FIRST + WM_LBUTTONUP];
public :
  virtual BOOL IsChecked();
  virtual void Check();
  virtual void Uncheck();
};

/*
_CLASSDEF(YCheckBox1)
class YCheckBox1 : public YCheckBox
{
public:

  YCheckBox1(PTWindowsObject, int, LPSTR, int, int, int, int, BOOL, PTModule AModule = NULL);
  ~YCheckBox1();

  protected:
  virtual LPSTR GetClassName();
  virtual void GetWindowClass(WNDCLASS &);
  virtual void Paint(HDC, PAINTSTRUCT &);
};
*/

_CLASSDEF(YStatic)
class YStatic : public TControl
{
public:

  YStatic(PTWindowsObject, int, LPSTR, int, int, int, int, WORD, PTModule AModule = NULL);
  ~YStatic();

protected:
  char *_str;
  WORD  _len;
  virtual LPSTR GetClassName();
  virtual void GetWindowClass(WNDCLASS &);
  virtual void Paint(HDC, PAINTSTRUCT &);
  virtual void Repaint(BOOL);

public :
  virtual void SetText(char *);
  virtual void GetText(char *);
};

#endif

