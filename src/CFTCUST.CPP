#include "cftcust.h"
#include <ddeml.h>
#include <stdio.h>
#include <string.h>


#define MAX_DDETOPIC   10
#define MAX_TRY     10000

DWORD   idInst;
HCONV   hConv;
HSZ     hszService;
HSZ     hszTopic;
HSZ     hszItem[MAX_DDETOPIC];
FARPROC lpfnCallBack;
BOOL    OX = FALSE;

HDDEDATA FAR PASCAL _export CallBack(WORD wType, WORD wFmt, HCONV hConv, HSZ hsz1, HSZ hsz2, HDDEDATA hData, DWORD, DWORD);

SHORT HUGE *AllocMemoryForSpectrum(HGLOBAL *hg, DWORD mSize)
{
     GlobalCompact(mSize);
     if ((*hg = GlobalAlloc(GPTR, mSize)) != NULL) {
	return (SHORT HUGE *)GlobalLock(*hg);
     } else return NULL;
}

void FreeHGlobal(HGLOBAL *hg)
{
     if (*hg != NULL) {
	GlobalUnlock(*hg);
	GlobalFree(*hg);
	*hg = NULL;
     }
}

BOOL InitializeCustom(HINSTANCE hInst)
{
     char Item[MAX_DDETOPIC][20] = {
	     "SP", "PL", "EX", "Y", "GETCP", "SETCP", "DRAWCT", "NULL", "NULL", "NULL"
	  };
     idInst = 0;
     hConv = 0;
     hszService = 0;
     hszTopic   = 0;
     for (int i = 0; i < MAX_DDETOPIC; i++) hszItem[i] = 0;

     lpfnCallBack = MakeProcInstance((FARPROC)CallBack, hInst);
     if (lpfnCallBack != NULL) {
	if (DdeInitialize( &idInst, (PFNCALLBACK)lpfnCallBack, APPCMD_CLIENTONLY, 0L) == DMLERR_NO_ERROR) {
           BOOL OX = TRUE;
	   hszService = DdeCreateStringHandle( idInst, "CFighterService", CP_WINANSI );
	   if (hszService == NULL) OX = FALSE;
	   hszTopic = DdeCreateStringHandle(idInst, "CFighterTopic", CP_WINANSI);
	   if (hszTopic == NULL) OX = FALSE;
           for (i = 0; i < MAX_DDETOPIC; i++) {
	       hszItem[i]  = DdeCreateStringHandle(idInst, Item[i],    CP_WINANSI);
	       if (hszItem[i] == NULL) OX = FALSE;
           }
	   if (OX) {
              return TRUE;
           } else return FALSE;
        } else return FALSE;
    } else return FALSE;
}

BOOL TerminateCustom()
{
     int i;

     if (hConv != 0) {
        DdeDisconnect(hConv);  
        hConv = 0;
     }
     if (idInst != 0) {
        if (hszService != NULL) {
           DdeFreeStringHandle(idInst, hszService);
           hszService = NULL;
	}
	if (hszTopic != NULL) {
	   DdeFreeStringHandle(idInst, hszTopic);
           hszTopic = NULL;
	}
	for (i = 0; i < MAX_DDETOPIC; i++ ) {
            if (hszItem[i] != NULL) {
               DdeFreeStringHandle(idInst, hszItem[i]);
	       hszItem[i] = NULL;
            }
        }
        DdeUninitialize(idInst);
        idInst = 0;
     }
     if (lpfnCallBack != NULL) {
        FreeProcInstance(lpfnCallBack);
        lpfnCallBack = NULL;
     }
     return TRUE;
}

int WhichTopicAndItem(HSZ hsz1, HSZ hsz2)
{
      int n1, n2;
      n1 = DdeCmpStringHandles(hszTopic, hsz1);
      for (int i = 0; i < MAX_DDETOPIC; i++) {
	  n2 = DdeCmpStringHandles(hszItem[i], hsz2);
	  if ((n1 == 0) && (n2 == 0)) return i;
      }
      return -1;
}

BOOL ConnectWithCFighter()
{
     hConv = DdeConnect(idInst, hszService, hszTopic, NULL);
     if (hConv != 0) return TRUE;
     else return FALSE;
}

BOOL GetSpectrumInfo(SPECTRUMINFO FAR *SP)
{
     HDDEDATA hData;

     if (hConv == 0) return FALSE;
     hData = DdeClientTransaction(NULL, 0, hConv, hszItem[0], CF_TEXT, XTYP_REQUEST, 0, NULL);
     if (hData != FALSE) {
        DdeGetData( hData, SP, sizeof(SPECTRUMINFO), 0);
	return TRUE;
     } else {
        return FALSE;
     } 
}

BOOL GetSpectrum(SHORT HUGE *Y, DWORD mSize)
{
     HDDEDATA hData;
     BOOL OX = FALSE;
     SHORT    N = 0;

     do {
        if (hConv == 0) OX =  FALSE;
        hData = DdeClientTransaction(NULL, 0, hConv, hszItem[3], CF_TEXT, XTYP_REQUEST, 0, NULL);
        if (hData != FALSE) {
	   DdeGetData( hData, Y, mSize, 0);
	   OX = TRUE;
        } else {
	   OX = FALSE;
	}
	N++;
     } while (!OX && (N < MAX_TRY));
     return TRUE;
}

BOOL GetCursorPosition(REAL *R)
{
     HDDEDATA hData;

     if (hConv == 0) return FALSE;
     hData = DdeClientTransaction(NULL, 0, hConv, hszItem[4], CF_TEXT, XTYP_REQUEST, 1000, NULL);
     if (hData != FALSE) {
	DdeGetData( hData, R, sizeof(REAL), 0);
	return TRUE;
     } else {
	return FALSE;
     } 
}

BOOL SetCursorPosition(REAL R)
{
     if (hConv == 0) return FALSE;
     else {
	REAL R1 = R;
	DdeClientTransaction(&R1, sizeof(REAL), hConv, hszItem[5], CF_TEXT, XTYP_POKE, 0, NULL);
	return TRUE;
     }
}

BOOL SetCursorText(REAL Time, CHAR *str)
{
     if (hConv == 0) return FALSE;
     else {
	CTEXTSTRUCT cs;
	cs.TOF = Time;
	strncpy(cs.CText, str, LEN_CURSORTEXT-1);
	cs.CText[LEN_CURSORTEXT-1] = '\0';
	DdeClientTransaction(&cs, sizeof(CTEXTSTRUCT), hConv, hszItem[6], CF_TEXT, XTYP_POKE, 0, NULL);
	return TRUE;
     }
}

#pragma argsused
HDDEDATA FAR PASCAL _export CallBack(WORD wType, WORD, HCONV hCon, HSZ hsz1, HSZ hsz2, HDDEDATA hData, DWORD, DWORD )
{
    switch( wType )
    {
        case XTYP_ADVDATA :
            return (HDDEDATA)DDE_FACK;
        case XTYP_XACT_COMPLETE :
            break;
        case XTYP_DISCONNECT :
            hConv = 0;
            break;
        case XTYP_ERROR :
            break;
        default :
            break;
    }
    return NULL;
}

