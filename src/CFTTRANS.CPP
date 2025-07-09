#include "cft.h"
#include "cftdlg.h"
#include "cftctrl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dir.h>
#include <math.h>
#include <mem.h>

char  MEM[12][3] = { "M1", "M2", "M3", "M4", "TA", "TB", "TC", "TD", "C1", "C2", "C3", "C4" };

void CFTFrame::CMGPIBConnect(RTMessage)
{
     GPIBSTRUCT GS;
     if (!GPIBOX) {
	lstrcpy(GS.BrdName, "GPIB0");
	lstrcpy(GS.DevName, "LECROY1");
	GS.DevAd = 4;
     } else {
	lstrcpy(GS.BrdName, TR->GetBoardName());
	lstrcpy(GS.DevName, TR->GetDeviceName());
	GS.DevAd = TR->GetDeviceAddress();
     }
     if (GetApplication()->ExecDialog(new GPIBConnectDLG(this, GS))) {
	if (GPIBOX) delete TR;
	TR = new GPIB(GS.BrdName, GS.DevName, GS.DevAd);
	GPIBOX = (TR->IsConnected()) ? TRUE : FALSE;
	if (!GPIBOX) {
	   CHAR EText[80];
           LoadString(GetApplication()->hInstance, IDS_CONNECTERRORTEXT, EText, 79);
           CHAR ETitle[40];
           LoadString(GetApplication()->hInstance, IDS_CONNECTERRORTITLE, ETitle, 39);
	   MessageBox(NULL, EText, ETitle, MB_OK | MB_ICONSTOP);
	}
     }
}

BOOL SpectrumWindow::Transfer9350(SHORT n, SHORT mem)
{
     if (!Transfer9350WaveDesc(n, mem, 1)) return FALSE;
     if (!Transfer9350Spectrum(n, mem, Y[n], 1)) return FALSE;

     GetDefaultSpectrumMinMax(n);
     GetDefaultPlotInfo(n);
     InitialCursorState(n);

     return TRUE;
}

BOOL SpectrumWindow::CountBinary9350(SHORT n, SHORT mem)
{
     COUNTSTRUCT XX;
     // TMessage msg;
     HDC         hdc;
     RECT        rect;
     SHORT       i, j, N = 0;

     if (GetApplication()->ExecDialog(new CountDLG(this, XX))) {

        PCFTFrame PARENT = (PCFTFrame)Parent;

	if (!Transfer9350WaveDesc(n, mem, 0)) return FALSE;

	HGLOBAL hgAY = GlobalAlloc(GPTR, PARENT->WD->WaveArray1+10L);
	if (hgAY == NULL) return FALSE;
	SHORT HUGE *AY = (SHORT HUGE *)GlobalLock(hgAY);
	SHORT Threshold = (SHORT)floor((XX.Threshold-SP[n]->yOffset)/SP[n]->yGain+0.5);
	//BOOL  OX = (SP[n]->yGain > 0.0) ? TRUE : FALSE;
	BOOL  OX = (SP[n]->yGain > 0.0) ? ((XX.IsUpperBound) ? 0 : 1) : ((XX.IsUpperBound) ? 1 : 0);
        DWORD dt = XX.IdlingTime;

	SP[n]->yGain = 1.0;
	SP[n]->yOffset = 0.0;

	long m;
        DWORD t1;

	CancelDLG *ADlg = new CancelDLG(this, "Accumulation");
	GetApplication()->MakeWindow(ADlg);

	char STR[80];
        sprintf(STR, "Total Number of Scans : %d", XX.nTot);
	ADlg->SetText(STR);
        int Nfail = 0;
	for (m = 0L; m < SP[n]->Ntot; m++)  Y[n][m] = 0;
	for (i = 0; i < XX.nTot; i++) {
	    t1 = GetTickCount();
	    if ((i%XX.nSeg) == 0) {
	       sprintf(SP[n]->TS.YUnit, "Counts");
	       GetDefaultSpectrumMinMax(n);
               GetDefaultPlotInfo(n);
	       InitialCursorState(n);

	       GetClientRect(HWindow, &rect);
               PS.Rect.left   = rect.left;
               PS.Rect.top    = rect.top+25;
               PS.Rect.right  = rect.right;
               PS.Rect.bottom = rect.bottom;

	       hdc = GetDC(HWindow);
	       SaveDC(hdc);

               if (PS.Rect.top < PS.Rect.bottom) {
                  GetDevicePlotStruct();
		  SelectObject(hdc, GetStockObject(WHITE_PEN));
		  SelectObject(hdc, GetStockObject(WHITE_BRUSH));
		  Rectangle(hdc, PS.Rect.left, PS.Rect.top, PS.Rect.right, PS.Rect.bottom);
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
	             for (j = 0; j < MAX_SPEC; j++)
	                if (SelectOX[j] && LoadOX[j] && (j != SPEC)) N++;
	          }
	          if (SelectOX[SPEC] && LoadOX[SPEC]) N++;
	          if (N > 0) Rectangle(hdc, PS.Rect.left+PS.Plot.left, PS.Rect.top+PS.Plot.top,
		        	  PS.Rect.left+PS.Plot.right+1, PS.Rect.top+PS.Plot.bottom+1);
        
	          if (SelectOX[SPEC] && LoadOX[SPEC]) {
                     PlotTitle(hdc, SPEC, SCREEN);
                     PlotGrid (hdc, SPEC, SCREEN);
	          }

                  if (OverlayOX) {
                      for (j = 0; j < MAX_SPEC; j++)
	                  if (SelectOX[j] && LoadOX[j] && (j != SPEC)) PlotSpectrum(hdc, j);
                  }
	          if (SelectOX[SPEC] && LoadOX[SPEC]) {
	             PlotSpectrum(hdc, SPEC);
	             if (SP[SPEC]->NoMark > 0) {
	                for (j = 0; j < SP[SPEC]->NoMark; j++)
		            DrawMark(hdc, SPEC, j);
                     }
	             if (SP[SPEC]->NoCursorText == 1) {
	                CursorTextOut(hdc, SPEC, 0, TE_BEGIN | TE_END, SCREEN);
	             } else {
	                if (SP[SPEC]->NoCursorText > 1) {
		           CursorTextOut(hdc, SPEC, 0, TE_BEGIN, SCREEN);
		           for (j = 0; j < (SP[SPEC]->NoCursorText-1); j++)
		               CursorTextOut(hdc, SPEC, j, 0, SCREEN);
		           CursorTextOut(hdc, SPEC, SP[SPEC]->NoCursorText-1, TE_END, SCREEN);
                        }
	             }
	             if (CursorMode[SPEC] == CU_SINGLE) {
                        DrawCursor(hdc, SPEC, CENTER_CURSOR, TRUE);
	             }
                  }
               }

               RestoreDC(hdc, -1);
               ReleaseDC(HWindow, hdc);



	       // SendMessage(HWindow, WM_PAINT, (WPARAM)0, (LPARAM)0);
               // WMPaint(msg);
	       // SendMessage(HWindow, WM_PAINT, (WPARAM)0, (LPARAM)0);
	       // t1 = GetTickCount();
	       // while ((GetTickCount()-t1) < 500L);
	    }
	    sprintf(STR, "Scan : %d", i+1);
	    ADlg->SetDone(STR);
	    if (ADlg->IsCancelPressed()) break;
	    if (Transfer9350Spectrum(n, mem, AY, 0)) {
	       for (m = 0L; m < SP[n]->Ntot; m++)   {
		   if (OX) {
		      if (AY[m] > Threshold) Y[n][m]++;
	           } else {
		      if (AY[m] < Threshold) Y[n][m]++;
                   }
	       }
	    } else {
	       i--;
	       Nfail++;
	       if (Nfail >= 10) break;
            }
	    while ((GetTickCount()-t1) < dt);
	    
	}

	DestroyWindow(ADlg->HWindow);
	sprintf(SP[n]->TS.YUnit, "Counts");
	sprintf(EX[n]->Commentaire[0], "Total number of scans : %d", i);

	if (i <= 0) {
	   GetDefaultSpectrumMinMax(n);
           GetDefaultPlotInfo(n);
	   InitialCursorState(n);
	   GlobalUnlock(hgAY);
	   GlobalFree(hgAY);
	   return FALSE;
	}

	
        GetDefaultSpectrumMinMax(n);
        GetDefaultPlotInfo(n);
	InitialCursorState(n);

        GlobalUnlock(hgAY);
	GlobalFree(hgAY);
        return TRUE;
     } else  return FALSE;
} 


BOOL SpectrumWindow::Transfer9350Spectrum(SHORT n, SHORT mem, SHORT HUGE *AY, int ox)
{
     PCFTFrame PARENT = (PCFTFrame)Parent;
     CHAR CMD[40];
     CHAR buf[40];
     LONG NP;
     CHAR EText[80];
     CHAR ETitle[40];

     PARENT->TR->WriteStr("CORD LO; WFSU SP, 0, NP, 0, FP, 0, SN, 0; CHDR SHORT");
     if (PARENT->TR->GetLastCount() < 52) goto L1;

     sprintf(CMD, "%s:WF? DAT1", MEM[mem]);
     PARENT->TR->WriteStr(CMD);
     if (PARENT->TR->GetLastCount() < 11) goto L1;
     PARENT->TR->Read(buf, 22);
     buf[39] = '\0';
     if (PARENT->TR->GetLastCount() < 22) goto L2;
     if (PARENT->WD->FirstValidPnt > 0L) {
	PARENT->TR->Read((CHAR HUGE *)AY, PARENT->WD->FirstValidPnt*sizeof(SHORT));
	if (PARENT->TR->GetLastCount() < PARENT->WD->FirstValidPnt*sizeof(SHORT)) goto L2;
     }

     NP = PARENT->WD->LastValidPnt-PARENT->WD->FirstValidPnt+1L;
     PARENT->TR->Read((CHAR HUGE *)AY, NP*sizeof(SHORT));
     if (PARENT->TR->GetLastCount() < NP*sizeof(SHORT)) goto L2;

     PARENT->TR->WriteStr("CORD LO; WFSU SP, 0, NP, 0, FP, 0, SN, 0; CHDR SHORT");
     if (PARENT->TR->GetLastCount() < 52) goto L1;

     return TRUE;

L1 : PARENT->TR->WriteStr("WFSU SP, 0, NP, 0, FP, 0, SN, 0; CHDR SHORT");
     LoadString(GetApplication()->hInstance, IDS_GWRITEERRORTEXT, EText, 79);
     LoadString(GetApplication()->hInstance, IDS_GWRITEERRORTITLE, ETitle, 39);
     if (ox) MessageBox(NULL, EText, ETitle, MB_OK | MB_ICONSTOP);
     return FALSE;

L2 : PARENT->TR->WriteStr("WFSU SP, 0, NP, 0, FP, 0, SN, 0; CHDR SHORT");
     LoadString(GetApplication()->hInstance, IDS_GREADERRORTEXT, EText, 79);
     LoadString(GetApplication()->hInstance, IDS_GREADERRORTITLE, ETitle, 39);
     if (ox) MessageBox(NULL, EText, ETitle, MB_OK | MB_ICONSTOP);
     return FALSE;
}

BOOL SpectrumWindow::Transfer9350WaveDesc(SHORT n, SHORT mem, int ox)
{
     PCFTFrame PARENT = (PCFTFrame)Parent;
     CHAR CMD[40];
     CHAR buf[40];
     CHAR str[80];
     LONG NP;
     CHAR EText[80];
     CHAR ETitle[40];

     if (!PARENT->GPIBOX || (mem < 0)) goto L0;
     PARENT->TR->WriteStr("CORD LO; WFSU SP, 0, NP, 0, FP, 0, SN, 0; CHDR SHORT");
     if (PARENT->TR->GetLastCount() < 52) goto L1;

     sprintf(CMD, "%s:WF? DESC", MEM[mem]);
     PARENT->TR->WriteStr(CMD);
     if (PARENT->TR->GetLastCount() < 11) goto L1;
     PARENT->TR->Read(buf, 22);
     buf[39] = '\0';
     if (PARENT->TR->GetLastCount() < 22) goto L2;
     PARENT->TR->Read((CHAR HUGE *)PARENT->WD, (DWORD)sizeof(WAVEDESC));
     if (PARENT->TR->GetLastCount() < sizeof(WAVEDESC)) goto L2;

     if (!UpdateMemory(n, PARENT->WD->WaveArray1+10L)) goto L3;
     ClearVariables(n);

     NP = PARENT->WD->LastValidPnt-PARENT->WD->FirstValidPnt+1L;

     PARENT->TR->WriteStr("CORD LO; WFSU SP, 0, NP, 0, FP, 0, SN, 0; CHDR SHORT");
     if (PARENT->TR->GetLastCount() < 52) goto L1;

     memcpy(EX[n], PARENT->EXSTD, sizeof(EXPERIMENTINFO));
     DefaultSpectrumHead(n);
     SP[n]->yGain    = -(REAL)PARENT->WD->VerticalGain*1.0E+3;
     SP[n]->yOffset  =  (REAL)PARENT->WD->VerticalOffset*1.0E+3;
     SP[n]->xGain    =  (REAL)PARENT->WD->HorizInterval*1.0E+6;
     SP[n]->xOffset  =  SP[n]->xGain*(REAL)PARENT->WD->FirstValidPnt+(REAL)PARENT->WD->HorizOffset*1.0E+6;
     lstrcpy(SP[n]->FileName, "Untitled.C11");
     SP[n]->nFileOffset = 0;
     sprintf(str,"%02d/%02d/%02d", (SHORT)PARENT->WD->TriggerTime.Days,
				   (SHORT)PARENT->WD->TriggerTime.Months,
				   (SHORT)(PARENT->WD->TriggerTime.Year % 100));
     lstrcpy(SP[n]->Date, str);
     sprintf(str,"%02d:%02d:%02d", (SHORT)PARENT->WD->TriggerTime.Hours,
				   (SHORT)PARENT->WD->TriggerTime.Minutes,
				   (SHORT)PARENT->WD->TriggerTime.Seconds);
     lstrcpy(SP[n]->Time, str);
     SP[n]->Ntot = NP;
     SP[n]->NoCalib =  SP[n]->NoCursorText = SP[n]->NoMark = 0;
     lstrcpy(SP[n]->TS.Title,"");

     FileType[n] = OTHERFILETYPE;
     SaveOX[n]   = FALSE;
     LoadOX[n] = TRUE;

     memcpy(EX[n], ((PCFTFrame)Parent)->EXSTD, sizeof(EXPERIMENTINFO));

     return TRUE;

L0 : 
     LoadString(GetApplication()->hInstance, IDS_CONNECTERRORTEXT, EText, 79);
     LoadString(GetApplication()->hInstance, IDS_CONNECTERRORTITLE, ETitle, 39);
     if (ox) MessageBox(NULL, EText, ETitle, MB_OK | MB_ICONSTOP);
     return FALSE;

L1 : PARENT->TR->WriteStr("WFSU SP, 0, NP, 0, FP, 0, SN, 0; CHDR SHORT");
     LoadString(GetApplication()->hInstance, IDS_GWRITEERRORTEXT, EText, 79);
     LoadString(GetApplication()->hInstance, IDS_GWRITEERRORTITLE, ETitle, 39);
     if (ox) MessageBox(NULL, EText, ETitle, MB_OK | MB_ICONSTOP);
     return FALSE;

L2 : PARENT->TR->WriteStr("WFSU SP, 0, NP, 0, FP, 0, SN, 0; CHDR SHORT");
     LoadString(GetApplication()->hInstance, IDS_GREADERRORTEXT, EText, 79);
     LoadString(GetApplication()->hInstance, IDS_GREADERRORTITLE, ETitle, 39);
     if (ox) MessageBox(NULL, EText, ETitle, MB_OK | MB_ICONSTOP);
     return FALSE;

L3 : PARENT->TR->WriteStr("WFSU SP, 0, NP, 0, FP, 0, SN, 0; CHDR SHORT");
     LoadString(GetApplication()->hInstance, IDS_MEMALLOCERRORTEXT, EText, 79);
     LoadString(GetApplication()->hInstance, IDS_MEMALLOCERRORTITLE, ETitle, 39);
     if (ox) MessageBox(NULL, EText, ETitle, MB_OK | MB_ICONSTOP);
     return FALSE;
}


