#include "cft.h"

PCFTFrame THIS = NULL;

BOOL CFTFrame::SetupDDE()
{
     CHAR Item[MAX_DDETOPIC][20] = {
	     "SP", "PL", "EX", "Y", "GETCP", "SETCP", "DRAWCT", "NULL", "NULL", "NULL"
	  };
     THIS = this;
     idInst = 0;
     hConv = 0;
     hszService = 0;
     hszTopic   = 0;
     for (SHORT i = 0; i < MAX_DDETOPIC; i++) hszItem[i] = 0;

     lpfnCallBack = MakeProcInstance((FARPROC)CFTFrame::CallBack, GetApplication()->hInstance);
     if (lpfnCallBack != NULL) {
	if (DdeInitialize( &idInst, (PFNCALLBACK)lpfnCallBack, CBF_FAIL_EXECUTES, 0L) == DMLERR_NO_ERROR) {
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
	      if (DdeNameService(idInst, hszService, NULL, DNS_REGISTER) != 0)
	         return TRUE;
	      else return FALSE;	   
           } else return FALSE;
        } else return FALSE;
    } else return FALSE;
}

void CFTFrame::QuitDDE()
{
     SHORT i;

     if (hConv != 0) {
        DdeDisconnect(hConv);  
        hConv = 0;
     }
     if (idInst != 0) {
        DdeNameService(idInst, hszService, NULL, DNS_UNREGISTER);
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
}

SHORT CFTFrame::WhichTopicAndService(HSZ hsz1, HSZ hsz2)
{
      if (DdeCmpStringHandles(hszService, hsz2) != 0) return -1;
      else {
	 if (DdeCmpStringHandles(hszTopic, hsz1) == 0) return 1;
         else return -1;
      } 
}

SHORT CFTFrame::WhichTopicAndItem(HSZ hsz1, HSZ hsz2)
{
      SHORT n1, n2;

      n1 = DdeCmpStringHandles(hszTopic, hsz1);
      for (SHORT i = 0; i < MAX_DDETOPIC; i++) {
	  n2 = DdeCmpStringHandles(hszItem[i], hsz2);
	  if ((n1 == 0) && (n2 == 0)) return i;
      }
      return -1;
}

HDDEDATA CFTFrame::WildConnect( HSZ hsz1, HSZ hsz2, WORD wFmt )
{
     HSZPAIR hszpTemp[2];


     hszpTemp[0].hszSvc   = hszService;
     hszpTemp[0].hszTopic = hszTopic;

     if ((hsz1 == NULL) && (hsz2 == NULL)){
        return DdeCreateDataHandle(idInst, (LPBYTE)&hszpTemp[0], sizeof(hszpTemp), 0L, 0, wFmt, 0);
     }
     if ((hsz1 == NULL) && (DdeCmpStringHandles(hsz2, hszService) == 0)) {
        return DdeCreateDataHandle(idInst, (LPBYTE)&hszpTemp[0], sizeof(hszpTemp), 0L, 0, wFmt, 0);
     }
     BOOL OX = FALSE;
     if (DdeCmpStringHandles(hsz1, hszTopic) == 0) OX = TRUE;
     if (OX && (hsz2 == NULL)) {
        return DdeCreateDataHandle(idInst, (LPBYTE)&hszpTemp[0], sizeof(hszpTemp), 0L, 0, wFmt, 0);
     }
     return NULL;
}

HDDEDATA CFTFrame::DDEAnswerForRequest(WORD wFmt, SHORT topic)
{
     PSpectrumWindow PChild = (PSpectrumWindow)ActiveChild;
     SHORT SPEC = PChild->SPEC;

     if (PChild != NULL) {
	switch (topic) {
	   case 0 :
	      return DdeCreateDataHandle(idInst, PChild->SP[SPEC], (DWORD)sizeof(SPECTRUMINFO),
					 0L, hszItem[0], wFmt, 0);
	   case 1 :
	      return DdeCreateDataHandle(idInst, PChild->PL[SPEC], (DWORD)sizeof(PLOTINFO),
					 0L, hszItem[1], wFmt, 0);
	   case 2 :
	      return DdeCreateDataHandle(idInst, PChild->EX[SPEC], (DWORD)sizeof(EXPERIMENTINFO),
					 0L, hszItem[2], wFmt, 0);
	   case 3 :
	      return DdeCreateDataHandle(idInst, PChild->Y[SPEC], PChild->SP[SPEC]->Ntot*2L,
					    0L, hszItem[3], wFmt, 0);
	   case 4 :
	      SHORT SPEC = PChild->SPEC;
	      REAL CPos = (REAL)PChild->CursorPos[SPEC][0];
              REAL T    = PChild->SP[SPEC]->xGain*CPos+PChild->SP[SPEC]->xOffset;
	      return DdeCreateDataHandle(idInst, &T, (DWORD)sizeof(REAL),
					 0L, hszItem[4], wFmt, 0);
	}
        return NULL;
     } return NULL;
}

void CFTFrame::DDEPoke(HDDEDATA hData, SHORT topic)
{
     PSpectrumWindow PChild = (PSpectrumWindow)ActiveChild;

     if (PChild != NULL) {
	switch (topic) {
	    case 5 :
               REAL N;
	       DdeGetData(hData, &N, sizeof(REAL), 0);
	       PChild->SetCursorPosition(PChild->SPEC, N);
	       break;
	    case 6 :
	       CTEXTSTRUCT ct;
	       DdeGetData(hData, &ct, sizeof(CTEXTSTRUCT), 0);
               PChild->InsertCursorText(PChild->SPEC, ct.TOF, ct.CText);
	       break;
        }
     }

}

HDDEDATA FAR PASCAL CFTFrame::CallBack(WORD wType, WORD wFmt, HCONV hConv, HSZ hsz1, HSZ hsz2, HDDEDATA hData, DWORD, DWORD)
{
     SHORT topic;

     switch( wType ) {
        case XTYP_ADVREQ :
            if (THIS->WhichTopicAndItem(hsz1, hsz2) >= 0) {
                return NULL; //pStaticThis->DataRequested( wFmt );
            }
            return NULL;
        case XTYP_ADVSTART :
	    if (THIS->WhichTopicAndItem(hsz1, hsz2) >= 0) {
                return NULL; //pStaticThis->tfLoop = TRUE;
                // return (HDDEDATA)1;
            } else {
                return 0;
            }
        case XTYP_ADVSTOP :
            if (THIS->WhichTopicAndItem(hsz1, hsz2) >= 0) {
	       ;
            }
            break;
	case XTYP_CONNECT :
	    if (THIS->hConv == 0) {
		if (THIS->WhichTopicAndService(hsz1, hsz2) >= 0) {
                    return (HDDEDATA)1;
                }
	    }
            return 0;
        case XTYP_CONNECT_CONFIRM :
	    THIS->hConv = hConv;
            break;
        case XTYP_DISCONNECT :
            if (hConv == THIS->hConv) {
                THIS->hConv = 0;
            }
            break;
        case XTYP_ERROR :
            MessageBox(THIS->HWindow, "A critical DDE error has occured.", THIS->Title, MB_ICONINFORMATION );
            break;
	case XTYP_EXECUTE :
            return DDE_FNOTPROCESSED;
	case XTYP_POKE :
	    if ((topic = THIS->WhichTopicAndItem(hsz1, hsz2)) >= 0) {
	       THIS->DDEPoke(hData, topic); //pStaticThis->DataRequested( wFmt );
	    }
	    return (HDDEDATA)DDE_FACK;
	case XTYP_REQUEST :
            if ((topic = THIS->WhichTopicAndItem(hsz1, hsz2)) >= 0) {
		return THIS->DDEAnswerForRequest(wFmt, topic); //pStaticThis->DataRequested( wFmt );
	    }
            return NULL;
        case XTYP_WILDCONNECT :
	    return THIS->WildConnect(hsz1, hsz2, wFmt);
        default :
            break;
    }
    return NULL;
}
