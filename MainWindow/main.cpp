﻿#include<Windows.h>
#include<stdio.h>
#include"resource.h"

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

	wClass.hIcon = LoadIcon(hInstasce, MAKEINTRESOURCE(IDI_ICON_BITCOIN));
	wClass.hIconSm = LoadIcon(hInstasce, MAKEINTRESOURCE(IDI_ICON_ATOM));	//Sm - Small
	//wClass.hIcon = (HICON)LoadImage(hInstasce, "atom.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
	//wClass.hIconSm = (HICON)LoadImage(hInstasce, "bitcoin.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
	//wClass.hCursor = LoadCursor(hInstasce, MAKEINTRESOURCE(IDC_CURSOR1));
	wClass.hCursor = (HCURSOR)LoadImage
	(
		hInstasce, 
		"starcraft-original\\Working In Background.ani",
		IMAGE_CURSOR,
		LR_DEFAULTSIZE, LR_DEFAULTSIZE,
		LR_LOADFROMFILE
	);
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

	INT screen_width = GetSystemMetrics(SM_CXSCREEN);
	INT screen_height = GetSystemMetrics(SM_CYSCREEN);

	INT window_width = screen_width * 0.75;
	INT window_height = screen_height * 3 / 4;

	INT window_start_x = screen_width / 8;
	INT window_start_y = screen_height / 8;			
	
	HWND hwnd = CreateWindowEx
	(
		NULL,							//ExStyle
		g_sz_CLASS_NAME,				//ClassName
		g_sz_CLASS_NAME,				//WindowName (Title) - эта строка отображается в заголовке окна
		WS_OVERLAPPEDWINDOW,			//Такой стиль задается для всех главных окон. 
										//Это окно будет родительским для других окон приложения.
		window_start_x, window_start_y,	//Position. Позиция окна
		window_width, window_height,	//Size. Размер окна
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

	case WM_MOVE:
	case WM_SIZE:
	{
		RECT window_rect;					//Rectangle - Прямоугольник
		GetWindowRect(hwnd, &window_rect);
		INT window_width = window_rect.right - window_rect.left;
		INT window_height = window_rect.bottom - window_rect.top;
		CONST INT SIZE = 256;
		CHAR sz_title[SIZE] = {};
		sprintf
		(
			sz_title, 
			"%s - Position: %i x %i, Size: %i x %i", 
			g_sz_CLASS_NAME, 
			window_rect.left, window_rect.top,
			window_width, window_height
		);

		SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)sz_title);
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
