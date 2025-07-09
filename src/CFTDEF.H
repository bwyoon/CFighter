
#define DESC110 "CFIGHTER_110"        
#define CFT110INI "cft110.ini"
#define TEMPFILE1 "c:\\cfttemp.dat"
#define TEMPFILE2 "c:\\cfttemp.des"

#define MAX_SPEC    5
#define MAX_OBJECT 40
#define MAX_CHILD  1200
#define MAX_CURSORTEXT 80
#define LEN_CURSORTEXT 40
#define MAX_CURSOR     20
#define MAX_CALIB      80
#define MAX_MARK       80
#define MAX_DDETOPIC   10
#define MAX_COEFFMAT    6
#define COEFF_INCREM   0.00001

#define DEFAULTSIZE   10000L

#define CM_NEWWND     0x0010
#define CM_LOAD       0x0011
#define CM_OVERLOAD   0x0012
#define CM_SAVE       0x0013
#define CM_SAVEAS     0x0014
#define CM_SAVEALL    0x0015
#define CM_QPRO       0x1601
#define CM_SPECASCII  0x1602
#define CM_CALIBASCII 0x1603
#define CM_PRINT      0x0017
#define CM_PRINTONFILE      0x001f
#define CM_PRINTSETUP 0x0018
#define CM_ELEMENT    0x0019
#define CM_GETINFO    0x001a
#define CM_INFOWIN    0x001b
#define CM_QUIT       0x001c

#define CM_TITLE         0x0021
#define CM_EXPCONDITION  0x0022
#define CM_CURSORTEXTBIS 0x0023
#define CM_EDITCTEXT     0x0024
#define CM_INSERTCTEXT   0x0025
#define CM_CLEARALLCTEXT 0x0026
#define CM_PUTCURSORTEXT 0x0027
#define CM_ADDMARK       0x0028
#define CM_CLEARMARK     0x0029
#define CM_CLEARSPEC     0x002a

#define CM_TRANSFER9350 0x0031
#define CM_TRANSFER9400 0x0032
#define CM_COUNTACC     0x0033
#define CM_TCONNECT     0x0034

#define CM_TOTALSPEC     0x0041
#define CM_ZOOMAUTO      0x0042
#define CM_ZOOMMAN       0x0043
#define CM_ZOOMALL       0x0044
#define CM_SCALEMAN      0x0045
#define CM_AUTOSCALE     0x0046
#define CM_SAMESCALE     0x0047
#define CM_INVERSE       0x0048
#define CM_PLOTOPTION    0X0049

#define CM_CALIBADD   0x0051
#define CM_CALIBNEW   0x0052
#define CM_CALIBINS   0x0053
#define CM_CALIBVIEW  0x0054
#define CM_CALIBDRAW  0x0055

#define CM_VISUALAREA 0x0061
#define CM_CALCULFRAG 0x0062
#define CM_BKSUBTRACT 0x0063
#define CM_LSSMOOTH   0x0064
#define CM_SPECSUM    0x0065

#define CM_HELP       0x0080
#define CM_CFTHELP    0x0081
#define CM_HELPHELP   0x0082
#define CM_ABOUT      0x0083

#define CM_CHILDDEL   0x0101
                           
#define CM_MEM1       0x0111
#define CM_MEM2       0x0112
#define CM_MEM3       0x0113
#define CM_MEM4       0x0114
#define CM_MEM5       0x0115

#define CM_OVERLAY    0x0121

#define CM_CALIBDO    0x0201
#define CM_CALIBQUIT  0x0202

#define CM_YSHIFTUP     0x0311
#define CM_YSHIFTDOWN   0x0312
#define CM_XSHIFTLEFT   0x0313
#define CM_XSHIFTRIGHT  0x0314
#define CM_FYSHIFTUP    0x0315
#define CM_FYSHIFTDOWN  0x0316
#define CM_FXSHIFTLEFT  0x0317
#define CM_FXSHIFTRIGHT 0x0318

#define CM_SCALEUP      0x0321
#define CM_SCALEDOWN    0x0322
#define CM_ZOOM         0x0323
#define CM_UNZOOM       0x0324
#define CM_FSCALEUP     0x0325
#define CM_FSCALEDOWN   0x0326
#define CM_FZOOM        0x0327
#define CM_FUNZOOM      0x0328

#define TE_BEGIN   0x0001
#define TE_COURSE  0x0002
#define TE_END     0x0004

#define SA_ASCII   0x0001
#define SA_QPRO    0x0002
#define SA_CALIB   0x0003

#define CM_CC_SLEFT      0x1111
#define CM_CC_SRIGHT     0x1121
#define CM_CC_FLEFT      0x1211
#define CM_CC_FRIGHT     0x1221

#define SQR(x) ((x)*(x))
#define ROUND(x) (floor((x)+0.5))
#define min(x,y) (((x) < (y)) ? (x) : (y))

#define IDS_DDEERRORTEXT       0x7001
#define IDS_DDEERRORTITLE      0x7002
#define IDS_SPECWINNAME        0x7003
#define IDS_FATALERRORTEXT     0x7004
#define IDS_FATALERRORTITLE    0x7005
#define IDS_MEMALLOCERRORTEXT  0x7006
#define IDS_MEMALLOCERRORTITLE 0x7007
#define IDS_MULTIPLE           0x7008
#define IDS_OPENSPECTRUM       0x7009
#define IDS_STARTDOC           0x700a
#define IDS_PCONFIGERRORTEXT   0x700b
#define IDS_PCONFIGERRORTITLE  0x700c
#define IDS_FFORMATERRORTEXT   0x700d
#define IDS_FFORMATERRORTITLE  0x700e
#define IDS_FWRITEERRORTEXT    0x700f
#define IDS_FWRITEERRORTITLE   0x7010
#define IDS_FCREATEERRORTEXT   0x7011
#define IDS_FCREATEERRORTITLE  0x7012
#define IDS_DELCALCOEFFTEXT    0x7013
#define IDS_DELCALCOEFFTITLE   0x7014
#define IDS_CONNECTERRORTEXT   0x7015
#define IDS_CONNECTERRORTITLE  0x7016
#define IDS_GWRITEERRORTEXT    0x7017
#define IDS_GWRITEERRORTITLE   0x7018
#define IDS_GREADERRORTEXT     0x7019
#define IDS_GREADERRORTITLE    0x701a
#define IDS_CLEARMARKTEXT      0x701b
#define IDS_CLEARMARKTITLE     0x701c
#define IDS_CLEARCTEXTTEXT     0x701d
#define IDS_CLEARCTEXTTITLE    0x701e
#define IDS_CLEARCALIBTEXT     0x701f
#define IDS_CLEARCALIBTITLE    0x7020
#define IDS_NOCALIBOVERTEXT    0x7021
#define IDS_NOCALIBOVERTITLE   0x7022
#define IDS_CALIBFAILTEXT      0x7023
#define IDS_CALIBFAILTITLE     0x7024
#define IDS_NOCALIBINSUFFTEXT  0x7025
#define IDS_NOCALIBINSUFFTITLE 0x7026
#define IDS_NOMARKOVERTEXT     0x7027
#define IDS_NOMARKOVERTITLE    0x7028
#define IDS_ZOOMDLGTITLE       0x7029
#define IDS_ZOOMDLGMIN         0x702a
#define IDS_ZOOMDLGMAX         0x702b
#define IDS_ALLZOOMDLGTITLE    0x702c
#define IDS_ALLZOOMDLGMIN      0x702d
#define IDS_ALLZOOMDLGMAX      0x702e
#define IDS_SCALEDLGTITLE      0x702f
#define IDS_SCALEDLGMIN        0x7030
#define IDS_SCALEDLGMAX        0x7031
#define IDS_ALLSCALEDLGTITLE   0x7032
#define IDS_NOMARKINSUFFTEXT   0x7033
#define IDS_NOMARKINSUFFTITLE  0x7034
#define IDS_BKSUBFAILTEXT      0x7035
#define IDS_BKSUBFAILTITLE     0x7036
#define IDS_SPECSUMFAILTEXT    0x7037
#define IDS_SPECSUMFAILTITLE   0x7038
#define IDS_LSFILTERFAILTEXT   0x7039
#define IDS_LSFILTERFAILTITLE  0x703a
#define IDS_CLEARSPECTEXT      0x703b
#define IDS_CLEARSPECTITLE     0x703c