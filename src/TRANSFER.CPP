#include <windows.h>
#include "gpib.h"
#include <stdio.h>
#include <string.h>

#define DEVICE "LECROY1"
#define SETUP  "WFSU SP, 0, NP, 0, FP, 0, SN, 0; CHDR SHORT"
#define TEMPFILE "c:\\cfttemp.bin"
#define GETMEMTITLE "Type LeCroy memory >> "

char  MEM[12][3] = {
	 "M1", "M2", "M3", "M4", "TA", "TB", "TC", "TD", "C1", "C2", "C3", "C4"
      };

short GetMem(const char *str)
{
    char buf[10];
    printf(str);
    gets(buf);
    strupr(buf);
    for (int i = 0; i < 12; i++)
	if (strcmp(buf, MEM[i]) == 0) return i;
    return -1;
}

short GetSpectrum(short mem, GPIB *T)
{
    char CMD[80];
    char str[80];

    T->WriteStr(SETUP);
    strcpy(CMD, MEM[mem]);
    strcat(CMD, ":INSP? \'WAVE_ARRAY_COUNT\'");
    T->Write(CMD, strlen(CMD));
    if (T->Read(str, 51) == 0) {
       printf("Error count = %lu\n", T->GetCount());

    }
    printf("response : %s\n", str);
    strcpy(CMD, MEM[mem]);
    strcat(CMD, ":INSP? \'VERTICAL_GAIN\'");
    T->Write(CMD, strlen(CMD));
    if (T->Read(str, 51) == 0) {
       printf("Error count = %lu\n", T->GetCount());

    }
    printf("response : %s\n", str);
    T->LocalMode();
    return 1;
}

int main()
{
    GPIB *T = new GPIB(DEVICE);
    short N;

    if (T->IsConnected()) {
       do {
          N = GetMem(GETMEMTITLE);
	  if (N >= 0) GetSpectrum(N, T);
       } while (N >= 0);
    } else {
	printf("GPIB not connected\n");
	return 1;
    }
    return 0;
}