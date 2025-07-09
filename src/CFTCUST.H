#ifndef __CFTCUST_H
#define __CFTCUST_H

#define STRICT
#include <windows.h>

#define HUGE _huge
#define LEN_CURSORTEXT 40

typedef signed int SHORT;
typedef char       CHAR;
typedef double     REAL;
typedef float      FLOAT;
typedef REAL CALIBTABLE[2];

typedef struct {
	   CHAR  Title[128];
	   CHAR  XUnit[80];
           CHAR  YUnit[80];
	} TITLESTRUCT;

typedef struct {
	   REAL  a[3];
	} COEFFSTRUCT;

// Definition of the variable type for Spectrum Informations.
typedef struct {
   WORD  Version;       // CFighter Version.
   CHAR  Desc[16];      // Descriptor
   CHAR  FileName[256]; // File Name
   UINT  nFileOffset;
   CHAR  Time[20];      // Time
   CHAR  Date[20];      // Date
   TITLESTRUCT TS;      // Title of spectrum, X-axis Unit, and Y-axis Unit
   LONG  Ntot;          // Total number of channel
   REAL  xMin;          // Minimum of x
   REAL  xMax;          // Maximum of x
   REAL  yMin;          // Minimum of y
   REAL  yMax;          // Maximum of y
   REAL  xGain;         // Horizontal gain
   REAL  xOffset;       // Horizontal offset
   REAL  yGain;         // Vertical gain 70
   REAL  yOffset;       // Vertical offset
   SHORT NoCalib;       // The Number of Calibration Data Points
   COEFFSTRUCT A;       // Coefficients of calibration equation
   SHORT NoCursorText;        // number of text
} SPECTRUMINFO;

// Definition of the variable type for PLOT Informations.
typedef struct {
   LONG  Initial;    // Initial channel to be plotted
   LONG  Ntot;       // Total number of channel to be plotted
   REAL  xMin;       // Minimum of value of data to be plotted
   REAL  xMax;       // Maximum of value of data to be plotted
   REAL  yMin;       // Minimum of value of data to be plotted
   REAL  yMax;       // Maximum of value of data to be plotted
} PLOTINFO;

// Experimental SetUp Parameters (binary type)
typedef struct {
   CHAR  Element[20];   // Essential Material
   FLOAT Four;
   FLOAT Couvercle;
   FLOAT He;
   FLOAT Ar;
   CHAR  Other[40];
   CHAR  Ioniseur[40];
   FLOAT IEnergie;
   CHAR  IComment[80];
   CHAR  Ioniseur1[40];
   FLOAT IEnergie1;
   CHAR  IComment1[80];
   FLOAT Accel1;
   FLOAT Accel2;
   FLOAT Accel3;
   FLOAT Lentille1;
   FLOAT Lentille2;
   FLOAT Chican21;
   FLOAT Chicane2;
   FLOAT ReflecV1;
   FLOAT ReflecV2;
   FLOAT ReflecV3;
   FLOAT ReflecAngle;
   CHAR  RComment[80];
   CHAR  Commentaire[3][80];
} EXPERIMENTINFO;

typedef struct {
   REAL TOF;
   CHAR  CText[LEN_CURSORTEXT];
} CTEXTSTRUCT;

typedef struct {
   LONG  Position;
   CHAR  TextStr[LEN_CURSORTEXT];
} CURSORTEXT;

#ifdef __cplusplus
extern "C" {
#endif

SHORT HUGE * AllocMemoryForSpectrum(HGLOBAL *hg, DWORD mSize);
void FreeHGlobal(HGLOBAL *hg);
BOOL InitializeCustom(HINSTANCE hInst);
BOOL TerminateCustom(void);
BOOL ConnectWithCFighter(void);
BOOL GetSpectrumInfo(SPECTRUMINFO FAR *SP);
BOOL GetSpectrum(SHORT HUGE *Y, DWORD mSize);
BOOL GetCursorPosition(REAL *R);
BOOL SetCursorPosition(REAL R);
BOOL SetCursorText(REAL Time, CHAR *str);

#ifdef __cplusplus
}
#endif

#endif