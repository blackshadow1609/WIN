#include<Windows.h>

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


	return 0;
} 


