#include<Windows.h>
#include"resource.h"

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); //прототип функции процедуры окна
//#define MESSAGE_BOX

// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messagebox
//Документация для изучения

INT WINAPI WinMain(HINSTANCE hInstasce, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow) //обязательная основа
{

#ifdef MESSAGE_BOX
	MessageBox
	(
		NULL,
		"Привет WinAPI\n Это самое простое окно - окно сообщения\t\t\t\t(MessageBox)",  //текст сообщения в окне
		"Привет!",																		//навние в заголовке окна
		MB_ABORTRETRYIGNORE | MB_ICONINFORMATION | MB_HELP								//Кнопки
		| MB_DEFBUTTON3																	//установка кнопки по умолчанию
		| MB_TOPMOST																	//Расположение окна (модальность)
	);

	/* MB_ - MessageBox
	 Венгерская нотация (Hungarian notation) — соглашение об
	 именовании переменных, констант и прочих идентификаторов
	 в коде программ. Суть — в использовании префиксов, которые
	 обозначают тип данных или назначение идентификатора. */
#endif // MESSAGE_BOX

	DialogBoxParam(hInstasce, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc, 0);
									 //если есть ошибка прописать (DLGPROC)DlgProc.

	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)				//реализация
{
	switch (uMsg)
	{
	case WM_INITDIALOG:						//выполняется один раз - при запуске окна
		break;

	case WM_COMMAND:						//обрабатывает нажатие кнопок, перемещения мыши и т.д.
		switch (LOWORD(wParam))
		{
		case IDOK:
			MessageBox(hwnd, "Была нажата кнопка 'ОК'", "Info", MB_OK | MB_ICONINFORMATION);
			break;
		case IDCANCEL:
			EndDialog(hwnd, 0);
			break;
		}
		break;

	case WM_CLOSE:							//отрабатывает при нажатии на кнопку "Закрыть Х"
		EndDialog(hwnd, 0);					
		break;
	}
	return FALSE;
}

