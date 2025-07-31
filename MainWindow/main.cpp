#include<Windows.h>

CONST CHAR g_sz_CLASS_NAME[] = "My First Window";	//Абсолютно у любого класса окна есть имя.
													//Имя класса окна - это самая обычная строка.

INT WINAPI WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstasce, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	// 1) Регистрация класса окна:

	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(wClass));

	wClass.style = 0;
	wClass.cbSize = sizeof(wClass);						//cb - Count Bytes
	wClass.cbWndExtra = 0;
	wClass.cbClsExtra = 0;

	wClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);	//Sm - Small
	wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wClass.hbrBackground = (HBRUSH)COLOR_WINDOW;

	wClass.hInstance = hInstasce;
	wClass.lpfnWndProc = (WNDPROC)WndProc;
	wClass.lpszMenuName = NULL;
	wClass.lpszClassName = g_sz_CLASS_NAME;

	if (RegisterClassEx(&wClass) == NULL)
	{
		MessageBox(NULL, "Class registration failed", "", MB_OK | MB_ICONERROR);
		return 0;
	}

	// 2) Создание окна:
	
	HWND hwnd = CreateWindowEx
	(
		NULL,							//ExStyle
		g_sz_CLASS_NAME,				//ClassName
		g_sz_CLASS_NAME,				//WindowName (Title) - эта строка отображается в заголовке окна
		WS_OVERLAPPEDWINDOW,			//Такой стиль задается для всех главных окон. 
										//Это окно будет родительским для других окон приложения.
		CW_USEDEFAULT, CW_USEDEFAULT,	//Position. Позиция окна
		CW_USEDEFAULT, CW_USEDEFAULT,	//Size. Размер окна
		NULL,							//ParentWindow  
		NULL,							//Строка меню для главного окна, или же ID-ресурса для дочернего окна
		hInstasce,						//Это экземпляр *.ехе - файла нашей программы 
		NULL
	);
	if (hwnd == NULL)
	{
		MessageBox(NULL, "Window creation failed", "", MB_OK | MB_ICONERROR);
		return 0;
	}
	ShowWindow(hwnd, nCmdShow);			//Задает режим отображения окна: развернуто/свернуто/свернуто на панель задач
	UpdateWindow(hwnd);					//Прорисовка рабочей обрасти окна
	
	// 3) Запуск цикла сообщений:
	 
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
							
	return msg.message;
}

INT WINAPI WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		break;
	case WM_COMMAND:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return FALSE;
}
