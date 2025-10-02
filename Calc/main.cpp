#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <float.h>
#include <stdio.h>
#include <iostream>
#include "resource.h"
#include "Constants.h"
#include <sal.h>

// Функции логирования
VOID LogMessage(const CHAR* format, ...)
{
    va_list args;
    va_start(args, format);

    CHAR buffer[512];
    vsprintf_s(buffer, sizeof(buffer), format, args);

    std::cout << buffer << std::endl;

    va_end(args);
}

VOID LogError(const CHAR* context, DWORD errorCode = 0)
{
    if (errorCode == 0)
        errorCode = GetLastError();

    LPSTR errorMsg = NULL;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&errorMsg,
        0,
        NULL
    );

    if (errorMsg)
    {
        LogMessage("ERROR [%s]: %s (Code: %d)", context, errorMsg, errorCode);
        LocalFree(errorMsg);
    }
    else
    {
        LogMessage("ERROR [%s]: Unknown error (Code: %d)", context, errorCode);
    }
}

INT WINAPI WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID SetSkin(HWND hwnd, CONST CHAR sz_skin[]);
VOID SetSkinFromDLL(HWND hwnd, CONST CHAR sz_skin[]);
VOID LoadFontFromDLL(HMODULE hFontModule, INT resourceID);
VOID LoadFontsFromDLL(HMODULE hFontModule);
VOID SetFont(HWND hwnd, CONST CHAR font_name[]);
VOID UpdateWindowColors(HWND hwnd, INT new_index); // Новая функция для обновления цветов

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow
)
{
    //1) Регистрация класса окна:
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

    //2) Создание окна:
    HWND hwnd = CreateWindowEx
    (
        NULL,
        g_sz_CLASS_NAME,
        g_sz_CLASS_NAME,
        WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        g_i_WINDOW_WIDTH, g_i_WINDOW_HEIGHT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL)
    {
        LogError("Create main window");
        MessageBox(NULL, "Window creation failed", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    //3) Запуск цикла сообщений:
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

// Новая функция для обновления цветов окна
VOID UpdateWindowColors(HWND hwnd, INT new_index)
{
    // Обновляем фон окна
    HBRUSH hbrBackground = CreateSolidBrush(g_WINDOW_BACKGROUND[new_index]);
    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hbrBackground);

    // Принудительная перерисовка всего окна
    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
}

INT WINAPI WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static DOUBLE a = DBL_MIN;
    static DOUBLE b = DBL_MIN;
    static INT operation = 0;
    static BOOL input = FALSE;
    static BOOL input_operation = FALSE;

    static INT index = 0;
    static INT font_index = 0;

    // Статические переменные для кистей
    static HBRUSH hbrEditBackground = NULL;
    static HBRUSH hbrWindowBackground = NULL;

    switch (uMsg)
    {
    case WM_CREATE:
    {
        LogMessage("=== Starting window creation ===");

        AllocConsole();
        FILE* freopen_result = freopen("CONOUT$", "w", stdout);
        if (freopen_result == NULL)
        {
            LogError("Redirect stdout");
            MessageBox(NULL, "Failed to redirect stdout", "Error", MB_OK | MB_ICONERROR);
        }
        system("CHCP 1251");

        // Создание дисплея
        LogMessage("Creating display...");
        HWND hEditDisplay = CreateWindowEx(
            NULL, "Edit", "0",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT | ES_NOHIDESEL, // Убрал ES_READONLY
            g_i_BUTTON_START_X, g_i_START_Y,
            g_i_DISPLAY_WIDTH, g_i_DISPLAY_HEIGHT,
            hwnd,
            (HMENU)IDC_EDIT_DISPLAY,
            GetModuleHandle(NULL),
            NULL
        );
        if (hEditDisplay == NULL)
        {
            LogError("Create display edit control");
            CHAR szError[256];
            sprintf_s(szError, sizeof(szError), "Failed to create display edit control. Error: %d", GetLastError());
            MessageBoxA(hwnd, szError, "Error", MB_OK | MB_ICONERROR);
        }
        else
        {
            LogMessage("Display edit control created successfully");
        }

        // Создание кнопок 1-9
        LogMessage("Creating digit buttons 1-9...");
        INT iDigit = IDC_BUTTON_1;
        CHAR szDigit[2] = {};
        for (int i = 6; i >= 0; i -= 3)
        {
            for (int j = 0; j < 3; j++)
            {
                szDigit[0] = iDigit - IDC_BUTTON_0 + '0';
                HWND hButton = CreateWindowEx(
                    NULL, "Button", szDigit,
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    g_i_BUTTON_START_X + (g_i_BUTTON_SIZE + g_i_INTERVAL) * j,
                    g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * i / 3,
                    g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
                    hwnd,
                    (HMENU)iDigit,
                    GetModuleHandle(NULL),
                    NULL
                );
                if (hButton == NULL)
                {
                    LogError("Create digit button");
                    CHAR szError[256];
                    sprintf_s(szError, sizeof(szError), "Failed to create button %d. Error: %d", iDigit, GetLastError());
                    MessageBoxA(hwnd, szError, "Error", MB_OK | MB_ICONERROR);
                }
                else
                {
                    LogMessage("Button %d created successfully", iDigit);
                }
                iDigit++;
            }
        }

        // Создание кнопки 0
        LogMessage("Creating button 0...");
        HWND hButton0 = CreateWindowEx(
            NULL, "Button", "0",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            g_i_BUTTON_START_X, g_i_BUTTON_START_Y + (g_i_BUTTON_SPACE) * 3,
            g_i_BUTTON_SIZE_DOUBLE, g_i_BUTTON_SIZE,
            hwnd,
            (HMENU)IDC_BUTTON_0,
            GetModuleHandle(NULL),
            NULL
        );
        if (hButton0 == NULL)
        {
            LogError("Create button 0");
            CHAR szError[256];
            sprintf_s(szError, sizeof(szError), "Failed to create button 0. Error: %d", GetLastError());
            MessageBoxA(hwnd, szError, "Error", MB_OK | MB_ICONERROR);
        }
        else
        {
            LogMessage("Button 0 created successfully");
        }

        // Создание кнопки точки
        LogMessage("Creating button point...");
        HWND hButtonPoint = CreateWindowEx(
            NULL, "Button", ".",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            g_i_BUTTON_START_X + g_i_BUTTON_SPACE * 2,
            g_i_BUTTON_START_Y + g_i_BUTTON_SPACE * 3,
            g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
            hwnd,
            (HMENU)IDC_BUTTON_POINT,
            GetModuleHandle(NULL),
            NULL
        );
        if (hButtonPoint == NULL)
        {
            LogError("Create button point");
            CHAR szError[256];
            sprintf_s(szError, sizeof(szError), "Failed to create button point. Error: %d", GetLastError());
            MessageBoxA(hwnd, szError, "Error", MB_OK | MB_ICONERROR);
        }
        else
        {
            LogMessage("Button point created successfully");
        }

        // Создание кнопок операций
        LogMessage("Creating operation buttons...");
        for (int i = 0; i < 4; i++)
        {
            HWND hButtonOp = CreateWindowEx(
                NULL, "Button", g_sz_OPERATIONS[i],
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                g_i_BUTTON_START_X + g_i_BUTTON_SPACE * 3,
                g_i_BUTTON_START_Y + g_i_BUTTON_SPACE * (3 - i),
                g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
                hwnd,
                (HMENU)(IDC_BUTTON_PLUS + i),
                GetModuleHandle(NULL),
                NULL
            );
            if (hButtonOp == NULL)
            {
                LogError("Create operation button");
                CHAR szError[256];
                sprintf_s(szError, sizeof(szError), "Failed to create operation button %s. Error: %d", g_sz_OPERATIONS[i], GetLastError());
                MessageBoxA(hwnd, szError, "Error", MB_OK | MB_ICONERROR);
            }
            else
            {
                LogMessage("Operation button %s created successfully", g_sz_OPERATIONS[i]);
            }
        }

        // Создание кнопок редактирования
        LogMessage("Creating edit buttons...");
        for (int i = 0; i < 3; i++)
        {
            HWND hButtonEdit = CreateWindowEx(
                NULL, "Button", g_sz_EDIT[i],
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                g_i_BUTTON_START_X + g_i_BUTTON_SPACE * 4,
                g_i_BUTTON_START_Y + g_i_BUTTON_SPACE * i,
                g_i_BUTTON_SIZE, i < 2 ? g_i_BUTTON_SIZE : g_i_BUTTON_SIZE_DOUBLE,
                hwnd,
                (HMENU)(IDC_BUTTON_BSP + i),
                GetModuleHandle(NULL),
                NULL
            );
            if (hButtonEdit == NULL)
            {
                LogError("Create edit button");
                CHAR szError[256];
                sprintf_s(szError, sizeof(szError), "Failed to create edit button %s. Error: %d", g_sz_EDIT[i], GetLastError());
                MessageBoxA(hwnd, szError, "Error", MB_OK | MB_ICONERROR);
            }
            else
            {
                LogMessage("Edit button %s created successfully", g_sz_EDIT[i]);
            }
        }

        LogMessage("Window creation completed");

        // Сначала устанавливаем скин, потом шрифты
        SetSkinFromDLL(hwnd, "square_blue");

        HMODULE hFonts = LoadLibraryA("Fonts.DLL");
        if (hFonts != NULL)
        {
            LoadFontsFromDLL(hFonts);
            SetFont(hwnd, g_sz_FONT[font_index]);
        }
        else
        {
            LogError("Load fonts DLL");
            SetFont(hwnd, "Arial");
        }

        // Инициализируем цвета
        UpdateWindowColors(hwnd, index);
    }
    break;

    case WM_CTLCOLOREDIT:
    {
        HDC hdcEdit = (HDC)wParam;
        SetBkMode(hdcEdit, OPAQUE);
        SetTextColor(hdcEdit, g_DISPLAY_FOREGROUND[index]); // Исправлено: теперь цвет применяется
        SetBkColor(hdcEdit, g_DISPLAY_BACKGROUND[index]);

        // Освобождаем старую кисть, если она существует
        if (hbrEditBackground != NULL)
        {
            DeleteObject(hbrEditBackground);
        }
        hbrEditBackground = CreateSolidBrush(g_DISPLAY_BACKGROUND[index]);

        return (LRESULT)hbrEditBackground;
    }
    break;

    case WM_CTLCOLORBTN:
    {
        // Обработка цвета для кнопок
        HDC hdcButton = (HDC)wParam;
        SetBkColor(hdcButton, g_WINDOW_BACKGROUND[index]);

        if (hbrWindowBackground == NULL)
        {
            hbrWindowBackground = CreateSolidBrush(g_WINDOW_BACKGROUND[index]);
        }

        return (LRESULT)hbrWindowBackground;
    }
    break;

    case WM_ERASEBKGND:
    {
        // Обработка фона окна
        HDC hdc = (HDC)wParam;
        RECT rect;
        GetClientRect(hwnd, &rect);

        if (hbrWindowBackground == NULL)
        {
            hbrWindowBackground = CreateSolidBrush(g_WINDOW_BACKGROUND[index]);
        }

        FillRect(hdc, &rect, hbrWindowBackground);
        return 1;
    }
    break;

    case WM_COMMAND:
    {
        CHAR szDisplay[g_SIZE] = {};
        CHAR szDigit[2] = {};
        HWND hEditDisplay = GetDlgItem(hwnd, IDC_EDIT_DISPLAY);
        if (LOWORD(wParam) >= IDC_BUTTON_0 && LOWORD(wParam) <= IDC_BUTTON_POINT)
        {
            if (input == FALSE) SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)"0");
            if (LOWORD(wParam) == IDC_BUTTON_POINT)	szDigit[0] = '.';
            else szDigit[0] = LOWORD(wParam) - IDC_BUTTON_0 + '0';
            SendMessage(hEditDisplay, WM_GETTEXT, g_SIZE, (LPARAM)szDisplay);
            if (szDisplay[0] == '0' && szDisplay[1] != '.') szDisplay[0] = 0;
            if (szDigit[0] == '.' && strchr(szDisplay, '.')) break;

            strcat(szDisplay, szDigit);
            SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)szDisplay);
            input = TRUE;
        }
        if (LOWORD(wParam) >= IDC_BUTTON_PLUS && LOWORD(wParam) <= IDC_BUTTON_SLASH)
        {
            SendMessage(hEditDisplay, WM_GETTEXT, g_SIZE, (LPARAM)szDisplay);
            if (input && a == DBL_MIN) a = atof(szDisplay);
            if (input) b = atof(szDisplay);

            input = FALSE;

            SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_EQUAL, 0);
            operation = LOWORD(wParam);
            input_operation = TRUE;
        }
        if (LOWORD(wParam) == IDC_BUTTON_BSP)
        {
            SendMessage(hEditDisplay, WM_GETTEXT, g_SIZE, (LPARAM)szDisplay);
            if (strlen(szDisplay) > 1) szDisplay[strlen(szDisplay) - 1] = 0;
            else szDisplay[0] = '0';
            SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)szDisplay);
        }
        if (LOWORD(wParam) == IDC_BUTTON_CLR)
        {
            a = b = DBL_MIN;
            operation = 0;
            input = input_operation = FALSE;
            SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)"0");
        }
        if (LOWORD(wParam) == IDC_BUTTON_EQUAL)
        {
            SendMessage(hEditDisplay, WM_GETTEXT, g_SIZE, (LPARAM)szDisplay);
            if (input && a == DBL_MIN) a = atof(szDisplay);
            if (input) b = atof(szDisplay);
            if (a == DBL_MIN) break;

            input = FALSE;
            switch (operation)
            {
            case IDC_BUTTON_PLUS:	a += b;		break;
            case IDC_BUTTON_MINUS:	a -= b;		break;
            case IDC_BUTTON_ASTER:	a *= b;		break;
            case IDC_BUTTON_SLASH:
                if (b != 0) a /= b;
                else
                {
                    SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)"Error");
                    a = DBL_MIN;
                    break;
                }
                break;
            }
            input_operation = FALSE;
            sprintf(szDisplay, "%g", a);
            SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)szDisplay);
        }
        SetFocus(hwnd);
    }
    break;

    case WM_KEYDOWN:
    {
        if (GetKeyState(VK_SHIFT) < 0)
        {
            if (wParam == 0x38)
            {
                SendMessage(GetDlgItem(hwnd, IDC_BUTTON_ASTER), BM_SETSTATE, TRUE, 0);
            }
        }
        else if (wParam >= '0' && wParam <= '9')
        {
            SendMessage(GetDlgItem(hwnd, LOWORD(wParam) - '0' + IDC_BUTTON_0), BM_SETSTATE, TRUE, 0);
        }
        else if (wParam >= 0x60 && wParam <= 0x69)
        {
            SendMessage(GetDlgItem(hwnd, LOWORD(wParam) - 0x60 + IDC_BUTTON_0), BM_SETSTATE, TRUE, 0);
        }

        switch (wParam)
        {
        default:
            break;
        case VK_OEM_PERIOD:
        case VK_DECIMAL:	SendMessage(GetDlgItem(hwnd, IDC_BUTTON_POINT), BM_SETSTATE, TRUE, 0); break;
        case VK_ADD:
        case VK_OEM_PLUS:	SendMessage(GetDlgItem(hwnd, IDC_BUTTON_PLUS), BM_SETSTATE, TRUE, 0); break;
        case VK_SUBTRACT:
        case VK_OEM_MINUS:	SendMessage(GetDlgItem(hwnd, IDC_BUTTON_MINUS), BM_SETSTATE, TRUE, 0); break;
        case VK_MULTIPLY:	SendMessage(GetDlgItem(hwnd, IDC_BUTTON_ASTER), BM_SETSTATE, TRUE, 0); break;
        case VK_DIVIDE:
        case VK_OEM_2:		SendMessage(GetDlgItem(hwnd, IDC_BUTTON_SLASH), BM_SETSTATE, TRUE, 0); break;
        case VK_BACK:		SendMessage(GetDlgItem(hwnd, IDC_BUTTON_BSP), BM_SETSTATE, TRUE, 0); break;
        case VK_ESCAPE:		SendMessage(GetDlgItem(hwnd, IDC_BUTTON_CLR), BM_SETSTATE, TRUE, 0); break;
        case VK_RETURN:		SendMessage(GetDlgItem(hwnd, IDC_BUTTON_EQUAL), BM_SETSTATE, TRUE, 0); break;
        }
    }
    break;

    case WM_KEYUP:
    {
        if (GetKeyState(VK_SHIFT) < 0)
        {
            if (wParam == 0x38)
            {
                SendMessage(GetDlgItem(hwnd, IDC_BUTTON_ASTER), BM_SETSTATE, FALSE, 0);
                SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_ASTER, 0);
            }
        }
        else if (wParam >= '0' && wParam <= '9')
        {
            SendMessage(GetDlgItem(hwnd, LOWORD(wParam) - '0' + IDC_BUTTON_0), BM_SETSTATE, FALSE, 0);
            SendMessage(hwnd, WM_COMMAND, LOWORD(wParam) - '0' + IDC_BUTTON_0, 0);
        }
        else if (wParam >= 0x60 && wParam <= 0x69)
        {
            SendMessage(GetDlgItem(hwnd, LOWORD(wParam) - 0x60 + IDC_BUTTON_0), BM_SETSTATE, FALSE, 0);
            SendMessage(hwnd, WM_COMMAND, LOWORD(wParam) - 0x60 + IDC_BUTTON_0, 0);
        }

        switch (wParam)
        {
        default:
            break;
        case VK_OEM_PERIOD:
        case VK_DECIMAL:
            SendMessage(GetDlgItem(hwnd, IDC_BUTTON_POINT), BM_SETSTATE, FALSE, 0);
            SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_POINT, 0);
            break;
        case VK_ADD:
        case VK_OEM_PLUS:
            SendMessage(GetDlgItem(hwnd, IDC_BUTTON_PLUS), BM_SETSTATE, FALSE, 0);
            SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_PLUS, 0);
            break;
        case VK_SUBTRACT:
        case VK_OEM_MINUS:
            SendMessage(GetDlgItem(hwnd, IDC_BUTTON_MINUS), BM_SETSTATE, FALSE, 0);
            SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_MINUS, 0);
            break;
        case VK_MULTIPLY:
            SendMessage(GetDlgItem(hwnd, IDC_BUTTON_ASTER), BM_SETSTATE, FALSE, 0);
            SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_ASTER, 0);
            break;
        case VK_DIVIDE:
        case VK_OEM_2:
            SendMessage(GetDlgItem(hwnd, IDC_BUTTON_SLASH), BM_SETSTATE, FALSE, 0);
            SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_SLASH, 0);
            break;
        case VK_BACK:
            SendMessage(GetDlgItem(hwnd, IDC_BUTTON_BSP), BM_SETSTATE, FALSE, 0);
            SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_BSP, 0);
            break;
        case VK_ESCAPE:
            SendMessage(GetDlgItem(hwnd, IDC_BUTTON_CLR), BM_SETSTATE, FALSE, 0);
            SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_CLR, 0);
            break;
        case VK_RETURN:
            SendMessage(GetDlgItem(hwnd, IDC_BUTTON_EQUAL), BM_SETSTATE, FALSE, 0);
            SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_EQUAL, 0);
            break;
        }
    }
    break;

    case WM_CONTEXTMENU:
    {
        HMENU hMainMenu = CreatePopupMenu();
        InsertMenu(hMainMenu, 0, MF_BYPOSITION | MF_STRING, CM_EXIT, "Exit");
        InsertMenu(hMainMenu, 0, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
        InsertMenu(hMainMenu, 0, MF_BYPOSITION | MF_STRING, CM_SQUARE_BLUE, "Square Blue");
        InsertMenu(hMainMenu, 0, MF_BYPOSITION | MF_STRING, CM_METAL_MISTRAL, "Metal Mistral");
        InsertMenu(hMainMenu, 0, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);

        for (INT i = 0; i < 4; i++)
        {
            InsertMenu(hMainMenu, i, MF_BYPOSITION | MF_STRING, i + 300, g_sz_FONT[i]);
        }

        INT item = TrackPopupMenuEx
        (
            hMainMenu,
            TPM_RETURNCMD | TPM_RIGHTALIGN | TPM_BOTTOMALIGN,
            LOWORD(lParam),
            HIWORD(lParam),
            hwnd,
            NULL
        );

        switch (item)
        {
        case CM_EXIT:
            SendMessage(hwnd, WM_DESTROY, 0, 0);
            break;
        case CM_SQUARE_BLUE:
            SetSkinFromDLL(hwnd, "square_blue");
            index = 0; // Устанавливаем индекс темы
            UpdateWindowColors(hwnd, index);
            break;
        case CM_METAL_MISTRAL:
            SetSkinFromDLL(hwnd, "metal_mistral");
            index = 1; // Устанавливаем индекс темы
            UpdateWindowColors(hwnd, index);
            break;
        default:
            // Обработка выбора шрифта
            if (item >= 301 && item <= 304)
            {
                font_index = item - 301; // Исправлено: правильный расчет индекса
                SetFont(hwnd, g_sz_FONT[font_index]);
            }
            break;
        }

        DestroyMenu(hMainMenu);
    }
    break;

    case WM_DESTROY:
        // Освобождаем ресурсы
        if (hbrEditBackground != NULL)
        {
            DeleteObject(hbrEditBackground);
            hbrEditBackground = NULL;
        }
        if (hbrWindowBackground != NULL)
        {
            DeleteObject(hbrWindowBackground);
            hbrWindowBackground = NULL;
        }

        FreeConsole();
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

LPSTR FormatLastError(DWORD dwErrorID)
{
    LPSTR lpszMessage = NULL;
    FormatMessage
    (
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dwErrorID,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_RUSSIAN_RUSSIA),
        (LPSTR)&lpszMessage,
        NULL,
        NULL
    );
    return lpszMessage;
}

VOID PrintLastError(DWORD dwErrorID)
{
    LPSTR lpszMessage = FormatLastError(GetLastError());
    std::cout << lpszMessage << std::endl;
    LocalFree(lpszMessage);
}

VOID SetSkin(HWND hwnd, CONST CHAR sz_skin[])
{
    std::cout << "SetSkin()" << std::endl;
    CHAR sz_filename[FILENAME_MAX] = {};
    for (int i = 0; i <= 9; i++)
    {
        sprintf(sz_filename, "BMP\\%s\\button_%i.bmp", sz_skin, i);
        HBITMAP bmpIcon = (HBITMAP)LoadImage
        (
            GetModuleHandle(NULL),
            sz_filename,
            IMAGE_BITMAP,
            i == 0 ? g_i_BUTTON_SIZE_DOUBLE : g_i_BUTTON_SIZE,
            g_i_BUTTON_SIZE,
            LR_LOADFROMFILE
        );
        PrintLastError(GetLastError());
        SendMessage(GetDlgItem(hwnd, IDC_BUTTON_0 + i), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmpIcon);
    }
    for (int i = IDC_BUTTON_POINT; i <= IDC_BUTTON_EQUAL; i++)
    {
        sprintf(sz_filename, "BMP\\%s\\button_%s.bmp", sz_skin, g_sz_BUTTON_FILENAMES[i - IDC_BUTTON_POINT]);
        HBITMAP bmpIcon = (HBITMAP)LoadImage
        (
            GetModuleHandle(NULL),
            sz_filename,
            IMAGE_BITMAP,
            g_i_BUTTON_SIZE,
            i == IDC_BUTTON_EQUAL ? g_i_BUTTON_SIZE_DOUBLE : g_i_BUTTON_SIZE,
            LR_LOADFROMFILE
        );
        SendMessage(GetDlgItem(hwnd, i), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmpIcon);
    }
    std::cout << delimiter << std::endl;
}

VOID SetSkinFromDLL(HWND hwnd, CONST CHAR sz_skin[])
{
    HMODULE hButtonsModule = LoadLibraryA(sz_skin);
    if (hButtonsModule == NULL)
    {
        LogError("Load skin DLL");
        MessageBoxA(hwnd, "Failed to load skin DLL", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    for (int i = IDC_BUTTON_0; i <= IDC_BUTTON_EQUAL; i++)
    {
        int width, height;

        if (i == IDC_BUTTON_0)
        {
            width = g_i_BUTTON_SIZE_DOUBLE;
            height = g_i_BUTTON_SIZE;
        }
        else if (i == IDC_BUTTON_EQUAL)
        {
            width = g_i_BUTTON_SIZE;
            height = g_i_BUTTON_SIZE_DOUBLE;
        }
        else
        {
            width = g_i_BUTTON_SIZE;
            height = g_i_BUTTON_SIZE;
        }

        HBITMAP bmpButton = (HBITMAP)LoadImageA(
            hButtonsModule,
            MAKEINTRESOURCEA(i),
            IMAGE_BITMAP,
            width,
            height,
            LR_SHARED
        );

        if (bmpButton != NULL)
        {
            SendMessageA(GetDlgItem(hwnd, i), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmpButton);
            LogMessage("Bitmap for button %d loaded successfully", i);
        }
        else
        {
            LogMessage("Failed to load bitmap for button %d", i);
        }
    }

    // Не освобождаем библиотеку здесь, если битмапы используются
}

VOID LoadFontFromDLL(HMODULE hFontModule, INT resourceID)
{
    if (hFontModule == NULL) return;

    HRSRC hFntRes = FindResourceA(hFontModule, MAKEINTRESOURCEA(resourceID), RT_FONT);
    if (hFntRes == NULL) return;

    HGLOBAL hFntMem = LoadResource(hFontModule, hFntRes);
    if (hFntMem == NULL) return;

    DWORD len = SizeofResource(hFontModule, hFntRes);
    if (len == 0) return;

    VOID* fntData = LockResource(hFntMem);
    if (fntData == NULL) return;

    DWORD nFonts = 0;
    HANDLE hFont = AddFontMemResourceEx(fntData, len, NULL, &nFonts);

    if (hFont == NULL)
    {
        LogError("Add font from memory");
    }
    else
    {
        LogMessage("Font resource %d loaded successfully", resourceID);
    }
}

VOID LoadFontsFromDLL(HMODULE hFontModule)
{
    if (hFontModule == NULL) return;

    for (int i = 301; i <= 304; i++)
    {
        LoadFontFromDLL(hFontModule, i);
    }
}

VOID SetFont(HWND hwnd, CONST CHAR font_name[])
{
    HWND hEditDisplay = GetDlgItem(hwnd, IDC_EDIT_DISPLAY);

    // Сначала удаляем старый шрифт, если он есть
    HFONT hOldFont = (HFONT)SendMessage(hEditDisplay, WM_GETFONT, 0, 0);
    if (hOldFont != NULL)
    {
        DeleteObject(hOldFont);
    }

    HFONT hFont = CreateFont
    (
        g_i_DISPLAY_HEIGHT - 2, g_i_DISPLAY_HEIGHT / 3,
        0,
        0,
        FW_BOLD,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_TT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,
        FF_DONTCARE,
        font_name
    );

    if (hFont != NULL)
    {
        SendMessage(hEditDisplay, WM_SETFONT, (WPARAM)hFont, TRUE);
        LogMessage("Font set to: %s", font_name);

        // Принудительная перерисовка для обновления цвета текста
        InvalidateRect(hEditDisplay, NULL, TRUE);
        UpdateWindow(hEditDisplay);
    }
    else
    {
        LogError("Create font");
        HFONT hDefaultFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        SendMessage(hEditDisplay, WM_SETFONT, (WPARAM)hDefaultFont, TRUE);
    }
}