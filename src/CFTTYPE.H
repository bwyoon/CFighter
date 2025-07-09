#ifndef __CFTTYPE_H
#define __CFTTYPE_H

#include "cftdef.h"
#include <windows.h>

#define HUGE _huge

typedef signed int SHORT;
typedef char       CHAR;
typedef double     REAL;
typedef float      FLOAT;

enum FILETYPE { CFT110, CFT100, LECROY9400, LECROY9350, IPNDMI400, OTHERFILETYPE};
enum { CONFIGINTERDIT, CONFIGABSENT, CONFIGPRESENT };
enum WINTYPE { SPECTRUMWIN, INFOWIN, FRAGMENTWIN };
enum YSHIFTMODE  { SY_UP,   SY_DOWN  };
enum XSHIFTMODE  { SX_LEFT, SX_RIGHT };
enum SCALEMODE   { SC_UP,   SC_DOWN  };
enum CURSORMODE  { CU_SINGLE, CU_THREE };
enum WHICHCURSOR { CENTER_CURSOR, LEFT_CURSOR, RIGHT_CURSOR };
enum { DDE_SPECINFO, DDE_PLOTINFO, DDE_EXPINFO, DDE_SPECTRUM, DDE_GETCP, DDE_SETCP,
       DDE_INSERTCT };
enum DEVICETYPE  { SCREEN, PRINTER };
enum FITMODE     { LINEAR, QUADRATIC, SINUSOIDAL };

typedef struct {
	   REAL Time;
	   REAL Mass;
	} CALIBTABLE;

typedef struct {
	   CHAR  Title[128];
	   CHAR  XUnit[80];
           CHAR  YUnit[80];
	} TITLESTRUCT;

typedef struct {
	   SHORT Num;
	   REAL  Time;
	   REAL  Mass;
	} MASSSTRUCT;

typedef struct {
	   REAL  a[3];
	} COEFFSTRUCT;

typedef struct {
	   CHAR  DlgTitle[40];
	   CHAR  MinTitle[40];
	   CHAR  MaxTitle[40];
	   REAL  Min;
	   REAL  Max;
	   BOOL  Option;
	} MINMAXSTRUCT;

typedef struct {
	   REAL Area[14];
	} AREA;

typedef struct {
	   CHAR   Title[80];
	   CHAR   FileName[256];
           UINT   nFileOffset;
           DWORD  FileSize;
	} FILENAMESTRUCT;

typedef struct {
           RECT   Rect;
	   POINT  Date;
           POINT  Element;
	   POINT  Title;
	   POINT  fName;
	   POINT  xUnit;
	   POINT  yUnit;
	   SHORT  xGridW;
	   SHORT  yGridW;
	   SHORT  xValue;
           SHORT  yValue;
	   RECT   Plot;
	   SHORT  PlotThick;
	   SHORT  FrameThick;
	   SHORT  GridThick;
	   SHORT  FontHeight;
	} PLOTSTRUCT;

typedef struct {
	   signed long Ntotal;
	   signed long Ni;
	   signed long Nf;
	   double yGain;
	   double yOffset;
	   double xGain;
	   double xOffset;
	} TR9350INFO;

typedef struct {
	  CHAR  BrdName[20];
	  CHAR  DevName[20];
	  SHORT DevAd;
	} GPIBSTRUCT;

typedef struct {
	  BOOL Title;
          BOOL FileName;
          BOOL Date;
          BOOL Element;
          BOOL Frame;
          BOOL XGrid;
          BOOL XGridLabel;
          BOOL YGrid;
          BOOL YGridLabel;
          BOOL XUnit;
	  BOOL YUnit;
	} HIDEPLOT;

typedef struct {
   LONG  Position;
   CHAR  TextStr[LEN_CURSORTEXT];
} CURSORTEXT;

typedef REAL MATRIX[MAX_COEFFMAT][MAX_COEFFMAT];
typedef REAL FITCOEFF[MAX_COEFFMAT];

typedef struct {
   REAL  TOF;
   CHAR  CText[LEN_CURSORTEXT];
} CTEXTSTRUCT;

typedef struct {
   REAL x;
   REAL y;
} POINTXY;

typedef struct {
   FITMODE  Mode;
   FITCOEFF a;
   SHORT    Source;
   SHORT    Target;
} BKSUBSTRUCT;

typedef struct {
   SHORT Source;
   SHORT Target;
   SHORT N;
} SUMSTRUCT;

typedef struct {
   WORD  Version;       // CFighter Version.
   CHAR  Desc[16];      // Descriptor
   CHAR  FileName[256]; // File Name
   UINT  nFileOffset;   // 
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
   REAL  yGain;         // Vertical gain
   REAL  yOffset;       // Vertical offset
   COEFFSTRUCT A;       // Coefficients of calibration equation
   SHORT NoCalib;       // The Number of Calibration Data Points
   SHORT NoCursorText;  // number of text
   SHORT NoMark;        // number of Mark
   SHORT Reserved1;
   SHORT Reserved2;
   SHORT Reserved3;
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
   FLOAT Chicane1;
   FLOAT Chicane2;
   FLOAT ReflecV1;
   FLOAT ReflecV2;
   FLOAT ReflecV3;
   FLOAT ReflecAngle;
   CHAR  RComment[80];
   CHAR  Commentaire[3][80];
} EXPERIMENTINFO;

typedef struct {
   REAL Seconds;
   BYTE Minutes;
   BYTE Hours;
   BYTE Days;
   BYTE Months;
   WORD Year;
   WORD Unused;
} TIMESTAMP;

// Wave Description
typedef struct {
   CHAR  DescriptorName[16];
   CHAR  TemplateName[16];
   SHORT CommType;
   SHORT CommOrder;
   LONG  WaveDescriptor;
   LONG  UserText;
   LONG  ResDesc1;
   LONG  TrigTimeArray;
   LONG  RisTimeArray;
   LONG  ResArray1;
   LONG  WaveArray1;
   LONG  WaveArray2;
   LONG  ResArray2;
   LONG  ResArray3;
   CHAR  InstrumentArray[16];
   LONG  InstrumentNumber;
   CHAR  TraceLabel[16];
   WORD  Reserved1;
   WORD  Reserved2;
   LONG  WaveArrayCount;
   LONG  PntsPerScreen;
   LONG  FirstValidPnt;
   LONG  LastValidPnt;
   LONG  FirstPoint;
   LONG  SparsingFactor;
   LONG  SegmentIndex;
   LONG  SubarrayCount;
   LONG  SweepsPerAcq;
   LONG  Obsolete1;
   FLOAT VerticalGain;
   FLOAT VerticalOffset;
   FLOAT MaxValue;
   FLOAT MinValue;
   WORD  NominalBits;
   WORD  NomSubarrayCount;
   FLOAT HorizInterval;
   REAL  HorizOffset;
   REAL  PixelOffset;
   CHAR  VertUnit[48];
   CHAR  HorUnit[48];
   WORD  Reserved3;
   WORD  Reserved4;
   TIMESTAMP TriggerTime;
   FLOAT AcqDuration;
   SHORT RecordType;
   SHORT ProccessingDone;
   WORD  Reserved5;
   WORD  RisSweeps;
   SHORT TimeBase;
   SHORT VertCoupling;
   FLOAT ProbeAtt;
   SHORT FixedVertGain;
   SHORT BandWidthLimit;
   FLOAT VerticalVernier;
   FLOAT AcqVertOffset;
   SHORT WaveSource;
} WAVEDESC;

typedef struct {
   WORD  Version;       // CFighter Version.
   SHORT FileType;      // File Type
   BOOL  SaveOX;        // Whether it is saves as CFighter File
   CHAR  FileDir[80];   // Directory of file
   CHAR  FileName[16];  // File Name
   CHAR  Time[20];      // Time
   CHAR  Date[20];      // Date
   CHAR  Material[20];  // Essential Material
   CHAR  Title[80];     // Title of spectrum
   CHAR  xTitle[40];    // Title of x-axis of spectrum
   CHAR  yTitle[40];    // Title of y-axis of spectrum
   LONG  Ntot;          // Total number of channel
   SHORT yMin;          // Minimum of spectra
   SHORT yMax;          // Maximum of spectra
   REAL  xGain;         // Horizontal gain
   REAL  xOffset;       // Horizontal offset
   REAL  yGain;         // Vertical gain
   REAL  yOffset;       // Vertical offset
   REAL  a[3];          // Coefficients of calibration equation
   SHORT NoCalib;       // The Number of Calibration Data Points
   SHORT Config;        // Information of configuration
   CHAR  Reserved1[80]; // For the Future Use 
   SHORT Reserved2[5];  // For the Future Use
   REAL  Reserved3[5];  // For the Future Use 
} SPECTRUMINFO100;

typedef struct {
   SHORT PlotType;   // Plot Type  ex) pixel plot, line plot, histogram
   BOOL  BkLineOX;   // Background on or not
   LONG  Initial;    // Initial channel to be plotted
   LONG  Ntot;       // Total number of channel to be plotted
   LONG  LMark;      // Left mark channel
   LONG  RMark;      // Right mark channel
   LONG  Cursor;     // Cursor channel
   REAL  yMin;       // Minimum of value of data to be plotted
   REAL  yMax;       // Maximum of value of data to be plotted
   REAL  yWidth;     // Width of y-value to be plotted 
   SHORT yDisp;      // displacement of x-axix to be plotted
   REAL  BkCoeff[2]; // Background Coefficients
} PLOTINFO100;

typedef struct {
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
   FLOAT ReflecV1;
   FLOAT ReflecV2;
   FLOAT ReflecV3;
   FLOAT ReflecAngle;
   CHAR  RComment[80];
   CHAR  Commentaire[3][80];
} EXPINFO100;

typedef struct {
   CHAR RUN[16];
   CHAR NUMR[8];
   CHAR NOM[16];
   CHAR NUMS[8];
   CHAR DATE[20];
   CHAR TYPE[12];
   CHAR CODE[8];
   CHAR NCX[8];
   CHAR DX[8];
   CHAR NCY[8];
   CHAR DY[8];
   CHAR NCZ[8];
   CHAR DZ[8];
   CHAR NCT[8];
   CHAR DT[8];
   CHAR SWAP[4];
   CHAR RES1[4];
   CHAR COMMENT[80];
   CHAR IDAPLIC[8];
   CHAR HLONG[8];
   CHAR APPLIC[16];
   CHAR VERLAB[8];
   CHAR NBMQ[8];
   CHAR AX[16];
   CHAR BX[16];
   CHAR UX[8];
   CHAR AY[16];
   CHAR BY[16];
   CHAR UY[8];
   CHAR LIBRE[16];
   CHAR MQ[16][7];
   CHAR FIN[16];
} IPNLAB;

typedef struct {
	SHORT nTot;
        SHORT nSeg;
	REAL  Threshold;
	BOOL  IsUpperBound;
	DWORD IdlingTime;
} COUNTSTRUCT;

#endif