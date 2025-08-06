#include<Windows.h>
#include"resource.h"

CONST CHAR g_sz_CLASS_NAME[] = "MyCalc";

INT WINAPI WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	//1)Регистрация класса окна:

	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(wClass));

	wClass.style = 0;
	wClass.cbSize = sizeof(wClass);
	wClass.cbWndExtra = 0;
	wClass.cbClsExtra = 0;

	wClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	wClass.hInstance = hInstance;
	wClass.lpszMenuName = NULL;
	wClass.lpszClassName = g_sz_CLASS_NAME;
	wClass.lpfnWndProc = (WNDPROC)WndProc;

	if (!RegisterClassEx(&wClass))
	{
		MessageBox(NULL, "Class registration failed", "", MB_OK | MB_ICONERROR);
		return 0;
	}

	//2)Создание окна:

	HWND hwnd = CreateWindowEx
	(
		NULL,
		g_sz_CLASS_NAME,
		g_sz_CLASS_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);						
	
	//3)Запуск цикла сообщений

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

INT WINAPI WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        HWND hEditDisplay = CreateWindowEx(
            NULL, "Edit", "0",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
            10, 10,
            230, 30,
            hwnd,
            (HMENU)IDC_EDIT_DISPLAY,
            GetModuleHandle(NULL),
            NULL
        );

        CreateWindow("BUTTON", "7", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            10, 50, 50, 30, hwnd, (HMENU)IDC_BUTTON_7, NULL, NULL);
        CreateWindow("BUTTON", "8", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            70, 50, 50, 30, hwnd, (HMENU)IDC_BUTTON_8, NULL, NULL);
        CreateWindow("BUTTON", "9", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            130, 50, 50, 30, hwnd, (HMENU)IDC_BUTTON_9, NULL, NULL);
        CreateWindow("BUTTON", "+", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            190, 50, 50, 30, hwnd, (HMENU)IDC_BUTTON_PLUS, NULL, NULL);

        CreateWindow("BUTTON", "4", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            10, 90, 50, 30, hwnd, (HMENU)IDC_BUTTON_4, NULL, NULL);
        CreateWindow("BUTTON", "5", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            70, 90, 50, 30, hwnd, (HMENU)IDC_BUTTON_5, NULL, NULL);
        CreateWindow("BUTTON", "6", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            130, 90, 50, 30, hwnd, (HMENU)IDC_BUTTON_6, NULL, NULL);
        CreateWindow("BUTTON", "-", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            190, 90, 50, 30, hwnd, (HMENU)IDC_BUTTON_MINUS, NULL, NULL);

        CreateWindow("BUTTON", "1", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            10, 130, 50, 30, hwnd, (HMENU)IDC_BUTTON_1, NULL, NULL);
        CreateWindow("BUTTON", "2", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            70, 130, 50, 30, hwnd, (HMENU)IDC_BUTTON_2, NULL, NULL);
        CreateWindow("BUTTON", "3", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            130, 130, 50, 30, hwnd, (HMENU)IDC_BUTTON_3, NULL, NULL);
        CreateWindow("BUTTON", "*", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            190, 130, 50, 30, hwnd, (HMENU)IDC_BUTTON_ASTER, NULL, NULL);

        CreateWindow("BUTTON", "0", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            10, 170, 50, 30, hwnd, (HMENU)IDC_BUTTON_0, NULL, NULL);
        CreateWindow("BUTTON", ".", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            70, 170, 50, 30, hwnd, (HMENU)IDC_BUTTON_POINT, NULL, NULL);
        CreateWindow("BUTTON", "=", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            130, 170, 50, 30, hwnd, (HMENU)IDC_BUTTON_EQUAL, NULL, NULL);
        CreateWindow("BUTTON", "/", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            190, 170, 50, 30, hwnd, (HMENU)IDC_BUTTON_SLASH, NULL, NULL);

        CreateWindow("BUTTON", "BSP", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            10, 210, 110, 30, hwnd, (HMENU)IDC_BUTTON_BSP, NULL, NULL);
        CreateWindow("BUTTON", "CLR", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            130, 210, 110, 30, hwnd, (HMENU)IDC_BUTTON_CLR, NULL, NULL);
    }
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