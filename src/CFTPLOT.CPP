#include "cft.h"
#include "cftbmp.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

REAL     REF[5] = { 1.0, 2.0, 4.0, 5.0, 8.0 };

void SpectrumWindow::PlotSpectrum(HDC hdc, SHORT n)
{
   if (PL[n]->Ntot > 0) {
      SHORT XX, YY;
      REAL  y;
      LONG  Ntot    = PL[n]->Ntot;
      LONG  Initial = PL[n]->Initial;
      REAL  RatioW  = (REAL)(PS.Plot.right - PS.Plot.left)/(PL[n]->xMax - PL[n]->xMin);
      REAL  RatioH  = (REAL)(PS.Plot.bottom - PS.Plot.top)/(PL[n]->yMax - PL[n]->yMin);
      REAL  TOP     = PS.Rect.top+PS.Plot.top;
      REAL  BOTTOM  = PS.Rect.top+PS.Plot.bottom;
      REAL  LEFT    = PS.Rect.left+PS.Plot.left;
      REAL  DTOP    = (REAL)TOP;
      REAL  DBOTTOM = (REAL)BOTTOM;
      REAL  YGAIN   = -SP[n]->yGain*RatioH;
      REAL  YOFFSET = (PL[n]->yMin-SP[n]->yOffset)*RatioH+DBOTTOM;
      REAL  XGAIN   = SP[n]->xGain*RatioW;
      REAL  XOFFSET = (SP[n]->xOffset-PL[n]->xMin)*RatioW;

      HPEN  hpen = CreatePen(PS_SOLID, PS.PlotThick, ((PCFTFrame)Parent)->COLOR[n]);
      HPEN  hPenOld = (HPEN)SelectObject(hdc, hpen);

      SHORT OldXX, OldYY, MinYY, MaxYY;
      y = YGAIN*(REAL)Y[n][Initial]+YOFFSET;
      OldYY = MinYY = MaxYY = YY = (y < DTOP) ? TOP : (y > DBOTTOM) ? BOTTOM : (SHORT)ROUND(y);
      OldXX = XX = LEFT + (SHORT)ROUND(XGAIN*(REAL)Initial+XOFFSET);
      for (long i = Initial+1; i < Initial+Ntot; i++) {
	  y = YGAIN*(REAL)Y[n][i]+YOFFSET;
	  YY = (y < DTOP) ? TOP : (y > DBOTTOM) ? BOTTOM : (SHORT)ROUND(y);
	  XX = LEFT + (SHORT)ROUND(XGAIN*(REAL)i+XOFFSET);
	  if (XX != OldXX) {
	     MoveTo(hdc, OldXX, MaxYY);
	     LineTo(hdc, OldXX, MinYY-1);
	     MoveTo(hdc, OldXX, OldYY);
	     LineTo(hdc, XX, YY);
	     OldXX = XX;
             OldYY = MinYY = MaxYY = YY;
	  } else {
	     OldXX = XX;
             OldYY = YY;
	     if (MinYY > YY) MinYY = YY;
	     if (MaxYY < YY) MaxYY = YY;
	     if (i == Initial+Ntot-1) {
		MoveTo(hdc, XX, MaxYY);
	        LineTo(hdc, XX, MinYY-1);
             }
          }
      }

      /*
      y = YGAIN*(REAL)Y[n][Initial]+YOFFSET;
      YY = (y < DTOP) ? TOP : (y > DBOTTOM) ? BOTTOM : (SHORT)ROUND(y);
      XX = LEFT + (SHORT)ROUND(XGAIN*(REAL)Initial+XOFFSET);
      MoveTo(hdc, XX, YY);
      for (long i = Initial+1; i < Initial+Ntot; i++) {
	  y = YGAIN*(REAL)Y[n][i]+YOFFSET;
	  YY = (y < DTOP) ? TOP : (y > DBOTTOM) ? BOTTOM : (SHORT)ROUND(y);
	  XX = LEFT + (SHORT)ROUND(XGAIN*(REAL)i+XOFFSET);
	  LineTo(hdc, XX, YY);
      } */
      DeleteObject(SelectObject(hdc, hPenOld));
   }
}

void SpectrumWindow::PlotGrid(HDC hdc, SHORT n, DEVICETYPE DT)
{
     SHORT XX, YY, i;
     REAL  xx, yy;
     SHORT Xo = PS.Rect.left;
     SHORT Yo = PS.Rect.top;
     REAL  RatioW  = (REAL)(PS.Plot.right - PS.Plot.left)/(PL[n]->xMax - PL[n]->xMin);
     REAL  RatioH  = (REAL)(PS.Plot.bottom - PS.Plot.top)/(PL[n]->yMax - PL[n]->yMin);
     REAL  TOP     = PS.Rect.top+PS.Plot.top;
     REAL  BOTTOM  = PS.Rect.top+PS.Plot.bottom;
     REAL  LEFT    = PS.Rect.left+PS.Plot.left;
     REAL  RIGHT   = PS.Rect.left+PS.Plot.right;
     REAL  DW      = (PL[n]->xMax - PL[n]->xMin)/4.0;
     REAL  DH      = (PL[n]->yMax - PL[n]->yMin)/4.0;
     REAL  wIndex  = floor(log10(DW));
     REAL  hIndex  = floor(log10(DH));
     REAL  W10     = exp(wIndex*log(10.0));
     REAL  H10     = exp(hIndex*log(10.0));
     REAL  YOFFSET = -PL[n]->yMin*RatioH;
     REAL  XOFFSET = -PL[n]->xMin*RatioW;
     CHAR  STR[20];
     CHAR  Format[10];
     COLORREF Color;

     DW /= W10;
     DH /= H10;

     if (DW < 1.0) DW = 1.0;
     if (DH < 1.0) DH = 1.0;

     for (i = 1; i < 5; i++)
	 if (DW < REF[i]) break;
     DW = REF[i-1]/2.0*W10;

     for (i = 1; i < 5; i++)
	 if (DH < REF[i]) break;
     DH = REF[i-1]/2.0*H10;

     HPEN  hpen = CreatePen(PS_SOLID, PS.GridThick, RGB(0,0,0));
     HPEN  hPenOld = (HPEN)SelectObject(hdc, hpen);
     HFONT hf;

     if (DT == PRINTER)
	hf = CreateFont(PS.FontHeight, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0,
			   ((PCFTFrame)Parent)->NormalFont);
     else hf = CreateFont((SHORT)ROUND((REAL)PS.FontHeight*1.33), 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0,
			   ((PCFTFrame)Parent)->NormalFont);
     
     SelectObject(hdc, hf);

     SHORT wGrid0   = PS.xGridW;
     SHORT wGrid1   = (SHORT)ROUND((REAL)PS.xGridW*2.0/3.0);
     SetTextAlign(hdc, TA_CENTER);
     Color = SetTextColor(hdc, RGB(0,0,0));
     if ( wIndex < 0.0)  sprintf(Format, "%%.%df", -(SHORT)wIndex);
     else lstrcpy(Format, "%.0f");
     for (i = (SHORT)ceil(PL[n]->xMin/DW); 1; i++) {
         xx = DW*(REAL)i;
	 if (xx <= PL[n]->xMax) {
	    XX = LEFT + (SHORT)ROUND(RatioW*xx+XOFFSET);
	    MoveTo(hdc, XX, BOTTOM+1);
	    if (i%2 == 0) {
	       LineTo(hdc, XX, BOTTOM+1+wGrid0);
	       sprintf(STR, Format, xx);
	       TextOut(hdc, XX, Yo+PS.xValue, STR, strlen(STR));
	    } else LineTo(hdc, XX, BOTTOM+1+wGrid1);
	    MoveTo(hdc, XX, TOP-1);
	    if (i%2 == 0) LineTo(hdc, XX, TOP-1-wGrid0/2);
	    else LineTo(hdc, XX, TOP-1-wGrid1/2);
	 } else break;
     }

     wGrid0   = PS.yGridW;
     wGrid1   = (SHORT)ROUND((REAL)PS.yGridW*2.0/3.0);
     SetTextAlign(hdc, TA_RIGHT);
     if ( hIndex < 0.0)  sprintf(Format, "%%.%df", -(SHORT)hIndex);
     else lstrcpy(Format, "%.0f");
     for (i = (SHORT)ceil(PL[n]->yMin/DH); 1; i++) {
         yy = DH*(REAL)i;
	 if (yy <= PL[n]->yMax) {
	    YY = BOTTOM - (SHORT)ROUND(RatioH*yy+YOFFSET);
	    MoveTo(hdc, LEFT-1, YY);
	    if (i%2 == 0) {
	       LineTo(hdc, LEFT-1-wGrid0, YY);
	       sprintf(STR, Format, yy);
	       TextOut(hdc,  Xo+PS.yValue, YY-(SHORT)ROUND((REAL)PS.FontHeight/2.0),
	                     STR, strlen(STR));
	    } else LineTo(hdc, LEFT-1-wGrid1, YY);
	    MoveTo(hdc, RIGHT+1, YY);
	    if (i%2 == 0)  LineTo(hdc, RIGHT+1+wGrid0/2, YY);
	    else LineTo(hdc, RIGHT+1+wGrid1/2, YY);
	 } else break; 
     }

     SetTextColor(hdc, Color);
     DeleteObject(SelectObject(hdc, hPenOld));
     DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
}

void SpectrumWindow::GetDevicePlotStruct()
{
     REAL xRatio = (REAL)(PS.Rect.right-PS.Rect.left)/(REAL)(((PCFTFrame)Parent)->PS.Rect.right);
     REAL yRatio = (REAL)(PS.Rect.bottom-PS.Rect.top)/(REAL)(((PCFTFrame)Parent)->PS.Rect.bottom);
     REAL Ratio  = min(xRatio, yRatio);

     PS.Date.x      = (int)ROUND((REAL)(((PCFTFrame)Parent)->PS.Date.x)*xRatio);
     PS.Date.y      = (int)ROUND((REAL)(((PCFTFrame)Parent)->PS.Date.y)*yRatio);
     PS.Element.x   = (int)ROUND((REAL)(((PCFTFrame)Parent)->PS.Element.x)*xRatio);
     PS.Element.y   = (int)ROUND((REAL)(((PCFTFrame)Parent)->PS.Element.y)*yRatio);
     PS.Plot.left   = (int)ROUND((REAL)(((PCFTFrame)Parent)->PS.Plot.left)*xRatio);
     PS.Plot.top    = (int)ROUND((REAL)(((PCFTFrame)Parent)->PS.Plot.top)*yRatio);
     PS.Plot.right  = (int)ROUND((REAL)(((PCFTFrame)Parent)->PS.Plot.right)*xRatio);
     PS.Plot.bottom = (int)ROUND((REAL)(((PCFTFrame)Parent)->PS.Plot.bottom)*yRatio);
     PS.Title.x = (int)ROUND((REAL)(((PCFTFrame)Parent)->PS.Title.x)*xRatio);
     PS.Title.y = (int)ROUND((REAL)(((PCFTFrame)Parent)->PS.Title.y)*yRatio);
     PS.fName.x = (int)ROUND((REAL)(((PCFTFrame)Parent)->PS.fName.x)*xRatio);
     PS.fName.y = (int)ROUND((REAL)(((PCFTFrame)Parent)->PS.fName.y)*yRatio);
     PS.xUnit.x = (int)ROUND((REAL)(((PCFTFrame)Parent)->PS.xUnit.x)*xRatio);
     PS.xUnit.y = (int)ROUND((REAL)(((PCFTFrame)Parent)->PS.xUnit.y)*yRatio);
     PS.yUnit.x = (int)ROUND((REAL)(((PCFTFrame)Parent)->PS.yUnit.x)*xRatio);
     PS.yUnit.y = (int)ROUND((REAL)(((PCFTFrame)Parent)->PS.yUnit.y)*yRatio);
     PS.xGridW  = (int)ROUND((REAL)(((PCFTFrame)Parent)->PS.xGridW)*yRatio);
     PS.yGridW  = (int)ROUND((REAL)(((PCFTFrame)Parent)->PS.yGridW)*xRatio);
     PS.xValue  = (int)ROUND((REAL)(((PCFTFrame)Parent)->PS.xValue)*yRatio);
     PS.yValue  = (int)ROUND((REAL)(((PCFTFrame)Parent)->PS.yValue)*xRatio);
     PS.PlotThick  = (int)floor((REAL)(((PCFTFrame)Parent)->PS.PlotThick)*Ratio);
     if (PS.PlotThick == 0) PS.PlotThick = 1;
     PS.FrameThick = (int)floor((REAL)(((PCFTFrame)Parent)->PS.FrameThick)*Ratio);
     if (PS.FrameThick == 0) PS.FrameThick = 1;
     PS.GridThick  = (int)floor((REAL)(((PCFTFrame)Parent)->PS.GridThick)*Ratio);
     if (PS.GridThick == 0) PS.GridThick = 1;
     PS.FontHeight = (int)ceil((REAL)(((PCFTFrame)Parent)->PS.FontHeight)*Ratio);
     if (PS.FontHeight == 0) PS.FontHeight = 1;
}

void SpectrumWindow::PlotTitle(HDC hdc, SHORT n, DEVICETYPE DT)
{
     SHORT Xo = PS.Rect.left;
     SHORT Yo = PS.Rect.top;
     COLORREF Color;

     HFONT hf;
     if (DT == PRINTER)
	hf = CreateFont(PS.FontHeight, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0,
			   ((PCFTFrame)Parent)->NormalFont);
     else hf = CreateFont((SHORT)ROUND((REAL)PS.FontHeight*1.33), 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0,
			   ((PCFTFrame)Parent)->NormalFont);
     HFONT vf;
     if (DT == PRINTER)
	vf = CreateFont(PS.FontHeight, 0, 900, 900, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0,
			   ((PCFTFrame)Parent)->NormalFont);
     else vf = CreateFont((SHORT)ROUND((REAL)PS.FontHeight*1.33), 0, 900, 900, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0,
			   ((PCFTFrame)Parent)->NormalFont);

     SelectObject(hdc, hf);
     SetTextAlign(hdc, TA_LEFT);
     Color = SetTextColor(hdc, ((PCFTFrame)Parent)->COLOR[n]);
     TextOut(hdc, Xo+PS.Title.x, Yo+PS.Title.y, SP[n]->TS.Title, strlen(SP[n]->TS.Title));
     TextOut(hdc, Xo+PS.Date.x, Yo+PS.Date.y, SP[n]->Date, strlen(SP[n]->Date));
     TextOut(hdc, Xo+PS.Element.x, Yo+PS.Element.y, EX[n]->Element, strlen(EX[n]->Element));
     SetTextAlign(hdc, TA_RIGHT);
     TextOut(hdc, Xo+PS.fName.x, Yo+PS.fName.y, SP[n]->FileName+SP[n]->nFileOffset,
		  strlen(SP[n]->FileName+SP[n]->nFileOffset));
     SetTextAlign(hdc, TA_CENTER);
     SetTextColor(hdc, RGB(0,0,0));
     TextOut(hdc, Xo+PS.xUnit.x, Yo+PS.xUnit.y, SP[n]->TS.XUnit, strlen(SP[n]->TS.XUnit));
     DeleteObject(SelectObject(hdc, vf));
     TextOut(hdc, Xo+PS.yUnit.x, Yo+PS.yUnit.y, SP[n]->TS.YUnit, strlen(SP[n]->TS.YUnit));
     
     SetTextColor(hdc, Color);
     DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
}

void SpectrumWindow::DrawCursor(HDC hdc, SHORT n, WHICHCURSOR wc, BOOL OX)
{
     SelectObject(hdc, GetStockObject(WHITE_PEN));
     if (GetROP2(hdc) != R2_XORPEN) SetROP2(hdc, R2_XORPEN);

     REAL  hdcW    = (REAL)(PS.Plot.right - PS.Plot.left);
     REAL  hdcH    = (REAL)(PS.Plot.bottom - PS.Plot.top);
     REAL  dataW   = PL[n]->xMax - PL[n]->xMin;
     REAL  dataH   = PL[n]->yMax - PL[n]->yMin;

     SHORT N = (wc == CENTER_CURSOR) ? 0 : (wc == LEFT_CURSOR) ? 1 : 2;
     LONG  CP = CursorPos[n][N];

     if ((CP >= PL[n]->Initial) && (CP < (PL[n]->Initial+PL[n]->Ntot))) {
        REAL xx = SP[n]->xGain*(REAL)CP+SP[n]->xOffset;
        SHORT XX = PS.Rect.left + PS.Plot.left + (SHORT)ROUND((xx-PL[n]->xMin)*hdcW/dataW);

        MoveTo(hdc, XX, PS.Rect.top+PS.Plot.top+1);
        LineTo(hdc, XX, PS.Rect.top+PS.Plot.bottom);
                
        REAL yy = SP[n]->yGain*(REAL)Y[n][CP]+SP[n]->yOffset;
	if ((yy > PL[n]->yMin) && (yy < PL[n]->yMax)) {
	   REAL  y  = (REAL)(PS.Rect.top+PS.Plot.bottom)-(yy-PL[n]->yMin)*hdcH/dataH;
	   SHORT YY = (SHORT)ROUND(y);
	   MoveTo(hdc, XX-PS.yGridW, YY-PS.xGridW*2);
	   LineTo(hdc, XX, YY);
	   LineTo(hdc, XX+PS.yGridW, YY-PS.xGridW*2);
	   LineTo(hdc, XX+PS.yGridW, YY-PS.xGridW*2-1);
	}

	CHAR str[60];
	if (OX) {
	   if ((SP[n]->NoCalib > 3) || (SP[n]->NoCalib == -1))
	      sprintf(str, "X = %.3f   M = %.3f     Y = %.3f", xx, CalculMass(n, CP), yy);
	   else sprintf(str, "X = %.3f     Y = %.3f", xx, yy);
	   CCValue->SetText(str);
	}
     }
}

void SpectrumWindow::DrawMark(HDC hdc, SHORT n, SHORT nm)
{
     SelectObject(hdc, GetStockObject(WHITE_PEN));
     SelectObject(hdc, GetStockObject(WHITE_BRUSH));
     if (GetROP2(hdc) != R2_XORPEN) SetROP2(hdc, R2_XORPEN);

     REAL  hdcW    = (REAL)(PS.Plot.right - PS.Plot.left);
     REAL  hdcH    = (REAL)(PS.Plot.bottom - PS.Plot.top);
     REAL  dataW   = PL[n]->xMax - PL[n]->xMin;
     REAL  dataH   = PL[n]->yMax - PL[n]->yMin;

     LONG  CP = MK[n][nm];

     if ((CP >= PL[n]->Initial) && (CP < (PL[n]->Initial+PL[n]->Ntot))) {
        REAL xx = SP[n]->xGain*(REAL)CP+SP[n]->xOffset;
        SHORT XX = PS.Rect.left + PS.Plot.left + (SHORT)ROUND((xx-PL[n]->xMin)*hdcW/dataW);

        REAL yy = SP[n]->yGain*(REAL)Y[n][CP]+SP[n]->yOffset;
	if ((yy > PL[n]->yMin) && (yy < PL[n]->yMax)) {
	   REAL  y  = (REAL)(PS.Rect.top+PS.Plot.bottom)-(yy-PL[n]->yMin)*hdcH/dataH;
	   SHORT YY = (SHORT)ROUND(y);
	   MoveTo(hdc, XX, YY-4*PS.xGridW);
	   LineTo(hdc, XX, YY);
	}
     }
}

void SpectrumWindow::CursorTextOut(HDC hdc, SHORT n, SHORT No, UINT, DEVICETYPE DT)
{
     HFONT hf;
     // HFONT hfOld;
     HPEN  hPen;
     HPEN  hPenOld;
     int   BkModeOld;
     int   TAOld;
     COLORREF ColorOld;
     SHORT FontH  = (DT == PRINTER) ? (SHORT)ROUND((REAL)PS.FontHeight*2.0/3.0)
				    : (SHORT)ROUND((REAL)PS.FontHeight*2.0/3.0*1.33);

   /*  if ((flags & TE_BEGIN) != 0) { */
	hf = CreateFont(FontH, 0, 900, 900, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0,
			     ((PCFTFrame)Parent)->NormalFont);
        SelectObject(hdc, hf);
	hPen = CreatePen(PS_SOLID, PS.PlotThick, ((PCFTFrame)Parent)->COLOR[n]);
	hPenOld = (HPEN)SelectObject(hdc, hPen);
	BkModeOld = SetBkMode(hdc, TRANSPARENT);
	TAOld = SetTextAlign(hdc, TA_LEFT | TA_TOP);
        ColorOld = SetTextColor(hdc, ((PCFTFrame)Parent)->COLOR[n]);
    /* } */

     if (GetROP2(hdc) != R2_COPYPEN) SetROP2(hdc, R2_COPYPEN);

     REAL  hdcW    = (REAL)(PS.Plot.right - PS.Plot.left);
     REAL  hdcH    = (REAL)(PS.Plot.bottom - PS.Plot.top);
     REAL  dataW   = PL[n]->xMax - PL[n]->xMin;
     REAL  dataH   = PL[n]->yMax - PL[n]->yMin;
     SHORT Top    = PS.Rect.top+PS.Plot.top;
     SHORT Bottom = PS.Rect.top+PS.Plot.bottom;
     SHORT LineH  = PS.xGridW*3;
     SHORT Space  = PS.xGridW;

     LONG  CP = TE[n][No].Position;

     if ((CP >= PL[n]->Initial) && (CP < (PL[n]->Initial+PL[n]->Ntot))) {
        REAL  xx = SP[n]->xGain*(REAL)CP+SP[n]->xOffset;
	SHORT XX = PS.Rect.left + PS.Plot.left + (SHORT)ROUND((xx-PL[n]->xMin)*hdcW/dataW);
	SHORT XT = XX-FontH/2;
	REAL  yy = SP[n]->yGain*(REAL)Y[n][CP]+SP[n]->yOffset;
	REAL  y  = (REAL)(PS.Rect.top+PS.Plot.bottom)-(yy-PL[n]->yMin)*hdcH/dataH;
	SHORT YY = (y > (REAL)Bottom) ? Bottom : (y < (REAL)Top) ? Top : (SHORT)ROUND(y);
	DWORD TEXTLEN = GetTextExtent(hdc, TE[n][No].TextStr, lstrlen(TE[n][No].TextStr));
        SHORT TextW   = (SHORT)LOWORD(TEXTLEN);

	if (YY >= (Top+TextW+2*Space+LineH)) {
	   MoveTo(hdc, XX, YY-Space);
	   LineTo(hdc, XX, YY-Space-LineH);
	   MoveTo(hdc, XX-PS.yGridW/2, YY-Space-PS.xGridW);
	   LineTo(hdc, XX, YY-Space);
	   LineTo(hdc, XX+PS.yGridW/2, YY-Space-PS.xGridW);
	   LineTo(hdc, XX+PS.yGridW/2, YY-Space-PS.xGridW+1);
	   TextOut(hdc, XT, YY-2*Space-LineH, TE[n][No].TextStr, strlen(TE[n][No].TextStr));
	} else if (YY >= (Top+2*Space+LineH)) {
	   MoveTo(hdc, XX, YY-Space);
	   LineTo(hdc, XX, YY-Space-LineH);
	   MoveTo(hdc, XX-PS.yGridW/2, YY-Space-PS.xGridW);
	   LineTo(hdc, XX, YY-Space);
	   LineTo(hdc, XX+PS.yGridW/2, YY-Space-PS.xGridW);
	   LineTo(hdc, XX+PS.yGridW/2, YY-Space-PS.xGridW+1);
	   TextOut(hdc, XT, YY+LineH+TextW, TE[n][No].TextStr, strlen(TE[n][No].TextStr));
        } else {
	   MoveTo(hdc, XX, YY+Space);
	   LineTo(hdc, XX, YY+Space+LineH);
	   MoveTo(hdc, XX-PS.yGridW/2, YY+Space+PS.xGridW);
	   LineTo(hdc, XX, YY+Space);
	   LineTo(hdc, XX+PS.yGridW/2, YY+Space+PS.xGridW);
	   LineTo(hdc, XX+PS.yGridW/2, YY+Space+PS.xGridW+1);
	   TextOut(hdc, XT, YY+2*Space+LineH+TextW, TE[n][No].TextStr, strlen(TE[n][No].TextStr));
	}
     }

  /*   if ((flags & TE_END) != 0) { */
	DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
	DeleteObject(SelectObject(hdc, hPenOld));
        SetBkMode(hdc, BkModeOld);
	SetTextAlign(hdc, TAOld);
        SetTextColor(hdc, ColorOld);
/*     } */
}

void CFTFrame::BeginFrame(HDC hdc, RECT rc)
{
     Bitmap *BT = new Bitmap(GetApplication()->hInstance, "CLUSTER");
     BT->Display(hdc, rc.right/2-90, rc.bottom/2-120);
     delete BT;
     DWORD Ti = GetTickCount();
     while (GetTickCount() < (Ti+2000L));
     IsFirst = FALSE; 
}

void SpectrumWindow::RedrawCursor(SHORT n, WHICHCURSOR wc, LONG Pos)
{
     HDC hdc = GetDC(HWindow);
     if (hdc) {
        SHORT N = (wc == CENTER_CURSOR) ? 0 : (wc == LEFT_CURSOR) ? 1 : 2;
        SaveDC(hdc); 
        DrawCursor(hdc, n, wc, FALSE);
        CursorPos[n][N] = Pos;
        DrawCursor(hdc, n, wc, TRUE);
	RestoreDC(hdc, -1);
	ReleaseDC(HWindow, hdc);
     }
}

POINT Bitmap::GetSize(HDC hDC)
{
     BITMAP bm;
     POINT ptSize;

     GetBitmap(&bm);
     ptSize.x = bm.bmWidth;
     ptSize.y = bm.bmHeight;
     DPtoLP(hDC, &ptSize, 1);
     return ptSize; 
}

void Bitmap::Display( HDC hDC, short xStart, short yStart )
{
    POINT ptSize, ptOrigin;
    HBITMAP hBitmapOld;

    CompatibleDC MemoryDC(hDC);
    HDC hDCMem = MemoryDC.GetHDC();
    hBitmapOld = (HBITMAP)SelectObject(hDCMem, hBitmap);

    ptSize = GetSize(hDC);
    ptOrigin.x = ptOrigin.y = 0;
    DPtoLP(hDCMem, &ptOrigin, 1);

    BitBlt(hDC, xStart, yStart, ptSize.x, ptSize.y, hDCMem, ptOrigin.x, ptOrigin.y, SRCCOPY);
    SelectObject(hDCMem, hBitmapOld);
    DeleteObject(hBitmapOld);
}