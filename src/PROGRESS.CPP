#include <owl.h>
#include <dialog.h>
#include <edit.h>
#include <scrollba.h>
#include <checkbox.h>
#include <radiobut.h>
#include <dir.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#pragma hdrstop
#include "profftid.h" // Symbolske konstanter
#include "profft.h"   // Klassedefinisjoner

class TProgressDialog : public TDialog
{
public:
	TProgressDialog(PTWindowsObject AParent, LPSTR lpszDialogName, LPSTR lpszCaption);
	~TProgressDialog();

	virtual void WMInitDialog(RTMessage Msg) = [WM_FIRST + WM_INITDIALOG];
	virtual void WMCommand(RTMessage Msg) = [WM_FIRST + WM_COMMAND];

	virtual void WMSetText(RTMessage Msg) = [WM_FIRST + WM_SETPROGRESSTEXT];
	virtual void WMSetProgress(RTMessage Msg) = [WM_FIRST + WM_SETPROGRESS];

	BOOL CancelPressed();
private:
	BOOL bCancel;

	char acPercentage[5];  	
	char acMessage[MAX_CAPTION_LENGTH];

	long lSoFar;

	HBRUSH hBrush;

	HDC hdc;
	RECT rect;

	HWND hCtrl;
};

TProgressDialog::TProgressDialog(PTWindowsObject AParent, LPSTR lpszDialogName,
		LPSTR lpszCaption) : TDialog(AParent, lpszDialogName)
{
	SetCaption(lpszCaption);

	lSoFar = 0;
	bCancel = FALSE;
	strcpy(acMessage, "");

	new TStatic(this, DB_TEXT, DB_TEXTLENGTH, NULL);
	new TStatic(this, DB_PERCENTAGE, 5, NULL);
	new TStatic(this, DB_PAINTAREA, 0, NULL);
}

TProgressDialog::~TProgressDialog()
{
	DeleteObject(hBrush);
	ReleaseDC(HWindow, hdc);
}

void TProgressDialog::WMInitDialog(RTMessage Msg)
{
	TDialog::WMInitDialog(Msg);

	hCtrl = GetDlgItem(HWindow, DB_PAINTAREA);
	hdc = GetDC(hCtrl);
	GetClientRect(hCtrl, &rect);

	hBrush = CreateSolidBrush(GetSysColor(COLOR_ACTIVECAPTION));
	hBrush = SelectObject(hdc, hBrush);
	SetROP2(hdc, R2_COPYPEN);
}

void TProgressDialog::WMSetText(RTMessage Msg)
{
	SetDlgItemText(HWindow, DB_TEXT, (LPSTR) Msg.LParam);
}

void TProgressDialog::WMSetProgress(RTMessage Msg)
{
	if (Msg.LParam!=lSoFar)
	{
		lSoFar = Msg.LParam;

		wsprintf(acPercentage, "%ld%%", lSoFar);
		Rectangle(hdc, rect.left,  rect.top,
				(long) (rect.left + ((rect.right-rect.left)*lSoFar)/100), rect.bottom);
		SetDlgItemText(HWindow, DB_PERCENTAGE, acPercentage);
		UpdateWindow(hCtrl);
	}
}

void TProgressDialog::WMCommand(RTMessage)
{
	bCancel = TRUE;
}

BOOL TProgressDialog::CancelPressed()
{
	MSG msg;

	while (!bCancel && PeekMessage(&msg, HWindow, 0, 0, PM_REMOVE))
	{
		if (IsDialogMessage(HWindow, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return bCancel;
}

