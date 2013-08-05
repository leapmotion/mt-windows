#include "stdafx.h"
#include "TouchDisplay.h"
#include "TouchRepresentation.h"
#include <vector>
#include <utility>

using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
vector<TouchRepresentation> touches;
vector<TouchRepresentation> clicks;

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

// Brushes used for painting:
HBRUSH g_hBrush[] =
{
	CreateSolidBrush(RGB(0x00, 0xFF, 0xFF)),
	CreateSolidBrush(RGB(0x00, 0x00, 0xFF)),
	CreateSolidBrush(RGB(0xFF, 0x00, 0x00)),
	CreateSolidBrush(RGB(0xFF, 0xFF, 0x00)),
	CreateSolidBrush(RGB(0x00, 0xFF, 0x00)),
	CreateSolidBrush(RGB(0xFF, 0x00, 0xFF))
};

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TOUCHDISPLAY, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if(!InitInstance(hInstance, nCmdShow))
		return false;

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TOUCHDISPLAY));

	// Main message loop:
	while(GetMessage(&msg, NULL, 0, 0))
		if(!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TOUCHDISPLAY));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_TOUCHDISPLAY);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance;
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	if(!hWnd)
		return false;
	
	// This is required to get WM_TOUCH instead of WM_GESTURE
	RegisterTouchWindow(hWnd, TWF_FINETOUCH);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return true;
}

void DoDraw(HDC hdc)
{
	for(size_t i = touches.size(); i--;)
	{
		auto& touch = touches[i];
		ASSERT(!touch.IsMouseClick());

		RECT rc = {0, 0, 1, 1};
		InflateRect(&rc, 2, 2);
		OffsetRect(&rc, touch.GetX(), touch.GetY());
		InflateRect(&rc, 5, 5);
		FillRect(
			hdc,
			&rc,
			g_hBrush[touch.GetID() % ARRAYSIZE(g_hBrush)]
		);
	}
	
	for(size_t i = clicks.size(); i--;)
	{
		auto& click = clicks[i];
		ASSERT(click.IsMouseClick());

		RECT rc = {0, 0, 1, 1};
		InflateRect(&rc, 1, 1);
		OffsetRect(&rc, click.GetX(), click.GetY());
		FillRect(hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch(message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		// Parse the menu selections:
		switch(wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_EDIT_CLEAR:
			touches.clear();
      clicks.clear();
			InvalidateRect(hWnd, nullptr, true);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_LBUTTONUP:
		{
			POINT pt = {(LONG)(lParam & 0xFFFF), (LONG)(lParam >> 16)};
			clicks.push_back(TouchRepresentation(pt));
			InvalidateRect(hWnd, nullptr, true);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DoDraw(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_TOUCH:
		TouchRepresentation::Translate(touches, hWnd, wParam, lParam);
		CloseTouchInputHandle((HTOUCHINPUT)lParam);
		InvalidateRect(hWnd, nullptr, true);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch(message)
	{
	case WM_INITDIALOG:
		return true;

	case WM_COMMAND:
		if(LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return true;
		}
		break;
	}
	return false;
}
