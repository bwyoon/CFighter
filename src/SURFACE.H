#ifndef __SURFACE_H
#define __SURFACE_H

#include <owl.h>
#include <edit.h>
#include <button.h>
#include "cftctrl.h"
#include "surfdef.h"

typedef char   CHAR;
typedef double REAL;
enum    FRAGMENTMODE {UNIMOL, IMPACT};

_CLASSDEF(ImpactApp)
class ImpactApp : public TApplication
{
   public:
      ImpactApp(LPSTR AName, HINSTANCE hInstance, HINSTANCE hPrevInstance,
                  LPSTR lpCmdLine, int nCmdShow)
      : TApplication(AName, hInstance, hPrevInstance, lpCmdLine, nCmdShow) {};
      virtual void InitMainWindow();
};

_CLASSDEF(RealEdit)
class RealEdit : public TEdit
{
   private :
      REAL value;
      CHAR str[20];

   public  :
      RealEdit(PTWindowsObject, int, REAL, int, int, int, int, WORD, BOOL);
      virtual void SetupWindow();
      void SetReal(REAL);
      REAL GetReal();
};

_CLASSDEF(ImpactWindow)
class ImpactWindow : public TWindow
{
   private :
      REAL  Mp, Q, To, Vo, V1, V2, L0, L1, L2, Th, DV, DL;
      REAL  Mf, Th1, Ee, TOF, DT;
      BOOL  UniOX;
      BOOL  MassCalcul;

      PRealEdit  pMp, pQ, pTo, pVo, pEc, pV1, pV2, pL0, pL1, pL2, pTh, pDV, pDL;
      PRealEdit  pMf, pTh1, pEe, pTOF;
      PYCheckBox pUniOX;

   public  :
      ImpactWindow(PTWindowsObject, LPSTR);
      ~ImpactWindow();
      virtual void  GetWindowClass(WNDCLASS &AWndClass);
      virtual LPSTR GetClassName()
      { return "Surface1.00"; }
      virtual void WMPaint(RTMessage) = [WM_FIRST+WM_PAINT];
      void CMCalculTOF(RTMessage)  = [ID_FIRST+ID_CALCULTOF];
      void CMCalibrate(RTMessage)  = [ID_FIRST+ID_CALIBRATE];
      void CMCalculMass(RTMessage) = [ID_FIRST+ID_CALCULMASS];
      void CMSetCursor(RTMessage)  = [ID_FIRST+ID_SETCURSOR];
      void CMGetCursor(RTMessage)  = [ID_FIRST+ID_GETCURSOR];
      BOOL CalculTOF();
      BOOL GetParameters();
};

#endif