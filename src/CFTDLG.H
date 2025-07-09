#ifndef __CFTDLG_H
#define __CFTDLG_H

#include <owl.h>
#include <dialog.h>
#include <listbox.h>
#include <static.h>
#include <edit.h>
#include <button.h>
#include <groupbox.h>
#include <radiobut.h>
#include "cfttype.h"
#include "dlgdef.h"
#include "cftbmp.h"

class InputDLG : public TDialog
{
   protected :
      LPSTR Title;
      LPSTR Prompt;
      LPSTR str;
      UINT  len;

   public  :
      InputDLG(PTWindowsObject, LPSTR, LPSTR, LPSTR, UINT, int);
      ~InputDLG() {};
      virtual void SetupWindow();
      virtual void Ok(RTMessage Msg)     = [ID_FIRST + IDOK];
      virtual void Cancel(RTMessage Msg) = [ID_FIRST + IDCANCEL] { CloseWindow(FALSE); };
};

class AboutDLG : public TDialog
{
   private :
      Bitmap *BT;
   public  :
      AboutDLG(PTWindowsObject PTWO) : TDialog(PTWO, ABOUT) {};
      ~AboutDLG() { delete BT; };
      virtual void SetupWindow();
      virtual void WMPaint(TMessage &) = [WM_FIRST+WM_PAINT];
};

class TitleDLG : public TDialog
{
   private :
      TITLESTRUCT *ts;

   public  :
      TitleDLG(PTWindowsObject AParent, TITLESTRUCT &TS) : TDialog(AParent, TITLE)
      { ts = &TS; };
      ~TitleDLG() {};
      virtual void SetupWindow();
      virtual void Ok(RTMessage Msg)     = [ID_FIRST + IDOK];
      virtual void Cancel(RTMessage Msg) = [ID_FIRST + IDCANCEL] { CloseWindow(FALSE); };
};

class CalibAddDLG : public TDialog
{
   private :
      MASSSTRUCT *ms;
      CHAR       Mass[16];

   public  :
      CalibAddDLG(PTWindowsObject PTWO, MASSSTRUCT &MS) : TDialog(PTWO, CALIBADD)
      { ms = &MS; };
      ~CalibAddDLG() {};
      virtual void SetupWindow();
      virtual void Ok(RTMessage Msg)     = [ID_FIRST + IDOK];
      virtual void Cancel(RTMessage Msg) = [ID_FIRST + IDCANCEL] { CloseWindow(FALSE); };
};

class CalibInsDLG : public TDialog
{
   private :
      COEFFSTRUCT *cs;
      Bitmap      *BT;
      BOOL        *MultiOX;

   public  :
      CalibInsDLG(PTWindowsObject PTWO, COEFFSTRUCT &CS, BOOL &AMultiOX) : TDialog(PTWO, CALIBINS)
      { cs = &CS; MultiOX = &AMultiOX; };
      ~CalibInsDLG() {  delete BT; };
      virtual void SetupWindow();
      virtual void WMPaint(TMessage &) = [WM_FIRST+WM_PAINT];
      virtual void Ok(RTMessage Msg)     = [ID_FIRST + IDOK];
      virtual void Cancel(RTMessage Msg) = [ID_FIRST + IDCANCEL] { CloseWindow(FALSE); };
};

class CalibViewDLG : public TDialog
{
   private :
      COEFFSTRUCT *cs;
      Bitmap      *BT;

   public  :
      CalibViewDLG(PTWindowsObject PTWO, COEFFSTRUCT &CS) : TDialog(PTWO, CALIBVIEW)
      { cs = &CS; };
      ~CalibViewDLG() {  delete BT; };
      virtual void SetupWindow();
      virtual void WMPaint(TMessage &) = [WM_FIRST+WM_PAINT];
      virtual void Ok(RTMessage Msg)     = [ID_FIRST + IDOK] { CloseWindow(TRUE); };
};

class MinMaxDLG : public TDialog
{
   private :
      MINMAXSTRUCT *ms;

   public  :
      MinMaxDLG(PTWindowsObject PTWO, MINMAXSTRUCT &MS) : TDialog(PTWO, MINMAX)
      { ms = &MS; };
      ~MinMaxDLG() {};
      virtual void SetupWindow();
      virtual void Ok(RTMessage Msg)     = [ID_FIRST + IDOK];
      virtual void Cancel(RTMessage Msg) = [ID_FIRST + IDCANCEL] { CloseWindow(FALSE); };
};

class SameScaleDLG : public TDialog
{
   private :
      MINMAXSTRUCT *ms;

   public  :
      SameScaleDLG(PTWindowsObject PTWO, MINMAXSTRUCT &MS) : TDialog(PTWO, SAMESCALE)
      { ms = &MS; };
      ~SameScaleDLG() {};
      virtual void SetupWindow();
      virtual void Ok(RTMessage Msg)     = [ID_FIRST + IDOK];
      virtual void Cancel(RTMessage Msg) = [ID_FIRST + IDCANCEL] { CloseWindow(FALSE); };
};

class Transfer9350DLG : public TDialog
{
   private :
      int *mem;

   public  :
      Transfer9350DLG(PTWindowsObject PTWO, int &Mem) : TDialog(PTWO, TRANSFER9350)
      { mem = &Mem; };
      ~Transfer9350DLG() {};
      virtual void SetupWindow();
      virtual void Ok(RTMessage Msg)     = [ID_FIRST + IDOK];
      virtual void Cancel(RTMessage Msg) = [ID_FIRST + IDCANCEL] { CloseWindow(FALSE); };
};

class VisualAreaDLG : public TDialog
{
   private :
      AREA    *a;
      Bitmap  *BT[8];

   public  :
      VisualAreaDLG(PTWindowsObject PTWO, AREA &A) : TDialog(PTWO, VISUAREA)
      {  a = &A;  };
      ~VisualAreaDLG();
      virtual void SetupWindow();
      virtual void WMPaint(TMessage &) = [WM_FIRST+WM_PAINT];
};

class ExpConditionDLG : public TDialog
{
   private :
      EXPERIMENTINFO *es;

   public  :
      ExpConditionDLG(PTWindowsObject PTWO, EXPERIMENTINFO *EX) : TDialog(PTWO, EXPCONDITION)
      {	 es = EX;  };
      ~ExpConditionDLG() {};
      virtual void SetupWindow();
      virtual void Ok(RTMessage) = [ID_FIRST+IDOK];
};

class InputCTextDLG : public InputDLG
{
   private :
      REAL  tof;

   public  :
      InputCTextDLG(PTWindowsObject, LPSTR, LPSTR, LPSTR, UINT, REAL);
      ~InputCTextDLG() {};
      virtual void AddTOF(RTMessage Msg) = [ID_FIRST + ID_TOF];
      virtual void AddMass(RTMessage Msg) = [ID_FIRST + ID_MASS];
};

class InsertCTextDLG : public TDialog
{
   private :
      CTEXTSTRUCT *cs;

   public  :
      InsertCTextDLG(PTWindowsObject PTWO, CTEXTSTRUCT &CS) : TDialog(PTWO, INSERTCTEXT)
      { cs = &CS; };
      ~InsertCTextDLG() {};
      virtual void Ok(RTMessage Msg)     = [ID_FIRST + IDOK];
      virtual void Cancel(RTMessage Msg) = [ID_FIRST + IDCANCEL] { CloseWindow(FALSE); };
};

class EditCTextDLG : public TDialog
{
   private :
      CURSORTEXT FAR *te;
      SPECTRUMINFO FAR *sp;

      PTListBox List;
      PTStatic  Time;
      PTEdit    CText;
      BOOL      SelectedOX;
      BOOL      ChangeOX;
      SHORT     Num;

   public  :
      EditCTextDLG(PTWindowsObject, CURSORTEXT FAR*, SPECTRUMINFO FAR *);
      ~EditCTextDLG() {};
      virtual void SetupWindow();
      virtual void Ok(RTMessage Msg) = [ID_FIRST + IDOK];
      void IDCTextSelected()         = [ID_FIRST + ID_CTEXTLIST];
      void IDModify()                = [ID_FIRST + ID_MODIFY];
      void IDDelete()                = [ID_FIRST + ID_DELETE];
};

class EditCalibDLG : public TDialog
{
   private :
      CALIBTABLE FAR *ct;
      SPECTRUMINFO FAR *sp;

      PTListBox List;
      PTStatic  Time;
      PTEdit    Mass;
      BOOL      SelectedOX;
      BOOL      ChangeOX;
      SHORT     Num;

      REAL  DrawX;
      REAL  DrawY;
      REAL  DrawW;
      REAL  DrawH;
      REAL  MinTime;
      REAL  MaxTime;
      REAL  MinMass;
      REAL  MaxMass;    

   public  :
      EditCalibDLG(PTWindowsObject, CALIBTABLE FAR*, SPECTRUMINFO FAR *);
      ~EditCalibDLG() {};
      virtual void SetupWindow();
      virtual void WMPaint()         = [WM_FIRST + WM_PAINT];
      virtual void Ok(RTMessage Msg) = [ID_FIRST + IDOK];
      void IDCalibSelected()         = [ID_FIRST + ID_CALIBLIST];
      void IDModify()                = [ID_FIRST + ID_MODIFY];
      void IDDelete()                = [ID_FIRST + ID_DELETE];
      void GetMinMax();
      void RecalculCoeff();
      void Repaint(BOOL);
      void DrawDataPoint(HDC);
      void DrawFunction(HDC);
      void HilitePoint(HDC, SHORT);
};

class BKSubtractDLG : public TDialog
{
   private :
      BKSUBSTRUCT *bk;
      Bitmap *Bmp1;
      Bitmap *Bmp2;
      Bitmap *Bmp3;
      Bitmap *Bmp[6];

   public  :
      BKSubtractDLG(PTWindowsObject, BKSUBSTRUCT FAR*);
      ~BKSubtractDLG();
      virtual void SetupWindow();
      virtual void WMPaint()             = [WM_FIRST + WM_PAINT];
      virtual void Ok(RTMessage Msg)     = [ID_FIRST + IDOK];
      virtual void Cancel(RTMessage Msg) = [ID_FIRST + IDCANCEL] { CloseWindow(FALSE); };
};

class SpectrumSumDLG : public TDialog
{
   private :
      SUMSTRUCT *ss;
      CHAR Title[80];

   public  :
      SpectrumSumDLG(PTWindowsObject, LPSTR, SUMSTRUCT FAR*);
      ~SpectrumSumDLG();
      virtual void SetupWindow();
      virtual void Ok(RTMessage Msg)     = [ID_FIRST + IDOK];
      virtual void Cancel(RTMessage Msg) = [ID_FIRST + IDCANCEL] { CloseWindow(FALSE); };
};

class GPIBConnectDLG : public TDialog
{
   private :
      GPIBSTRUCT *gs;

   public  :
      GPIBConnectDLG(PTWindowsObject, GPIBSTRUCT FAR &);
      ~GPIBConnectDLG();
      virtual void SetupWindow();
      virtual void Ok(RTMessage Msg)     = [ID_FIRST + IDOK];
      virtual void Cancel(RTMessage Msg) = [ID_FIRST + IDCANCEL] { CloseWindow(FALSE); };
};

class CountDLG : public TDialog
{
   private :
      COUNTSTRUCT *ts;

   public  :
      CountDLG(PTWindowsObject AParent, COUNTSTRUCT &TS) : TDialog(AParent, COUNT)
      { ts = &TS; };
      ~CountDLG() {};
      virtual void SetupWindow();
      virtual void Ok(RTMessage Msg)     = [ID_FIRST + IDOK];
      virtual void Cancel(RTMessage Msg) = [ID_FIRST + IDCANCEL] { CloseWindow(FALSE); };
};

class CancelDLG : public TDialog
{
public:
	BOOL CancelOX;

	CancelDLG(PTWindowsObject, LPSTR);
	~CancelDLG();

	virtual void WMInitDialog(RTMessage) = [WM_FIRST + WM_INITDIALOG];
	virtual void WMCommand(RTMessage) = [WM_FIRST + WM_COMMAND];

	virtual void SetText(char *);
	virtual void SetDone(char *);

	BOOL IsCancelPressed();
	
};

#endif