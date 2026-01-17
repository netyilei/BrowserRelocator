#ifndef PROGRESS_DIALOG_H
#define PROGRESS_DIALOG_H

#include "../common.h"
#include "../browser_types.h"

extern HWND g_hProgressDialog;
extern HWND g_hProgressBar;
extern HWND g_hStatusText;
extern HWND g_hDetailText;

HWND CreateProgressDialog(HWND hParent);
LRESULT CALLBACK ProgressDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void UpdateProgress(int current, int total, LPCWSTR status);
void SetProgressDetail(LPCWSTR detail);
void CloseProgressDialog();

#endif // PROGRESS_DIALOG_H