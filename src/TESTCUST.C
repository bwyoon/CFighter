#include "cftcust.h"
#include <stdio.h>
#include <conio.h>
#include <string.h>

int main()
{
     int c;
     SPECTRUMINFO SP;
     REAL Time;
     CHAR str[40];
     LONG i;
     HGLOBAL hg;
     SHORT HUGE *Y;


     if (!InitializeCustom(NULL)) {
	printf("Initialization Error");
	TerminateCustom();
	return 1;
     }  else {
	do {
	   printf("Menu :\n");
	   printf(" 0 ==> Get Spectrum Info\n");
	   printf(" 1 ==> Get Plot Info\n");
	   printf(" 2 ==> Get Experiment Info\n");
	   printf(" 3 ==> Get Spectrum data\n");
	   printf(" 4 ==> Get Cursor Position\n");
	   printf(" 5 ==> Set Cursor Position\n");
	   printf(" 6 ==> Put Cursor Text\n");
           printf(" q ==> Quit\n");
	   do {
	      c = getche();
	      printf("\n");
	   } while ((c >= '1') && (c <= '0'));
	   switch (c) {
	      case '0' :
		 if (GetSpectrumInfo(&SP)) 
		    printf("File Title  :  %s\n", SP.TS.Title);
		 else printf("Get Spectrum Info Error\n");
		 break;
	      case '3' :
		 if (GetSpectrumInfo(&SP)) {
		    printf("No of Point : %ld\n", SP.Ntot);
		    if ((Y = AllocMemoryForSpectrum(&hg, SP.Ntot*2L)) != NULL) {
		       if (GetSpectrum(Y, 100L*2L)) {
			  for (i = 0L; i < 100L; i++) {
			      printf("X : %.3f", SP.xGain*(REAL)i+SP.xOffset);
                              printf("\tY : %.3f\n", SP.yGain*(REAL)Y[i]+SP.yOffset);
			  }
                       }
		       FreeHGlobal(&hg);
                    }
		    
		 } else printf("Get Spectrum Info Error\n");
		 break;

	      case '4' :
		 if (GetCursorPosition(&Time))
		    printf("Cursor Position : %.3f µs\n", Time);
		 else printf("Get Cursor Position Error\n");
		 break;
	      case '5' :
		 printf("TOF (µs) : ");
		 scanf("%le", &Time);
		 SetCursorPosition(Time);
		 break;
	      case '6' :
		 printf("TOF (µs) : ");
		 scanf("%le", &Time);
		 printf("Cusor Text : ");
		 gets(str);
		 SetCursorText(Time, str);
		 break;
	   }
	} while (c != 'q');
	TerminateCustom();
    }
    return 0;
}
