#include "gpib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEVICE "LECROY1"
#define SETUP  "WFSU SP, 0, NP, 0, FP, 0, SN, 0; CHDR SHORT"
#define TEMPFILE1 "c:\\cfttemp.dat"
#define TEMPFILE2 "c:\\cfttemp.des"
#define GETMEMTITLE "Type LeCroy memory >> "

typedef struct {
	   signed long Ntotal;
	   signed long Ni;
	   signed long Nf;
	   double yGain;
	   double yOffset;
	   double xGain;
	   double xOffset;
	} TR9350INFO;

char  MEM[12][3] = {
	 "M1", "M2", "M3", "M4", "TA", "TB", "TC", "TD", "C1", "C2", "C3", "C4"
      };

class TR9350 : public GPIB
{
   private :
      short  mem;
      unsigned long   Ntotal;
      TR9350INFO Info;

   public  :
      TR9350(const char *DEV) : GPIB(DEV) {};
      ~TR9350();

      short GetMem();
      short GetSpectrum();
      void  Run();
};

char *CutString(char *str, char *STR)
{
     STR[48] = '\0';
     return strcpy(str, strchr(strchr(STR, ':')+1,':')+1);
}

short TR9350::GetMem()
{
    if (!IsConnected()) return -1;
    char buf[10];
    printf("Type LeCroy memory >> ");
    gets(buf);
    strupr(buf);
    for (int i = 0; i < 12; i++)
	if (strcmp(buf, MEM[i]) == 0) {
	   mem  = i;
	   return i;
	}
    return -1;
}

short TR9350::GetSpectrum()
{
     char CMD[80];
     char str[80];
     char STR[80];

     if (GetMem() < 0) return -1;
     WriteStr(SETUP);
     strcpy(CMD, MEM[mem]);
     strcat(CMD, ":INSP? \'WAVE_ARRAY_COUNT\'");
     if (WriteStr(CMD) == 0) return -1;
     if (Read(STR,51) == 0) return -1;

     Info.Ntotal = (unsigned long)atol(CutString(str, STR));

     strcpy(CMD, MEM[mem]);
     strcat(CMD, ":INSP? \'FIRST_VALID_PNT\'");
     if (WriteStr(CMD) == 0) return -1;
     if (Read(STR,51) == 0) return -1;

     Info.Ni = (unsigned long)atol(CutString(str, STR));

     strcpy(CMD, MEM[mem]);
     strcat(CMD, ":INSP? \'LAST_VALID_PNT\'");
     if (WriteStr(CMD) == 0) return -1;
     if (Read(STR,51) == 0) return -1;

     Info.Nf = (unsigned long)atol(CutString(str, STR));

     strcpy(CMD, MEM[mem]);
     strcat(CMD,":INSP? \'VERTICAL_GAIN\'");
     if (WriteStr(CMD) == 0) return -1;
     if (Read(STR,51) == 0) return -1;

     Info.yGain = atof(CutString(str, STR))*1.0E+3;

     strcpy(CMD, MEM[mem]);
     strcat(CMD,":INSP? \'VERTICAL_OFFSET\'");
     if (WriteStr(CMD) == 0) return -1;
     if (Read(STR,51) == 0) return -1;

     Info.yOffset = atof(CutString(str, STR))*1.0E+3;

     strcpy(CMD, MEM[mem]);
     strcat(CMD,":INSP? \'HORIZ_INTERVAL\'");
     if (WriteStr(CMD) == 0) return -1;
     if (Read(STR,51) == 0) return -1;

     Info.xGain = atof(CutString(str, STR))*1.0E+6;

     strcpy(CMD, MEM[mem]);
     strcat(CMD,":INSP? \'HORIZ_OFFSET\'");
     if (WriteStr(CMD) == 0) return -1;
     if (Read(STR,51) == 0) return -1;

     Info.xOffset = atof(CutString(str, STR))*1.0E+6;

     strcpy(CMD, MEM[mem]);
     strcat(CMD,":INSP? \'SIMPLE\',WORD");
     if (WriteStr(CMD) == 0) return -1;
     if (Read(STR, 11) == 0) return -1;
     ReadF(TEMPFILE1);
     unsigned long N = GetLastCount()/2L;
     if (N < Ntotal) Ntotal = N;
     LocalMode();
     FILE *f;

     if ((f = fopen(TEMPFILE2, "wb")) == NULL) return -1;
     fwrite(&Info, sizeof(TR9350INFO), 1, f);
     fclose(f);
     return 1;
}

void TR9350::Run()
{
     while (GetSpectrum() >= 0);
}


void main()
{
    TR9350 *T = new TR9350(DEVICE);
    T->Run();
}