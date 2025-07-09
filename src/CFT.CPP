/*
    CFighter version 1.10
    Time of Flight Mass Spectrum Analyzer

    YOON Bokwon
    Laboratoire Aimé COTTON
    Bât. 505  Campus d'Orsay
    91405 Orsay  France
*/

#include "cft.h"
#include "cftdlg.h"
#include "cftctrl.h"
#include "cftfunc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dir.h>
#include <math.h>
#include <mem.h>
#include <commdlg.h>
#include <print.h>

/* Constructor of class CFTFrame */
CFTFrame::CFTFrame(LPSTR ATitle, LPSTR Menu) : TMDIFrame(ATitle, Menu)
{
      if (!SetupDDE()) 
	 CFTMessage(this, IDS_DDEERRORTEXT, IDS_DDEERRORTITLE, MB_OK);

      IsFirst = TRUE;

      Dollar = 0;
      N_SPECWIN = 0;
      OpenFileFilterIndex = SaveFileFilterIndex = 0L;
      InitFileName = new CHAR[256];
      SpecWin      = new PTWindowsObject[MAX_CHILD];
      NormalFont   = new CHAR[LF_FACESIZE];
      SymbolFont   = new CHAR[LF_FACESIZE];
      DriverName   = new CHAR[40];
      DeviceName   = new CHAR[40];
      OutputName   = new CHAR[40];
      DevMode      = new DEVMODE;
      GetDefaultPrinter();
      lstrcpy(NormalFont, "Arial");
      lstrcpy(SymbolFont, "Symbol");
      InitPlotStruct();
      WaitCursor   = LoadCursor(NULL, IDC_WAIT);
      NormalCursor = LoadCursor(NULL, IDC_ARROW);
      CrossCursor  = LoadCursor(NULL, IDC_CROSS);

      TR       = NULL;
      GPIBOX   = FALSE;
      hgSP = hgPL = hgEX = hgWD = hgIP = NULL;
      BOOL OX = FALSE;
      if ((hgSP = AllocHGlobal(GPTR, (DWORD)sizeof(SPECTRUMINFO100))) != NULL) { 
         SP = (SPECTRUMINFO100 FAR *)GlobalLock(hgSP);
      } else {	 OX = TRUE;  goto STOP; }

      if ((hgPL = AllocHGlobal(GPTR, (DWORD)sizeof(PLOTINFO100))) != NULL) {
          PL = (PLOTINFO100 FAR *)GlobalLock(hgPL);
      } else {	 OX = TRUE;  goto STOP; }

      if ((hgEX = AllocHGlobal(GPTR, (DWORD)sizeof(EXPINFO100))) != NULL) {
          EX = (EXPINFO100 FAR *)GlobalLock(hgEX);
      } else {	 OX = TRUE;  goto STOP; }

      if ((hgEXSTD = AllocHGlobal(GPTR, (DWORD)sizeof(EXPERIMENTINFO))) != NULL) {
          EXSTD = (EXPERIMENTINFO FAR *)GlobalLock(hgEXSTD);
      } else {	 OX = TRUE;  goto STOP; }
      memset(EXSTD, 0, sizeof(EXPERIMENTINFO));

      if ((hgWD = AllocHGlobal(GPTR, (DWORD)sizeof(WAVEDESC))) != NULL) {
          WD = (WAVEDESC FAR *)GlobalLock(hgWD);
      } else {	 OX = TRUE;  goto STOP; }

      if ((hgIP = AllocHGlobal(GPTR, (DWORD)sizeof(IPNLAB))) != NULL) {
	  IP = (IPNLAB FAR *)GlobalLock(hgIP);
      } else {	 OX = TRUE;  goto STOP; }

      STOP :
      HFILE hf;
      GetWindowsDirectory(InitFileName, 240);
      if (strlen(InitFileName) == 3) lstrcat(InitFileName, CFT110INI);
      else {
	 lstrcat(InitFileName, "\\");
	 lstrcat(InitFileName, CFT110INI);
      }

      if ((hf = _lopen(InitFileName, READ)) != HFILE_ERROR) {
	 _lread(hf, EXSTD, sizeof(EXPERIMENTINFO));
         _lclose(hf);
      }

      if (OX) {
	 CFTMessage(this, IDS_FATALERRORTEXT, IDS_FATALERRORTITLE, MB_OK);
      }
}

/* Destructor of the clas CFTFrame */
CFTFrame::~CFTFrame()
{     
}

/* Free the global memory objects in the class CFTFrame */
void CFTFrame::FreeMemory()
{
     FreeHGlobal(hgSP);
     FreeHGlobal(hgPL);
     FreeHGlobal(hgEX);
     FreeHGlobal(hgEXSTD);
     FreeHGlobal(hgWD);
     FreeHGlobal(hgIP);
}

/* Setup CFTFrame MDI frame */
void CFTFrame::SetupWindow()
{
     TMDIFrame::SetupWindow();
     CreateSpectrumWindow();
}

/* Redefine Window Class of CFTFrame */ 
void CFTFrame::GetWindowClass(WNDCLASS& AWndClass)
{
     TMDIFrame::GetWindowClass(AWndClass);
     AWndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
     AWndClass.hIcon = LoadIcon(GetApplication()->hInstance, "CFighterIcon");
}

/* Close CFTFrame or not */
BOOL CFTFrame::CanClose()
{
     HFILE    hf;
     OFSTRUCT of;
     if ((hf = OpenFile(InitFileName, &of, OF_CREATE)) != HFILE_ERROR) {
        _lwrite(hf, EXSTD, sizeof(EXPERIMENTINFO));
        _lclose(hf);
     }
     BOOL OX = CloseChildren();
     if (OX) {    
        delete InitFileName;
        delete SpecWin;
        delete NormalFont;
        delete SymbolFont;
        delete DriverName;
        delete DeviceName;
        delete OutputName;
        delete DevMode;
        FreeMemory();
	QuitDDE();
	char str[256];
	sprintf(str, "You analyzed %d spectra.\nSo you should pay %d U.S. dollars to Bokwon YOON.\n"
		     "OK ?", Dollar, Dollar);
	MessageBox(HWindow, str, "Attention", MB_OK | MB_ICONEXCLAMATION);
     }
     return OX;
}     

/* Respose to the WM_PAINT message */
void CFTFrame::WMPaint(RTMessage)
{
     PAINTSTRUCT ps;
     RECT        rc;

     GetClientRect(HWindow, &rc);
     BeginPaint(HWindow, &ps);
     SaveDC(ps.hdc);

     if (IsFirst) BeginFrame(ps.hdc, rc);

     RestoreDC(ps.hdc, -1);
     EndPaint(HWindow, &ps);
}

/* Create child spectrum window */
void CFTFrame::CreateSpectrumWindow()
{
     CHAR str[25];
     CHAR Fmt[80];
     LoadString(GetApplication()->hInstance, IDS_SPECWINNAME, Fmt, 79);
     sprintf(str, Fmt, N_SPECWIN+1);
     GetApplication()->MakeWindow(new SpectrumWindow(this, str));
}

/* Configuration of printer */
void CFTFrame::CMPrintSetup(RTMessage)
{
     PRINTDLG     pd;
     LPDEVNAMES   DevNames;
     DEVMODE FAR *DM;

     memset(&pd, 0, sizeof(PRINTDLG));

     pd.lStructSize = sizeof(PRINTDLG);
     pd.hwndOwner   = HWindow;
     pd.Flags       = PD_PRINTSETUP;

     if (PrintDlg(&pd)) {
	if (pd.hDevMode  != NULL)  {
	   DM = (DEVMODE FAR *)GlobalLock(pd.hDevMode);
	   memcpy(DevMode, DM, sizeof(DEVMODE));
	   FreeHGlobal(pd.hDevMode);
	}
	if (pd.hDevNames != NULL) {
	   DevNames = (LPDEVNAMES)GlobalLock(pd.hDevNames);
	   lstrcpy(DriverName, (LPSTR)DevNames+DevNames->wDriverOffset);
	   lstrcpy(DeviceName, (LPSTR)DevNames+DevNames->wDeviceOffset);
	   lstrcpy(OutputName, (LPSTR)DevNames+DevNames->wOutputOffset);
	   FreeHGlobal(pd.hDevNames);
	}
     }
}

/* run SURFACE.EXE */
void CFTFrame::CMCalculFragment(RTMessage)
{
     if (WinExec("SURFACE.EXE", SW_SHOWNORMAL) < 32)
	MessageBox(HWindow, "Echec dans le lancement de SURFACE !", "Erreur", MB_ICONSTOP);
}

/* About this program */
void CFTFrame::CMAbout(RTMessage)
{
     GetApplication()->ExecDialog(new AboutDLG(this));
}

/* Help of CFighter */
void CFTFrame::CMCFTHelp(RTMessage)
{
}

/* Help on help */
void CFTFrame::CMHelpHelp(RTMessage)
{
     WinHelp( HWindow, "WINHELP.HLP", HELP_HELPONHELP, 0L );
}

/* initialize plot structure */
void CFTFrame::InitPlotStruct()
{
     PS.Rect.left   = 0;
     PS.Rect.top    = 0;
     PS.Rect.right  = 32500;
     PS.Rect.bottom = 23000;
     PS.Date.x      = 300;
     PS.Date.y      = 300;
     PS.Element.x   = 300;
     PS.Element.y   = 1100;
     PS.Plot.left   = 4000;
     PS.Plot.top    = 1500;
     PS.Plot.right  = 31500;
     PS.Plot.bottom = 20700;
     PS.Title.x = 4000;
     PS.Title.y = 300;
     PS.fName.x = 31500;
     PS.fName.y = 300;
     PS.xUnit.x = 18000;
     PS.xUnit.y = 22000;
     PS.yUnit.x = 300;
     PS.yUnit.y = 10000;
     PS.xGridW  = 300;
     PS.yGridW  = 300;
     PS.xValue  = 21000;
     PS.yValue  = 3600;
     PS.PlotThick  = 13;
     PS.FrameThick = 40;
     PS.GridThick  = 27;
     PS.FontHeight = 600;

     
     COLOR[0] = RGB(0,0,128);
     COLOR[1] = RGB(192,0,0);
     COLOR[2] = RGB(0,128,0);
     COLOR[3] = RGB(128,0,0);
     COLOR[4] = RGB(128,128,128);
     
}

static Pchar FetchStr(Pchar& Str)
{
     Pchar result = Str;
     LPSTR tempStr = Str;

     if (Str == NULL) return result;
     while ((*tempStr != '\0') && (*tempStr != ','))
        tempStr = AnsiNext(tempStr);
     if (*tempStr != '\0') {
        *tempStr = '\0';
        tempStr++;
        while (*tempStr == ' ')
                tempStr = AnsiNext(tempStr);
     }
     Str += int(tempStr - (LPSTR) Str);
     return result;
}

/* retreive defalt printer */
void CFTFrame::GetDefaultPrinter()
{
     CHAR Printer[80];
     Pchar Cur;

     GetProfileString("windows", "device", "", Printer, sizeof(Printer) - 1);
     Cur = Printer;
     lstrcpy(DeviceName, FetchStr(Cur));
     lstrcpy(DriverName, FetchStr(Cur));
     lstrcpy(OutputName, FetchStr(Cur));
}

/* Constructor of the class SpectrumWindow */
SpectrumWindow::SpectrumWindow(PTWindowsObject AParent, LPSTR ATitle)
          : CFTWindow(AParent, ATitle, SPECTRUMWIN)
{
     CHAR str[3];

     FN = new FILENAMESTRUCT;
     for (int i = 0; i < MAX_SPEC; i++) {
         sprintf(str, "%d", i+1);
	 MemCHK[i] = new YCheckBox(this, CM_MEM1+i, str, 40*i+9, 4, 30, 15, ((CFTFrame *)Parent)->COLOR[i], NULL);
     }
  
     CHAR Multiple[10];
     LoadString(GetApplication()->hInstance, IDS_MULTIPLE, Multiple, 9);
     OverlayCHK = new YCheckBox(this, CM_OVERLAY, Multiple, 213, 4, 70, 15, RGB(255, 0, 0), NULL);
     SpecNo = new YStatic(this, -1, "", 297, 4, 13, 15, 1);
     CCValue = new YStatic(this, -1, "", 332, 4, 288, 15, 58);
     
     NWIN = ((PCFTFrame)Parent)->N_SPECWIN;
     ((PCFTFrame)Parent)->N_SPECWIN++;

     SPEC  = 0;
     for (i = 0; i < MAX_SPEC; i++) { 
         LoadOX[i] = AllocOX[i] = SelectOX[i] = FALSE;
         SaveOX[i] = TRUE; 
	 Size[i]   = 0L;
         hgY[i]    = hgSP[i] = hgPL[i] = hgEX[i] = hgCT[i] = hgTE[i] = NULL;
     }

     hgXY           = NULL;
     SelectOX[SPEC] = TRUE;
     OverlayOX      = FALSE;
     LBDownOX       = FALSE;
     AutoZoomOX     = FALSE; 
}

/* Destructor of the class SpectrumWindow */
SpectrumWindow::~SpectrumWindow()
{
     
}

/* Allocate the memories for a spectrum */
BOOL SpectrumWindow::AllocateMemory(SHORT n, DWORD MSize)
{
     CHAR EText[80];
     CHAR ETitle[40];

     if ((hgY[n] = AllocHGlobal(GPTR, (DWORD)(MSize))) != NULL) {
         Y[n] = (SHORT HUGE *)GlobalLock(hgY[n]);
     } else goto STOP;

     if ((hgSP[n] = AllocHGlobal(GPTR, (DWORD)sizeof(SPECTRUMINFO))) != NULL) {
         SP[n] = (SPECTRUMINFO FAR *)GlobalLock(hgSP[n]);
     } else goto STOP;

     if ((hgPL[n] = AllocHGlobal(GPTR, (DWORD)sizeof(PLOTINFO))) != NULL) {
         PL[n] = (PLOTINFO FAR *)GlobalLock(hgPL[n]);
     } else goto STOP;

     if ((hgEX[n] = AllocHGlobal(GPTR, (DWORD)sizeof(EXPERIMENTINFO))) != NULL) {
         EX[n] = (EXPERIMENTINFO FAR *)GlobalLock(hgEX[n]);
     } else goto STOP;

     if ((hgCT[n] = AllocHGlobal(GPTR, (DWORD)sizeof(CALIBTABLE)*MAX_CALIB)) != NULL) {
         CT[n] = (CALIBTABLE FAR *)GlobalLock(hgCT[n]);
     } else goto STOP;

     if ((hgTE[n] = AllocHGlobal(GPTR, (DWORD)sizeof(CURSORTEXT)*MAX_CURSORTEXT)) != NULL) {
         TE[n] = (CURSORTEXT FAR *)GlobalLock(hgTE[n]);
     } else goto STOP;

     if ((hgMK[n] = AllocHGlobal(GPTR, (DWORD)sizeof(LONG)*MAX_MARK)) != NULL) {
         MK[n] = (LONG FAR *)GlobalLock(hgMK[n]);
     } else goto STOP;

     Size[n] = MSize;
     AllocOX[n] = TRUE;
     return TRUE;

     STOP :
     LoadString(GetApplication()->hInstance, IDS_MEMALLOCERRORTEXT, EText, 79);
     LoadString(GetApplication()->hInstance, IDS_MEMALLOCERRORTITLE, ETitle, 39);
     MessageBox(NULL, EText, ETitle, MB_OK);
     FreeMemory(n);
     return FALSE;
}

/* Setup spectrum window */
void SpectrumWindow::SetupWindow()
{
     TWindow::SetupWindow();
     MemCHK[SPEC]->Check();
     SpecNo->SetText("1");
}

/* Update the memory for the spectrum */
BOOL SpectrumWindow::UpdateMemory(SHORT n, DWORD MSize)
{
     BOOL OX = FALSE;

     if (!AllocOX[n]) {
        OX = AllocateMemory(n, MSize);
     } else {
	if (Size[n] < MSize) {
           FreeHGlobal(hgY[n]);
           Size[n] = 0L;
           if ((hgY[n] = AllocHGlobal(GPTR, (DWORD)(MSize))) != NULL) {
	      Y[n] = (SHORT HUGE *)GlobalLock(hgY[n]);
              Size[n] = MSize;
              OX = TRUE;
	   } else {
              CFTMessage(this, IDS_MEMALLOCERRORTEXT, IDS_MEMALLOCERRORTITLE, MB_OK | MB_ICONSTOP);
              FreeMemory(n);
              OX = FALSE;
           }
        } else  OX = TRUE;
     }

     return OX;
}

/* Free the memories  */
void SpectrumWindow::FreeMemory(SHORT n)
{
     FreeHGlobal(hgY[n]);
     FreeHGlobal(hgSP[n]);
     FreeHGlobal(hgPL[n]);
     FreeHGlobal(hgEX[n]);
     FreeHGlobal(hgCT[n]);
     FreeHGlobal(hgTE[n]);
     FreeHGlobal(hgMK[n]);
     AllocOX[n] = FALSE;
}

/* Redefine the Window Class */
void SpectrumWindow::GetWindowClass(WNDCLASS& AWndClass)
{
     CFTWindow::GetWindowClass(AWndClass);
     AWndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
     AWndClass.hIcon = LoadIcon(GetApplication()->hInstance, "SpectrumIcon");
}

/* Send painting message */
void SpectrumWindow::Repaint(BOOL OX)
{
     RECT rc;
     GetClientRect(HWindow, &rc);
     rc.top += 25;
     InvalidateRect(HWindow, &rc, OX);
}

/* Response to the WM_PAINT message */
void SpectrumWindow::WMPaint(RTMessage)
{
     HDC         hdc;
     PAINTSTRUCT pst;
     RECT        rect;
     SHORT       i, N = 0;

     GetClientRect(HWindow, &rect);
     PS.Rect.left   = rect.left;
     PS.Rect.top    = rect.top+25;
     PS.Rect.right  = rect.right;
     PS.Rect.bottom = rect.bottom;

     hdc = BeginPaint(HWindow, &pst);
     SaveDC(hdc);

     if (PS.Rect.top < PS.Rect.bottom) {
        GetDevicePlotStruct();
	SelectObject(hdc, GetStockObject(WHITE_PEN));
	SelectObject(hdc, GetStockObject(LTGRAY_BRUSH));
	Rectangle(hdc, rect.left, rect.top, rect.right+1, rect.top+23);
	SelectObject(hdc, GetStockObject(BLACK_PEN));
	MoveTo(hdc, rect.left+1, rect.top+22);
	LineTo(hdc, rect.right,  rect.top+22);
	LineTo(hdc, rect.right,  rect.top);
	SelectObject(hdc, GetStockObject(WHITE_PEN));
	MoveTo(hdc, rect.left+3,   rect.top+20);
	LineTo(hdc, rect.left+205, rect.top+20);
	LineTo(hdc, rect.left+205, rect.top+1);
	MoveTo(hdc, rect.left+208, rect.top+20);
	LineTo(hdc, rect.left+292, rect.top+20);
	LineTo(hdc, rect.left+292, rect.top+1);
	MoveTo(hdc, rect.left+295, rect.top+20);
	LineTo(hdc, rect.left+312, rect.top+20);
	LineTo(hdc, rect.left+312, rect.top+1);
	MoveTo(hdc, rect.left+326, rect.top+20);
	LineTo(hdc, rect.left+626, rect.top+20);
	LineTo(hdc, rect.left+626, rect.top+1);
	SelectObject(hdc, CreatePen(PS_SOLID, 1, RGB(128,128,128)));
	MoveTo(hdc, rect.left+204, rect.top+2);
	LineTo(hdc, rect.left+3,   rect.top+2);
	LineTo(hdc, rect.left+3,   rect.top+20);
	MoveTo(hdc, rect.left+291, rect.top+2);
	LineTo(hdc, rect.left+208, rect.top+2);
	LineTo(hdc, rect.left+208, rect.top+20);
	MoveTo(hdc, rect.left+311, rect.top+2);
	LineTo(hdc, rect.left+295, rect.top+2);
	LineTo(hdc, rect.left+295, rect.top+20);
	MoveTo(hdc, rect.left+625, rect.top+2);
	LineTo(hdc, rect.left+326, rect.top+2);
	LineTo(hdc, rect.left+326, rect.top+20);
	DeleteObject(SelectObject(hdc, GetStockObject(BLACK_PEN)));
	
	SelectObject(hdc, GetStockObject(WHITE_BRUSH));
        if (OverlayOX) {
	   for (i = 0; i < MAX_SPEC; i++)
	       if (SelectOX[i] && LoadOX[i] && (i != SPEC)) N++;
	}
	if (SelectOX[SPEC] && LoadOX[SPEC]) N++;
	if (N > 0) Rectangle(hdc, PS.Rect.left+PS.Plot.left, PS.Rect.top+PS.Plot.top,
		        	  PS.Rect.left+PS.Plot.right+1, PS.Rect.top+PS.Plot.bottom+1);
        
	if (SelectOX[SPEC] && LoadOX[SPEC]) {
           PlotTitle(hdc, SPEC, SCREEN);
           PlotGrid (hdc, SPEC, SCREEN);
	}

        if (OverlayOX) {
           for (i = 0; i < MAX_SPEC; i++)
	       if (SelectOX[i] && LoadOX[i] && (i != SPEC)) PlotSpectrum(hdc, i);
        }
	if (SelectOX[SPEC] && LoadOX[SPEC]) {
	   PlotSpectrum(hdc, SPEC);
	   if (SP[SPEC]->NoMark > 0) {
	      for (i = 0; i < SP[SPEC]->NoMark; i++)
		  DrawMark(hdc, SPEC, i);
           }
	   if (SP[SPEC]->NoCursorText == 1) {
	      CursorTextOut(hdc, SPEC, 0, TE_BEGIN | TE_END, SCREEN);
	   } else {
	      if (SP[SPEC]->NoCursorText > 1) {
		 CursorTextOut(hdc, SPEC, 0, TE_BEGIN, SCREEN);
		 for (i = 1; i < (SP[SPEC]->NoCursorText-1); i++)
		     CursorTextOut(hdc, SPEC, i, 0, SCREEN);
		 CursorTextOut(hdc, SPEC, SP[SPEC]->NoCursorText-1, TE_END, SCREEN);
              }
	   }
	   if (CursorMode[SPEC] == CU_SINGLE) {
              DrawCursor(hdc, SPEC, CENTER_CURSOR, TRUE);
	   }
        }
     }

     RestoreDC(hdc, -1);
     EndPaint(HWindow, &pst);
}

/* Print the given spectrum */
void SpectrumWindow::CMPrint(RTMessage)
{
     HDC         hdc;

     hdc = CreateDC(((PCFTFrame)Parent)->DriverName, ((PCFTFrame)Parent)->DeviceName,
		    ((PCFTFrame)Parent)->OutputName, ((PCFTFrame)Parent)->DevMode);

     if (hdc != NULL) {
	SetCursor(((PCFTFrame)Parent)->WaitCursor);

	PLOTSTRUCT  PSOld;
	COLORREF    Color[MAX_SPEC];
        memcpy(&PSOld, &PS, sizeof(PLOTSTRUCT));
        memcpy(Color, ((PCFTFrame)Parent)->COLOR, sizeof(COLORREF)*MAX_SPEC);

        SHORT W = GetDeviceCaps(hdc,HORZRES);
        SHORT H = GetDeviceCaps(hdc,VERTRES);
        PS.Rect.left  = 0;
        PS.Rect.right = W;
	PS.Rect.top   = 0;
        if ((REAL)H > ((REAL)W*215.0/297.0)) PS.Rect.bottom = (SHORT)ROUND((REAL)W*210.0/297.0);
	else PS.Rect.bottom = H;

	if ((GetDeviceCaps(hdc, RASTERCAPS) & RC_PALETTE) != 0) {
	   if (GetDeviceCaps(hdc, COLORRES) < 4)
	      memset(((PCFTFrame)Parent)->COLOR, 0, sizeof(COLORREF)*MAX_SPEC);
	} 

	GetDevicePlotStruct();
	CHAR StartDoc[80];
        LoadString(GetApplication()->hInstance, IDS_STARTDOC, StartDoc, 79);
	Escape(hdc, STARTDOC, 10, StartDoc, NULL);
	SHORT i, N = 0;
        if (OverlayOX) {
           for (i = 0; i < MAX_SPEC; i++)
               if (SelectOX[i] && LoadOX[i] && (i != SPEC)) N++;
        }
        if (SelectOX[SPEC] && LoadOX[SPEC]) N++;
        HPEN hp = CreatePen(PS_SOLID, PS.FrameThick, RGB(0,0,0));
        SelectObject(hdc, hp);
	if (N > 0)
           Rectangle(hdc, PS.Rect.left+PS.Plot.left, PS.Rect.top+PS.Plot.top,
		          PS.Rect.left+PS.Plot.right+1, PS.Rect.top+PS.Plot.bottom+1);
       
        if (SelectOX[SPEC] && LoadOX[SPEC]) {
           PlotTitle(hdc, SPEC, PRINTER);
           PlotGrid(hdc, SPEC, PRINTER);
        }

        if (OverlayOX) {
           for (i = 0; i < MAX_SPEC; i++)
               if (SelectOX[i] && LoadOX[i] && (i != SPEC)) PlotSpectrum(hdc, i);
        }
	if (SelectOX[SPEC] && LoadOX[SPEC]) PlotSpectrum(hdc, SPEC);
	if (SP[SPEC]->NoCursorText == 1) {
	   CursorTextOut(hdc, SPEC, 0, TE_BEGIN | TE_END, PRINTER);
	} else {
	   if (SP[SPEC]->NoCursorText > 1) {
	      CursorTextOut(hdc, SPEC, 0, TE_BEGIN, PRINTER);
	      for (i = 1; i < (SP[SPEC]->NoCursorText-1); i++)
		  CursorTextOut(hdc, SPEC, i, 0,PRINTER);
	      CursorTextOut(hdc, SPEC, SP[SPEC]->NoCursorText-1, TE_END, PRINTER);
           }
	}

        Escape(hdc, NEWFRAME, 0, NULL, NULL);
        Escape(hdc, ENDDOC,   0, NULL, NULL);

        memcpy(&PS, &PSOld, sizeof(PLOTSTRUCT));
        memcpy(((PCFTFrame)Parent)->COLOR, Color, sizeof(COLORREF)*MAX_SPEC);
        DeleteObject(hp);

	DeleteDC(hdc);
	SetCursor(((PCFTFrame)Parent)->NormalCursor);
     } else 
	CFTMessage(this, IDS_PCONFIGERRORTEXT, IDS_PCONFIGERRORTITLE, MB_OK | MB_ICONSTOP);
}

void SpectrumWindow::CMPrintOnFile(RTMessage)
{
     OPENFILENAME ofn;
     CHAR  FileName[80] = "print.out";
     CHAR  FileTitle[80];
     CHAR  Filter[128] = "Tous les fichiers (*.*)\0*.*\0";

     memset(&ofn, 0, sizeof(OPENFILENAME));

     ofn.lStructSize       = sizeof(OPENFILENAME);
     ofn.hwndOwner         = HWindow;
     ofn.hInstance         = GetApplication()->hInstance;
     ofn.lpstrFilter       = (LPSTR)Filter;
     ofn.lpstrFile         = (LPSTR)FileName;
     ofn.nMaxFile          = 79;
     ofn.lpstrFileTitle    = (LPSTR)FileTitle;
     ofn.nMaxFileTitle     = 79;
     ofn.lpstrTitle        = "File Name for Print";
     ofn.lpstrDefExt       = "*";
     ofn.Flags = OFN_OVERWRITEPROMPT;

     if (!GetSaveFileName(&ofn)) return; 

     HDC         hdc;

     hdc = CreateDC(((PCFTFrame)Parent)->DriverName, ((PCFTFrame)Parent)->DeviceName,
		    ((PCFTFrame)Parent)->OutputName, ((PCFTFrame)Parent)->DevMode);

     if (hdc != NULL) {
	SetCursor(((PCFTFrame)Parent)->WaitCursor);

	PLOTSTRUCT  PSOld;
	COLORREF    Color[MAX_SPEC];
        memcpy(&PSOld, &PS, sizeof(PLOTSTRUCT));
        memcpy(Color, ((PCFTFrame)Parent)->COLOR, sizeof(COLORREF)*MAX_SPEC);

        SHORT W = GetDeviceCaps(hdc,HORZRES);
        SHORT H = GetDeviceCaps(hdc,VERTRES);
        PS.Rect.left  = 0;
        PS.Rect.right = W;
	PS.Rect.top   = 0;
        if ((REAL)H > ((REAL)W*215.0/297.0)) PS.Rect.bottom = (SHORT)ROUND((REAL)W*210.0/297.0);
	else PS.Rect.bottom = H;

	if ((GetDeviceCaps(hdc, RASTERCAPS) & RC_PALETTE) != 0) {
	   if (GetDeviceCaps(hdc, COLORRES) < 4)
	      memset(((PCFTFrame)Parent)->COLOR, 0, sizeof(COLORREF)*MAX_SPEC);
	} 

	GetDevicePlotStruct();
	CHAR StartDoc1[80];
	LoadString(GetApplication()->hInstance, IDS_STARTDOC, StartDoc1, 79);
	DOCINFO DocInfo;
	DocInfo.cbSize = sizeof(DOCINFO);
	DocInfo.lpszDocName = StartDoc1;
	DocInfo.lpszOutput  = FileName;

	StartDoc(hdc, &DocInfo);
        StartPage(hdc);
	SHORT i, N = 0;
        if (OverlayOX) {
           for (i = 0; i < MAX_SPEC; i++)
               if (SelectOX[i] && LoadOX[i] && (i != SPEC)) N++;
        }
        if (SelectOX[SPEC] && LoadOX[SPEC]) N++;
        HPEN hp = CreatePen(PS_SOLID, PS.FrameThick, RGB(0,0,0));
        SelectObject(hdc, hp);
	if (N > 0)
           Rectangle(hdc, PS.Rect.left+PS.Plot.left, PS.Rect.top+PS.Plot.top,
		          PS.Rect.left+PS.Plot.right+1, PS.Rect.top+PS.Plot.bottom+1);
       
        if (SelectOX[SPEC] && LoadOX[SPEC]) {
           PlotTitle(hdc, SPEC, PRINTER);
           PlotGrid(hdc, SPEC, PRINTER);
        }

        if (OverlayOX) {
           for (i = 0; i < MAX_SPEC; i++)
               if (SelectOX[i] && LoadOX[i] && (i != SPEC)) PlotSpectrum(hdc, i);
        }
	if (SelectOX[SPEC] && LoadOX[SPEC]) PlotSpectrum(hdc, SPEC);
	if (SP[SPEC]->NoCursorText == 1) {
	   CursorTextOut(hdc, SPEC, 0, TE_BEGIN | TE_END, PRINTER);
	} else {
	   if (SP[SPEC]->NoCursorText > 1) {
	      CursorTextOut(hdc, SPEC, 0, TE_BEGIN, PRINTER);
	      for (i = 1; i < (SP[SPEC]->NoCursorText-1); i++)
		  CursorTextOut(hdc, SPEC, i, 0,PRINTER);
	      CursorTextOut(hdc, SPEC, SP[SPEC]->NoCursorText-1, TE_END, PRINTER);
           }
	}

	EndPage(hdc);
	EndDoc(hdc);

        memcpy(&PS, &PSOld, sizeof(PLOTSTRUCT));
        memcpy(((PCFTFrame)Parent)->COLOR, Color, sizeof(COLORREF)*MAX_SPEC);
        DeleteObject(hp);

	DeleteDC(hdc);
	SetCursor(((PCFTFrame)Parent)->NormalCursor);
     } else 
	CFTMessage(this, IDS_PCONFIGERRORTEXT, IDS_PCONFIGERRORTITLE, MB_OK | MB_ICONSTOP);
}

/* Close spectrum window or not */
BOOL SpectrumWindow::CanClose()
{
     for (SHORT i = 0; i < MAX_SPEC; i++)
	 if (!IsSaved(i, FALSE)) return FALSE;
     delete FN;
     for (i = 0; i < MAX_SPEC; i++)
         if (AllocOX[i]) FreeMemory(i);
     ((PCFTFrame)Parent)->SpecWin[NWIN] = NULL;
     FreeHGlobal(hgXY);
     return TRUE;
}

/* Load a spectrum file */
void SpectrumWindow::CMLoad(RTMessage)
{
     if (IsSaved(SPEC, FALSE)) {
     CHAR Fmt[80];
     LoadString(GetApplication()->hInstance, IDS_OPENSPECTRUM, Fmt, 79);
     sprintf(FN->Title, Fmt, SPEC+1);
     if (GetOpenFile(*FN)) {
        if (UpdateMemory(SPEC, FN->FileSize)) {
	   if (GetSpectrum(SPEC, FN->FileName)) {
	      LoadOX[SPEC] = SaveOX[SPEC] = TRUE;
	      InitialCursorState(SPEC);
	      Repaint(TRUE);
	      ((CFTFrame *)Parent)->Dollar++;
	   } else {
              CFTMessage(this, IDS_FFORMATERRORTEXT, IDS_FFORMATERRORTITLE, MB_OK | MB_ICONSTOP);
              LoadOX[SPEC] = FALSE;
              SaveOX[SPEC] = TRUE;
           }
        }
     }
     }
}

/* Save the spectrum changed */
void SpectrumWindow::CMSave(RTMessage)
{
     if (LoadOX[SPEC]) {
	CHAR BUF[128];
	lstrcpy(BUF, SP[SPEC]->FileName+SP[SPEC]->nFileOffset);
	IsSaved(SPEC, TRUE);
	if (strcmp(BUF, SP[SPEC]->FileName+SP[SPEC]->nFileOffset) != 0)	Repaint(TRUE);
     }
}

/* Save a spectrum with the other name */
void SpectrumWindow::CMSaveAs(RTMessage)
{
     if (LoadOX[SPEC]) {
	CHAR BUF[128];
	lstrcpy(BUF, SP[SPEC]->FileName+SP[SPEC]->nFileOffset);
	SaveAsCFighter(SPEC);
	if (strcmp(BUF, SP[SPEC]->FileName+SP[SPEC]->nFileOffset) != 0)	Repaint(TRUE);
     }
}

/* Save all spectra */
void SpectrumWindow::CMSaveAll(RTMessage)
{
     for (SHORT i = 0; i < MAX_SPEC; i++)
	 if (!IsSaved(i, FALSE)) break;
}

void SpectrumWindow::CMClearSpectrum(RTMessage)
{
     if (CFTMessage(this, IDS_CLEARSPECTEXT, IDS_CLEARSPECTITLE, MB_YESNO | MB_ICONQUESTION)
	 == IDYES) {
	if (IsSaved(SPEC, FALSE)) {
	   LoadOX[SPEC] = FALSE;
	   Repaint(TRUE);
        }
     }
}

/* Transfer the LeCroy 9350 spectrum */
void SpectrumWindow::CMTransfer9350(RTMessage)
{
     if (IsSaved(SPEC, FALSE)) {
	SHORT mem = 0;
	if (GetApplication()->ExecDialog(new Transfer9350DLG(this, mem))) {
	   if (Transfer9350(SPEC, mem)) {
	      ((CFTFrame *)Parent)->Dollar++;
	      Repaint(TRUE);
           }
	}
     }
}

/* Transfer the LeCroy 9350 spectrum */
void SpectrumWindow::CMAccumulation9350(RTMessage)
{
     if (IsSaved(SPEC, FALSE)) {
        SHORT mem = 0;
	if (GetApplication()->ExecDialog(new Transfer9350DLG(this, mem))) {
	   if (CountBinary9350(SPEC, mem)) Repaint(TRUE);
	}
     }
}

/* Modify the Title, X-unit, and Y-unit */
void SpectrumWindow::CMTitle(RTMessage)
{
     if (LoadOX[SPEC]) {
	TITLESTRUCT *ts = new TITLESTRUCT;
        memcpy(ts, &SP[SPEC]->TS, sizeof(TITLESTRUCT));
	if (GetApplication()->ExecDialog(new TitleDLG(this, SP[SPEC]->TS))) {
	   BOOL OX = FALSE;
	   if (strcmp(ts->Title, SP[SPEC]->TS.Title) != 0) OX = TRUE;
	   if (strcmp(ts->XUnit, SP[SPEC]->TS.XUnit) != 0) OX = TRUE;
	   if (strcmp(ts->YUnit, SP[SPEC]->TS.YUnit) != 0) OX = TRUE;
	   if (OX) {
	      SaveOX[SPEC]  = FALSE;
	      Repaint(TRUE);
           }
	}
     }
}

/* Add calibration data */
void SpectrumWindow::CMCalibAdd(RTMessage)
{
     if (LoadOX[SPEC]) {
	BOOL OX = TRUE;
	if (SP[SPEC]->NoCalib < 0) {
           OX = (CFTMessage(this, IDS_DELCALCOEFFTEXT, IDS_DELCALCOEFFTITLE, MB_YESNO | MB_ICONQUESTION) == IDYES);
           if (OX) SP[SPEC]->NoCalib = 0;
	}
	if (SP[SPEC]->NoCalib >= MAX_CALIB) {
	   OX = FALSE;
	   CFTMessage(this, IDS_NOCALIBOVERTEXT, IDS_NOCALIBOVERTITLE, MB_YESNO | MB_ICONEXCLAMATION);
        }
	if (OX) {	   
           MASSSTRUCT MS;
           MS.Num  = SP[SPEC]->NoCalib+1;
	   MS.Time = SP[SPEC]->xGain*(REAL)CursorPos[SPEC][0]+SP[SPEC]->xOffset;
	   MS.Mass = (SP[SPEC]->NoCalib > 3) ? CalculMass(SPEC, CursorPos[SPEC][0]) : MS.Time;
	   if (GetApplication()->ExecDialog(new CalibAddDLG(this, MS))) {
	      CT[SPEC][SP[SPEC]->NoCalib  ].Time = MS.Time;
	      CT[SPEC][SP[SPEC]->NoCalib++].Mass = MS.Mass;
	      if (SP[SPEC]->NoCalib > 3) {
		 if (!CalculCalibCoeff(SPEC))
		    CFTMessage(this, IDS_CALIBFAILTEXT, IDS_CALIBFAILTITLE, MB_OK | MB_ICONSTOP);
		 RedrawCursor(SPEC, CENTER_CURSOR, CursorPos[SPEC][0]);
              }
	   }
	   SaveOX[SPEC]  = FALSE;
	}
     }
}

/* Insert the calibration coefficients */
void SpectrumWindow::CMCalibIns(RTMessage)
{
     if (LoadOX[SPEC]) {
	COEFFSTRUCT CS;
	BOOL MultiCalibOX;
	BOOL OX = TRUE;
	if (GetApplication()->ExecDialog(new CalibInsDLG(this, CS, MultiCalibOX))) {
	   if (!MultiCalibOX) {
              OX = TRUE;
	      if (SP[SPEC]->NoCalib != 0) 
	         OX = (CFTMessage(this, IDS_DELCALCOEFFTEXT, IDS_DELCALCOEFFTITLE, MB_YESNO | MB_ICONQUESTION) == IDYES);
	      if (OX) {
		 SP[SPEC]->A.a[0] = CS.a[0];
	         SP[SPEC]->A.a[1] = CS.a[1];
	         SP[SPEC]->A.a[2] = CS.a[2];
		 SP[SPEC]->NoCalib = -1;
		 SaveOX[SPEC] = FALSE;
		 RedrawCursor(SPEC, CENTER_CURSOR, CursorPos[SPEC][0]);
              }
	   } else {
              CHAR STR[128];
	      for (int i = 0; i < MAX_SPEC; i++) {
		  OX = (LoadOX[i]) ? IDYES : IDNO;
		  if ((OX == IDYES) && (SP[i]->NoCalib != 0)) {
		     sprintf(STR, "Changer les coefficients de l'étalonnage du spectre N° %d ?", i+1);
		     OX = MessageBox(HWindow, STR, "Attention", MB_YESNOCANCEL | MB_ICONEXCLAMATION);
                  }
		  if (OX == IDYES) {
	             SP[i]->A.a[0] = CS.a[0];
	             SP[i]->A.a[1] = CS.a[1];
	             SP[i]->A.a[2] = CS.a[2];
		     SP[i]->NoCalib = -1;
		     SaveOX[i] = FALSE;
		  } else if (OX == IDCANCEL) break;
	      }
	      RedrawCursor(SPEC, CENTER_CURSOR, CursorPos[SPEC][0]);
           }
	}
     }

	/*
	if (SP[SPEC]->NoCalib != 0) {
	   OX = (CFTMessage(this, IDS_DELCALCOEFFTEXT, IDS_DELCALCOEFFTITLE, MB_YESNO | MB_ICONQUESTION) == IDYES);
           if (OX) SP[SPEC]->NoCalib = 0;
	}
        if (OX) {
	   
	   if (GetApplication()->ExecDialog(new CalibInsDLG(this, CS))) {
	      SP[SPEC]->A.a[0] = CS.a[0];
	      SP[SPEC]->A.a[1] = CS.a[1];
	      SP[SPEC]->A.a[2] = CS.a[2];
	      SP[SPEC]->NoCalib = -1;
	      RedrawCursor(SPEC, CENTER_CURSOR, CursorPos[SPEC][0]);
	   }
	   SaveOX[SPEC]  = FALSE;
        }
     } */
}

/* Clear the calibration coefficients */
void SpectrumWindow::CMClearCalib(RTMessage)
{
     if (LoadOX[SPEC])
     if (SP[SPEC]->NoCalib != 0) {
	if (CFTMessage(this, IDS_CLEARCALIBTEXT, IDS_CLEARCALIBTITLE, MB_OKCANCEL |
	    MB_ICONQUESTION) == IDOK)  {
	    SP[SPEC]->A.a[0] = 0;
	    SP[SPEC]->A.a[1] = 0;
	    SP[SPEC]->A.a[2] = 1;
	    SP[SPEC]->NoCalib = 0;
	    SaveOX[SPEC]  = FALSE;
	    RedrawCursor(SPEC, CENTER_CURSOR, CursorPos[SPEC][0]);
        }
     }
}

/* Edit calibration data */
void SpectrumWindow::CMEditCalib(RTMessage)
{
     if (LoadOX[SPEC])
     if (SP[SPEC]->NoCalib > 0) {
	if (GetApplication()->ExecDialog(new EditCalibDLG(this, CT[SPEC], SP[SPEC]))) {
	   SaveOX[SPEC]  = FALSE;
           RedrawCursor(SPEC, CENTER_CURSOR, CursorPos[SPEC][0]);
        }
     }
}

/* Show calibration coefficients */
void SpectrumWindow::CMCalibView(RTMessage)
{
     if (LoadOX[SPEC]) {
	if ((SP[SPEC]->NoCalib > 3) || (SP[SPEC]->NoCalib == -1)) {
	   GetApplication()->ExecDialog(new CalibViewDLG(this, SP[SPEC]->A));
	} else 
	   CFTMessage(this, IDS_NOCALIBINSUFFTEXT, IDS_NOCALIBINSUFFTITLE, MB_OK | MB_ICONEXCLAMATION);
     }
}

/* Add a mark */
void SpectrumWindow::CMAddMark(RTMessage)
{
     if (LoadOX[SPEC]) {
	if (SP[SPEC]->NoMark < MAX_MARK) {
	   SHORT N = SP[SPEC]->NoMark++;
	   MK[SPEC][N] = CursorPos[SPEC][0];
	   SaveOX[SPEC]  = FALSE;
	   Repaint(FALSE);
	   PAINTSTRUCT ps;
	   HDC hdc = BeginPaint(HWindow, &ps);
           SaveDC(hdc);
	   DrawMark(hdc, SPEC, N); 
	   RestoreDC(hdc, -1);
	   EndPaint(HWindow, &ps); 
	} else 
           CFTMessage(this, IDS_NOMARKOVERTEXT, IDS_NOMARKOVERTITLE, MB_OK);
     }
}

/* Clear all the mark */
void SpectrumWindow::CMClearMark(RTMessage)
{
     if (LoadOX[SPEC])
     if (SP[SPEC]->NoMark < MAX_MARK) {
	int OX = CFTMessage(this, IDS_CLEARMARKTEXT, IDS_CLEARMARKTITLE, MB_YESNO | MB_ICONQUESTION);
	if (OX == IDYES) {
	   SP[SPEC]->NoMark = 0;
	   SaveOX[SPEC]  = FALSE;
	   Repaint(TRUE);
	}
     }
}

/* Draw all the data point */
void SpectrumWindow::CMTotalSpectrum(RTMessage)
{
     if (LoadOX[SPEC]) {
	REAL ymin = PL[SPEC]->yMin;
	REAL ymax = PL[SPEC]->yMax;
	GetDefaultPlotInfo(SPEC);
	PL[SPEC]->yMin = ymin;
	PL[SPEC]->yMax = ymax;
	UpdatePlotInfo(SPEC);
	Repaint(TRUE);
     }
}

void SpectrumWindow::CMZoomAuto(RTMessage)
{
     if (LoadOX[SPEC]) AutoZoomOX = TRUE;
}


void SpectrumWindow::CMZoomMan(RTMessage)
{
     if (LoadOX[SPEC]) {
	MINMAXSTRUCT MS;
	LoadString(GetApplication()->hInstance, IDS_ZOOMDLGTITLE,  MS.DlgTitle, 39);
	LoadString(GetApplication()->hInstance, IDS_ZOOMDLGMIN,    MS.MinTitle, 39);
	LoadString(GetApplication()->hInstance, IDS_ZOOMDLGMAX,    MS.MaxTitle, 39);
        MS.Min = PL[SPEC]->xMin;
        MS.Max = PL[SPEC]->xMax;
        if (GetApplication()->ExecDialog(new MinMaxDLG(this, MS))) {
           PL[SPEC]->xMin = MS.Min;
           PL[SPEC]->xMax = MS.Max;
	   UpdatePlotInfo(SPEC);
	   Repaint(TRUE);
        }
     }
}

void SpectrumWindow::CMZoomAll(RTMessage)
{
     SHORT N = 0;
     for (SHORT i = 0; i < MAX_SPEC; i++) if (LoadOX[i]) N++;

     if ((N > 0) && LoadOX[SPEC]) {
	MINMAXSTRUCT MS;
	LoadString(GetApplication()->hInstance, IDS_ALLZOOMDLGTITLE,  MS.DlgTitle, 39);
	LoadString(GetApplication()->hInstance, IDS_ALLZOOMDLGMIN,    MS.MinTitle, 39);
	LoadString(GetApplication()->hInstance, IDS_ALLZOOMDLGMAX,    MS.MaxTitle, 39);
        MS.Min = PL[SPEC]->xMin;
        MS.Max = PL[SPEC]->xMax;
        if (GetApplication()->ExecDialog(new MinMaxDLG(this, MS))) {
           for (i = 0; i < MAX_SPEC; i++) 
               if (LoadOX[i]) {
                  PL[i]->xMin = MS.Min;
                  PL[i]->xMax = MS.Max;
                  UpdatePlotInfo(i);
	       }
	   Repaint(TRUE);
        }
     }
}

void SpectrumWindow::CMInverse(RTMessage)
{
     if (LoadOX[SPEC]) {
        SP[SPEC]->yGain   = -SP[SPEC]->yGain;
        SP[SPEC]->yOffset = -SP[SPEC]->yOffset;
        REAL BUF = SP[SPEC]->yMin;
        SP[SPEC]->yMin    = -SP[SPEC]->yMax;
        SP[SPEC]->yMax    = -BUF;
	GetPlotMinMax(SPEC);
	SaveOX[SPEC]  = FALSE;
	Repaint(TRUE);
     }
}

void SpectrumWindow::CMScaleMan(RTMessage)
{
     if (LoadOX[SPEC]) {
	MINMAXSTRUCT MS;
	LoadString(GetApplication()->hInstance, IDS_SCALEDLGTITLE,  MS.DlgTitle, 39);
	LoadString(GetApplication()->hInstance, IDS_SCALEDLGMIN,    MS.MinTitle, 39);
	LoadString(GetApplication()->hInstance, IDS_SCALEDLGMAX,    MS.MaxTitle, 39);
        MS.Min = PL[SPEC]->yMin;
        MS.Max = PL[SPEC]->yMax;
        if (GetApplication()->ExecDialog(new MinMaxDLG(this, MS))) {
           PL[SPEC]->yMin = MS.Min;
	   PL[SPEC]->yMax = MS.Max;
	   Repaint(TRUE);
        }
     }
}

void SpectrumWindow::CMAutoScale(RTMessage)
{
     if (LoadOX[SPEC]) {
	GetPlotMinMax(SPEC);
	Repaint(TRUE);
     }
}

void SpectrumWindow::CMSameScale(RTMessage)
{
     SHORT i;

     SHORT N = 0;
     for (i = 0; i < MAX_SPEC; i++) if (LoadOX[i]) N++;

     if ((N > 0) && LoadOX[SPEC]) {
	MINMAXSTRUCT MS;
	LoadString(GetApplication()->hInstance, IDS_ALLSCALEDLGTITLE,  MS.DlgTitle, 39);
        MS.Min = PL[SPEC]->yMin;
        MS.Max = PL[SPEC]->yMax;
        MS.Option = FALSE;
        if (GetApplication()->ExecDialog(new SameScaleDLG(this, MS))) {
           if (MS.Option == 0) {
              for (i = 0; i < MAX_SPEC; i++)
                  if (LoadOX[i]) {
                     PL[i]->yMin = MS.Min;
                     PL[i]->yMax = MS.Max;
		  }
	      Repaint(TRUE);
           } else {
              REAL H = MS.Max - MS.Min;
              for (i = 0; i < MAX_SPEC; i++) 
		  if (LoadOX[i]) PL[i]->yMax = PL[i]->yMin + H;
	      Repaint(TRUE);
           }
        }
     }
}

void SpectrumWindow::CMVisualArea(RTMessage)
{
     if (LoadOX[SPEC]) {
	if (SP[SPEC]->NoMark == 3) {
	   AREA A;
           CalculateArea(SPEC, &A);
	   GetApplication()->ExecDialog(new VisualAreaDLG(this, A));
	} else {
	   CFTMessage(this, IDS_NOMARKINSUFFTEXT, IDS_NOMARKINSUFFTITLE, MB_OK | MB_ICONEXCLAMATION);  
	}
     }
}

void SpectrumWindow::CMExpCondition(RTMessage)
{
     if (LoadOX[SPEC]) {
	CHAR buf[20];
        lstrcpy(buf, EX[SPEC]->Element);
	if (GetApplication()->ExecDialog(new ExpConditionDLG(this, EX[SPEC]))) {
	   if (strcmp(buf, EX[SPEC]->Element) != 0)
	      SaveOX[SPEC]  = FALSE;
	      Repaint(TRUE);
	}
     } else {
	GetApplication()->ExecDialog(new ExpConditionDLG(this, ((PCFTFrame)Parent)->EXSTD));
     }
}

void SpectrumWindow::CMBKSubtract(RTMessage)
{
     if (LoadOX[SPEC]) {
	if (SP[SPEC]->NoMark > 6) {
	   BKSUBSTRUCT BK;
           memset(&BK, 0, sizeof(BKSUBSTRUCT));
	   BK.Mode   = LINEAR;
	   BK.Source = SPEC;
           SHORT N;
	   BK.Target = ((N = SearchEmptyMemory()) < 0) ? 4 : N;
	   if (GetApplication()->ExecDialog(new BKSubtractDLG(this, &BK))) {
	      BOOL OX = TRUE;
	      SetCursor(((PCFTFrame)Parent)->WaitCursor);
	      if (!IsSaved(BK.Target, FALSE)) OX = FALSE;
              if (OX) {
		 if (!SearchBackground(SPEC, BK.a, BK.Mode)) {
		    MessageBox(HWindow, "Background Subtraction Failed !", "Error",
				  MB_OK | MB_ICONEXCLAMATION);
		    OX = FALSE;
                 }
              }
	      if (OX) {
                 REAL yGain, yOffset;
		 ScalingAfterBKSubtraction(SPEC, BK.a, BK.Mode, yGain, yOffset);
		 if (!WriteBKSubSpectrum(SPEC, BK.Target, BK.a, BK.Mode, yGain, yOffset)) {
		    CFTMessage(this, IDS_BKSUBFAILTEXT, IDS_BKSUBFAILTITLE,
				  MB_OK | MB_ICONEXCLAMATION);
		    OX = FALSE;
                 }
	      }
	      SetCursor(((PCFTFrame)Parent)->NormalCursor);
              if (OX) {
	         if (!OverlayOX) {
		    OverlayOX = TRUE;
                    OverlayCHK->Check();
	         }
                 if (!SelectOX[BK.Target]) {
		    SelectOX[BK.Target] = TRUE;
                    MemCHK[BK.Target]->Check();
		 }
		 LoadOX[BK.Target] = TRUE;
		 SaveOX[BK.Target]  = FALSE;
		 Repaint(TRUE);
	      } 
           }
	}
     }
}

void SpectrumWindow::CMSpectrumSum(RTMessage)
{
     if (LoadOX[SPEC]) {
	SUMSTRUCT SS;
	memset(&SS, 0, sizeof(SUMSTRUCT));
	SS.N      = 10;
	SS.Source = SPEC;
	SHORT N;
	SS.Target = ((N = SearchEmptyMemory()) < 0) ? 4 : N;
	if (GetApplication()->ExecDialog(new SpectrumSumDLG(this, "Spectrum Sum", &SS))) {
	   BOOL OX = TRUE;
	   SetCursor(((PCFTFrame)Parent)->WaitCursor);
           if (!IsSaved(SS.Target, FALSE)) OX = FALSE;
	   if (!WriteSpectrumSum(SS)) {
	      OX = FALSE;
	      CFTMessage(this, IDS_SPECSUMFAILTEXT, IDS_SPECSUMFAILTITLE,
				  MB_OK | MB_ICONEXCLAMATION);
           }
	   SetCursor(((PCFTFrame)Parent)->NormalCursor);
           if (OX) {
	      if (!OverlayOX) {
	         OverlayOX = TRUE;
                 OverlayCHK->Check();
	      }
	      if (!SelectOX[SS.Target]) {
	         SelectOX[SS.Target] = TRUE;
		 MemCHK[SS.Target]->Check();
	      }
	      LoadOX[SS.Target] = TRUE;
	      SaveOX[SS.Target]  = FALSE;
	      Repaint(TRUE);
	   } 
	}
     }
}

void SpectrumWindow::CMLSSmoothing(RTMessage)
{
     if (LoadOX[SPEC]) {
	SUMSTRUCT SS;
	memset(&SS, 0, sizeof(SUMSTRUCT));
	SS.N      = 10;
	SS.Source = SPEC;
	SHORT N;
	SS.Target = ((N = SearchEmptyMemory()) < 0) ? 4 : N;
	if (GetApplication()->ExecDialog(new SpectrumSumDLG(this, "LS Filter", &SS))) {
	   BOOL OX = TRUE;
	   SetCursor(((PCFTFrame)Parent)->WaitCursor);
           if (!IsSaved(SS.Target, FALSE)) OX = FALSE;
	   if (!WriteLSSmoothing(SS)) {
	      OX = FALSE;
	      CFTMessage(this, IDS_LSFILTERFAILTEXT, IDS_LSFILTERFAILTITLE,
				  MB_OK | MB_ICONEXCLAMATION);
           }
	   SetCursor(((PCFTFrame)Parent)->NormalCursor);
           if (OX) {
	      if (!OverlayOX) {
	         OverlayOX = TRUE;
		 OverlayCHK->Check();
	      }
	      if (!SelectOX[SS.Target]) {
	         SelectOX[SS.Target] = TRUE;
		 MemCHK[SS.Target]->Check();
	      }
	      LoadOX[SS.Target] = TRUE;
	      SaveOX[SS.Target]  = FALSE;
	      Repaint(TRUE);
	   } 
	}
     }
}

void SpectrumWindow::CMCCLeft(RTMessage)
{
     if (CursorPos[SPEC][0] > 0L)
	RedrawCursor(SPEC, CENTER_CURSOR, CursorPos[SPEC][0]-1);
}

void SpectrumWindow::CMCCRight(RTMessage)
{
     if (CursorPos[SPEC][0] < (SP[SPEC]->Ntot-1))
	RedrawCursor(SPEC, CENTER_CURSOR, CursorPos[SPEC][0]+1);
}

void SpectrumWindow::CMCCLeftFast(RTMessage)
{
     if (CursorPos[SPEC][0] > 10L)
	RedrawCursor(SPEC, CENTER_CURSOR, CursorPos[SPEC][0]-10);
}

void SpectrumWindow::CMCCRightFast(RTMessage)
{
     if (CursorPos[SPEC][0] < (SP[SPEC]->Ntot-11))
	RedrawCursor(SPEC, CENTER_CURSOR, CursorPos[SPEC][0]+10);
}

void SpectrumWindow::CMZoom(RTMessage)
{
     if (LoadOX[SPEC]) {
	REAL DW = fabs(PL[SPEC]->xMax-PL[SPEC]->xMin)/50.0;
	PL[SPEC]->xMin += DW;
	PL[SPEC]->xMax -= DW;
	UpdatePlotInfo(SPEC);
	Repaint(TRUE);
     }
}

void SpectrumWindow::CMZoomFast(RTMessage)
{
     if (LoadOX[SPEC]) {
	REAL DW = fabs(PL[SPEC]->xMax-PL[SPEC]->xMin)/10.0;
	PL[SPEC]->xMin += DW;
	PL[SPEC]->xMax -= DW;
	UpdatePlotInfo(SPEC);
	Repaint(TRUE);
     }
}

void SpectrumWindow::CMUnzoom(RTMessage)
{
     if (LoadOX[SPEC]) {
	REAL DW = fabs(PL[SPEC]->xMax-PL[SPEC]->xMin)/50.0;
	PL[SPEC]->xMin -= DW;
	PL[SPEC]->xMax += DW;
	UpdatePlotInfo(SPEC);
	Repaint(TRUE);
     }
}

void SpectrumWindow::CMUnzoomFast(RTMessage)
{
     if (LoadOX[SPEC]) {
	REAL DW = fabs(PL[SPEC]->xMax-PL[SPEC]->xMin)/10.0;
	PL[SPEC]->xMin -= DW;
	PL[SPEC]->xMax += DW;
	UpdatePlotInfo(SPEC);
	Repaint(TRUE);
     }
}

void SpectrumWindow::WMLButtonDown(RTMessage Message)
{
     SHORT X = (SHORT)(Message.LP.Lo);
     SHORT Y = (SHORT)(Message.LP.Hi);
     SHORT Left   = PS.Rect.left+PS.Plot.left;
     SHORT Right  = PS.Rect.left+PS.Plot.right;
     SHORT Top    = PS.Rect.top+PS.Plot.top;
     SHORT Bottom = PS.Rect.top+PS.Plot.bottom;

     if (LoadOX[SPEC])
     if (Message.WParam == MK_LBUTTON)
     if ((X >= Left) && (X <= Right) && (Y >= Top) && (Y <= Bottom)) {
	if (AutoZoomOX) {
           if (GetCursor() != ((PCFTFrame)Parent)->CrossCursor)
	      SetCursor(((PCFTFrame)Parent)->CrossCursor);
	   Xini = X;
	   Yini = Y;
	   HDC hDC = GetDC(HWindow);
           SaveDC(hDC);
	   int Old = SetROP2(hDC, R2_NOT);
	   LOGBRUSH lb;
	   lb.lbStyle = BS_HOLLOW;
	   HBRUSH OldBr = (HBRUSH)SelectObject(hDC, CreateBrushIndirect(&lb));

           Rectangle(hDC, Xini, Yini, X, Y);

	   DeleteObject(SelectObject(hDC, OldBr));
           SetROP2(hDC, Old);
           RestoreDC(hDC, -1);
	   ReleaseDC(HWindow, hDC);
           Xfin = X;
	   Yfin = Y;
           LBDownOX = TRUE;
        } else {
	   REAL x = (REAL)(X-Left)*(PL[SPEC]->xMax-PL[SPEC]->xMin)/(REAL)(Right-Left);
	   x += PL[SPEC]->xMin;
	   LONG Pos = (LONG)ROUND((x-SP[SPEC]->xOffset)/SP[SPEC]->xGain);
	   if (Pos < 0L) Pos = 0L;
	   if (Pos >= SP[SPEC]->Ntot) Pos = SP[SPEC]->Ntot-1L;
	   if (Pos != CursorPos[SPEC][0]) {
	      RedrawCursor(SPEC, CENTER_CURSOR, Pos);
              LBDownOX = TRUE;
	   }
        }
     } else AutoZoomOX = FALSE;
}

void SpectrumWindow::WMMouseMove(RTMessage Message)
{
     SHORT X = (SHORT)(Message.LP.Lo);
     SHORT Y = (SHORT)(Message.LP.Hi);
     SHORT Left   = PS.Rect.left+PS.Plot.left;
     SHORT Right  = PS.Rect.left+PS.Plot.right;
     SHORT Top    = PS.Rect.top+PS.Plot.top;
     SHORT Bottom = PS.Rect.top+PS.Plot.bottom;

     if ((X >= Left) && (X <= Right) && (Y >= Top) && (Y <= Bottom)) {
        if (AutoZoomOX)
	if (GetCursor() != ((PCFTFrame)Parent)->CrossCursor)
		 SetCursor(((PCFTFrame)Parent)->CrossCursor);
        if (LBDownOX) {
	   if (AutoZoomOX) {
              if ((X != Xfin) || (Y != Yfin)) {
	         HDC hDC = GetDC(HWindow);
                 SaveDC(hDC);
	         int Old = SetROP2(hDC, R2_NOT);
	         LOGBRUSH lb;
	         lb.lbStyle = BS_HOLLOW;
	         HBRUSH OldBr = (HBRUSH)SelectObject(hDC, CreateBrushIndirect(&lb));

	         Rectangle(hDC, Xini, Yini, Xfin, Yfin);
	         Rectangle(hDC, Xini, Yini, X, Y);

	         DeleteObject(SelectObject(hDC, OldBr));
                 SetROP2(hDC, Old);
                 RestoreDC(hDC, -1);
	         ReleaseDC(HWindow, hDC);
                 Xfin = X;
	         Yfin = Y;
              }
           } else {
	      REAL x = (REAL)(X-Left)*(PL[SPEC]->xMax-PL[SPEC]->xMin)/(REAL)(Right-Left);
	      x += PL[SPEC]->xMin;
	      LONG Pos = (LONG)ROUND((x-SP[SPEC]->xOffset)/SP[SPEC]->xGain);
	      if (Pos < 0L) Pos = 0L;
	      if (Pos >= SP[SPEC]->Ntot) Pos = SP[SPEC]->Ntot-1L;
	      if (Pos != CursorPos[SPEC][0]) RedrawCursor(SPEC, CENTER_CURSOR, Pos);
           }
	}
     } else {
	if (AutoZoomOX) 
	if (GetCursor() != ((PCFTFrame)Parent)->NormalCursor)
		 SetCursor(((PCFTFrame)Parent)->NormalCursor);
     }
}

void SpectrumWindow::WMLButtonUp(RTMessage)
{
     SHORT Left   = PS.Rect.left+PS.Plot.left;
     SHORT Right  = PS.Rect.left+PS.Plot.right;
     SHORT Top    = PS.Rect.top+PS.Plot.top;
     SHORT Bottom = PS.Rect.top+PS.Plot.bottom;

     LBDownOX = FALSE;
     if (AutoZoomOX) {
	if (GetCursor() != ((PCFTFrame)Parent)->NormalCursor)
	      SetCursor(((PCFTFrame)Parent)->NormalCursor);
	HDC hDC = GetDC(HWindow);
        SaveDC(hDC);
	int Old = SetROP2(hDC, R2_NOT);
	LOGBRUSH lb;
	lb.lbStyle = BS_HOLLOW;
	HBRUSH OldBr = (HBRUSH)SelectObject(hDC, CreateBrushIndirect(&lb));
	Rectangle(hDC, Xini, Yini, Xfin, Yfin);
	DeleteObject(SelectObject(hDC, OldBr));
        SetROP2(hDC, Old);
        RestoreDC(hDC, -1);
	ReleaseDC(HWindow, hDC);
	int Xmin = (Xini < Xfin) ? Xini : Xfin;
	int Xmax = (Xini > Xfin) ? Xini : Xfin;
	int Ymin = (Yini < Yfin) ? Yini : Yfin;
	int Ymax = (Yini > Yfin) ? Yini : Yfin;
	if (Xmin == Xmax) Xmax++;
        if (Ymin == Ymax) Ymax++;

	REAL xmin = (REAL)(Xmin-Left)*(PL[SPEC]->xMax-PL[SPEC]->xMin)/(REAL)(Right-Left);
	xmin += PL[SPEC]->xMin;
	REAL xmax = (REAL)(Xmax-Left)*(PL[SPEC]->xMax-PL[SPEC]->xMin)/(REAL)(Right-Left);
	xmax += PL[SPEC]->xMin;
	REAL ymin = (REAL)(Bottom-Ymax)*(PL[SPEC]->yMax-PL[SPEC]->yMin)/(REAL)(Bottom-Top);
	ymin += PL[SPEC]->yMin;
	REAL ymax = (REAL)(Bottom-Ymin)*(PL[SPEC]->yMax-PL[SPEC]->yMin)/(REAL)(Bottom-Top);
	ymax += PL[SPEC]->yMin;


	PL[SPEC]->xMin = xmin;
        PL[SPEC]->xMax = xmax;
	PL[SPEC]->yMin = ymin;
	PL[SPEC]->yMax = ymax;
	UpdatePlotInfo(SPEC);
	Repaint(TRUE);

        AutoZoomOX = FALSE;
     }
}

void SpectrumWindow::CMPutCursorText(RTMessage)
{
     if (LoadOX[SPEC]) {
        SHORT N   = SP[SPEC]->NoCursorText++;
        LONG  Pos = CursorPos[SPEC][0];
        TE[SPEC][N].Position = Pos;
        sprintf(TE[SPEC][N].TextStr, "%.3f µs", SP[SPEC]->xGain*(REAL)Pos+SP[SPEC]->xOffset);
	DrawCursorText(SPEC, N);
	SaveOX[SPEC] = FALSE;
     }
}

void SpectrumWindow::CMCursorTextBis(RTMessage)
{
     if (LoadOX[SPEC]) {
        CHAR str[LEN_CURSORTEXT];
	lstrcpy(str,"");
	LONG  Pos = CursorPos[SPEC][0];
	REAL  TOF = SP[SPEC]->xGain*(REAL)Pos+SP[SPEC]->xOffset;
	CHAR  Prompt[40];
        sprintf(Prompt, "T.O.F. : %.3f µs", TOF);
	if (GetApplication()->ExecDialog(new InputCTextDLG(this, "Input Cursor Text", Prompt,
						      str, LEN_CURSORTEXT-1, TOF)))  {
	   SHORT N   = SP[SPEC]->NoCursorText++;
	   TE[SPEC][N].Position = Pos;
           lstrcpy(TE[SPEC][N].TextStr, str);
	   DrawCursorText(SPEC, N);
	   SaveOX[SPEC] = FALSE;
        }
     }
}

void SpectrumWindow::CMInsertCText(RTMessage)
{
     if (LoadOX[SPEC]) {
        CTEXTSTRUCT CS; 
	if (GetApplication()->ExecDialog(new InsertCTextDLG(this, CS))) {
	   if (!InsertCursorText(SPEC, CS.TOF, CS.CText))
	      MessageBox(HWindow, "Invalid Value(s) !", "Error", MB_OK | MB_ICONSTOP);
	   else SaveOX[SPEC] = FALSE;
        }
     }
}

void SpectrumWindow::CMEditCText(RTMessage)
{
     if (LoadOX[SPEC])
     if (SP[SPEC]->NoCursorText > 0) {
	if (GetApplication()->ExecDialog(new EditCTextDLG(this, TE[SPEC], SP[SPEC])))
	   SaveOX[SPEC] = FALSE;
           Repaint(TRUE);
     }
}

void SpectrumWindow::CMClearAllCText(RTMessage)
{
     if (LoadOX[SPEC] && (SP[SPEC]->NoCursorText > 0)) {
        int OX = MessageBox(HWindow, "Vous êtes sûr ?", "confirm", MB_YESNO | MB_ICONQUESTION); 
	if (OX == IDYES) {
	   SP[SPEC]->NoCursorText = 0;
	   Repaint(TRUE);
	}
     } 
}

void SpectrumWindow::CMOverlay(RTMessage)
{
     if (OverlayOX) {
        SHORT N = 0;
	for (SHORT i = 0; i < MAX_SPEC; i++)
	    if (i != SPEC) {
	       SelectOX[i] = FALSE;
	       MemCHK[i]->Uncheck();
               N++;
	    }
	OverlayOX = FALSE;
	if (N > 0) Repaint(TRUE);
     } else OverlayOX = TRUE;
}

void SpectrumWindow::ChangeMem(SHORT n)
{
     if (OverlayOX) {
	BOOL CtrlKeyOX = ((GetKeyState(VK_CONTROL) & 0x8000) != 0) ? TRUE : FALSE;
	if (CtrlKeyOX) {
	   SHORT N = 0;
	   for (SHORT i = 0; i < MAX_SPEC; i++) if (SelectOX[i]) N++;
	   if (N > 1) {
              BOOL OX = FALSE;
	      SelectOX[n] = FALSE;
	      MemCHK[n]->Uncheck();
              if (SPEC == n) {
	         for (i = 0; i < MAX_SPEC; i++) if (SelectOX[i]) break;
		 SPEC = i;
		 CHAR str[3];
                 sprintf(str, "%d", SPEC+1);
		 SpecNo->SetText(str);
                 OX = TRUE;
	      } else {
                 OX = (LoadOX[n]) ? TRUE : FALSE;
              }
	      if (OX) Repaint(TRUE);
	   } else {
              MemCHK[n]->Check();
	      MessageBox(HWindow, "Cannot be discarded !", "Warning", MB_OK | MB_ICONSTOP);
           }
        } else {
           BOOL OX = (n != SPEC) ? TRUE : FALSE;
	   if (!SelectOX[n]) {
	      SelectOX[n] = TRUE;
	      SPEC = n;
	      CHAR str[3];
              sprintf(str, "%d", SPEC+1);
	      SpecNo->SetText(str);
	      MemCHK[SPEC]->Check();
	   } else {
	      SPEC = n;
	      CHAR str[3];
              sprintf(str, "%d", SPEC+1);
	      SpecNo->SetText(str);
	      MemCHK[SPEC]->Check();
	   }
	   if (OX) Repaint(TRUE);
        }
     } else {
	if (n != SPEC) {
	   SelectOX[SPEC] = FALSE;
	   MemCHK[SPEC]->Uncheck();
	   SPEC = n;
	   CHAR str[3];
           sprintf(str, "%d", SPEC+1);
	   SpecNo->SetText(str);
	   SelectOX[SPEC] = TRUE;
	   MemCHK[SPEC]->Check();
	   Repaint(TRUE);
	} else {
	   MemCHK[SPEC]->Check();
        }
     }
}

void SpectrumWindow::YShift(YSHIFTMODE mode)
{
     if (LoadOX[SPEC]) {
	REAL DH = fabs(PL[SPEC]->yMax-PL[SPEC]->yMin)/50.0;
	PL[SPEC]->yMin -= (mode == SY_UP) ? DH : -DH;
	PL[SPEC]->yMax -= (mode == SY_UP) ? DH : -DH;
	Repaint(TRUE);
     }
}

void SpectrumWindow::YShiftFast(YSHIFTMODE mode)
{
     if (LoadOX[SPEC]) {
	REAL DH = fabs(PL[SPEC]->yMax-PL[SPEC]->yMin)/10.0;
	PL[SPEC]->yMin -= (mode == SY_UP) ? DH : -DH;
	PL[SPEC]->yMax -= (mode == SY_UP) ? DH : -DH;
	Repaint(TRUE);
     }
}

void SpectrumWindow::XShift(XSHIFTMODE mode)
{
     if (LoadOX[SPEC]) {
	REAL DW = fabs(PL[SPEC]->xMax-PL[SPEC]->xMin)/100.0;
	PL[SPEC]->xMin += (mode == SX_LEFT) ? DW : -DW;
	PL[SPEC]->xMax += (mode == SX_LEFT) ? DW : -DW;
	UpdatePlotInfo(SPEC);
	Repaint(TRUE);
     }
}

void SpectrumWindow::XShiftFast(XSHIFTMODE mode)
{
     if (LoadOX[SPEC]) {
	REAL DW = fabs(PL[SPEC]->xMax-PL[SPEC]->xMin)/20.0;
	PL[SPEC]->xMin += (mode == SX_LEFT) ? DW : -DW;
	PL[SPEC]->xMax += (mode == SX_LEFT) ? DW : -DW;
	UpdatePlotInfo(SPEC);
	Repaint(TRUE);
     }
}

void SpectrumWindow::ScaleUpDown(SCALEMODE mode)
{
     if (LoadOX[SPEC]) {
	REAL DH = fabs(PL[SPEC]->yMax-PL[SPEC]->yMin)/25.0;
	PL[SPEC]->yMax -= (mode == SC_UP) ? DH : -DH;
	Repaint(TRUE);
     }
}

void SpectrumWindow::ScaleUpDownFast(SCALEMODE mode)
{
     if (LoadOX[SPEC]) {
	REAL DH = fabs(PL[SPEC]->yMax-PL[SPEC]->yMin)/5.0;
	PL[SPEC]->yMax -= (mode == SC_UP) ? DH : -DH;
	Repaint(TRUE);
     }
}

void SpectrumWindow::DrawCursorText(SHORT Spec, SHORT N)
{
     Repaint(FALSE);
     PAINTSTRUCT ps;
     HDC hdc = BeginPaint(HWindow, &ps);
     SaveDC(hdc);
     DrawCursor(hdc, Spec, CENTER_CURSOR, FALSE);
     CursorTextOut(hdc, Spec, N, TE_BEGIN | TE_END, SCREEN);
     DrawCursor(hdc, Spec, CENTER_CURSOR, FALSE);
     RestoreDC(hdc, -1);
     EndPaint(HWindow, &ps);
}

BOOL SpectrumWindow::InsertCursorText(SHORT Spec, REAL tof, LPSTR str)
{
     if (!LoadOX[Spec]) return FALSE;
     LONG Pos = (LONG)ROUND((tof-SP[Spec]->xOffset)/SP[Spec]->xGain);
     if ((Pos < 0L) || ( Pos >= SP[Spec]->Ntot)) return FALSE;
     SHORT N = SP[Spec]->NoCursorText++;
     TE[Spec][N].Position = Pos;
     lstrcpy(TE[Spec][N].TextStr, str);
     DrawCursorText(Spec, N);
     return TRUE;
}

BOOL SpectrumWindow::DeleteCursorText(SHORT Spec, SHORT n)
{
     if (LoadOX[Spec] && (SP[Spec]->NoCursorText > 0)) {
        BOOL OX = FALSE;
	for (SHORT i = n+1; i < SP[Spec]->NoCursorText; i++) {
	    TE[Spec][i-1].Position = TE[Spec][i].Position;
	    lstrcpy(TE[Spec][i-1].TextStr, TE[Spec][i].TextStr);
            if (!OX) OX = TRUE;
	}
	if (OX) {
	   SP[Spec]->NoCursorText--;
	   return TRUE;
        } else return FALSE;
     } else return FALSE;
}

void SpectrumWindow::SetCursorPosition(SHORT n, REAL TOF)
{
     if (LoadOX[n]) {
	LONG  Pos = (LONG)ROUND((TOF-SP[n]->xOffset)/SP[n]->xGain);
	if ((Pos >= 0L) && (Pos < SP[n]->Ntot)) RedrawCursor(n, CENTER_CURSOR, Pos);
     }
}

void SpectrumWindow::CalculateArea(SHORT n, AREA *a)
{
     SHORT k, l;
     REAL Atot, Atot1, Aleft, Aleft1, Aleft2, Aright, Aright1, Aright2;
     REAL y;

     LONG Pos[3], PosBuf;
     memcpy(Pos, MK[n], sizeof(LONG)*3);
     for (k = 0; k < 3; k++)
     for (l = k+1; l < 3; l++) {
	 if (Pos[k] > Pos[l]) {
	    PosBuf = Pos[k];
	    Pos[k] = Pos[l];
            Pos[l] = PosBuf;
         }
     }

     REAL Wleft  = (REAL)(Pos[1] - Pos[0]);
     REAL Wright = (REAL)(Pos[2] - Pos[1]);
     REAL Wtot   = Wleft+Wright;
     REAL xleft  = (REAL)Pos[0];
     REAL xright = (REAL)(Pos[2]-1L);
     REAL yleft  =  SP[n]->yGain*(REAL)Y[n][Pos[0]]+SP[n]->yOffset;
     REAL ycenter=  SP[n]->yGain*(REAL)Y[n][Pos[1]]+SP[n]->yOffset;
     REAL yright =  SP[n]->yGain*(REAL)Y[n][Pos[2]-1L]+SP[n]->yOffset;
     REAL A = (yleft-yright)/(xleft-xright);
     REAL B = yleft-A*xleft;

     Aleft = Aright = Aleft1 = Aright1 = Aleft2 = Aright2 = 0.0;

     for (LONG i = Pos[0]; i < Pos[1]; i++)
	  Aleft += (SP[n]->yGain*(REAL)Y[n][i]+SP[n]->yOffset);

     for (i = Pos[1]; i < Pos[2]; i++)
	 Aright += (SP[n]->yGain*(REAL)Y[n][i]+SP[n]->yOffset);

     Atot  = Aleft + Aright;

     REAL BK = (yleft+yright)/2.0*Wtot;
     Atot1 = Atot-BK;

     y = A*(REAL)(Pos[1]-1L)+B;
     BK = (yleft+y)/2.0*Wleft;
     Aleft1 = Aleft-BK;

     y = A*(REAL)(Pos[1])+B;
     BK = (yright+y)/2.0*Wright;
     Aright1 = Aright-BK;

     BK = SP[n]->yGain*(REAL)(Y[n][Pos[0]]+Y[n][Pos[1]-1L])+2.0*SP[n]->yOffset;
     BK *= (Wleft/2.0);
     Aleft2 = Aleft - BK;

     BK = SP[n]->yGain*(REAL)(Y[n][Pos[1]]+Y[n][Pos[2]-1L])+2.0*SP[n]->yOffset;
     BK *= (Wright/2.0);
     Aright2 = Aright - BK;

     a->Area[0] = Atot;
     a->Area[1] = Atot1;
     a->Area[2] = Aleft;
     a->Area[3] = Aright;
     a->Area[4] = Aleft1;
     a->Area[5] = Aright1;
     a->Area[6] = Aleft2;
     a->Area[7] = Aright2;
     a->Area[8] = a->Area[2]-yleft*Wleft;
     a->Area[9] = a->Area[3]-yleft*Wright;
     a->Area[10] = a->Area[2]-yright*Wleft;
     a->Area[11] = a->Area[3]-yright*Wright;
     a->Area[12] = a->Area[2]-ycenter*Wleft;
     a->Area[13] = a->Area[3]-ycenter*Wright;
}

SHORT SpectrumWindow::SearchEmptyMemory()
{
      for (SHORT i = MAX_SPEC-1; i >= 0; i--)
	  if (!LoadOX[i]) return i;
      return -1;
}

void CFTApp::InitInstance()
{
     TApplication::InitInstance();
     if ( Status == 0 )
     {
	HAccTable = LoadAccelerators(hInstance, "Accel");
        if ( HAccTable == 0 )
	Status = EM_INVALIDWINDOW;
     }
}

void CFTApp::InitMainWindow()
{
     MainWindow = new CFTFrame("CFighter 1.10", "CFighter");
     ((PCFTFrame)MainWindow)->ChildMenuPos = 6;
     
}

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR     lpCmdLine, int       nCmdShow)
{
    CFTApp CFT110App("CFT110 pour Windows", hInstance, hPrevInstance,
		     lpCmdLine, nCmdShow);
    CFT110App.Run();
    return CFT110App.Status;
}