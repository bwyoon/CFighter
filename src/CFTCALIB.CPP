#include "cft.h"
#include <stdlib.h>
#include <string.h>
#include <mem.h>

REAL Det(REAL A[3][3])
{
     REAL sum = 0.0;

     sum += (A[0][0]*(A[1][1]*A[2][2]-A[1][2]*A[2][1]));
     sum -= (A[0][1]*(A[1][0]*A[2][2]-A[1][2]*A[2][0]));
     sum += (A[0][2]*(A[1][0]*A[2][1]-A[1][1]*A[2][0]));
     return sum;
}

// Least Square Fit with Calibration Data
BOOL SpectrumWindow::CalcCoeff(SHORT n)
{
     REAL N = (REAL)SP[n]->NoCalib;
     REAL x0, y0, x, x2, x3, x4, y, xy, x2y;
     REAL Delta, A[3][3], B[3][3];

     x = x2 = x3 = x4 = y = xy = x2y = 0.0;
     for (int i = 0; i < SP[n]->NoCalib; i++) {
	      x0   = CT[n][i].Mass;
	      y0   = CT[n][i].Time;
	      x   += x0;
	      x2  += x0*x0;
	      x3  += x0*x0*x0;
	      x4  += x0*x0*x0*x0;
	      y   += y0;
	      xy  += x0*y0;
         x2y += x0*x0*y0;
     }

     B[0][0] = N;
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
     SP[n]->A.a[0] = Det(A)/Delta;

     memcpy(A, B, (size_t)(sizeof(REAL)*9));
     A[0][1] = y;
     A[1][1] = xy;
     A[2][1] = x2y;
     SP[n]->A.a[1] = Det(A)/Delta;

     memcpy(A, B, (size_t)(sizeof(REAL)*9));
     A[0][2] = y;
     A[1][2] = xy;
     A[2][2] = x2y;
     SP[n]->A.a[2] = Det(A)/Delta;

     return TRUE;
}