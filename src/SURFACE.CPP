#include "cftcust.h"
#include "surface.h"
#include <bgrpbox.h>
#include <bstatic.h>
#include <stdio.h>
#include <math.h>

#define X0_1    11
#define WIDTH0 541 

#define X_1   12
#define X_2  192
#define X_3  372
#define WIDTH1 179

RealEdit::RealEdit(PTWindowsObject PTWO, int AnId, REAL Val, int X, int Y, int W, int H,
WORD TextLen, BOOL MultiOX) : TEdit(PTWO, AnId, "", X, Y, W, H, TextLen, MultiOX)
{
     value = Val;
}

void RealEdit::SetupWindow()
{
     TEdit::SetupWindow();
     SetReal(value);
}

void RealEdit::SetReal(REAL Val)
{
     value = Val; 
     sprintf(str, "%.3f", value);
     SetText(str);
}

REAL RealEdit::GetReal()
{
     GetText(str, 19);
     return (value = atof(str));
}

ImpactWindow::ImpactWindow(PTWindowsObject PTWO, LPSTR ATitle) : TWindow(PTWO, ATitle)
{
      Attr.Style = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX;
      Attr.X     = CW_USEDEFAULT;
      Attr.Y     = CW_USEDEFAULT;
      Attr.W     = 562;
      Attr.H     = 340;

      if (!InitializeCustom(GetApplication()->hInstance))
	 MessageBox(NULL, "Custom Initialization failed", "Error", MB_OK | MB_ICONSTOP);
      else if (!ConnectWithCFighter())
	      MessageBox(NULL, "Connection with CFighter failed", "Error", MB_OK | MB_ICONINFORMATION);
      new YStatic(this, -1,   "Initial parameters", X_1, 10, 200, 15, 19);
	  new       YStatic (this, -1,   "      Mp",  X_1+10,  36,  70, 20, 14);
	  pMp = new RealEdit(this, -1,        195.0,  X_1+84,  33,  85, 23, 38, FALSE);
	  new       YStatic (this, -1,    "      Q",  X_2+10,  36,  70, 20, 14);
	  pQ  = new RealEdit(this, -1,          1.0,  X_2+84,  33,  85, 23, 38, FALSE);
	  new       YStatic (this, -1, "   To (µs)",  X_3+10,  36,  70, 20, 14);
	  pTo = new RealEdit(this, -1,          0.0,  X_3+84,  33,  85, 23, 38, FALSE);

	  new       YStatic (this, -1, "   Vacc (V)", X_1+10,  66,  70, 20, 14);
	  pVo = new RealEdit(this, -1,        2800.0, X_1+84,  63,  85, 23, 38, FALSE);
	  new       YStatic (this, -1, "  Vref1 (V)", X_2+10,  66,  70, 20, 14);
	  pV1 = new RealEdit(this, -1,        3000.0, X_2+84,  63,  85, 23, 38, FALSE);
	  new       YStatic (this, -1, "  Vref2 (V)", X_3+10,  66,  70, 20, 14);
	  pV2 = new RealEdit(this, -1,           0.0, X_3+84,  63,  85, 23, 38, FALSE);

	  new       YStatic (this, -1,   "  L0 (mm)", X_1+10,  96,  70, 20, 14);
	  pL0 = new RealEdit(this, -1,        1145.0, X_1+84,  93,  85, 23, 38, FALSE);
	  new       YStatic (this, -1,   "  L1 (mm)", X_2+10,  96,  70, 20, 14);
	  pL1 = new RealEdit(this, -1,          20.0, X_2+84,  93,  85, 23, 38, FALSE);
	  new       YStatic (this, -1,   "  L2 (mm)", X_3+10,  96,  70, 20, 14);
	  pL2 = new RealEdit(this, -1,          10.0, X_3+84,  93,  85, 23, 38, FALSE);

	  new       YStatic (this, -1, "theta fixed", X_1+10, 126,  70, 20, 14);
	  pTh = new RealEdit(this, -1,           1.3, X_1+84, 123,  85, 23, 38, FALSE);
	  new       YStatic (this, -1,    "  Vd (V)", X_2+10, 126,  70, 20, 14);
	  pDV = new RealEdit(this, -1,           0.0, X_2+84, 123,  85, 23, 38, FALSE);
	  new       YStatic (this, -1,   "  Ld (mm)", X_3+10, 126,  70, 20, 14);
	  pDL = new RealEdit(this, -1,          10.0, X_3+84, 123,  85, 23, 38, FALSE);

       new YStatic(this, -1, "Calculate", X0_1, 160, 200, 15, 10);
	  new           YStatic  (this, -1,     "    M", X_1+10, 186,     70, 20, 14);
	  pMf     = new RealEdit (this, -1,       156.0, X_1+84, 183,     85, 23, 38, FALSE);
	  pUniOX  = new YCheckBox(this, -1, "Unimol. ?", X_2+10, 189,    150, 14, FALSE);
	  new           YStatic  (this, -1,  "TOF (µs)", X_3+10, 186,     70, 20, 14);
	  pTOF    = new RealEdit (this, -1,         0.0, X_3+84, 183,     85, 23, 38, FALSE);

	  new        YStatic (this, -1,          "   Ee (eV)", X_1+10, 216,  70, 20, 14);
	  pEe = new RealEdit(this, -1,                   1.0, X_1+84, 213,  85, 23, 38, FALSE);
	  new TButton(this, ID_CALCULTOF,  "Calculate TOF  »", X_2+10, 213, 160, 23, FALSE);
	  new TButton(this, ID_SETCURSOR,        "Set Cursor", X_3+10, 213, 160, 25, FALSE);

	  new        YStatic (this, -1,           "Theta (°)", X_1+10, 246,  70, 20, 14);
	  pTh1 = new RealEdit(this, -1,                   1.3, X_1+84, 243,  85, 23, 38, FALSE);
	  new TButton(this, ID_CALCULMASS, "« Calculate Mass", X_2+10, 243,    160, 23, FALSE);
	  new TButton(this, ID_GETCURSOR,        "Get Cursor", X_3+10, 243,    160, 25, FALSE);

	  new TButton(this, ID_CALIBRATE,     "^ Calibrate ^", X_2+10, 271,    160, 25, FALSE);
}

ImpactWindow::~ImpactWindow()
{
     TerminateCustom();
}

void ImpactWindow::WMPaint(RTMessage)
{
     PAINTSTRUCT ps;
     HDC hdc = BeginPaint(HWindow, &ps);
     SaveDC(hdc);

     SelectObject(hdc, GetStockObject(LTGRAY_BRUSH));
     SelectObject(hdc, GetStockObject(NULL_PEN));
     Rectangle(hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right+1, ps.rcPaint.bottom+1);
     SelectObject(hdc, GetStockObject(BLACK_PEN));
     Rectangle(hdc, X0_1,  28, X0_1+WIDTH0, 151);
     Rectangle(hdc, X0_1, 178, X0_1+WIDTH0, 301);

     RestoreDC(hdc, -1);
     EndPaint(HWindow, &ps);
}

void ImpactWindow::GetWindowClass(WNDCLASS &AWndClass)
{
     TWindow::GetWindowClass(AWndClass);
     AWndClass.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
     AWndClass.hIcon = LoadIcon(GetApplication()->hInstance, "IMPACT");
}

void ImpactWindow::CMCalculTOF(RTMessage)
{
     MassCalcul = FALSE;
     if (CalculTOF()) pTOF->SetReal(DT+To);
     else pTOF->SetText("failed");
}

void ImpactWindow::CMCalibrate(RTMessage)
{
     MassCalcul = FALSE;
     if (CalculTOF()) pTo->SetReal(TOF-DT);
     else pTo->SetText("failed");
}

void ImpactWindow::CMCalculMass(RTMessage)
{
     REAL Time, Error = 1.0, Error1;;
     BOOL OX = TRUE;
     REAL DM;
     int  N = 0;

     MassCalcul = TRUE;
     if ((Mf = pMf->GetReal()) != 0.0) DM = Mf/2.0;
     else {
	Mf = 2.0;
	DM = 1.0;
     }
     do {
	if (DM > Mf) DM = Mf/2.0;
	if (CalculTOF()) Time = DT+To;
	else OX = FALSE;
	Error1 = (Time-TOF)/TOF;
	if ((N > 300) || (Mf < 1.0)) OX = FALSE;
	N++;
	if (OX && (fabs(Error1) > 1.0e-7)) {
	   if (Error1 > 0.0) Mf -= DM;
	   else Mf += DM;
	   if (Mf <= 0.0) OX = FALSE;
	}
	if ((Error*Error1) < 0.0) DM /= 2.0;
	if (OX)  Error = Error1;
     } while (OX && (fabs(Error1) > 1.0e-7));
     if (!OX) pMf->SetText("failed");
     else pMf->SetReal(Mf);
}

void ImpactWindow::CMSetCursor(RTMessage)
{
     REAL Time = pTOF->GetReal();
     SetCursorPosition(Time);
}

void ImpactWindow::CMGetCursor(RTMessage)
{
     REAL Time;
     GetCursorPosition(&Time);
     pTOF->SetReal(Time);
     
}

BOOL ImpactWindow::GetParameters()
{
     if ((Mp = pMp->GetReal()) <= 0.0) return FALSE;
     pMp->SetReal(Mp);
     if ((Q  =  pQ->GetReal()) <= 0.0) return FALSE;
     pQ->SetReal(Q);
     To = pTo->GetReal();
     pTo->SetReal(To);
     if ((Vo = pVo->GetReal()) <= 0.0) return FALSE;
     pVo->SetReal(Vo);
     if ((V1 = pV1->GetReal()) <= 0.0) return FALSE;
     pV1->SetReal(V1);
     if ((V2 = pV2->GetReal()) <  0.0) return FALSE;
     pV2->SetReal(V2);
     if ((L0 = pL0->GetReal()) <= 0.0) return FALSE;
     pL0->SetReal(L0);
     if ((L1 = pL1->GetReal()) <= 0.0) return FALSE;
     pL1->SetReal(L1);
     if ((L2 = pL2->GetReal()) <= 0.0) return FALSE;
     pL2->SetReal(L2);
     if ((Th = pTh->GetReal()) <= 0.0) return FALSE;
     pTh->SetReal(Th);
     DV = pDV->GetReal();
     pDV->SetReal(DV);
     DL = pDL->GetReal();
     pDL->SetReal(DL);
     if (!MassCalcul) {
	if ((Mf = pMf->GetReal()) <= 0.0) return FALSE;
        pMf->SetReal(Mf);
     }
     Ee = pEe->GetReal();
     pEe->SetReal(Ee);
     if ((Th1 = pTh1->GetReal()) <= 0.0) return FALSE;
     pTh1->SetReal(Th1);
     TOF = pTOF->GetReal();
     pTOF->SetReal(TOF);
     UniOX = pUniOX->IsChecked();
     if (UniOX && (Mf > Mp)) return FALSE;
     return TRUE;
}

BOOL ImpactWindow::CalculTOF()
{
     int  Mode;
     REAL v1, v2, v3, E1;
     REAL a1, a2;

     if (!GetParameters()) return FALSE;

     REAL cos_th   = cos(Th1/360.0*M_PI);
     REAL cos2_th  = cos_th*cos_th;

     REAL Mi = (UniOX) ? Mf : Mp;
     REAL Eo = Mi/Mp*Q*Vo;
     REAL Ez = Eo*cos2_th;
     REAL l0 = L0/_c/cos(Th/180.0*M_PI);
     REAL l1 = L1/_c;
     REAL l2 = L2/_c;
     if (Ez <= Q*V1) Mode = 0;
     else if (V1 <= V2) {
	     Mode = (Ez <= Q*V2) ? 1 : 2;
	  } else return FALSE;
     REAL Mo = (UniOX) ? Mf : (Mode  == 2) ? Mf : Mp; 
     if (!UniOX && (Mp != Mf) && (Mode != 2)) return FALSE;

     v1 = sqrt(2.0*Ez/Mi/_mp);
     a1 = l1*Mi*_mp/Q/V1;
     if (Mode >= 1) {
	v2 = sqrt(2.0*(Ez-Q*V1)/Mi/_mp);
	a2 = l2*Mi*_mp/Q/(V2-V1);
     }
     if (Mode == 2) v3 = sqrt(2.0*(Ez-Q*V2)/Mi/_mp);
     switch (Mode) {
	case 0 :
	   DT = 2.0*a1*v1+l0/sqrt(2.0*Eo/Mi/_mp);
	   break;
	case 1 :
	   DT = 2.0*a1*(v1-v2)+2.0*a2*v2+l0/sqrt(2.0*Eo/Mi/_mp);
	   break;
	case 2 :
	   DT = a1*(v1-v2)+a2*(v2-v3);
	   break;
     }

     if (Mode == 2) {
	E1 = Ee + Q*V2;
	REAL cos_th1 = cos((Th1-Th/2.0)/180.0*M_PI);
        REAL cos2_th1 = cos_th1*cos_th1;
	REAL Ez1 = E1*cos2_th1;
        if (Ez1 < Q*V2) return 4;
	v1 = sqrt(2.0*Ez1/Mo/_mp);
        a1 = l1*Mo*_mp/Q/V1;
	v2 = sqrt(2.0*(Ez1-Q*V1)/Mo/_mp);
	a2 = l2*Mo*_mp/Q/(V2-V1);
	v3 = sqrt(2.0*(Ez1-Q*V2)/Mo/_mp);
	DT += (a1*(v1-v2)+a2*(v2-v3)+l0/sqrt(2.0*E1/Mf/_mp));
     }

     if (fabs(DV) > 1.0e-5) {
	E1 = (Mode == 2) ? Ee+Q*V2 : Eo;
        v1 = sqrt(2.0*E1/Mo/_mp);
	a1 = DL/_c*Mo*_mp/Q/DV;
        v2 = sqrt(2.0*(E1-Q*DV)/Mo/_mp);
	DT += (a1*(v1-v2)+DL/_c/v2);
     }

     return TRUE;
}

void ImpactApp::InitMainWindow()
{
     MainWindow = new ImpactWindow(NULL, "Surface Reactivity");
}

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
  LPSTR lpCmdLine, int nCmdShow)
{
     ImpactApp MyApp("Surface Reactivity", hInstance, hPrevInstance,
               lpCmdLine, nCmdShow);
     MyApp.Run();
     return MyApp.Status;
}
