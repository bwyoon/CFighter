#ifndef __CFT_H
#define __CFT_H

#include <owl.h>
#include <mdi.h>
#include <stdio.h>
#include <print.h>
#include <ddeml.h>
#include "cftdef.h"
#include "cfttype.h"
#include "cftctrl.h"
#include "gpibw.h"

_CLASSDEF(CFTFrame)
class CFTFrame : public TMDIFrame
{
   public:
      SHORT  Dollar;
      SHORT  N_SPECWIN;
      BOOL   IsFirst;
      BOOL   GPIBOX;
      PTWindowsObject *SpecWin;
      DWORD OpenFileFilterIndex;
      DWORD SaveFileFilterIndex;
      PLOTSTRUCT PS;
      COLORREF COLOR[MAX_SPEC];
      CHAR  *InitFileName;
      CHAR  *NormalFont;
      CHAR  *SymbolFont;
      CHAR  *DriverName;
      CHAR  *DeviceName;
      CHAR  *OutputName;
      DEVMODE *DevMode;
      HGLOBAL hgSP, hgPL, hgEX, hgWD, hgIP, hgEXSTD;
      HCURSOR WaitCursor;
      HCURSOR NormalCursor;
      HCURSOR CrossCursor;
      SPECTRUMINFO100 FAR *SP;
      PLOTINFO100     FAR *PL;
      EXPINFO100      FAR *EX;
      WAVEDESC        FAR *WD;
      IPNLAB          FAR *IP;
      EXPERIMENTINFO  FAR *EXSTD;
      GPIB     *TR;
      HIDEPLOT  HP;

      DWORD   idInst;
      HCONV   hConv;
      HSZ     hszService;
      HSZ     hszTopic;
      HSZ     hszItem[MAX_DDETOPIC];
      FARPROC lpfnCallBack;

      CFTFrame(LPSTR, LPSTR);
      ~CFTFrame();
      virtual void SetupWindow();
      virtual void GetWindowClass(WNDCLASS&);
      virtual LPSTR GetClassName() { return "CFIGHTER110"; };
      virtual BOOL CanClose();
      virtual void WMPaint(RTMessage) = [WM_FIRST+WM_PAINT];
      void CMNewWindow(RTMessage)     = [CM_FIRST+CM_NEWWND]
      { CreateSpectrumWindow(); };
      void CMPrintSetup(RTMessage)    = [CM_FIRST+CM_PRINTSETUP];     
      void CMQuit(RTMessage)          = [CM_FIRST+CM_QUIT]
      { CloseWindow(); };
      void CMGPIBConnect(RTMessage)   = [CM_FIRST+CM_TCONNECT];
      void CMCalculFragment(RTMessage)= [CM_FIRST+CM_CALCULFRAG];
      void CMCFTHelp(RTMessage)       = [CM_FIRST+CM_CFTHELP];
      void CMHelpHelp(RTMessage)      = [CM_FIRST+CM_HELPHELP];
      void CMAbout(RTMessage)         = [CM_FIRST+CM_ABOUT];
      void BeginFrame(HDC, RECT);
      void CreateSpectrumWindow();
      void InitPlotStruct();
      void GetDefaultPrinter();
      void FreeMemory();

      BOOL SetupDDE();
      void QuitDDE();
      SHORT WhichTopicAndService(HSZ, HSZ);
      SHORT WhichTopicAndItem(HSZ, HSZ);
      HDDEDATA WildConnect(HSZ, HSZ, WORD);
      HDDEDATA DDEAnswerForRequest(WORD, SHORT);
      void DDEPoke(HDDEDATA, SHORT);
      static HDDEDATA FAR PASCAL _export CallBack( WORD, WORD, HCONV, HSZ, HSZ, HDDEDATA, DWORD, DWORD );
};

class CFTApp : public TApplication
{
   public:
      CFTApp(LPSTR AName, HINSTANCE hInst, HINSTANCE hPrevInstance,
	     LPSTR lpCmdLine, int nCmdShow)
	    : TApplication(AName, hInst, hPrevInstance, lpCmdLine, nCmdShow) {};
      virtual void InitInstance();
      virtual void InitMainWindow();
};

_CLASSDEF(CFTWindow)
class CFTWindow : public TWindow
{
   public  :
      WINTYPE WinType;

      CFTWindow(PTWindowsObject PTWO, LPSTR ATitle, WINTYPE wintype) : TWindow(PTWO, ATitle) {};
      ~CFTWindow() {};
      virtual void GetWindowClass(WNDCLASS &AWndClass) { TWindow::GetWindowClass(AWndClass); };
};

_CLASSDEF(SpectrumWindow)
class SpectrumWindow : public CFTWindow
{
   public:
      int  NWIN;
      int  SPEC;
      BOOL SaveOX[MAX_SPEC];
      BOOL LoadOX[MAX_SPEC];
      BOOL SelectOX[MAX_SPEC];
      BOOL AllocOX[MAX_SPEC];
      BOOL OverlayOX;
      BOOL LBDownOX;
      BOOL AutoZoomOX;
      int  Xini, Yini, Xfin, Yfin;
      CURSORMODE CursorMode[MAX_SPEC];
      LONG CursorPos[MAX_SPEC][3];
      FILETYPE FileType[MAX_SPEC];
      PYCheckBox  MemCHK[MAX_SPEC];
      PYCheckBox OverlayCHK;
      PYStatic   SpecNo;
      PYStatic   CCValue;
      HGLOBAL hgY[MAX_SPEC],hgSP[MAX_SPEC], hgPL[MAX_SPEC], hgEX[MAX_SPEC], hgTE[MAX_SPEC];
      HGLOBAL hgCT[MAX_SPEC], hgMK[MAX_SPEC], hgXY;
      SHORT          HUGE *Y[MAX_SPEC];
      SPECTRUMINFO   FAR  *SP[MAX_SPEC];
      PLOTINFO       FAR  *PL[MAX_SPEC];
      EXPERIMENTINFO FAR  *EX[MAX_SPEC];
      CURSORTEXT     FAR  *TE[MAX_SPEC];
      CALIBTABLE     FAR  *CT[MAX_SPEC];
      LONG           FAR  *MK[MAX_SPEC];
      POINTXY        FAR  *XY;
      
      DWORD          Size[MAX_SPEC];
      FILENAMESTRUCT *FN;
      PLOTSTRUCT PS;

      SpectrumWindow(PTWindowsObject, LPSTR);
      ~SpectrumWindow();
      virtual void SetupWindow();
      virtual void GetWindowClass(WNDCLASS&);
      virtual LPSTR GetClassName() { return "SPECTRUM"; };
      virtual BOOL CanClose();
      virtual void WMPaint(RTMessage) = [WM_FIRST+WM_PAINT];
      virtual void WMLButtonDown(RTMessage) = [WM_FIRST+WM_LBUTTONDOWN];
      virtual void WMLButtonUp(RTMessage) = [WM_FIRST+WM_LBUTTONUP];
      void WMMouseMove(RTMessage)     = [WM_FIRST+WM_MOUSEMOVE];
      void CMPrint(RTMessage)         = [CM_FIRST+CM_PRINT];
      void CMPrintOnFile(RTMessage)   = [CM_FIRST+CM_PRINTONFILE];
      void CMLoad(RTMessage)          = [CM_FIRST+CM_LOAD];
      void CMSave(RTMessage)          = [CM_FIRST+CM_SAVE];
      void CMSaveAs(RTMessage)        = [CM_FIRST+CM_SAVEAS];
      void CMSaveAll(RTMessage)       = [CM_FIRST+CM_SAVEALL];
      void CMClearSpectrum(RTMessage) = [CM_FIRST+CM_CLEARSPEC];
      void CMTransfer9350(RTMessage)  = [CM_FIRST+CM_TRANSFER9350];
      void CMAccumulation9350(RTMessage)  = [CM_FIRST+CM_COUNTACC];
      void CMSaveAsAscii(RTMessage)   = [CM_FIRST+CM_SPECASCII];
      void CMSaveAsQPRO(RTMessage)    = [CM_FIRST+CM_QPRO];
      void CMSaveCalib(RTMessage)     = [CM_FIRST+CM_CALIBASCII];
      void CMTitle(RTMessage)         = [CM_FIRST+CM_TITLE];
      void CMCalibAdd(RTMessage)      = [CM_FIRST+CM_CALIBADD];
      void CMCalibIns(RTMessage)      = [CM_FIRST+CM_CALIBINS];
      void CMCalibView(RTMessage)     = [CM_FIRST+CM_CALIBVIEW];
      void CMClearCalib(RTMessage)    = [CM_FIRST+CM_CALIBNEW];
      void CMEditCalib(RTMessage)     = [CM_FIRST+CM_CALIBDRAW];
      void CMTotalSpectrum(RTMessage) = [CM_FIRST+CM_TOTALSPEC];
      void CMZoomAuto(RTMessage)      = [CM_FIRST+CM_ZOOMAUTO];
      void CMZoomMan(RTMessage)       = [CM_FIRST+CM_ZOOMMAN];
      void CMZoomAll(RTMessage)       = [CM_FIRST+CM_ZOOMALL];
      void CMInverse(RTMessage)       = [CM_FIRST+CM_INVERSE];
      void CMScaleMan(RTMessage)      = [CM_FIRST+CM_SCALEMAN];
      void CMAutoScale(RTMessage)     = [CM_FIRST+CM_AUTOSCALE];
      void CMSameScale(RTMessage)     = [CM_FIRST+CM_SAMESCALE];
      void CMVisualArea(RTMessage)    = [CM_FIRST+CM_VISUALAREA];
      void CMExpCondition(RTMessage)  = [CM_FIRST+CM_EXPCONDITION];
      void CMBKSubtract(RTMessage)    = [CM_FIRST+CM_BKSUBTRACT];
      void CMSpectrumSum(RTMessage)   = [CM_FIRST+CM_SPECSUM];
      void CMLSSmoothing(RTMessage)   = [CM_FIRST+CM_LSSMOOTH];
      void CMYShiftUp(RTMessage)      = [CM_FIRST+CM_YSHIFTUP]    { YShift(SY_UP);    };
      void CMYShiftDown(RTMessage)    = [CM_FIRST+CM_YSHIFTDOWN]  { YShift(SY_DOWN);  };
      void CMXShiftLeft(RTMessage)    = [CM_FIRST+CM_XSHIFTLEFT]  { XShift(SX_LEFT);  };
      void CMXShiftRight(RTMessage)   = [CM_FIRST+CM_XSHIFTRIGHT] { XShift(SX_RIGHT); };
      void CMYShiftUpFast(RTMessage)    = [CM_FIRST+CM_FYSHIFTUP]    { YShiftFast(SY_UP);    };
      void CMYShiftDownFast(RTMessage)  = [CM_FIRST+CM_FYSHIFTDOWN]  { YShiftFast(SY_DOWN);  };
      void CMXShiftLeftFast(RTMessage)  = [CM_FIRST+CM_FXSHIFTLEFT]  { XShiftFast(SX_LEFT);  };
      void CMXShiftRightFast(RTMessage) = [CM_FIRST+CM_FXSHIFTRIGHT] { XShiftFast(SX_RIGHT); };
      void CMScaleUp(RTMessage)       = [CM_FIRST+CM_SCALEUP]    { ScaleUpDown(SC_UP);   };
      void CMScaleDown(RTMessage)     = [CM_FIRST+CM_SCALEDOWN]  { ScaleUpDown(SC_DOWN); };
      void CMScaleUpFast(RTMessage)   = [CM_FIRST+CM_FSCALEUP]   { ScaleUpDownFast(SC_UP);   };
      void CMScaleDownFast(RTMessage) = [CM_FIRST+CM_FSCALEDOWN] { ScaleUpDownFast(SC_DOWN); };
      void CMZoom(RTMessage)          = [CM_FIRST+CM_ZOOM];
      void CMUnzoom(RTMessage)        = [CM_FIRST+CM_UNZOOM];
      void CMZoomFast(RTMessage)      = [CM_FIRST+CM_FZOOM];
      void CMUnzoomFast(RTMessage)    = [CM_FIRST+CM_FUNZOOM];
      void CMCCLeft(RTMessage)        = [CM_FIRST+CM_CC_SLEFT];
      void CMCCRight(RTMessage)       = [CM_FIRST+CM_CC_SRIGHT];
      void CMCCLeftFast(RTMessage)    = [CM_FIRST+CM_CC_FLEFT];
      void CMCCRightFast(RTMessage)   = [CM_FIRST+CM_CC_FRIGHT];
      void CMPutCursorText(RTMessage) = [CM_FIRST+CM_PUTCURSORTEXT];
      void CMCursorTextBis(RTMessage) = [CM_FIRST+CM_CURSORTEXTBIS];
      void CMInsertCText(RTMessage)   = [CM_FIRST+CM_INSERTCTEXT];
      void CMAddMark(RTMessage)       = [CM_FIRST+CM_ADDMARK];
      void CMClearMark(RTMessage)     = [CM_FIRST+CM_CLEARMARK];
      void CMEditCText(RTMessage)     = [CM_FIRST+CM_EDITCTEXT];
      void CMClearAllCText(RTMessage) = [CM_FIRST+CM_CLEARALLCTEXT];
      void CMOverlay(RTMessage)       = [CM_FIRST+CM_OVERLAY];
      void CMMem1(RTMessage)          = [CM_FIRST+CM_MEM1] { ChangeMem(0); };
      void CMMem2(RTMessage)          = [CM_FIRST+CM_MEM2] { ChangeMem(1); };
      void CMMem3(RTMessage)          = [CM_FIRST+CM_MEM3] { ChangeMem(2); };
      void CMMem4(RTMessage)          = [CM_FIRST+CM_MEM4] { ChangeMem(3); };
      void CMMem5(RTMessage)          = [CM_FIRST+CM_MEM5] { ChangeMem(4); };
      BOOL AllocateMemory(SHORT, DWORD);
      BOOL UpdateMemory(SHORT, DWORD); 
      void FreeMemory(SHORT);
      void Repaint(BOOL);
      BOOL UpdatePlotInfo(SHORT);
      void DefaultSpectrumHead(SHORT);
      void GetDefaultSpectrumMinMax(SHORT);
      void GetPlotMinMax(SHORT);
      void GetDefaultPlotInfo(SHORT);
      void GetDevicePlotStruct();
      void PlotSpectrum(HDC, SHORT);
      void PlotGrid(HDC, SHORT, DEVICETYPE);
      void PlotTitle(HDC, SHORT, DEVICETYPE);
      void ClearVariables(SHORT);
      BOOL GetOpenFile(FILENAMESTRUCT &);
      BOOL GetSaveFile(FILENAMESTRUCT &);
      BOOL GetCFighter(HFILE, SHORT);
      BOOL GetLeCroy9350(HFILE, SHORT);
      BOOL GetLeCroy9400(FILE *, SHORT);
      BOOL GetIPN(HFILE, SHORT);
      BOOL GetCFT100(HFILE, SHORT);
      BOOL GetSpectrum(SHORT, LPSTR);
      BOOL SaveCFighter(HFILE, SHORT);
      BOOL SaveAsSpecial(FILE *, SHORT, UINT);
      BOOL SpecialSave(SHORT, UINT);
      void ChangeMem(SHORT);
      void YShift(YSHIFTMODE);
      void XShift(XSHIFTMODE);
      void YShiftFast(YSHIFTMODE);
      void XShiftFast(XSHIFTMODE);
      void ScaleUpDown(SCALEMODE);
      void ScaleUpDownFast(SCALEMODE);
      void InitialCursorState(SHORT);
      void DrawCursor(HDC, SHORT, WHICHCURSOR, BOOL);
      void DrawMark(HDC, SHORT, SHORT);
      void RedrawCursor(SHORT, WHICHCURSOR, LONG);
      void CursorTextOut(HDC, SHORT, SHORT, UINT, DEVICETYPE);
      void DrawCursorText(SHORT, SHORT);
      BOOL InsertCursorText(SHORT, REAL, LPSTR);
      BOOL DeleteCursorText(SHORT, SHORT);
      void SetCursorPosition(SHORT, REAL);
      BOOL LeastSquareFit(LONG, REAL HUGE *, REAL HUGE *, REAL *);
      BOOL CalculCalibCoeff(SHORT);
      REAL CalculMass(SHORT, LONG);
      void CalculateArea(SHORT, AREA *);
      BOOL AllocMemoryForFit();
      BOOL SetFitMemory(SHORT);
      REAL ChiSquare(SHORT, FITCOEFF, SHORT, FITMODE);
      BOOL AlphaBeta(SHORT, FITCOEFF, SHORT, FITMODE, MATRIX, FITCOEFF);
      BOOL SearchBackground(SHORT, FITCOEFF, FITMODE);
      void ScalingAfterBKSubtraction(SHORT, FITCOEFF, FITMODE, REAL &, REAL &);
      BOOL WriteBKSubSpectrum(SHORT, SHORT, FITCOEFF, FITMODE, REAL, REAL);
      SHORT SearchEmptyMemory();
      BOOL WriteSpectrumSum(SUMSTRUCT);
      BOOL WriteLSSmoothing(SUMSTRUCT);
      BOOL Transfer9350(SHORT, SHORT);
      BOOL CountBinary9350(SHORT, SHORT);
      BOOL Transfer9350Spectrum(SHORT, SHORT, SHORT HUGE *, int);
      BOOL Transfer9350WaveDesc(SHORT, SHORT, int);
      BOOL SaveAsCFighter(SHORT);
      BOOL IsSaved(SHORT, BOOL);
};

#endif