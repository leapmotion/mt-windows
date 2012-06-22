#include "stdafx.h"
#include "resource.h"
#include "ResourcePng.h"
#include "GdiplusInitializer.h"
#include <Objidl.h>
#include "../OcuInterface/OcuInterface.h"
#include "../OcuInterface/OcuIcon.h"
#include "../OcuInterface/OcuImage.h"
#include <iostream>

using namespace Gdiplus;
using namespace std;

COcuIcon* g_pIcon1;
COcuIcon* g_pIcon2;

CResourcePng* g_pCircles[16];
COcuImage* g_pImages[16];

// The current index on g_pIcon
int g_iconIndex = 14;
int g_time = 0;

// Timer routine to update the above global icon:
void CALLBACK ChangeIcon(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	g_time++;
	g_iconIndex = (g_iconIndex + 1) % 16;
	
	g_pIcon1->SetImage(g_pImages[g_iconIndex]);
	g_pIcon1->SetPosition(
		0.5 + cos(g_time / 40.0f) / 2.0,
		0.5 + sin(g_time / 40.0f) / 2.0
	);
	g_pIcon1->Update();
	
	g_pIcon2->SetImage(g_pImages[g_iconIndex]);
	g_pIcon2->SetPosition(
		0.5 + cos(10 + g_time / 50.0f) / 2.0,
		0.5 + sin(10 + g_time / 50.0f) / 2.0
	);
	g_pIcon2->Update();
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* lpCmdLine, int nCmdShow)
{
	// Initialize GDI+
	GdiplusInitializer init;

	COcuHidInstance* hid = COcuInterface().GetFirstCompliantInterface();
	if(!hid)
	{
		cout << "No compliant interfaces found" << endl;
		return 0;
	}

	int ids[] =
	{
		IDB_CIRCLE01, IDB_CIRCLE02, IDB_CIRCLE03, IDB_CIRCLE04,
		IDB_CIRCLE05, IDB_CIRCLE06, IDB_CIRCLE07, IDB_CIRCLE08,
		IDB_CIRCLE09, IDB_CIRCLE10, IDB_CIRCLE11, IDB_CIRCLE12,
		IDB_CIRCLE13, IDB_CIRCLE14, IDB_CIRCLE15, IDB_CIRCLE16
	};
	for(int i = 0; i < 16; i++)
	{
		g_pCircles[i] = new CResourcePng(hInstance, ids[i]);
		HDC hdc = g_pCircles[i]->ConstructDC();

		COcuImage*& pImg = g_pImages[i];
		pImg = new COcuImage(hid);

		// There are a few options here:  A direct call to SetDC,
		// or the convenience routine SetImage.
		pImg->SetDC(hdc);

		// Example call that could be made instead of SetDC:
		// pImg->SetImage(L"C:\\path\\to\\image.png");

		// Call update no matter what
		pImg->Update();
	}

	// Create a single overlay icon:
	hid->CreateIcon(g_pIcon1);
	hid->CreateIcon(g_pIcon2);

	// Timer is set to give us a periodic refresh that may be trapped.
	SetTimer(g_pIcon1->GetHwnd(), 0, 50, ChangeIcon);

	// Main message loop.
	for(MSG msg; GetMessage(&msg, nullptr, 0, 0) > 0; )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

