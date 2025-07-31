#include <Windows.h>
#include <cstdio>
#include "resource.h"

CONST CHAR* G_SZ_VALUES[] = { "This", "is", "my", "First", "List", "Box", "Хорошо", "живет", "на свете", "Винни", "Пух", "Как то так", };

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK InputDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
    DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, (DLGPROC)DlgProc, 0);
    return 0;
}

static CHAR g_szInputBuffer[256] = {};
static INT g_iSelectedIndex = -1;

BOOL CALLBACK InputDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        if (g_iSelectedIndex != -1)
        {
            HWND hEdit = GetDlgItem(hwnd, IDC_EDIT);
            SetWindowText(hEdit, g_szInputBuffer);
        }
        return TRUE;
    }

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            HWND hEdit = GetDlgItem(hwnd, IDC_EDIT);
            GetWindowText(hEdit, g_szInputBuffer, sizeof(g_szInputBuffer));
            EndDialog(hwnd, 1);
            return TRUE;
        }
        case IDCANCEL:
            EndDialog(hwnd, 0);
            return TRUE;
        }
        break;
    }
    }
    return FALSE;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));
        SendMessage(hwnd, WM_SETICON, 0, (LPARAM)hIcon);

        HWND hList = GetDlgItem(hwnd, IDC_LIST);
        for (int i = 0; i < sizeof(G_SZ_VALUES) / sizeof(G_SZ_VALUES[0]); i++)
        {
            SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)G_SZ_VALUES[i]);
        }
        return TRUE;
    }

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            CONST INT SIZE = 256;
            CHAR sz_buffer[SIZE] = {};
            CHAR sz_message[SIZE] = {};
            HWND hList = GetDlgItem(hwnd, IDC_LIST);
            INT i = SendMessage(hList, LB_GETCURSEL, 0, 0);
            SendMessage(hList, LB_GETTEXT, i, (LPARAM)sz_buffer);
            if (i == -1) strcpy(sz_message, "Выберите Ваш вариант: ");
            else
                sprintf(sz_message, "Вы выбрали пункт № %i со значением '%s'", i, sz_buffer);

            MessageBox(hwnd, sz_message, "Info", MB_OK | MB_ICONINFORMATION);
            return TRUE;
        }

        case IDCANCEL:
            EndDialog(hwnd, 0);
            return TRUE;

        case IDC_ADD: 
        {
            g_iSelectedIndex = -1;
            g_szInputBuffer[0] = '\0';
            if (DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_INPUT),
                hwnd, (DLGPROC)InputDlgProc, 0))
            {
                HWND hList = GetDlgItem(hwnd, IDC_LIST);
                SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)g_szInputBuffer);
            }
            return TRUE;
        }

        case IDC_DELETE: 
        {
            HWND hList = GetDlgItem(hwnd, IDC_LIST);
            INT i = SendMessage(hList, LB_GETCURSEL, 0, 0);
            if (i != -1)
            {
                SendMessage(hList, LB_DELETESTRING, i, 0);
            }
            return TRUE;
        }

        case IDC_LIST: 
        {
            if (HIWORD(wParam) == LBN_DBLCLK) 
            {
                HWND hList = GetDlgItem(hwnd, IDC_LIST);
                g_iSelectedIndex = SendMessage(hList, LB_GETCURSEL, 0, 0);
                if (g_iSelectedIndex != -1)
                {
                    SendMessage(hList, LB_GETTEXT, g_iSelectedIndex, (LPARAM)g_szInputBuffer);
                    if (DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_INPUT),
                        hwnd, (DLGPROC)InputDlgProc, 0))
                    {
                        SendMessage(hList, LB_DELETESTRING, g_iSelectedIndex, 0);
                        SendMessage(hList, LB_INSERTSTRING, g_iSelectedIndex, (LPARAM)g_szInputBuffer);
                        SendMessage(hList, LB_SETCURSEL, g_iSelectedIndex, 0);
                    }
                }
            }
            return TRUE;
        }
        }
        break;
    }

    case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
    }
    return FALSE;
}