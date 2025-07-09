#include "cft.h"
#include "cftdlg.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

InputDLG::InputDLG(PTWindowsObject PTWO, LPSTR ATitle, LPSTR APrompt, LPSTR STR, UINT Len,
                   int RID)
: TDialog(PTWO, RID)
{
     Title  = ATitle;
     Prompt = APrompt;
     str    = STR;
     len    = Len; 
}

void InputDLG::SetupWindow()
{
     TDialog::SetupWindow();
     SetCaption(Title);
     SetDlgItemText(HWindow, ID_I_PROMPT, Prompt);
     SetDlgItemText(HWindow, ID_I_TEXT, str);
};

void InputDLG::Ok(RTMessage)
{
     GetDlgItemText(HWindow, ID_I_TEXT, str, len);
     CloseWindow(TRUE);
}

void AboutDLG::SetupWindow()
{
     TDialog::SetupWindow();
     BT = new Bitmap(GetApplication()->hInstance, "CLUSTER");
};

void AboutDLG::WMPaint(TMessage &)
{
     PAINTSTRUCT ps;

     BeginPaint(HWindow, &ps);
     SaveDC(ps.hdc);

     BT->Display(ps.hdc, 0, 0);

     RestoreDC(ps.hdc, -1);
     EndPaint(HWindow, &ps);
     
}

void TitleDLG::SetupWindow()
{
     TDialog::SetupWindow();

     SendDlgItemMessage(HWindow, ID_TITLE,  WM_SETTEXT,   0, (LPARAM)(LPCSTR)ts->Title);
     SendDlgItemMessage(HWindow, ID_XUNIT,  WM_SETTEXT,   0, (LPARAM)(LPCSTR)ts->XUnit);
     SendDlgItemMessage(HWindow, ID_YUNIT,  WM_SETTEXT,   0, (LPARAM)(LPCSTR)ts->YUnit);
}

void TitleDLG::Ok(RTMessage)
{
     GetDlgItemText(HWindow, ID_TITLE, ts->Title, 127);
     GetDlgItemText(HWindow, ID_XUNIT, ts->XUnit, 79);
     GetDlgItemText(HWindow, ID_YUNIT, ts->YUnit, 79);
     CloseWindow(TRUE);
}

void CalibAddDLG::SetupWindow()
{
     CHAR buf[128];

     TDialog::SetupWindow();

     sprintf(buf, "Calibration Data N� : %d", ms->Num);
     SetDlgItemText(HWindow, ID_CALIBNUM, buf);
     sprintf(buf, "T.O.F.\t%.3f 탎", ms->Time);
     SetDlgItemText(HWindow, ID_TIME, buf);
     sprintf(Mass, "%.3f", ms->Mass);
     SendDlgItemMessage(HWindow, ID_MASS,  WM_SETTEXT,   0, (LPARAM)(LPCSTR)Mass);
}

void CalibAddDLG::Ok(RTMessage)
{
     GetDlgItemText(HWindow, ID_MASS, Mass, 15);
     ms->Mass = atof(Mass);
     CloseWindow(TRUE);
}

void CalibInsDLG::SetupWindow()
{
     TDialog::SetupWindow();
     BT = new Bitmap(GetApplication()->hInstance, "EQ001");
};

void CalibInsDLG::WMPaint(TMessage &)
{
     PAINTSTRUCT ps;

     BeginPaint(HWindow, &ps);
     SaveDC(ps.hdc);

     BT->Display(ps.hdc, 20, 10);

     RestoreDC(ps.hdc, -1);
     EndPaint(HWindow, &ps);
     
}

void CalibInsDLG::Ok(RTMessage)
{
     CHAR buf[16];

     GetDlgItemText(HWindow, ID_A, buf, 15);
     cs->a[0] = atof(buf);
     GetDlgItemText(HWindow, ID_B, buf, 15);
     cs->a[1] = atof(buf);
     GetDlgItemText(HWindow, ID_C, buf, 15);
     cs->a[2] = atof(buf);
     *MultiOX =  (IsDlgButtonChecked(HWindow, ID_OPTION)  > 0) ? TRUE : FALSE;
     CloseWindow(TRUE);
}

void CalibViewDLG::SetupWindow()
{
     TDialog::SetupWindow();
     CHAR *str = new CHAR[80];
     sprintf(str, " A = %.6e", cs->a[0]);
     SetDlgItemText(HWindow, ID_A, str);
     sprintf(str, " B = %.6e", cs->a[1]);
     SetDlgItemText(HWindow, ID_B, str);
     sprintf(str, " C = %.6e", cs->a[2]);
     SetDlgItemText(HWindow, ID_C, str);
     delete str;
     BT = new Bitmap(GetApplication()->hInstance, "EQ001");
}

void CalibViewDLG::WMPaint(TMessage &)
{
     PAINTSTRUCT ps;

     BeginPaint(HWindow, &ps);
     SaveDC(ps.hdc);

     BT->Display(ps.hdc, 20, 10);

     RestoreDC(ps.hdc, -1);
     EndPaint(HWindow, &ps);
}

void MinMaxDLG::SetupWindow()
{
     CHAR buf[15];

     sprintf(buf, "%.3f", ms->Min);
     SetDlgItemText(HWindow, ID_MIN, buf);
     sprintf(buf, "%.3f", ms->Max);
     SetDlgItemText(HWindow, ID_MAX, buf);
     SetCaption(ms->DlgTitle);
     SetDlgItemText(HWindow, ID_MINTITLE, ms->MinTitle);
     SetDlgItemText(HWindow, ID_MAXTITLE, ms->MaxTitle);
}

void MinMaxDLG::Ok(RTMessage)
{
     CHAR buf[40];

     GetDlgItemText(HWindow, ID_MIN, buf, 39);
     ms->Min = atof(buf);
     GetDlgItemText(HWindow, ID_MAX, buf, 39);
     ms->Max = atof(buf);
     if (ms->Min >= ms->Max) {
	if (MessageBox(HWindow, "Invalide values.\n Retry ?", "Error", MB_YESNO) == IDNO)
        CloseWindow(FALSE);
     } else CloseWindow(TRUE);
}

void SameScaleDLG::SetupWindow()
{
     CHAR buf[15];

     sprintf(buf, "%.3f", ms->Min);
     SetDlgItemText(HWindow, ID_MIN, buf);
     sprintf(buf, "%.3f", ms->Max);
     SetDlgItemText(HWindow, ID_MAX, buf);
     SetCaption(ms->DlgTitle);
     CheckDlgButton(HWindow, ID_OPTION, ms->Option);
}

void SameScaleDLG::Ok(RTMessage)
{
     CHAR buf[40];

     GetDlgItemText(HWindow, ID_MIN, buf, 39);
     ms->Min = atof(buf);
     GetDlgItemText(HWindow, ID_MAX, buf, 39);
     ms->Max = atof(buf);
     if (ms->Min >= ms->Max) {
	if (MessageBox(HWindow, "Invalide values.\n Retry ?", "Error", MB_YESNO) == IDNO)
        CloseWindow(FALSE);
     } else {
	ms->Option = IsDlgButtonChecked(HWindow, ID_OPTION);
	CloseWindow(TRUE);
     }
}

void Transfer9350DLG::SetupWindow()
{
     CheckRadioButton(HWindow, ID_MEM0, ID_MEMB, ID_MEM0+*mem);
}

void Transfer9350DLG::Ok(RTMessage)
{
     for (int i = 0; i < 12; i++)
	if (IsDlgButtonChecked(HWindow, ID_MEM0+i)) *mem = i;
     CloseWindow(TRUE);
}

void VisualAreaDLG::SetupWindow()
{
     CHAR buf[5];

     for (int i = 0; i < 8; i++) {
	 wsprintf(buf, "BK%d", i+1);
	 BT[i] = new Bitmap(GetApplication()->hInstance, buf);
     }
}

VisualAreaDLG::~VisualAreaDLG()
{
     for (int i = 0; i < 8; i++) delete BT[i];
}

void VisualAreaDLG::WMPaint(TMessage &)
{
     PAINTSTRUCT ps;
     CHAR buf[80];
     SHORT x[14] = {120, 120, 120, 120, 120, 120, 420, 420, 420, 420, 420, 420, 420, 420};  
     SHORT y[14] = { 30, 100, 160, 180, 230, 250,  20,  40,  90, 110, 160, 180, 230, 250};

     BeginPaint(HWindow, &ps);
     SaveDC(ps.hdc);

     for (int i = 0; i < 8; i++) BT[i]->Display(ps.hdc, 10+300*(i/4), 70*(i%4)+10);

     SetTextColor(ps.hdc, RGB(0,0,0));
     SetTextAlign(ps.hdc, TA_LEFT);
     for (i = 0; i < 14; i++) {
	 sprintf(buf, "Area %d :  %e", i+1, a->Area[i]);
	 TextOut(ps.hdc, x[i], y[i], buf, strlen(buf));
     }

     RestoreDC(ps.hdc,-1);
     EndPaint(HWindow, &ps);
     
}

void ExpConditionDLG::SetupWindow()
{
     CHAR buf[40];

     SetDlgItemText(HWindow, ID_SELEMENT, es->Element);
     sprintf(buf, "%.1f", es->Four);
     SetDlgItemText(HWindow, ID_SFOUR, buf);
     sprintf(buf, "%.1f", es->Couvercle);
     SetDlgItemText(HWindow, ID_SCOUV, buf);
     sprintf(buf, "%.1f", es->He);
     SetDlgItemText(HWindow, ID_SHe, buf);
     sprintf(buf, "%.1f", es->Ar);
     SetDlgItemText(HWindow, ID_SAr, buf);
     SetDlgItemText(HWindow, ID_SOTHER, es->Other);
     SetDlgItemText(HWindow, ID_ITYPE,  es->Ioniseur);
     SendDlgItemMessage(HWindow,  ID_ITYPE, CB_ADDSTRING, 0, (LPARAM)((LPCSTR)"Azote"));
     SendDlgItemMessage(HWindow,  ID_ITYPE, CB_ADDSTRING, 0, (LPARAM)((LPCSTR)"Yag3"));
     SendDlgItemMessage(HWindow,  ID_ITYPE, CB_ADDSTRING, 0, (LPARAM)((LPCSTR)"Yag4"));
     SendDlgItemMessage(HWindow,  ID_ITYPE, CB_ADDSTRING, 0, (LPARAM)((LPCSTR)"Eximere"));
     SendDlgItemMessage(HWindow,  ID_ITYPE, CB_ADDSTRING, 0, (LPARAM)((LPCSTR)"Electron"));
     sprintf(buf, "%.3f", es->IEnergie);
     SetDlgItemText(HWindow, ID_IENERGY,  buf);
     SetDlgItemText(HWindow, ID_ICOMMENT, es->IComment);
     SetDlgItemText(HWindow, ID_ITYPE1,  es->Ioniseur1);
     SendDlgItemMessage(HWindow,  ID_ITYPE1, CB_ADDSTRING, 0, (LPARAM)((LPCSTR)"Azote"));
     SendDlgItemMessage(HWindow,  ID_ITYPE1, CB_ADDSTRING, 0, (LPARAM)((LPCSTR)"Yag3"));
     SendDlgItemMessage(HWindow,  ID_ITYPE1, CB_ADDSTRING, 0, (LPARAM)((LPCSTR)"Yag4"));
     SendDlgItemMessage(HWindow,  ID_ITYPE1, CB_ADDSTRING, 0, (LPARAM)((LPCSTR)"Eximere"));
     SendDlgItemMessage(HWindow,  ID_ITYPE1, CB_ADDSTRING, 0, (LPARAM)((LPCSTR)"Electron"));
     sprintf(buf, "%.3f", es->IEnergie1);
     SetDlgItemText(HWindow, ID_IENERGY1,  buf);
     SetDlgItemText(HWindow, ID_ICOMMENT1, es->IComment1);
     sprintf(buf, "%.1f", es->Accel1);
     SetDlgItemText(HWindow, ID_APLATE1,  buf);
     sprintf(buf, "%.1f", es->Accel2);
     SetDlgItemText(HWindow, ID_APLATE2,  buf);
     sprintf(buf, "%.1f", es->Accel3);
     SetDlgItemText(HWindow, ID_APLATE3,  buf);
     sprintf(buf, "%.1f", es->Lentille1);
     SetDlgItemText(HWindow, ID_LENS1,  buf);
     sprintf(buf, "%.1f", es->Lentille2);
     SetDlgItemText(HWindow, ID_LENS2,  buf);
     sprintf(buf, "%.1f", es->ReflecV1);
     SetDlgItemText(HWindow, ID_REFV1,  buf);
     sprintf(buf, "%.1f", es->ReflecV2);
     SetDlgItemText(HWindow, ID_REFV2,  buf);
     sprintf(buf, "%.1f", es->ReflecV3);
     SetDlgItemText(HWindow, ID_REFV3,  buf);
     sprintf(buf, "%.1f", es->ReflecAngle);
     SetDlgItemText(HWindow, ID_RANGLE,  buf);
     SetDlgItemText(HWindow, ID_RCOMMENT,  es->RComment);
     sprintf(buf, "%.1f", es->Chicane1);
     SetDlgItemText(HWindow, ID_CHICANE1,  buf);
     sprintf(buf, "%.1f", es->Chicane2);
     SetDlgItemText(HWindow, ID_CHICANE2,  buf);
     SetDlgItemText(HWindow, ID_COMMENT1,  es->Commentaire[0]);
     SetDlgItemText(HWindow, ID_COMMENT2,  es->Commentaire[1]);
     SetDlgItemText(HWindow, ID_COMMENT3,  es->Commentaire[2]);
}

void ExpConditionDLG::Ok(RTMessage)
{
     CHAR buf[80];

     GetDlgItemText(HWindow, ID_SELEMENT, es->Element, 19);
     GetDlgItemText(HWindow, ID_SFOUR, buf, 40);
     es->Four = (FLOAT)atof(buf);
     GetDlgItemText(HWindow, ID_SCOUV, buf, 40);
     es->Couvercle = (FLOAT)atof(buf);
     GetDlgItemText(HWindow, ID_SHe, buf, 40);
     es->He = (FLOAT)atof(buf);
     GetDlgItemText(HWindow, ID_SAr, buf, 40);
     es->Ar = (FLOAT)atof(buf);
     GetDlgItemText(HWindow, ID_SOTHER, es->Other, 39);
     GetDlgItemText(HWindow, ID_ITYPE,  es->Ioniseur, 39);
     GetDlgItemText(HWindow, ID_IENERGY,  buf, 40);
     es->IEnergie = (FLOAT)atof(buf);
     GetDlgItemText(HWindow, ID_ICOMMENT, es->IComment, 79);
     GetDlgItemText(HWindow, ID_ITYPE1,  es->Ioniseur1, 39);
     GetDlgItemText(HWindow, ID_IENERGY1,  buf, 40);
     es->IEnergie1 = (FLOAT)atof(buf);
     GetDlgItemText(HWindow, ID_ICOMMENT1, es->IComment1, 79);
     GetDlgItemText(HWindow, ID_APLATE1,  buf, 40);
     es->Accel1 = (FLOAT)atof(buf);
     GetDlgItemText(HWindow, ID_APLATE2,  buf, 40);
     es->Accel2 = (FLOAT)atof(buf);
     GetDlgItemText(HWindow, ID_APLATE3,  buf, 40);
     es->Accel3 = (FLOAT)atof(buf);
     GetDlgItemText(HWindow, ID_LENS1,  buf, 40);
     es->Lentille1 = (FLOAT)atof(buf);
     GetDlgItemText(HWindow, ID_LENS2,  buf, 40);
     es->Lentille2 = (FLOAT)atof(buf);
     GetDlgItemText(HWindow, ID_REFV1,  buf, 40);
     es->ReflecV1 = (FLOAT)atof(buf);
     GetDlgItemText(HWindow, ID_REFV2,  buf, 40);
     es->ReflecV2 = (FLOAT)atof(buf);
     GetDlgItemText(HWindow, ID_REFV3,  buf, 40);
     es->ReflecV3 = (FLOAT)atof(buf);
     GetDlgItemText(HWindow, ID_RANGLE, buf, 40);
     es->ReflecAngle = (FLOAT)atof(buf);
     GetDlgItemText(HWindow, ID_RCOMMENT,  es->RComment, 79);
     GetDlgItemText(HWindow, ID_CHICANE1,  buf, 40);
     es->Chicane1 = (FLOAT)atof(buf);
     GetDlgItemText(HWindow, ID_CHICANE2,  buf, 40);
     es->Chicane2 = (FLOAT)atof(buf);
     GetDlgItemText(HWindow, ID_COMMENT1,  es->Commentaire[0], 79);
     GetDlgItemText(HWindow, ID_COMMENT2,  es->Commentaire[1], 79);
     GetDlgItemText(HWindow, ID_COMMENT3,  es->Commentaire[2], 79);
     CloseWindow(TRUE);
}

InputCTextDLG::InputCTextDLG(PTWindowsObject PTWO, LPSTR ATitle, LPSTR APrompt, LPSTR STR,
                             UINT Len, REAL TOF)
: InputDLG(PTWO, ATitle, APrompt, STR, Len, INPUTCTEXT)
{
     tof = TOF;
     if (((SpectrumWindow *)Parent)->SP[((SpectrumWindow *)Parent)->SPEC]->NoCalib == 0)
	EnableWindow(GetDlgItem(HWindow, ID_MASS), FALSE);
}

void InputCTextDLG::AddTOF(RTMessage)
{
     CHAR buf[15];

     GetDlgItemText(HWindow, ID_I_TEXT, str, len-15);
     str[len-15] = '\0';
     sprintf(buf, "(%.3f 탎)", tof);
     strcat(str, buf); 
     SetDlgItemText(HWindow, ID_I_TEXT, str);
}

void InputCTextDLG::AddMass(RTMessage)
{
     
     CHAR buf[15];
     LONG  CP = ((SpectrumWindow *)Parent)->CursorPos[((SpectrumWindow *)Parent)->SPEC][0];

     GetDlgItemText(HWindow, ID_I_TEXT, str, len-15);
     str[len-15] = '\0';
     sprintf(buf, "(m = %.3f)", ((SpectrumWindow *)Parent)->CalculMass(((SpectrumWindow *)Parent)->SPEC, CP));
     strcat(str, buf); 
     SetDlgItemText(HWindow, ID_I_TEXT, str);
}

void InsertCTextDLG::Ok(RTMessage)
{
     CHAR buf[20];

     GetDlgItemText(HWindow, ID_TOF, buf, 19);
     cs->TOF = atof(buf);
     GetDlgItemText(HWindow, ID_CTEXT, cs->CText, 19);
     CloseWindow(TRUE);
}

EditCTextDLG::EditCTextDLG(PTWindowsObject PTWO, CURSORTEXT FAR *TE, SPECTRUMINFO FAR *SP)
: TDialog(PTWO, CTEXTLIST)
{
     te = TE;
     sp = SP;
     List  = new TListBox(this, ID_CTEXTLIST);
     Time  = new TStatic(this,  ID_TIME, 20);
     CText = new TEdit(this,    ID_EDITCTEXT, 38);
     SelectedOX = ChangeOX = FALSE;
     Num = -1;
}

void EditCTextDLG::SetupWindow()
{
     TDialog::SetupWindow();

     for (SHORT i = 0; i < sp->NoCursorText; i++) List->AddString(te[i].TextStr);
}

void EditCTextDLG::IDCTextSelected()
{
     Num = List->GetSelIndex();
     if (Num >= 0) {
	SelectedOX = TRUE;
	CHAR buf[40];
	sprintf(buf, "%.3f 탎", sp->xGain*(REAL)te[Num].Position+sp->xOffset);
	Time ->SetText(buf);
	CText->SetText(te[Num].TextStr);
     }
}

void EditCTextDLG::IDModify()
{
     if (SelectedOX) {
	CText->GetText(te[Num].TextStr, 39);
	ChangeOX = TRUE;
	List->DeleteString(Num);
	List->InsertString(te[Num].TextStr, Num);
	List->SetSelIndex(Num);
     }
}

void EditCTextDLG::IDDelete()
{
     if (SelectedOX) {
	for (SHORT i = Num; i < sp->NoCursorText; i++) {
            te[i].Position = te[i+1].Position;
	    lstrcpy(te[i].TextStr, te[i+1].TextStr);
	}
	List->DeleteString(Num);
	sp->NoCursorText--;
	ChangeOX = TRUE;
        if (Num < sp->NoCursorText) {
	   CHAR buf[40];
	   sprintf(buf, "%.3f 탎", sp->xGain*(REAL)te[Num].Position+sp->xOffset);
	   Time ->SetText(buf);
	   CText->SetText(te[Num].TextStr);
	   List->SetSelIndex(Num);
	   SelectedOX = TRUE;
	} else {
	   Time ->SetText("");
	   CText->SetText("");
           SelectedOX = FALSE;
        }
     }
}

void EditCTextDLG::Ok(RTMessage)
{
     CloseWindow(ChangeOX);
}

EditCalibDLG::EditCalibDLG(PTWindowsObject PTWO, CALIBTABLE FAR *CT, SPECTRUMINFO FAR *SP)
: TDialog(PTWO, CALIBLIST)
{
     ct = CT;
     sp = SP;

     List  = new TListBox(this, ID_CALIBLIST);
     Time  = new TStatic(this,  ID_TIME, 20);
     Mass  = new TEdit(this,    ID_EDITCTEXT, 38);
     SelectedOX = ChangeOX = FALSE;
     Num = -1;
     DrawX = 50.0;
     DrawY = 20.0;
     DrawW = 370.0;
     DrawH = 270.0;
     GetMinMax();
}

void EditCalibDLG::SetupWindow()
{
     TDialog::SetupWindow();
     
     for (SHORT i = 0; i < sp->NoCalib; i++) {
	 CHAR buf[40];
	 sprintf(buf, "X = %.3f ns,   M = %.3f", ct[i].Time, ct[i].Mass);
	 List->AddString(buf);
     }
}

void EditCalibDLG::WMPaint()
{
     PAINTSTRUCT ps;
     HDC hdc = BeginPaint(HWindow, &ps);
     SaveDC(hdc);

     Rectangle(hdc, 50, 20, 421, 291);
     DrawFunction(hdc);
     DrawDataPoint(hdc);
     HilitePoint(hdc, Num);

     RestoreDC(hdc, -1);
     EndPaint(HWindow, &ps);
}
       
void EditCalibDLG::IDCalibSelected()
{
     SHORT NumBuf = Num;
     Num = List->GetSelIndex();
     if (Num >= 0) {
	SelectedOX = TRUE;
	CHAR buf[40];
	sprintf(buf, "%.3f 탎", ct[Num].Time);
	Time ->SetText(buf);
	sprintf(buf, "%.3f", ct[Num].Mass);
	Mass->SetText(buf);
     }
     Repaint(FALSE);
     PAINTSTRUCT ps;
     HDC hdc = BeginPaint(HWindow, &ps);
     SaveDC(hdc);
     HilitePoint(hdc, NumBuf);
     HilitePoint(hdc, Num);
     RestoreDC(hdc, -1);
     EndPaint(HWindow, &ps); 
}

void EditCalibDLG::IDModify()
{
     CHAR buf[40];
     if (SelectedOX) {
	Mass->GetText(buf, 39);
        ct[Num].Mass = atof(buf);
	ChangeOX = TRUE;
	List->DeleteString(Num);
	sprintf(buf, "X = %.3f 탎,   M = %.3f", ct[Num].Time, ct[Num].Mass);
	List->InsertString(buf, Num);
	List->SetSelIndex(Num);
	RecalculCoeff();
	GetMinMax();
	Repaint(TRUE);
     } 
}

void EditCalibDLG::IDDelete()
{
     if (SelectedOX && (sp->NoCalib > 4)) {
	for (SHORT i = Num; i < sp->NoCalib; i++) {
	    ct[i].Time = ct[i+1].Time;
	    ct[i].Mass = ct[i+1].Mass;
	}
	List->DeleteString(Num);
	sp->NoCalib--;
	ChangeOX = TRUE;
	if (Num < sp->NoCalib) {
	   CHAR buf[40];
           sprintf(buf, "%.3f 탎", ct[Num].Time);
	   Time ->SetText(buf);
	   sprintf(buf, "%.3f", ct[Num].Mass);
	   Mass->SetText(buf);
	   List->SetSelIndex(Num);
	   SelectedOX = TRUE;
	   RecalculCoeff();
	} else {
	   Time ->SetText("");
	   Mass->SetText("");
	   SelectedOX = FALSE;
           Num = -1;
	}
	GetMinMax();
	Repaint(TRUE);
     } 
}

void EditCalibDLG::Ok(RTMessage)
{
     CloseWindow(ChangeOX);
}

void EditCalibDLG::GetMinMax()
{
     MinTime = +1.0e+300;
     MaxTime = -1.0e+300;
     MinMass = +1.0e+300;
     MaxMass = -1.0e+300;
     REAL dt;
     for (SHORT i = 0; i < sp->NoCalib; i++) {
	 dt = ct[i].Time- sp->A.a[0]-sp->A.a[1]*sqrt(ct[i].Mass)-sp->A.a[2]*ct[i].Mass;
	 if (MinTime > dt) MinTime = dt;
	 if (MaxTime < dt) MaxTime = dt;
	 if (MinMass > ct[i].Time) MinMass = ct[i].Time;
	 if (MaxMass < ct[i].Time) MaxMass = ct[i].Time;
     }
     REAL W = (MaxMass-MinMass)/10.0;
     MinMass -= W;
     //if (MinMass < 0.0) MinMass = 0.0;
     MaxMass += W;
     W = (MaxTime-MinTime)/10.0;
     MinTime -= W;
     //if (MinTime < 0.0) MinTime = 0.0;
     MaxTime += W;
     /*
     REAL MinTime1 = +1.0e+300;
     REAL MaxTime1 = -1.0e+300;
     REAL MinMass1 = +1.0e+300;
     REAL MaxMass1 = -1.0e+300;
     REAL M, T;
     for (REAL j = 0.0; j < DrawW; j += 1.0) {
	 M = j/DrawW*(MaxMass-MinMass)+MinMass;
	 T = sp->A.a[0]+sp->A.a[1]*sqrt(M)+sp->A.a[2]*M;
	 if (MinTime1 > T) MinTime1 = T;
	 if (MaxTime1 < T) MaxTime1 = T;
	 if (MinMass1 > M) MinMass1 = M;
	 if (MaxMass1 < M) MaxMass1 = M;
     }
     if (MinTime > MinTime1) MinTime = MinTime1;
     if (MaxTime < MaxTime1) MaxTime = MaxTime1;
     if (MinMass > MinMass1) MinMass = MinMass1;
     if (MaxMass < MaxMass1) MaxMass = MaxMass1;
     */
}

void EditCalibDLG::RecalculCoeff()
{
     PSpectrumWindow PARENT = (PSpectrumWindow)Parent;
     if (PARENT->CalculCalibCoeff(PARENT->SPEC) == FALSE) {
	sp->A.a[0] = sp->A.a[1] = 0.0;
        sp->A.a[2] = 1.0;
     }  
}

void EditCalibDLG::Repaint(BOOL OX)
{
     RECT rc = { 0, 0, 435, 335 };
     InvalidateRect(HWindow, &rc, OX);
}

void EditCalibDLG::DrawDataPoint(HDC hdc)
{
     REAL dt;
     REAL X, Y;

     SelectObject(hdc, GetStockObject(BLACK_PEN));
     for (SHORT i = 0; i < sp->NoCalib; i++) {
	 X = DrawX+DrawW*(ct[i].Time-MinMass)/(MaxMass-MinMass);
	 dt = ct[i].Time- sp->A.a[0]-sp->A.a[1]*sqrt(ct[i].Mass)-sp->A.a[2]*ct[i].Mass;
	 Y = DrawY+DrawH*(MaxTime-dt)/(MaxTime-MinTime);
	 if ((X >= DrawX) && (X <= (DrawX+DrawW)) && (Y >= DrawY) && (Y <= (DrawY+DrawH))) {
            Rectangle(hdc,(SHORT)X-3, (SHORT)Y-3, (SHORT)X+4, (SHORT)Y+4); 
         }
     }
}

void EditCalibDLG::DrawFunction(HDC hdc)
{
     REAL Y = DrawY+DrawH*(MaxTime-0.0)/(MaxTime-MinTime);;

     SelectObject(hdc, GetStockObject(BLACK_PEN));
     MoveTo(hdc, (SHORT)DrawX, (SHORT)Y);
     LineTo(hdc, (SHORT)(DrawX+DrawW), (SHORT)Y);



     /*
     REAL M, T;
     M = MinMass;
     T = sp->A.a[0]+sp->A.a[1]*sqrt(M)+sp->A.a[2]*M;
     Y = DrawY+DrawH*(MaxTime-T)/(MaxTime-MinTime);
     MoveTo(hdc, (SHORT)DrawX, (SHORT)Y);
     for (REAL i = 1.0; i < DrawW; i = i+1.0) {
	 M = i/DrawW*(MaxMass-MinMass)+MinMass;
	 T = sp->A.a[0]+sp->A.a[1]*sqrt(M)+sp->A.a[2]*M;
	 Y = DrawY+DrawH*(MaxTime-T)/(MaxTime-MinTime);
	 LineTo(hdc, (SHORT)DrawX+i, (SHORT)Y);
     } */
}

void EditCalibDLG::HilitePoint(HDC hdc, SHORT n)
{
     if ((n >= 0) && (n < sp->NoCalib)) {
        SetROP2(hdc, R2_XORPEN);
        SelectObject(hdc,GetStockObject(WHITE_PEN));

	REAL X = DrawX+DrawW*(ct[n].Time-MinMass)/(MaxMass-MinMass);
	REAL dt = ct[n].Time- sp->A.a[0]-sp->A.a[1]*sqrt(ct[n].Mass)-sp->A.a[2]*ct[n].Mass;
	REAL Y = DrawY+DrawH*(MaxTime-dt)/(MaxTime-MinTime);
        if ((X >= DrawX) && (X <= (DrawX+DrawW)) && (Y >= DrawY) && (Y <= (DrawY+DrawH))) {
           MoveTo(hdc, (SHORT)X-5, (SHORT)Y);
	   LineTo(hdc, (SHORT)X+6, (SHORT)Y);
	   MoveTo(hdc, (SHORT)X, (SHORT)Y-5);
	   LineTo(hdc, (SHORT)X, (SHORT)Y+6);
        }
        SelectObject(hdc,GetStockObject(BLACK_PEN));
	SetROP2(hdc, R2_COPYPEN);
     }
}

BKSubtractDLG::BKSubtractDLG(PTWindowsObject PTWO, BKSUBSTRUCT FAR* BK)
: TDialog(PTWO, BKSUBTRACT)
{
     bk = BK;
}

BKSubtractDLG::~BKSubtractDLG()
{
     delete Bmp1;
     delete Bmp2;
     delete Bmp3;
     for (SHORT i = 0; i < 6; i++) delete Bmp[i];
}

void BKSubtractDLG::SetupWindow()
{
     TDialog::SetupWindow();
     Bmp1 = new Bitmap(GetApplication()->hInstance, "FITEQ01");
     Bmp2 = new Bitmap(GetApplication()->hInstance, "FITEQ02");
     Bmp3 = new Bitmap(GetApplication()->hInstance, "FITEQ03");

     CHAR str[40];
     for (SHORT i = 0; i < 6; i++ ) {
         sprintf(str,"COEFF_a%d", i);
	 Bmp[i] = new Bitmap(GetApplication()->hInstance, str);
     }
     SHORT n = (bk->Mode == LINEAR) ? 0 : (bk->Mode == QUADRATIC) ? 1 : 2;

     CheckRadioButton(HWindow, ID_EQ1, ID_EQ2, ID_EQ1+n);
     EnableWindow(GetDlgItem(HWindow, ID_MEM1+bk->Source),FALSE);
     if (bk->Source == bk->Target) n = (bk->Source == 4) ? 3 : 4;
     else n = bk->Target;
     CheckRadioButton(HWindow, ID_MEM1, ID_MEM5, ID_MEM1+n);
     for (i = 0; i < 6; i++) {
	 sprintf(str,"%.3f", bk->a[i]);
         SetDlgItemText(HWindow, ID_a0+i, str);
     }
};

void BKSubtractDLG::WMPaint()
{
     PAINTSTRUCT ps;

     BeginPaint(HWindow, &ps);
     SaveDC(ps.hdc);

     Bmp1->Display(ps.hdc, 60, 27);
     Bmp2->Display(ps.hdc, 60, 67);
     Bmp3->Display(ps.hdc, 60, 97);

     for (SHORT i = 0; i < 5; i++) Bmp[i]->Display(ps.hdc, 40, 195+i*30);

     RestoreDC(ps.hdc, -1);
     EndPaint(HWindow, &ps);
     
}

void BKSubtractDLG::Ok(RTMessage)
{
     char str[40];
     for (SHORT i = 0; i < 5; i++) {
         GetDlgItemText(HWindow, ID_a0+i, str, 38);
         bk->a[i] = atof(str);
     }

     bk->Mode = (IsDlgButtonChecked(HWindow, ID_EQ1) != 0) ? LINEAR :
		(IsDlgButtonChecked(HWindow, ID_EQ2) != 0) ? QUADRATIC : SINUSOIDAL;
     for (i = 0; i < 5; i++)
	 if (IsDlgButtonChecked(HWindow, ID_MEM1+i) != 0) break;
     bk->Target = i;
     CloseWindow(TRUE);
}

SpectrumSumDLG::SpectrumSumDLG(PTWindowsObject PTWO, LPSTR ATitle, SUMSTRUCT FAR* SS)
: TDialog(PTWO, SPECTRUMSUM)
{
     ss = SS;
     lstrcpy(Title, ATitle);
}

SpectrumSumDLG::~SpectrumSumDLG()
{
}

void SpectrumSumDLG::SetupWindow()
{
     TDialog::SetupWindow();
     SetCaption(Title);
     EnableWindow(GetDlgItem(HWindow, ID_MEM1+ss->Source),FALSE);
     SHORT n;
     if (ss->Source == ss->Target) n = (ss->Source == 4) ? 3 : 4;
     else n = ss->Target;
     CheckRadioButton(HWindow, ID_MEM1, ID_MEM5, ID_MEM1+n);
     CHAR str[40];
     sprintf(str, "%d", ss->N);
     SetDlgItemText(HWindow, ID_NPOINT, str);
}

void SpectrumSumDLG::Ok(RTMessage)
{
     for (SHORT i = 0; i < 5; i++)
	 if (IsDlgButtonChecked(HWindow, ID_MEM1+i) != 0) break;
     ss->Target = i;
     char str[40];
     GetDlgItemText(HWindow, ID_NPOINT, str, 38);
     if ((ss->N = atoi(str)) > 0) CloseWindow(TRUE);
     else MessageBox(HWindow, "Invalid Number !", "Error", MB_OK | MB_ICONSTOP);
}

GPIBConnectDLG::GPIBConnectDLG(PTWindowsObject PTWO, GPIBSTRUCT FAR& GS)
: TDialog(PTWO, GPIBCONNECT)
{
     gs = &GS;
}

GPIBConnectDLG::~GPIBConnectDLG()
{
}

void GPIBConnectDLG::SetupWindow()
{
     TDialog::SetupWindow();
     SetDlgItemText(HWindow, ID_BRDNAME, gs->BrdName);
     SetDlgItemText(HWindow, ID_DEVNAME, gs->DevName);
     CHAR buf[16];
     itoa(gs->DevAd, buf, 10);
     SetDlgItemText(HWindow, ID_DEVAD, buf);
}

void GPIBConnectDLG::Ok(RTMessage)
{
     GetDlgItemText(HWindow, ID_BRDNAME, gs->BrdName, 19);
     GetDlgItemText(HWindow, ID_DEVNAME, gs->DevName, 19);
     CHAR buf[16];
     GetDlgItemText(HWindow, ID_DEVAD, buf, 15);
     gs->DevAd = atoi(buf);
     CloseWindow(TRUE);
}


void CountDLG::SetupWindow()
{
     TDialog::SetupWindow();

     SendDlgItemMessage(HWindow, ID_NTOT,  WM_SETTEXT,   0, (LPARAM)(LPCSTR)"1000");
     SendDlgItemMessage(HWindow, ID_NSEG,  WM_SETTEXT,   0, (LPARAM)(LPCSTR)"100");
     SendDlgItemMessage(HWindow, ID_THRESHOLD,  WM_SETTEXT,   0, (LPARAM)(LPCSTR)"0.0");
     SendDlgItemMessage(HWindow, ID_TIME,  WM_SETTEXT,   0, (LPARAM)(LPCSTR)"33");
}

void CountDLG::Ok(RTMessage)
{
     CHAR STR[80];
     GetDlgItemText(HWindow, ID_NTOT, STR, 79);
     if ((ts->nTot = (SHORT)atoi(STR)) <= 0) return;
     GetDlgItemText(HWindow, ID_NSEG, STR, 79);
     if ((ts->nSeg = (SHORT)atoi(STR)) > ts->nTot) return;
     GetDlgItemText(HWindow, ID_THRESHOLD, STR, 79);
     ts->Threshold = (REAL)atof(STR);
     GetDlgItemText(HWindow, ID_TIME, STR, 79);
     ts->IdlingTime = (DWORD)atol(STR);
     if ((IsDlgButtonChecked(HWindow, ID_OPTION)) > 0) ts->IsUpperBound = TRUE;
     else ts->IsUpperBound = FALSE;
     CloseWindow(TRUE);
}

CancelDLG::CancelDLG(PTWindowsObject AParent, LPSTR ACaption) : TDialog(AParent, CANCELDLG)
{
	SetCaption(ACaption);
        CancelOX = FALSE;
}

CancelDLG::~CancelDLG()
{
}

void CancelDLG::WMInitDialog(RTMessage Msg)
{
	TDialog::WMInitDialog(Msg);
}

void CancelDLG::SetText(char *str)
{
     SetDlgItemText(HWindow, ID_TEXT, str);
}

void CancelDLG::SetDone(char *str)
{
     SetDlgItemText(HWindow, ID_DONE, str);
}

void CancelDLG::WMCommand(RTMessage)
{
	CancelOX = TRUE;
}

BOOL CancelDLG::IsCancelPressed()
{
	MSG msg;

	while (!CancelOX && PeekMessage(&msg, HWindow, 0, 0, PM_REMOVE))
	{
		if (IsDialogMessage(HWindow, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return CancelOX;
}