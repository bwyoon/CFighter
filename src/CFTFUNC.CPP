#include "cftfunc.h"

/* Allocate specified number of bytes from the global heap  */
HGLOBAL AllocHGlobal(UINT How, DWORD msize)
{
     GlobalCompact(msize);
     return GlobalAlloc(How, msize);
}

/* Free the given global memory object */
void FreeHGlobal(HGLOBAL &hg)
{
     if (hg != NULL) {
	GlobalUnlock(hg);
	GlobalFree(hg);
        hg = NULL;
     }
}

int CFTMessage(PTWindowsObject PWO, UINT TextId, UINT TitleId, UINT Flags)
{
     char EText[128];
     char ETitle[80];
     LoadString(PWO->GetApplication()->hInstance, TextId, EText, 127);
     LoadString(PWO->GetApplication()->hInstance, TitleId, ETitle, 79);
     HWND hWnd = (PWO->HWindow == 0) ? NULL : PWO->HWindow;
     return MessageBox(hWnd, EText, ETitle, Flags);
}