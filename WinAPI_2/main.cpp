#include<Windows.h>
#include"resource.h"

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


INT WINAPI WinMain(HINSTANCE hInstasce, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{



	DialogBoxParam(hInstasce, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc, 0);
	//если есть ошибка 'DlgProc', то прописать (DLGPROC)DlgProc.

	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{																						//
		HWND hEditLogin = GetDlgItem(hwnd, IDC_EDIT_LOGIN);									//
		SendMessage(hEditLogin, WM_SETTEXT, 0, (LPARAM)"Введите имя пользователя");			//
		break;																				//
	}																						//
	break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_EDIT_LOGIN:																
			if (HIWORD(wParam) == EN_SETFOCUS)												
			{																				
				HWND hEditLogin = GetDlgItem(hwnd, IDC_EDIT_LOGIN);							
				CHAR sz_buffer[256];														
				SendMessage(hEditLogin, WM_GETTEXT, 256, (LPARAM)sz_buffer);				
				if (strcmp(sz_buffer, "Введите имя пользователя") == 0)						
				{																			
					SendMessage(hEditLogin, WM_SETTEXT, 0, (LPARAM)"");						
				}																			
			}																				
			else if (HIWORD(wParam) == EN_KILLFOCUS)
			{
				HWND hEditLogin = GetDlgItem(hwnd, IDC_EDIT_LOGIN);
				CHAR sz_buffer[256];
				SendMessage(hEditLogin, WM_GETTEXT, 256, (LPARAM)sz_buffer);
				if (strlen(sz_buffer) == 0)
				{
					SendMessage(hEditLogin, WM_SETTEXT, 0, (LPARAM)"Введите имя пользователя");
				}
			}
			break;																				

		case IDC_BUTTON_COPY:
		{
			HWND hEditLogin = GetDlgItem(hwnd, IDC_EDIT_LOGIN);
			HWND hEditPassword = GetDlgItem(hwnd, IDC_EDIT_PASSWORD);
			CONST INT SIZE = 256;
			CHAR sz_buffer[SIZE] = {};
			SendMessage(hEditLogin, WM_GETTEXT, SIZE, (LPARAM)sz_buffer);
			SendMessage(hEditPassword, WM_SETTEXT, 0, (LPARAM)sz_buffer);
		}
		break;

		case IDOK:
			MessageBox(hwnd, "Была нажата кнопка 'ОК'", "Info", MB_OK | MB_ICONINFORMATION);
			break;

		case IDCANCEL:
			EndDialog(hwnd, 0);
			break;
		}
		break;

	case WM_CLOSE:
		EndDialog(hwnd, 0);
		break;
	}
	return FALSE;
}

