#include "cft.h"
#include "cftdlg.h"
#include <stdlib.h>
#include <string.h>
#include <commdlg.h>
#include <time.h>
#include <math.h>
#include <mem.h>
#include <dos.h>
#include <dir.h>

CHAR  DirName[256] = "C:\\";
CHAR  LECROY9350CODE[7] = {0x23, 0x39, 0x30, 0x30, 0x30, };
CHAR  LECROY9400CODE[10] = {0x10, 0x00, 0x03, 0x01, 0x01, 0x01, 0x00 };

void SpectrumWindow::ClearVariables(SHORT Spec)
{
     memset(SP[Spec], 0, sizeof(SPECTRUMINFO));
     memset(PL[Spec], 0, sizeof(PLOTINFO));
     memset(EX[Spec], 0, sizeof(EXPERIMENTINFO));
}

BOOL SpectrumWindow::UpdatePlotInfo(SHORT n)
{
     LONG Ntot    = SP[n]->Ntot;
     REAL xGain   = SP[n]->xGain;
     REAL xOffset = SP[n]->xOffset;
     REAL xMin    = PL[n]->xMin;
     REAL xMax    = PL[n]->xMax;

     for (LONG i = 0L; i < Ntot; i++) 
	 if ((xGain*(REAL)i+xOffset) >= xMin) break;

     if (i == Ntot) {
	PL[n]->Initial = PL[n]->Ntot    = 0;
        return FALSE;
     } else {
	for (LONG j = Ntot-1L; j >= i; j--)
	    if ((xGain*(REAL)j+xOffset) <= xMax) break;

	if (j < i) {
	   PL[n]->Initial = PL[n]->Ntot    = 0;
           return FALSE;
        } else {
	   PL[n]->Initial = i;
           PL[n]->Ntot    = j-i+1L;
	   return TRUE;
        }
     } 
}

void SpectrumWindow::DefaultSpectrumHead(SHORT n)
{
     lstrcpy(SP[n]->Desc, DESC110);
     SP[n]->Version = 0x0110;
     SP[n]->NoCalib = SP[n]->NoCursorText = SP[n]->NoMark = 0;
     lstrcpy(SP[n]->TS.XUnit, "T.O.F.(µs)\0");
     lstrcpy(SP[n]->TS.YUnit, "I (mV)\0");
}

void SpectrumWindow::GetDefaultSpectrumMinMax(SHORT n)
{
     REAL I;
     SP[n]->xMin = SP[n]->xOffset;
     SP[n]->xMax = SP[n]->xGain*(REAL)(SP[n]->Ntot-1L) + SP[n]->xOffset;
     SP[n]->yMin = 1.0e300;
     SP[n]->yMax = -1.0e300;
     for (LONG i = 0; i < SP[n]->Ntot; i++) {
         I = SP[n]->yGain*(REAL)Y[n][i] + SP[n]->yOffset;
         if (I < SP[n]->yMin) SP[n]->yMin = I;
         if (I > SP[n]->yMax) SP[n]->yMax = I;
     }
     REAL DW = (SP[n]->yMax-SP[n]->yMin)*0.1;
     if (DW <= 0.0) DW = 1.0;
     SP[n]->yMin -= DW;
     SP[n]->yMax += DW;
}

//  Evaluation of minimum and maximum value of spectra in the window of plot
void SpectrumWindow::GetPlotMinMax(SHORT n)
{
     REAL y;

     if (PL[n]->Ntot > 0) { 
        PL[n]->yMin = 1.0e+300;
        PL[n]->yMax = -1.0e+300;
	for (long i = PL[n]->Initial; i < PL[n]->Initial + PL[n]->Ntot; i++) {
	    y = SP[n]->yGain*(REAL)Y[n][i] + SP[n]->yOffset;
	    if (y > PL[n]->yMax) PL[n]->yMax = y;
	    if (y < PL[n]->yMin) PL[n]->yMin = y;
	}
        REAL H = PL[n]->yMax-PL[n]->yMin;
	PL[n]->yMax += H*0.1;
	PL[n]->yMin -= H*0.1;
     } else {
	PL[n]->yMax = SP[n]->yMax;
	PL[n]->yMin = SP[n]->yMin;
     }
}

//  Initialization of PLOT_INFO variable with default values
void SpectrumWindow::GetDefaultPlotInfo(SHORT n)
{
   PL[n]->Initial = 0L;
   PL[n]->Ntot    = SP[n]->Ntot;
   PL[n]->xMax    = SP[n]->xMax;
   PL[n]->xMin    = SP[n]->xMin;
   PL[n]->yMax    = SP[n]->yMax;
   PL[n]->yMin    = SP[n]->yMin;
}

void SpectrumWindow::InitialCursorState(SHORT n)
{
     CursorPos[n][0] = PL[n]->Initial+PL[n]->Ntot/2L;
     CursorMode[n]   = CU_SINGLE;
}

BOOL SpectrumWindow::GetCFighter(HFILE hf, SHORT n)
{
     if (_lread(hf, SP[n], sizeof(SPECTRUMINFO)) != sizeof(SPECTRUMINFO)) return FALSE;
     if (strcmp(SP[n]->Desc, DESC110) != 0) return FALSE;
     if (_lread(hf, PL[n], sizeof(PLOTINFO)) != sizeof(PLOTINFO)) return FALSE;
     if (_hread(hf, Y[n], SP[n]->Ntot*2L) != SP[n]->Ntot*2L) return FALSE;
     if (_lread(hf, EX[n], sizeof(EXPERIMENTINFO)) != sizeof(EXPERIMENTINFO)) return FALSE;
     if (SP[n]->NoCalib > 0)
     if (_lread(hf, CT[n], sizeof(CALIBTABLE)*SP[n]->NoCalib)
	 != sizeof(CALIBTABLE)*SP[n]->NoCalib) return FALSE;
     if (SP[n]->NoCursorText > 0)
     if (_lread(hf, TE[n], sizeof(CURSORTEXT)*SP[n]->NoCursorText)
	 != sizeof(CURSORTEXT)*SP[n]->NoCursorText) return FALSE;
     if (SP[n]->NoMark > 0)
     if (_lread(hf, MK[n], sizeof(LONG)*SP[n]->NoMark)
	 != sizeof(LONG)*SP[n]->NoMark) return FALSE;
     FileType[n] = CFT110;
     return TRUE;
}

CHAR *GetLeCroy9400Comment(FILE *f, CHAR *str)
{
     SHORT c, i = 0;
     CHAR buf[80];

     c = fgetc(f);
     while ((c != '\x00d') && (c != EOF)) {
        buf[i] = (CHAR)c;
        i++;
        c = fgetc(f);
     }
     if (c == '\x00d') fgetc(f);
     buf[i] = '\0';
     lstrcpy(str, buf);
     return str;
}

BOOL SpectrumWindow::GetLeCroy9400(FILE *f, SHORT n)
{
     SHORT x, y, z[4];
     LONG  ntot, ntot1;
     REAL  gain, vgain, offset, v[3] = {5.0, 10.0, 20.0};
     REAL  t_d,  d31,   r1, tconv[3] = {2.0, 5.0, 10.0};
     CHAR  buf[128];

     ClearVariables(n);
     if ((x = fgetc(f)) == EOF) return FALSE;
     if ((y = fgetc(f)) == EOF) return FALSE;
     ntot = ntot1 = x*0x0100+y;
     if ((x = fgetc(f)) == EOF) return FALSE;
     gain = pow(10.0,(REAL)((x-22)/3))*v[(x-22)%3];
     if ((x = fgetc(f)) == EOF) return FALSE;
     vgain = 0.005*(REAL)x+0.4;
     for (long i = 0; i < 2; i++ ) if ((y = fgetc(f)) == EOF) return FALSE;
     if ((x = fgetc(f)) == EOF) return FALSE;
     if ((y = fgetc(f)) == EOF) return FALSE;
     offset = (REAL)((x*0x0100+y)-200)*0.04;
     for (i = 0; i < 3; i++ ) if ((y = fgetc(f)) == EOF) return FALSE;
     if ((x = fgetc(f)) == EOF) return FALSE;
     t_d = 1.0E-03*pow(10.0,(REAL)((x-4)/3))*tconv[(x-4) % 3];
     
     for (i = 0; i < 7; i++ )
	 if ((y = fgetc(f)) == EOF) return FALSE;
	 else if ((y != LECROY9400CODE[(int)i]) && (i != 2)) return FALSE;
     if ((y = fgetc(f)) == EOF) return FALSE;
     r1 = 0.0;
     d31 = pow(2.0,31.0)-0.1;
     for (i = 1; i <= 4; i++ ) {
	 if ((z[(int)i-1] = fgetc(f)) == EOF) return FALSE;
         r1 +=(REAL)z[(int)i-1]*pow(256.0,(REAL)(4-i));
     }
     if (r1 > d31) r1 = pow(2.0,32.0)-r1;
     else if (r1 < d31) r1 = -r1;
     r1 /= 50.0;
     SP[n]->xOffset = r1*t_d;
     SP[n]->xGain = 10.0*t_d/(REAL)ntot1;
     SP[n]->A.a[0] = SP[n]->A.a[1] = 0.0;
     SP[n]->A.a[2] = 1.0;
     for (i = 22; i < 150; i++) if ((y = fgetc(f)) == EOF) return FALSE;
     for (i = 0; i < ntot; i++) if ((y = fgetc(f)) == EOF) return FALSE;
			        else Y[n][i] = (SHORT)((LONG)y*0x0100L-0x8000L);

     for (i = ntot; i < ntot1; i++) if ((y = fgetc(f)) == EOF) return FALSE;
     for (i = 0; i < ntot; i++)     if ((y = fgetc(f)) == EOF) break;
				    else Y[n][i] += y;

     if (i == 0) return FALSE;
     else {
	DefaultSpectrumHead(n);
	lstrcpy(SP[n]->FileName, FN->FileName);
	SP[n]->nFileOffset = FN->nFileOffset;
        SP[n]->Ntot = i;

        if ((ntot != ntot1) || (i != ntot)) {
           lstrcpy(EX[n]->Element, "");
           lstrcpy(SP[n]->Date, "Not Defined\0");
           lstrcpy(SP[n]->Time, "Not Defined\0");
           lstrcpy(SP[n]->TS.Title,"\0");
        } else {
	   lstrcpy(EX[n]->Commentaire[0], "Spectre enregistré par LeCroy 9400");
	   GetLeCroy9400Comment(f, SP[n]->Date);
	   GetLeCroy9400Comment(f, EX[n]->Element);
	   GetLeCroy9400Comment(f, SP[n]->TS.Title);
	   GetLeCroy9400Comment(f, EX[n]->Ioniseur);
	   GetLeCroy9400Comment(f, buf);
	   EX[n]->Accel1 = atof(buf);
	   GetLeCroy9400Comment(f, buf);
	   EX[n]->Accel2 = atof(buf);
	   GetLeCroy9400Comment(f, buf);
	   EX[n]->Accel3 = atof(buf);
	   GetLeCroy9400Comment(f, buf);
	   GetLeCroy9400Comment(f, buf);
	   GetLeCroy9400Comment(f, EX[n]->Ioniseur1);
	   GetLeCroy9400Comment(f, buf);
	   EX[n]->Four = atof(buf);
	   GetLeCroy9400Comment(f, buf);
	   GetLeCroy9400Comment(f, buf);
	   GetLeCroy9400Comment(f, SP[n]->Time);
        }

	SP[n]->yGain = gain/8192.0/vgain;
	SP[n]->yOffset =  - offset*8192.0*SP[n]->yGain;

	GetDefaultSpectrumMinMax(n);
	GetDefaultPlotInfo(n);
	InitialCursorState(n);
	FileType[n] = LECROY9400;
	return TRUE;
     }
}

// Load Lecroy 9350 Data File
BOOL SpectrumWindow::GetLeCroy9350(HFILE hF, SHORT n)
{
     CHAR buf[15], str[80];
     PCFTFrame PARENT = (PCFTFrame)Parent;

     ClearVariables(n);
     if (_lread(hF, buf, (size_t)11) != (UINT)11) return FALSE;
     for (int j = 0; j < 3; j++)
	 if (buf[j] != LECROY9350CODE[j]) return FALSE;
     if (_lread(hF, PARENT->WD, sizeof(WAVEDESC)) != sizeof(WAVEDESC)) return FALSE;
     LONG Ntot = PARENT->WD->LastValidPnt-PARENT->WD->FirstValidPnt+1L;
     if ((LONG)PARENT->WD->WaveArray1 <= 10) return FALSE;
     if (_hread(hF, Y[n], PARENT->WD->FirstValidPnt*2L) != PARENT->WD->FirstValidPnt*2L) return FALSE;
     if (_hread(hF, Y[n], Ntot*2L) != Ntot*2L) return FALSE;

     DefaultSpectrumHead(n);
     SP[n]->yGain    =  (REAL)PARENT->WD->VerticalGain*1.0E+3;
     SP[n]->yOffset  = -(REAL)PARENT->WD->VerticalOffset*1.0E+3;
     SP[n]->xGain    =  (REAL)PARENT->WD->HorizInterval*1.0E+6;
     SP[n]->xOffset  =  SP[n]->xGain*(REAL)PARENT->WD->FirstValidPnt+(REAL)PARENT->WD->HorizOffset*1.0E+6;
     lstrcpy(SP[n]->FileName, FN->FileName);
     SP[n]->nFileOffset = FN->nFileOffset;
     sprintf(str,"%02d/%02d/%02d", (SHORT)PARENT->WD->TriggerTime.Days,
				   (SHORT)PARENT->WD->TriggerTime.Months,
				   (SHORT)(PARENT->WD->TriggerTime.Year % 100));
     lstrcpy(SP[n]->Date, str);
     sprintf(str,"%02d:%02d:%02d", (SHORT)PARENT->WD->TriggerTime.Hours,
				   (SHORT)PARENT->WD->TriggerTime.Minutes,
				   (SHORT)PARENT->WD->TriggerTime.Seconds);
     lstrcpy(SP[n]->Time, str);
     SP[n]->Ntot = Ntot;
     SP[n]->NoCalib =  SP[n]->NoCursorText = SP[n]->NoMark = 0;
     lstrcpy(SP[n]->TS.Title,"");

     GetDefaultSpectrumMinMax(n);
     GetDefaultPlotInfo(n);
     InitialCursorState(n);
     FileType[n] = LECROY9350;
     return TRUE;
}

// Load Lecroy 9350 Data File
BOOL SpectrumWindow::GetIPN(HFILE hF, SHORT n)
{
     UINT  Result;
     DWORD  Ntot1,i;
     PCFTFrame PARENT = (PCFTFrame)Parent;

     ClearVariables(n);
     Result = _lread(hF, PARENT->IP, sizeof(IPNLAB));
     if (Result != sizeof(IPNLAB)) return FALSE;
     if ((Ntot1 = atol(PARENT->IP->NCX)) <= 0L) return FALSE;
     i = _hread(hF, Y[n], Ntot1*2L);
     if (i <= 0) return FALSE;
     else {
	DefaultSpectrumHead(n);
	SP[n]->yGain    = 1.0;
	SP[n]->yOffset  = 0.0; 
        SP[n]->xGain    = 1.0;
        SP[n]->xOffset  = 1.0;
        lstrcpy(SP[n]->FileName, FN->FileName);
        SP[n]->nFileOffset = FN->nFileOffset;
	SP[n]->Ntot = i/2L;
	SP[n]->NoCalib = SP[n]->NoCursorText = SP[n]->NoMark = 0;
	lstrcpy(SP[n]->TS.Title, PARENT->IP->NOM);
        lstrcpy(SP[n]->TS.XUnit, "Channel");
	lstrcpy(SP[n]->TS.YUnit, "Count");
	strncpy(SP[n]->Date, PARENT->IP->DATE, 10);
	SP[n]->Date[10] = '\0';

	GetDefaultSpectrumMinMax(n);
	GetDefaultPlotInfo(n);
	InitialCursorState(n);
	FileType[n] = IPNDMI400;
	return TRUE;
     }
}

BOOL SpectrumWindow::GetCFT100(HFILE hF, SHORT n)
{
     LONG N;
     PCFTFrame PARENT = (PCFTFrame)Parent;

     ClearVariables(n);
     if (_lread(hF, PARENT->SP, sizeof(SPECTRUMINFO100)) != sizeof(SPECTRUMINFO100)) return FALSE;
     if (PARENT->SP->Version != 0x0100) return FALSE;
     DefaultSpectrumHead(n);
     lstrcpy(SP[n]->FileName, FN->FileName);
     SP[n]->nFileOffset = FN->nFileOffset;
     lstrcpy(SP[n]->Time, PARENT->SP->Time);
     lstrcpy(SP[n]->Date, PARENT->SP->Date);
     lstrcpy(EX[n]->Element, PARENT->SP->Material);
     lstrcpy(SP[n]->TS.Title, PARENT->SP->Title);
     lstrcpy(SP[n]->TS.XUnit, PARENT->SP->xTitle);
     lstrcpy(SP[n]->TS.YUnit, PARENT->SP->yTitle);
     SP[n]->xGain = PARENT->SP->xGain;
     SP[n]->xOffset = PARENT->SP->xOffset;
     SP[n]->yGain = PARENT->SP->yGain;
     SP[n]->yOffset = PARENT->SP->yOffset*PARENT->SP->yGain;
     SP[n]->NoCalib = PARENT->SP->NoCalib;
     SP[n]->NoMark = 0;
     if (_lread(hF, PARENT->PL, sizeof(PLOTINFO100)) != sizeof(PLOTINFO100)) return FALSE;
     if ((N = _hread(hF, Y[n], (LONG)sizeof(SHORT)*PARENT->SP->Ntot))
	<= 10) return FALSE;
     SP[n]->Ntot = N/2;
     GetDefaultSpectrumMinMax(n);
     GetDefaultPlotInfo(n);
     InitialCursorState(n);

     if (N != ((LONG)sizeof(SHORT)*PARENT->SP->Ntot)) {
	SP[n]->NoCalib = SP[n]->NoMark = 0;
        return TRUE;
     } else {
	if (SP[n]->NoCalib > 0)
	if (_lread(hF, CT[n], sizeof(CALIBTABLE)*(size_t)SP[n]->NoCalib)
	   != (sizeof(CALIBTABLE)*(size_t)SP[n]->NoCalib)) {
	   PARENT->SP->NoCalib = 0;
	   return TRUE;
        }
	if (PARENT->SP->Config == CONFIGPRESENT) {
	   if (_lread(hF, PARENT->EX, sizeof(EXPINFO100))
	      != sizeof(EXPINFO100)) {
	      PARENT->SP->Config = CONFIGABSENT;
	      return TRUE;
	   } else { 
              EX[n]->Four = PARENT->EX->Four;
              EX[n]->Couvercle = PARENT->EX->Couvercle;
              EX[n]->He = PARENT->EX->He;
	      EX[n]->Ar = PARENT->EX->Ar;
	      lstrcpy(EX[n]->Other, PARENT->EX->Other);
	      lstrcpy(EX[n]->Ioniseur, PARENT->EX->Ioniseur);
	      EX[n]->IEnergie = PARENT->EX->IEnergie;
	      lstrcpy(EX[n]->IComment, PARENT->EX->IComment);
	      lstrcpy(EX[n]->Ioniseur1, PARENT->EX->Ioniseur1);
	      EX[n]->IEnergie1 = PARENT->EX->IEnergie1;
	      lstrcpy(EX[n]->IComment1, PARENT->EX->IComment1);
	      EX[n]->Accel1 = PARENT->EX->Accel1;
	      EX[n]->Accel2 = PARENT->EX->Accel2;
	      EX[n]->Accel3 = PARENT->EX->Accel3;
	      EX[n]->Lentille1 = PARENT->EX->Lentille1;
              EX[n]->Lentille2 = PARENT->EX->Lentille2;
	      EX[n]->ReflecV1 = PARENT->EX->ReflecV1;
	      EX[n]->ReflecV2 = PARENT->EX->ReflecV2;
	      EX[n]->ReflecV3 = PARENT->EX->ReflecV3;
	      EX[n]->ReflecAngle = PARENT->EX->ReflecAngle;
	      lstrcpy(EX[n]->RComment, PARENT->EX->RComment);
	      lstrcpy(EX[n]->Commentaire[0], PARENT->EX->Commentaire[0]);
	      lstrcpy(EX[n]->Commentaire[1], PARENT->EX->Commentaire[1]);
	      lstrcpy(EX[n]->Commentaire[2], PARENT->EX->Commentaire[2]);
	   }
	}
     }
     FileType[n] = CFT100;
     return TRUE;
}

BOOL SpectrumWindow::GetSpectrum(SHORT n, LPSTR FileName)
{
     FILE  *f;
     HFILE hF;
     BOOL  OX;

     if ((hF = _lopen(FileName, READ)) == HFILE_ERROR) return FALSE;
     OX = GetCFighter(hF, n);
     _lclose(hF);
     if (OX) return TRUE;

     if ((hF = _lopen(FileName, READ)) == HFILE_ERROR) return FALSE;
     OX = GetCFT100(hF, n);
     _lclose(hF);
     if (OX) return TRUE;

     if ((hF = _lopen(FileName, READ)) == HFILE_ERROR) return FALSE;
     OX = GetLeCroy9350(hF, n);
     _lclose(hF);
     if (OX) return TRUE;

     if ((f = fopen(FileName, "rb"))   == NULL)        return FALSE;
     OX = GetLeCroy9400(f, n);
     fclose(f);
     if (OX) return TRUE;

     if ((hF = _lopen(FileName, READ)) == HFILE_ERROR) return FALSE;
     OX = GetIPN(hF, n);
     _lclose(hF);

     return OX;
}

BOOL SpectrumWindow::SaveCFighter(HFILE hf, SHORT n)
{
     if (_lwrite(hf, SP[n], sizeof(SPECTRUMINFO)) != sizeof(SPECTRUMINFO)) return FALSE;
     if (_lwrite(hf, PL[n], sizeof(PLOTINFO)) != sizeof(PLOTINFO)) return FALSE;
     if (_hwrite(hf, Y[n], SP[n]->Ntot*2L) != SP[n]->Ntot*2L) return FALSE;
     if (_lwrite(hf, EX[n], sizeof(EXPERIMENTINFO)) != sizeof(EXPERIMENTINFO)) return FALSE;
     if (SP[n]->NoCalib > 0)
     if (_lwrite(hf, CT[n], sizeof(CALIBTABLE)*SP[n]->NoCalib)
	 != sizeof(CALIBTABLE)*SP[n]->NoCalib) return FALSE;
     if (SP[n]->NoCursorText > 0)
     if (_lwrite(hf, TE[n], sizeof(CURSORTEXT)*SP[n]->NoCursorText)
	 != sizeof(CURSORTEXT)*SP[n]->NoCursorText) return FALSE;
     if (SP[n]->NoMark > 0)
     if (_lwrite(hf, MK[n], sizeof(LONG)*SP[n]->NoMark)
	 != sizeof(LONG)*SP[n]->NoMark) return FALSE;
     SaveOX[n] = TRUE;
     FileType[n] = CFT110;
     return TRUE;
}

BOOL SpectrumWindow::SaveAsSpecial(FILE *f, SHORT n, UINT flags)
{
     FLOAT x, y;
     CHAR  fmt[40];

     if (flags == SA_QPRO) {
		if (fprintf(f, "\nT.O.F.(µs)\;Yield;\n") == EOF) return FALSE;
		lstrcpy(fmt, "%e;%e;\n");
     } else lstrcpy(fmt, "%e\t%e\n");

     if (flags != SA_CALIB) {
        REAL xGain   = SP[n]->xGain;
        REAL xOffSet = SP[n]->xOffset;
        REAL yGain   = SP[n]->yGain;
        REAL yOffSet = SP[n]->yOffset;

	for (LONG i = 0; i < SP[n]->Ntot; i++) {
      	    x = (FLOAT)(xGain*(REAL)i+xOffSet);
	    y = (FLOAT)(yGain*(REAL)Y[n][i]+yOffSet);
	    if (fprintf(f, fmt, x, y) == EOF) return FALSE;
	}
     } else {
	for (SHORT i = 0; i < SP[n]->NoCalib; i++) 
	    if (fprintf(f, fmt, CT[n][i].Time, CT[n][i].Mass) == EOF) return FALSE;
     }
     return TRUE;
}

BOOL SpectrumWindow::GetOpenFile(FILENAMESTRUCT &FS)
{
     OPENFILENAME ofn;
     CHAR  FileTitle[64];
     CHAR  Filter[250] = "Cfighter 1.10 (*.c11)\0*.c11\0Cfighter 1.00  (*.cft)\0*.cft\0"
			 "LeCroy 9350 (*.0*)\0*.0*\0LeCroy 9400 (*.sre)\0*.sre\0"
			 "IPN         (*.0*)\0*.0*\0All file         (*.*) \0*.*\0";

     strcpy(FS.FileName, "");
     memset(&ofn, 0, sizeof(OPENFILENAME));

     ofn.lStructSize       = sizeof(OPENFILENAME);
     ofn.hwndOwner         = HWindow;
     ofn.hInstance         = GetApplication()->hInstance;
     ofn.lpstrFilter       = (LPSTR)Filter;
     ofn.nFilterIndex      = ((PCFTFrame)Parent)->OpenFileFilterIndex;
     ofn.lpstrFile         = FS.FileName;
     ofn.nMaxFile          = sizeof(FS.FileName);
     ofn.lpstrFileTitle    = FileTitle;
     ofn.nMaxFileTitle     = sizeof(FileTitle);
     ofn.lpstrTitle        = FS.Title;
     ofn.lpstrDefExt       = "*";
     ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

     if (GetOpenFileName(&ofn)) {
        ((PCFTFrame)Parent)->OpenFileFilterIndex = ofn.nFilterIndex;
	FS.nFileOffset = ofn.nFileOffset;
	AnsiUpper(FS.FileName);
        struct ffblk ff;
        if (findfirst(FS.FileName, &ff, 0) == 0) {
	   FS.FileSize = (DWORD)ff.ff_fsize;
           return TRUE;
	} else return FALSE;
     } else return FALSE;

}

BOOL SpectrumWindow::GetSaveFile(FILENAMESTRUCT &FS)
{
     OPENFILENAME ofn;
     CHAR  FileTitle[64];
     CHAR  Filter[128] = "Cfighter 1.10 (*.c11)\0*.c11\0All file         (*.*) \0*.*\0";

     memset(&ofn, 0, sizeof(OPENFILENAME));

     ofn.lStructSize       = sizeof(OPENFILENAME);
     ofn.hwndOwner         = HWindow;
     ofn.hInstance         = GetApplication()->hInstance;
     ofn.lpstrFilter       = (LPSTR)Filter;
     ofn.lpstrFile         = FS.FileName;
     ofn.nMaxFile          = sizeof(FS.FileName);
     ofn.lpstrFileTitle    = FileTitle;
     ofn.nMaxFileTitle     = sizeof(FileTitle);
     ofn.lpstrTitle        = FS.Title;
     ofn.lpstrDefExt       = "*";
     ofn.Flags = OFN_OVERWRITEPROMPT;

     if (GetSaveFileName(&ofn)) {
	FS.nFileOffset = ofn.nFileOffset;
	AnsiUpper(FS.FileName);
	return TRUE;
     } else return FALSE;
}

BOOL SpectrumWindow::SaveAsCFighter(SHORT n)
{
     FILENAMESTRUCT FN0, FN1;
     CHAR           *File;
     FILETYPE        FT = FileType[n];

     lstrcpy(FN0.FileName, SP[n]->FileName+SP[n]->nFileOffset);
     if ((FT == LECROY9400) || (FT == LECROY9350)) {
	if ((File = strchr(FN0.FileName, '.')) != NULL)  lstrcpy(File, File+1);
        lstrcat(File, ".C11");
     } else {
	if ((File = strchr(FN0.FileName, '.')) != NULL)  lstrcpy(File, ".C11");
     }
     if (FT == OTHERFILETYPE) sprintf(FN0.FileName, "*.C11");
     sprintf(FN0.Title, "Enregistrer sous ...");
     if (GetSaveFile(FN0)) {
	HFILE    hf;
        OFSTRUCT of;
	if ((hf = OpenFile(FN0.FileName, &of, OF_CREATE)) != HFILE_ERROR) {
	   lstrcpy(FN1.FileName, SP[n]->FileName);
	   FN1.nFileOffset = SP[n]->nFileOffset;
	   lstrcpy(SP[n]->FileName, FN0.FileName);
	   SP[n]->nFileOffset = FN0.nFileOffset;
	   BOOL OX = SaveCFighter(hf, n);
	   _lclose(hf);
	   if (!OX) {
	      lstrcpy(SP[n]->FileName, FN1.FileName);
	      SP[n]->nFileOffset = FN1.nFileOffset;
	      CHAR Fmt[80];
	      CHAR ETitle[40];
	      LoadString(GetApplication()->hInstance, IDS_FWRITEERRORTEXT, Fmt, 79);
	      LoadString(GetApplication()->hInstance, IDS_FWRITEERRORTITLE, ETitle, 39);
              CHAR WStr[256];
              sprintf(WStr, Fmt, FN0.FileName);
	      MessageBox(HWindow, WStr, ETitle, MB_OK | MB_ICONSTOP);
	   }
           return (OX) ? TRUE : FALSE;  
	} else {
	   CHAR Fmt1[80];
	   CHAR ETitle1[40];
	   LoadString(GetApplication()->hInstance, IDS_FCREATEERRORTEXT, Fmt1, 79);
	   LoadString(GetApplication()->hInstance, IDS_FCREATEERRORTITLE, ETitle1, 39);
           CHAR MStr[256];
           sprintf(MStr, Fmt1, FN0.FileName);
	   MessageBox(HWindow, MStr, ETitle1, MB_OK | MB_ICONSTOP);
	   return FALSE;
        }
     } else return FALSE;
}

BOOL SpectrumWindow::SpecialSave(SHORT n, UINT flags)
{
     CHAR *File;
     CHAR ext[5];
     FILE *f;

     if (flags == SA_QPRO) lstrcpy(ext, ".qpr");
     else if (flags == SA_ASCII) lstrcpy(ext, ".asc");
     else lstrcpy(ext, ".cal");
	 lstrcpy(FN->FileName, SP[n]->FileName);
     if ((File = strchr(FN->FileName, '.')) != NULL)
     lstrcpy(File, ext);
     if (GetSaveFile(*FN)) {
	if ((f = fopen(FN->FileName, "wt")) != NULL) {
	   BOOL OX = SaveAsSpecial(f, n, flags);
	   fclose(f);
	   if (!OX)  {
	      CHAR Fmt[80];
	      CHAR ETitle[40];
	      LoadString(GetApplication()->hInstance, IDS_FWRITEERRORTEXT, Fmt, 79);
	      LoadString(GetApplication()->hInstance, IDS_FWRITEERRORTITLE, ETitle, 39);
              CHAR WStr[256];
              sprintf(WStr, Fmt, FN->FileName);
	      MessageBox(HWindow, WStr, ETitle, MB_OK | MB_ICONSTOP);
	   }
           return (OX) ? TRUE : FALSE;  
	} else {
	   CHAR Fmt1[80];
	   CHAR ETitle1[40];
	   LoadString(GetApplication()->hInstance, IDS_FCREATEERRORTEXT, Fmt1, 79);
	   LoadString(GetApplication()->hInstance, IDS_FCREATEERRORTITLE, ETitle1, 39);
           CHAR MStr[256];
           sprintf(MStr, Fmt1, FN->FileName);
	   MessageBox(HWindow, MStr, ETitle1, MB_OK | MB_ICONSTOP);
	   return FALSE;
        }
     } else return FALSE;
}

void SpectrumWindow::CMSaveAsAscii(RTMessage)
{
     if (LoadOX[SPEC]) SpecialSave(SPEC, SA_ASCII);
}

void SpectrumWindow::CMSaveAsQPRO(RTMessage)
{
     if (LoadOX[SPEC]) SpecialSave(SPEC, SA_QPRO);
}

void SpectrumWindow::CMSaveCalib(RTMessage)
{
     if (LoadOX[SPEC]) SpecialSave(SPEC, SA_CALIB);
}

BOOL SpectrumWindow::IsSaved(SHORT n, BOOL stype)
{
     if (!LoadOX[n]) return TRUE;
     if (SaveOX[n]) return TRUE;
     BOOL OX;
     if (stype) OX = TRUE;
     else {
        CHAR str[80];
        sprintf(str, "Spectrum of memory %d is changed.\nSave it ?", n+1);
	SHORT ID = MessageBox(HWindow, str, Title, MB_YESNOCANCEL);
	if (ID == IDCANCEL) return FALSE;
	if (ID == IDNO) return TRUE;
        OX = TRUE; 
     }
     if (OX) {
	if (FileType[n] == CFT110) {
	   HFILE    hf;
           OFSTRUCT of;
	   if ((hf = OpenFile(SP[n]->FileName, &of, OF_CREATE)) == HFILE_ERROR) {
	      CHAR Fmt1[80];
	      CHAR ETitle1[40];
	      LoadString(GetApplication()->hInstance, IDS_FCREATEERRORTEXT, Fmt1, 79);
	      LoadString(GetApplication()->hInstance, IDS_FCREATEERRORTITLE, ETitle1, 39);
              CHAR MStr[256];
              sprintf(MStr, Fmt1, FN->FileName);
	      MessageBox(HWindow, MStr, ETitle1, MB_OK | MB_ICONSTOP);
	      return FALSE;
	   } else {
	      BOOL OX1 = SaveCFighter(hf, n);
	      _lclose(hf);
	      if (OX1) SaveOX[n] = TRUE;
	      else {
		 CHAR Fmt[80];
	         CHAR ETitle[40];
	         LoadString(GetApplication()->hInstance, IDS_FWRITEERRORTEXT, Fmt, 79);
	         LoadString(GetApplication()->hInstance, IDS_FWRITEERRORTITLE, ETitle, 39);
                 CHAR WStr[256];
                 sprintf(WStr, Fmt, SP[n]->FileName);
	         MessageBox(HWindow, WStr, ETitle, MB_OK | MB_ICONSTOP);
	      }
              return OX1;
	   }
	} else {
           return SaveAsCFighter(n);
	}
     } else return FALSE;
}
