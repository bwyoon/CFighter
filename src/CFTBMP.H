#ifndef __CFTBMP_H
#define __CFTBMP_H

#include <windows.h>

class Bitmap
{
    private :
      HBITMAP hBitmap;
      int GetBitmap(BITMAP FAR *lpbm)
      {  return GetObject(hBitmap, sizeof(BITMAP), (LPSTR)lpbm); };
    public  :
      Bitmap(HINSTANCE hInstance, char FAR *lpszBitmapName)
      {  hBitmap = LoadBitmap(hInstance, lpszBitmapName); };
      ~Bitmap()
      {  DeleteObject(hBitmap); };
      void Display(HDC hDC, short xStart, short yStart );
      POINT GetSize(HDC hDC);
};

class CompatibleDC
{
    private :
        HDC hDCMem;
    public  :
        CompatibleDC(HDC hDC)
        {  hDCMem = CreateCompatibleDC(hDC);
           SetMapMode(hDCMem, GetMapMode(hDC)); };
        ~CompatibleDC() { DeleteDC( hDCMem ); };
        HDC GetHDC() { return hDCMem; }
};

#endif
