/*
 cl -DUNICODE rbm.c gdi32.lib user32.lib

 or for a smaller executable
 cl -DUNICODE /GS- /O2 rbm.c kernel32.lib gdi32.lib user32.lib /link /NODEFAULTLIB /ENTRY:startup
*/

#include <windows.h>

#define IDM_RADIO1 1001
#define IDM_RADIO2 1002
#define IDM_RADIO3 1003
#define IDM_RADIO4 1004

/*
 MFT_STRING and MFT_BITMAP cannot be used together. This is apparent once
 you read that the low word of dwDataType is used for the bitmap handle.
 The MIIM options do allow you to use them both together.
 Most things work - once you get a decent grasp on the API which isn't
 particularly clear.
 Main issue is that specifying an item image seems to disable the default
 radio or tick images, even though it is nothing to do with those.
*/

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT("RadioMenuDemo");
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    RegisterClass(&wndclass);

    hwnd = CreateWindow(szAppName, TEXT("Radio Menu Demo"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        400, 300,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HMENU hMainMenu, hRadioSubMenu;

    switch (message) {
    case WM_CREATE: {
        // Create main menu
        hMainMenu = CreateMenu();
        hRadioSubMenu = CreatePopupMenu();

        HDC hdc = GetDC(hwnd);
        HDC memDC = CreateCompatibleDC(hdc);
        RECT rect = { 0, 0, 16, 16 };

        HBITMAP hBitmapRed = CreateCompatibleBitmap(hdc, 16, 16);
        HBITMAP hBitmapGreen = CreateCompatibleBitmap(hdc, 16, 16);
        HBITMAP hBitmapBlue = CreateCompatibleBitmap(hdc, 16, 16);

{
        HGDIOBJ oldObj = SelectObject(memDC, hBitmapRed);
        HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
        FillRect(memDC, &rect, hBrush);
        SelectObject(memDC, oldObj);
}

{
        HGDIOBJ oldObj = SelectObject(memDC, hBitmapGreen);
        HBRUSH hBrush = CreateSolidBrush(RGB(0, 255, 0));
        FillRect(memDC, &rect, hBrush);
        SelectObject(memDC, oldObj);
}

{
        HGDIOBJ oldObj = SelectObject(memDC, hBitmapBlue);
        HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 255));
        FillRect(memDC, &rect, hBrush);
        SelectObject(memDC, oldObj);
}

{
        // First radio item. Simple String item.
        // Legacy fType  MTF_STRING insufficent for some reason
        // docs says also need to specify MMIM_STRING
        MENUITEMINFO mii1;
        ZeroMemory(&mii1, sizeof(mii1));
        mii1.cbSize = sizeof(MENUITEMINFO);
        LPWSTR lp1 = L"Simple text image with radio button or tick if checked";
        mii1.dwTypeData = lp1;
        mii1.cch = lstrlen(lp1);
        mii1.wID = IDM_RADIO1;
        mii1.fMask = MIIM_ID | MIIM_STRING;
        InsertMenuItem(hRadioSubMenu, 0, TRUE, &mii1);
}
{
        // Second radio item. Text and bitmap (blue). Green image when checked
        // Notice that when unchecked the image is where the check would and
        // it is moved to the right when the item is checked.
        MENUITEMINFO mii2;
        ZeroMemory(&mii2, sizeof(mii2));
        mii2.cbSize = sizeof(MENUITEMINFO);
        LPWSTR lp2 = L"green checked image, no unchecked image, blue item image and text";
        mii2.dwTypeData = lp2;
        mii2.cch = lstrlen(lp2);
        mii2.wID = IDM_RADIO2;
        mii2.hbmpChecked = hBitmapGreen;
        mii2.hbmpItem = hBitmapBlue;
        mii2.fType = MFT_STRING | MFT_RADIOCHECK;
        mii2.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_BITMAP | MIIM_ID | MIIM_CHECKMARKS;
        InsertMenuItem(hRadioSubMenu, 1, TRUE, &mii2);
}

{
        // Third radio item.
        // Expands on 2nd case - adds an UNCHECKED Image. So now there's always
        // 2 images displayed  Check image is green checked, red unchecked.
        // normal item image is blue
        MENUITEMINFO mii3;
        ZeroMemory(&mii3, sizeof(mii3));
        mii3.cbSize = sizeof(MENUITEMINFO);
        LPWSTR lp3 = L"green checked image, red unchecked image, blue item image and text";
        mii3.dwTypeData = lp3;
        mii3.cch = lstrlen(lp3);
        mii3.wID = IDM_RADIO3;
        mii3.hbmpChecked = hBitmapGreen;
        mii3.hbmpUnchecked = hBitmapRed;
        mii3.hbmpItem = hBitmapBlue;
        mii3.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_BITMAP | MIIM_ID | MIIM_CHECKMARKS;
        InsertMenuItem(hRadioSubMenu, 2, TRUE, &mii3);
}
{
        // Fourth radio item.  text and item image is blue
        // I was expecting the default behaviour for checking but no radio button is drawn.
        // I am even trying to set initial state as checked.
        // Probably doing something wrong, but effect is that setting an item image
        // means you never get a radio button. If you set your own check images that's
        // drawn (see items 2 and 3). Item image seems to be incompatible with default
        // checked images. Is that a problem on Windows 10 ??
        MENUITEMINFO mii4;
        ZeroMemory(&mii4, sizeof(mii4));
        mii4.cbSize = sizeof(MENUITEMINFO);
        LPWSTR lp3 = L"no checked or unchecked image, blue item image and text. Why no radio button ??";
        mii4.dwTypeData = lp3;
        mii4.cch = lstrlen(lp3);
        mii4.wID = IDM_RADIO4;
        mii4.hbmpItem = hBitmapBlue;
        mii4.fState = MFS_CHECKED;
        // docs say MFT_RADIOCHECK would use a radio button mark instead of a check mark
        // if hbmpChecked is NULL, doesn't happen for me. Doesn't look like a tick would
        // work either (ie not setting fType).
        mii4.fType = MFT_RADIOCHECK;
        mii4.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_BITMAP | MIIM_ID | MIIM_STATE ;
        InsertMenuItem(hRadioSubMenu, 3, TRUE, &mii4);
}

        // Add submenu to main menu
        AppendMenu(hMainMenu, MF_POPUP, (UINT_PTR)hRadioSubMenu, L"Options");
        SetMenu(hwnd, hMainMenu);

        // Set initial checked item using CheckMenuRadioItem

        // Don't call this so we can try to get item 4 checked with a radio button. Didn't help
        CheckMenuRadioItem(hRadioSubMenu,
            IDM_RADIO1, IDM_RADIO4,  // Group range
            IDM_RADIO4,              // Initial selection
            MF_BYCOMMAND);           // Identifier type

        return 0;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDM_RADIO1:
        case IDM_RADIO2:
        case IDM_RADIO3:
        case IDM_RADIO4:
            // Update radio selection
            CheckMenuRadioItem(hRadioSubMenu,
                IDM_RADIO1, IDM_RADIO4,
                LOWORD(wParam), MF_BYCOMMAND);

            break;
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}


void WINAPI startup()
{
    WinMain(GetModuleHandle(NULL), NULL, NULL, SW_SHOWDEFAULT);
    ExitProcess(0);
}
