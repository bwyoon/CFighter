#ifndef __GPIBW_H
#define __GPIBW_H

#include "windecl.h"
#include <string.h>

class GPIB
{
   protected :
      char  BRDNAME[20];
      char  DEVNAME[20];
      short DEVID;
      short BRDID;
      short DEVAD;
      int   ConnectOX;

   public    :
      GPIB(char *BrdName, char *DevName, short DevAd)
      {	   strcpy(BRDNAME, BrdName);
	   strcpy(DEVNAME, DevName);
           DEVAD = DevAd;
	   BRDID = ibfind(BRDNAME);
	   ConnectOX = ((BRDID & ERR) != 0) ? 0 : 1;
	   if (ConnectOX)
	      ConnectOX = ((ibsic(BRDID) & ERR) != 0) ? 0 : 1;
	   if (ConnectOX)
	      ConnectOX = ((ibdma (BRDID, 0) &ERR) != 0) ? 0 : 1;
	   if (ConnectOX) {
	      DEVID = ibfind(DEVNAME);
	      ConnectOX = ((DEVID & ERR) != 0) ? 0 : 1;
	   }
	   if (ConnectOX)
	      ConnectOX = ((ibbna(DEVID, BRDNAME) & ERR) != 0) ? 0 : 1;
	   if (ConnectOX)
              ConnectOX = ((ibpad(DEVID, DEVAD) & ERR) != 0) ? 0 : 1;
      };
      ~GPIB() {};
      char *GetDeviceName()    { return DEVNAME; };
      char *GetBoardName()     { return BRDNAME; };
      short GetDeviceAddress() { return   DEVAD; };
      int LocalMode()
      {   if (ConnectOX) {
	     ibloc(DEVID);
	     return 1;
	  } else return 0;
      };
      int  Write(char _huge *data, unsigned long len)
      {    if (ConnectOX) {
	      ibwrt(DEVID, data, len);
	      return ((ibsta & ERR) != 0) ? 0 : 1;
	   } else return 0;
      };
      int WriteStr(char _huge *data)
      {   return Write(data, strlen((const char*)data)); }
      int  Read(char _huge *data, unsigned long len)
      {    if (ConnectOX) {
	      ibrd(DEVID, data, len);
	      return ((ibsta & ERR) != 0) ? 0 : 1;
	   } else return 0;
      };
      int  ReadF(char *FileName)
      {    if (ConnectOX) {
	      ibrdf(DEVID, FileName);
	      return ((ibsta & ERR) != 0) ? 0 : 1;
	   } else return 0;
      };
      unsigned long GetLastCount() { return ibcntl; };
      int IsConnected() { return ConnectOX; };
};

#endif




