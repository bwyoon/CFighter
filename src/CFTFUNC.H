#ifndef __CFTFUNC_H
#define __CFTFUNC_H

#include <owl.h>

extern "C"
{
     HGLOBAL AllocHGlobal(UINT, DWORD);
     void FreeHGlobal(HGLOBAL &);
     int  CFTMessage(PTWindowsObject, UINT, UINT, UINT);
};

#endif