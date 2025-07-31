#include<Windows.h>
#include<cstdio>
#include"resource.h"



CONST CHAR* G_SZ_VALUES[] = { "This", "is", "my", "First", "Combo", "Box", "Хорошо", "живет", "на свете", "Винни", "Пух", "Как то так", };

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, (DLGPROC)DlgProc, 0);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
		//GetModuleHandle(NULL) - возвращает hInstance нашего *.ехе-файла
		SendMessage(hwnd, WM_SETICON, 0, (LPARAM)hIcon);

		HWND hCombo = GetDlgItem(hwnd, IDC_COMBO);
		for (int i = 0; i < sizeof(G_SZ_VALUES) / sizeof(G_SZ_VALUES[0]); i++)
		{
			SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)G_SZ_VALUES[i]);
		}
	}
	break;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			//A - ANSI ASCII;
			//W - Wide CHAR;

			CONST INT SIZE = 256;
			CHAR sz_buffer[SIZE] = {};
			CHAR sz_message[SIZE] = {};
			HWND hCombo = GetDlgItem(hwnd, IDC_COMBO);
			INT i = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
			SendMessage(hCombo, CB_GETLBTEXT, i, (LPARAM)sz_buffer);
			if (i == -1)strcpy(sz_message, "Выберите Ваш вариант: ");
			//strcat(dst, src); dst - строка получатель; src - строка источник;
			else
				sprintf(sz_message, "Вы выбрали пункт № %i со значением '%s'", i, sz_buffer);
			//sz_message - эта строка будет содержать результат форматирования;
			//"Вы выбрали...." - эта строка содержит формат, в ней указывается 
			//куда именно нужно вставить значения, идущие после этой строки, 
			//в данном случае это "i" и "sz_buffer";
			//sprintf() - это функция с произвольным числом параметров;
			//Спецификатор '%i' - заменяется целочисленным значением;
			//Спецификатор '%s' - заменяется строкой;
			//Спецификатор '%e' - дробное число;
			
			MessageBox(hwnd, sz_message, "Info", MB_OK | MB_ICONINFORMATION);
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, 0);
			break;
		}

	}
	break;

	case WM_CLOSE:
		EndDialog(hwnd, 0);
		break;
	}
	return FALSE;
}