#include "cft.h"
#include "cftfunc.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mem.h>

REAL Det(REAL A[3][3])
{
     REAL sum = 0.0;

     sum += (A[0][0]*(A[1][1]*A[2][2]-A[1][2]*A[2][1]));
     sum -= (A[0][1]*(A[1][0]*A[2][2]-A[1][2]*A[2][0]));
     sum += (A[0][2]*(A[1][0]*A[2][1]-A[1][1]*A[2][0]));
     return sum;
}

BOOL SpectrumWindow::LeastSquareFit(LONG NP, REAL HUGE *X, REAL HUGE *Y, REAL *a)
{
     REAL x0, y0, x, x2, x3, x4, y, xy, x2y;
     REAL Delta, A[3][3], B[3][3];

     x = x2 = x3 = x4 = y = xy = x2y = 0.0;
     for (LONG i = 0; i < NP; i++) {
	      x0   = X[i];
	      y0   = Y[i];
	      x   += x0;
	      x2  += x0*x0;
	      x3  += x0*x0*x0;
	      x4  += x0*x0*x0*x0;
	      y   += y0;
	      xy  += x0*y0;
         x2y += x0*x0*y0;
     }

     B[0][0] = (REAL)NP;
     B[0][1] = x;
     B[0][2] = x2;
     B[1][0] = x;
     B[1][1] = x2;
     B[1][2] = x3;
     B[2][0] = x2;
     B[2][1] = x3;
     B[2][2] = x4;

     if ((Delta = Det(B)) == 0.0) return FALSE;

     memcpy(A, B, (size_t)(sizeof(REAL)*9));
     A[0][0] = y;
     A[1][0] = xy;
     A[2][0] = x2y;
     a[0] = Det(A)/Delta;

     memcpy(A, B, (size_t)(sizeof(REAL)*9));
     A[0][1] = y;
     A[1][1] = xy;
     A[2][1] = x2y;
     a[1] = Det(A)/Delta;

     memcpy(A, B, (size_t)(sizeof(REAL)*9));
     A[0][2] = y;
     A[1][2] = xy;
     A[2][2] = x2y;
     a[2] = Det(A)/Delta;

     return TRUE;
}

// Least Square Fit with Calibration Data
BOOL SpectrumWindow::CalculCalibCoeff(SHORT n)
{
     LONG NP = SP[n]->NoCalib;
     HGLOBAL hgZ[2];
     REAL HUGE *Z[2];

     for (SHORT i = 0; i < 2; i++) {
	 if ((hgZ[i] = AllocHGlobal(GPTR, (DWORD)sizeof(REAL)*NP)) != NULL) {
            Z[i] = (REAL HUGE *)GlobalLock(hgZ[i]);
	 } else {
            return FALSE;
	 }
     }
     for (i = 0; i < (SHORT)NP; i++) {
	 Z[0][i] =  sqrt(CT[n][i].Mass);
	 Z[1][i] = CT[n][i].Time;
     }

     return LeastSquareFit(NP, Z[0], Z[1], SP[n]->A.a);
}

REAL SpectrumWindow::CalculMass(SHORT n, LONG i)
{
     REAL x = SP[n]->xGain*(REAL)i+SP[n]->xOffset;
     REAL y, det;

     if (SP[n]->A.a[2] == 0) {
	if (SP[n]->A.a[1] == 0.0) return 0.0;
        y = (x-SP[n]->A.a[0])/SP[n]->A.a[1];
        return (y > 0.0) ? (y*y) : 0.0;
     } else {
	if (SP[n]->A.a[1] == 0.0) {
	   return ((x-SP[n]->A.a[0])/SP[n]->A.a[2]);
	}
	det = SP[n]->A.a[1]*SP[n]->A.a[1]-4.0*SP[n]->A.a[2]*(SP[n]->A.a[0]-x);
	if (det < 0.0) return 0.0;
	y = (sqrt(det)-SP[n]->A.a[1])/2.0/SP[n]->A.a[2];
	return (y > 0.0) ? (y*y) : 0.0;
     }
}

SHORT MatrixInverse(SHORT n, MATRIX a, REAL *det)
{
   SHORT i, j, k, l, IK[MAX_COEFFMAT], JK[MAX_COEFFMAT];
   REAL  amax, buf;

   *det = 1.0;
   for (i = 0; i < MAX_COEFFMAT; i++) {
       IK[i] = JK[i] = -1;
   }
   for (k = 0; k < n; k++) {
       amax = 0.0;
       do {
	  do {
	     for (i = k; i < n; i++)
	     for (j = k; j < n; j++) if (fabs(amax) <= fabs(a[i][j])) {
					amax  = a[i][j];
					IK[k] = i;
					JK[k] = j;
				     }            
	     if (amax == 0.0) {
		*det = 0.0;
		return 0;
	     }            
	     i = IK[k];                                            
	  } while (i < k);   
	  if (i > k) for (j = 0; j < n; j++) {
			 buf = a[k][j];
			 a[k][j] = a[i][j];
			 a[i][j] = -buf;
		     }       
	  j = JK[k];
       } while (j < k);   
       if (j > k) for (i = 0; i < n; i++) {
		      buf = a[i][k];
		      a[i][k] = a[i][j];
		      a[i][j] = -buf;
		  }    
       for (i = 0; i < n; i++) 
	   if (i != k) a[i][k] /= (-amax);
       for (i = 0; i < n; i++)
       for (j = 0; j < n; j++) 
	   if ((i != k) && (j != k)) a[i][j] += (a[i][k]*a[k][j]);
       for (j = 0; j < n; j++) 
	   if (j != k) a[k][j] /= amax;
       a[k][k] = 1.0 / amax;
       *det *= amax;
   }   
   for (l = 0; l < n; l++) {
       k = n-l-1;
       j = IK[k];
       if (j > k) for (i = 0; i < n; i++) {
		      buf = a[i][k];
		      a[i][k] = -a[i][j];
		      a[i][j] = buf;
		  } 
       i = JK[k];
       if (i > k) for (j = 0; j < n; j++) {
		      buf = a[k][j];
		      a[k][j] = -a[i][j];
		      a[i][j] = buf;
		  }  
   }
   return 1;
}

BOOL SpectrumWindow::AllocMemoryForFit()
{
     BOOL OX = TRUE;
     if (hgXY == NULL) {
	if ((hgXY = AllocHGlobal(GPTR, (DWORD)sizeof(POINTXY)*(DWORD)MAX_MARK)) != NULL) {
	  XY = (POINTXY FAR *)GlobalLock(hgXY);
	} else OX = FALSE;
     } else OX = TRUE;
     return OX;
}


BOOL SpectrumWindow::SetFitMemory(SHORT n)
{
     if (!AllocMemoryForFit()) return FALSE;
     REAL xGain   = SP[n]->xGain;
     REAL xOffset = SP[n]->xOffset;
     REAL yGain   = SP[n]->yGain;
     REAL yOffset = SP[n]->yOffset;
     for (SHORT i = 0; i < SP[n]->NoMark; i++) {
	 XY[i].x = xGain*(REAL)MK[n][i]+xOffset;
	 XY[i].y = yGain*(REAL)Y[n][MK[n][i]]+yOffset;
     }
     return TRUE;
}

REAL sqr(REAL x)
{
     return (x*x);
}

REAL FitFunction(REAL x, FITCOEFF A, SHORT, FITMODE Mode)
{
     REAL y;
     y = (Mode == LINEAR)    ? (A[0]+A[1]*x) :
         (Mode == QUADRATIC) ? (A[0]+A[1]*sqr(x-A[2])) : (A[0]*sin(2.0*M_PI/A[1]*(x-A[2])));
     return y;
}

REAL SpectrumWindow::ChiSquare(SHORT N, FITCOEFF A, SHORT Nc, FITMODE Mode)
{
     REAL chisqr = 0.0;
     for (SHORT i = 0; i < N; i++) 
         chisqr += sqr(XY[i].y - FitFunction(XY[i].x, A, Nc, Mode));
     return chisqr;
}

BOOL SpectrumWindow::AlphaBeta(SHORT N, FITCOEFF A, SHORT Nc, FITMODE Mode, MATRIX alpha,
			       FITCOEFF beta)
{
     FITCOEFF A0;
     REAL     chi0;
     REAL     dA = 0.00001;
     FITCOEFF deriv1[2];
     SHORT i, j, k;

     if ((Mode == SINUSOIDAL) && (A[1] == 0.0)) return FALSE;
     chi0 = ChiSquare(N, A, Nc, Mode);
     for (i = 0; i < Nc; i++) {
	 for (k = 0; k < Nc; k++) A0[k] = A[k];
	 A0[i] += dA;
	 if ((Mode == SINUSOIDAL) && (A0[1] == 0.0)) return FALSE;
	 deriv1[0][i] = ChiSquare(N, A0, Nc, Mode);
	 A0[i] -= (2.0*dA);
	 if ((Mode == SINUSOIDAL) && (A0[1] == 0.0)) return FALSE;
	 deriv1[1][i] = ChiSquare(N, A0, Nc, Mode);
	 beta[i] = (deriv1[1][i] - deriv1[0][i])/2.0/dA;
	 alpha[i][i] = (deriv1[0][i] + deriv1[1][i] - 2.0*chi0)/dA/dA;
     }
     for (i = 0; i < Nc; i++) 
     for (j = i+1; j < Nc; j++) {
	 for (k = 0; k < Nc; k++) A0[k] = A[k];
	 A0[i] += dA;
	 A0[j] += dA;
	 if ((Mode == SINUSOIDAL) && (A0[1] == 0.0)) return FALSE;
	 alpha[i][j] = (ChiSquare(N, A0, Nc, Mode) - deriv1[0][i] - deriv1[0][j] + chi0)/dA/dA;
	 alpha[j][i] = alpha[i][j];
     }

     return TRUE;
}


BOOL SpectrumWindow::SearchBackground(SHORT n, FITCOEFF A, FITMODE Mode)
{
     if (!SetFitMemory(n)) return FALSE;
     HGLOBAL  hgAlpha;
     MATRIX   FAR *alpha;
     FITCOEFF beta;
     FITCOEFF A1, DA;

     if ((hgAlpha = AllocHGlobal(GPTR, (DWORD)sizeof(MATRIX)*2L)) == NULL) return FALSE;
     alpha = (MATRIX FAR *)GlobalLock(hgAlpha);

     SHORT N  = SP[n]->NoMark;
     SHORT Nc = (Mode == LINEAR) ? 2 : 3;
     REAL  chisqr0, chisqr1;
     REAL  FitError;
     REAL  det;
     REAL  lambda = 0.1;
     SHORT i, j;
     
     do {
	if ((Mode == SINUSOIDAL) && (A[1] == 0.0)) return FALSE;
	chisqr0 = ChiSquare(N, A, Nc, Mode);
	if (!AlphaBeta(N, A, Nc, Mode, alpha[0], beta)) return FALSE;
	do {
	   for (j = 0; j < Nc; j++) A1[j] = A[j];
	   for (i = 0; i < Nc; i++)
	   for (j = 0; j < Nc; j++) alpha[1][i][j] = alpha[0][i][j];
	   for (i = 0; i < Nc; i++) alpha[1][i][i] *= (1.0+lambda);
	   if (MatrixInverse(Nc, alpha[1], &det) == 0) {
	      FreeHGlobal(hgAlpha);
              return FALSE;
	   }
	   for (i = 0; i < Nc; i++) {
	       DA[i] = 0.0; 
	       for (j = 0; j < Nc; j++)
		   DA[i] += (beta[j]*alpha[1][i][j]);
               A1[i] += DA[i];
           }
	   if ((Mode == SINUSOIDAL) && (A1[1] == 0.0)) return FALSE;
	   chisqr1 = ChiSquare(N, A1, Nc, Mode);
	   if (chisqr1 > chisqr0) lambda *= 10.0;
	} while (chisqr1 > chisqr0);
	FitError = fabs(chisqr0 - chisqr1)/chisqr1;
	lambda /= 10.0;
	for (j = 0; j < Nc; j++) A[j] = A1[j];
     } while (FitError > 0.0001) ;

     FreeHGlobal(hgAlpha);
     return TRUE;
}

void SpectrumWindow::ScalingAfterBKSubtraction(SHORT n, FITCOEFF A, FITMODE Mode,
                                               REAL &YGAIN, REAL &YOFFSET)
{
     REAL xGain   = SP[n]->xGain;
     REAL xOffset = SP[n]->xOffset;
     REAL yGain   = SP[n]->yGain;
     REAL yOffset = SP[n]->yOffset;
     REAL x, y1, y2, y;
     REAL min = +1.0e+300;
     REAL max = -1.0e+300;

     for (LONG i = 0; i < SP[n]->Ntot; i++) {
	 x  = xGain*(REAL)i+xOffset;
	 y1 = yGain*Y[n][i]+yOffset;
	 y2 = FitFunction(x, A, 6, Mode);
	 y =  y1 - y2;
	 if (min > y) min = y;
         if (max < y) max = y;
     }

     REAL DH = (max - min)/10.0;
     min -= DH;
     max += DH;

     YGAIN   = (max-min)/65536.0;
     YOFFSET = (max+min)/2.0;
}

BOOL SpectrumWindow::WriteBKSubSpectrum(SHORT ns, SHORT nd, FITCOEFF A, FITMODE Mode,
				 REAL YGAIN, REAL YOFFSET)
{
     if (!UpdateMemory(nd, SP[ns]->Ntot*2L)) return FALSE;
     memcpy(SP[nd], SP[ns], sizeof(SPECTRUMINFO));
     memcpy(PL[nd], PL[ns], sizeof(PLOTINFO));
     memcpy(EX[nd], EX[ns], sizeof(EXPERIMENTINFO));
     if (SP[nd]->NoCalib > 0)
     memcpy(CT[nd], CT[ns], sizeof(CALIBTABLE)*SP[nd]->NoCalib);
     if (SP[nd]->NoCursorText > 0)
     memcpy(TE[nd], TE[ns], sizeof(CURSORTEXT)*SP[nd]->NoCursorText);
     if (SP[nd]->NoMark > 0)
     memcpy(MK[nd], MK[ns], sizeof(LONG)*SP[nd]->NoMark);
     SP[nd]->yGain   = YGAIN;
     SP[nd]->yOffset = YOFFSET;

     REAL xGain   = SP[ns]->xGain;
     REAL xOffset = SP[ns]->xOffset;
     REAL yGain   = SP[ns]->yGain;
     REAL yOffset = SP[ns]->yOffset;
     REAL x, y1, y2, y;
     for (LONG i = 0; i < SP[nd]->Ntot; i++) {
	 x  = xGain*(REAL)i+xOffset;
	 y1 = yGain*Y[ns][i]+yOffset;
	 y2 = FitFunction(x, A, 6, Mode);
	 y =  y1 - y2;
	 Y[nd][i] = (SHORT)ROUND((y-YOFFSET)/YGAIN);
     }
     SP[nd]->yMin = 1.0e300;
     SP[nd]->yMax = -1.0e300;
     REAL I;
     for (i = 0; i < SP[nd]->Ntot; i++) {
         I = SP[nd]->yGain*(REAL)Y[nd][i] + SP[nd]->yOffset;
         if (I < SP[nd]->yMin) SP[nd]->yMin = I;
         if (I > SP[nd]->yMax) SP[nd]->yMax = I;
     }
     REAL DW = (SP[nd]->yMax-SP[nd]->yMin)*0.1;
     SP[nd]->yMin -= DW;
     SP[nd]->yMax += DW;
     GetDefaultPlotInfo(nd);
     InitialCursorState(nd);
     LoadOX[nd] = TRUE;
     return TRUE;
}

BOOL SpectrumWindow::WriteSpectrumSum(SUMSTRUCT SS)
{
     SHORT Ns = SS.Source;
     SHORT Nt = SS.Target;
     if (UpdateMemory(Nt, Size[Ns]/(LONG)SS.N+1L)) {
	memcpy(SP[Nt], SP[Ns], sizeof(SPECTRUMINFO));
	memcpy(PL[Nt], PL[Ns], sizeof(PLOTINFO));
	memcpy(EX[Nt], EX[Ns], sizeof(EXPERIMENTINFO));
	SP[Nt]->NoCalib = SP[Nt]->NoCursorText = SP[Nt]->NoMark = 0;
	SP[Nt]->Ntot = SP[Ns]->Ntot/(LONG)SS.N;
	SP[Nt]->yGain *= (REAL)SS.N;
	SP[Nt]->yOffset *= (REAL)SS.N;
	REAL y;
        LONG i, j, RN = (LONG)SS.N;
	for (i = 0; i < SP[Nt]->Ntot; i++) {
            y = 0.0;
	    for (j = 0; j < RN; j++)
		y += (SP[Ns]->yGain*(REAL)Y[Ns][RN*i+j]+SP[Ns]->yOffset);
	    Y[Nt][i] = (SHORT)ROUND((y-SP[Nt]->yOffset)/SP[Nt]->yGain);
	}
	RN = SP[Ns]->Ntot%(LONG)SS.N;
	if (RN != 0L) {
           LONG N = SP[Nt]->Ntot*SS.N;
	   y = 0.0;
	   for (j = 0; j < RN; j++)
		y += (SP[Ns]->yGain*(REAL)Y[Ns][N+j]+SP[Ns]->yOffset);
	   y = y*(REAL)SS.N/(REAL)RN;
	   Y[Nt][SP[Nt]->Ntot++] = (SHORT)ROUND((y-SP[Nt]->yOffset)/SP[Nt]->yGain);
	}
	SP[Nt]->xOffset = SP[Nt]->xGain*(REAL)(SS.N-1)/2.0;
	SP[Nt]->xGain  *= (REAL)SS.N;
	SP[Nt]->yMax   *= (REAL)SS.N;
	SP[Nt]->yMin   *= (REAL)SS.N;
	SP[Nt]->xMax    =  SP[Nt]->xGain*(REAL)(SP[Nt]->Ntot-1L)+SP[Nt]->xOffset;
	SP[Nt]->xMin    = SP[Nt]->xOffset;
	GetDefaultPlotInfo(Nt);
	InitialCursorState(Nt);
	LoadOX[Nt] = TRUE;       
        return TRUE;
     } else return FALSE; 
}

BOOL SpectrumWindow::WriteLSSmoothing(SUMSTRUCT SS)
{
     SHORT Ns = SS.Source;
     SHORT Nt = SS.Target;
     if (SS.N < 5) return FALSE;
     if (SP[Ns]->Ntot < (LONG)SS.N) return FALSE;
     if (UpdateMemory(Nt, Size[Ns])) {
	HGLOBAL hgZ[2];
	REAL HUGE *Z[2];
	LONG NP = (LONG)SS.N;

        for (SHORT i = 0; i < 2; i++) {
	    if ((hgZ[i] = AllocHGlobal(GPTR, (DWORD)sizeof(REAL)*NP)) != NULL) {
               Z[i] = (REAL HUGE *)GlobalLock(hgZ[i]);
	    } else {
               return FALSE;
	    }
        }
	memcpy(SP[Nt], SP[Ns], sizeof(SPECTRUMINFO));
	memcpy(PL[Nt], PL[Ns], sizeof(PLOTINFO));
	memcpy(EX[Nt], EX[Ns], sizeof(EXPERIMENTINFO));
	SP[Nt]->NoCalib = SP[Nt]->NoCursorText = SP[Nt]->NoMark = 0;

	LONG Ni = (LONG)(SS.N-1)/2L;
	LONG Nf = (LONG)SS.N-Ni-1L;
	LONG  k;
        REAL  a[3];

	for (LONG j = 0L; j < SP[Ns]->Ntot; j++) {
	    if ((j-Ni) < 0) {
	       for (k = 0; k < (LONG)SS.N; k++) {
		   Z[0][k] = (REAL)k;
                   Z[1][k] = (REAL)Y[Ns][k];
	       }
	    } else if ((j+Nf) >= SP[Ns]->Ntot) {
	       for (k = 0; k < (LONG)SS.N; k++) {
		   Z[0][k] = (REAL)(SP[Ns]->Ntot-k-1L);
		   Z[1][k] = (REAL)Y[Ns][SP[Ns]->Ntot-k-1L];
	       }
	    } else {
	       for (k = 0; k < (LONG)SS.N; k++) {
		   Z[0][k] = (REAL)(j-Ni+k);
		   Z[1][k] = (REAL)Y[Ns][j-Ni+k];
	       }
	    }

	    if (!LeastSquareFit(NP, Z[0], Z[1], a)) return FALSE;
	    Y[Nt][j] = (SHORT)ROUND(a[0]+a[1]*(REAL)j+a[2]*(REAL)j*(REAL)j); 
	} 
	InitialCursorState(Nt);
	LoadOX[Nt] = TRUE;
        return TRUE;
     } else return FALSE; 
}