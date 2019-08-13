// InstrumentPanelApp.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Nudge.h"
#include "InstrumentPanelApp.h"
#include "InstrumentPanel.h"
#include "Instrument.h"

typedef struct _tagContext
{
	HANDLE stopListeningHandle;
	CRITICAL_SECTION clientDataInterlock;
	FSX_SIM_DATA* dataSet;
	wchar_t serveraddress[64];
	HANDLE hReader;

}FSX_CLIENT_CONTEXT;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
TCHAR szServerName[MAX_LOADSTRING];			// the main window class name
DWORD dwWindowMode;
UINT  uPort;
BOOL  useTurnAndSlip;
double  g_dBrightness;
UINT  nBackgroundColor;
UINT  nBackgroundCard;
FLOAT dpiX = 1.0f;
FLOAT dpiY = 1.0f;


// Forward declarations of functions included in this code module:
ATOM				InstrumentAppRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
int					ExitInstance(int retval);
bool			    InitialiseContext();
void				UninitialiseContext();
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
unsigned int __stdcall Reader(void* context);
bool				ReadConfiguration(HINSTANCE);
int					ParseCommandline();


const int TICKS_PER_SECOND = 60; // Update() rate per second
const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
const int MAX_FRAMESKIP = 10;

CInstrumentPanel* theInstrumentPanel = NULL;
FSX_SIM_DATA g_pSim_Data;
FSX_CLIENT_CONTEXT g_pSim_Context;
CNudgeCollection* g_nudges;



int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (ReadConfiguration(hInstance) == false)
		return FALSE;

	ParseCommandline();

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	// No Title bar LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_INSTRUMENTPANELAPP, szWindowClass, MAX_LOADSTRING);
	InstrumentAppRegisterClass(hInstance);

	// Perform application initialization:
	int cmdShow = (dwWindowMode == WS_POPUP) ? SW_MAXIMIZE : SW_NORMAL;
	if (!InitInstance(hInstance, cmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_INSTRUMENTPANELAPP));


	//int loops;
	msg.message = WM_NULL;
	ULONGLONG next_game_tick = GetTickCount64();
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			/*
			loops = 0;
			while (GetTickCount64() > next_game_tick && loops < MAX_FRAMESKIP)
			{
				//FSX_SIM_DATA simdata;
				EnterCriticalSection(&g_pSim_Context.clientDataInterlock);
				//memcpy_s(&simdata, sizeof(FSX_SIM_DATA), (FSX_SIM_DATA*)g_pSim_Context.dataSet, sizeof(FSX_SIM_DATA));
				theInstrumentPanel->Update(g_pSim_Context.dataSet);
				LeaveCriticalSection(&g_pSim_Context.clientDataInterlock);
				//theInstrumentPanel->Update(&simdata);
				next_game_tick += SKIP_TICKS;
				loops++;
			}*/

			if (bDesigntime == false)
			{
				EnterCriticalSection(&g_pSim_Context.clientDataInterlock);
				theInstrumentPanel->Update(g_pSim_Context.dataSet);
				LeaveCriticalSection(&g_pSim_Context.clientDataInterlock);
			}
			theInstrumentPanel->Render(bDesigntime);
		}
	}



	return ExitInstance((int)msg.wParam);

}

int ExitInstance(int retval)
{
	UninitialiseContext();
	theInstrumentPanel->UnInitialise();
	delete theInstrumentPanel;
	delete g_nudges;

	return retval;
}

void WriteConfiguration(HINSTANCE hInstance)
{
	wchar_t moduleName[_MAX_PATH * 4];
	DWORD dwWord = GetModuleFileNameW(hInstance, moduleName, _MAX_PATH);
	if (dwWord > 0)
	{
		TCHAR* p = _tcsrchr(moduleName, TCHAR('\\'));
		if (p == NULL) {
			MessageBoxW(NULL, L"Configuration file not available", L"InstrumentApp", MB_OK);
			return;
		}

		_tcscpy_s(++p, _MAX_PATH, L"config.json");
		if (PathFileExistsW(moduleName) == FALSE) {
			MessageBoxW(NULL, L"Configuration file missing or not available", L"InstrumentApp", MB_OK);
			return;
		}

		CkJsonObjectW json;
		if (json.LoadFile(moduleName) != true)
		{
			swprintf_s(moduleName, _MAX_PATH * 4, L"Configuration file: %s\n", json.lastErrorText());
			MessageBoxW(NULL, moduleName, L"InstrumentApp", MB_OK);
			return;
		}

		for (auto n : g_nudges->m_nudgelist)
		{
			wchar_t buf[16];
			swprintf_s(buf,(size_t)16, L"%d", n->m_id);
			CkJsonObjectW* nudgeObj = json.FindRecord(L"nudges", L"id", buf, true);
			if (json.get_LastMethodSuccess() == true) {
				
				nudgeObj->UpdateInt(L"x", n->m_x);
				nudgeObj->UpdateInt(L"y", n->m_y);
				
			}
			if (nudgeObj != NULL)
				delete nudgeObj;
		}

		json.put_EmitCrLf(true);
		json.put_EmitCompact(false);
		ATLTRACE(_T("%s\n"), json.emit());

		CkFileAccessW fileWriter;	
		bool bSuccess = fileWriter.WriteEntireTextFile(moduleName, json.emit(), L"us-ascii",false);
		

	}
		
}

bool ReadConfiguration(HINSTANCE hInstance)
{
	g_nudges = new CNudgeCollection();

	wchar_t moduleName[_MAX_PATH * 4];
	DWORD dwWord = GetModuleFileNameW(hInstance, moduleName, _MAX_PATH);
	if (dwWord > 0)
	{
		TCHAR* p = _tcsrchr(moduleName, TCHAR('\\'));
		if (p == NULL) {
			MessageBoxW(NULL, L"Configuration file not available", L"InstrumentApp", MB_OK);
			return false;
		}

		_tcscpy_s(++p, _MAX_PATH, L"config.json");
		if (PathFileExistsW(moduleName) == FALSE) {
			MessageBoxW(NULL, L"Configuration file missing or not available", L"InstrumentApp", MB_OK);
			return false;
		}

		CkJsonObjectW json;
		if (json.LoadFile(moduleName) != true)
		{
			swprintf_s(moduleName, _MAX_PATH * 4, L"Configuration file: %s\n", json.lastErrorText());
			MessageBoxW(NULL, moduleName, L"InstrumentApp", MB_OK);
			return false;
		}



		_tcscpy_s(szServerName, MAX_LOADSTRING, json.stringOf(L"configuration.server"));
		if (_tcsicmp(json.stringOf(L"configuration.window"), L"fullscreen") == 0)
			dwWindowMode = WS_POPUP;
		else
			dwWindowMode = WS_OVERLAPPEDWINDOW;

		uPort = (UINT)json.IntOf(L"configuration.port");
		if (uPort == 0)
			uPort = FSX_SERVER_PORT;


		useTurnAndSlip = (BOOL)json.IntOf(L"configuration.turnandslip");

		bDesigntime = (BOOL)json.IntOf(L"configuration.layoutadjust");

		bDefaultLayout = (BOOL)json.IntOf(L"configuration.defaultlayout");

		g_dBrightness = (double)(json.IntOf(L"configuration.brightness") / 100.0);

		LPCWSTR sgray = json.stringOf(L"configuration.backgroundgrey");
		if (json.get_LastMethodSuccess() == true) {
			int ngray = (int)ceil(255.0*_wtof(sgray));
			nBackgroundColor = PALETTERGB(ngray, ngray, ngray);
		}
		else
			nBackgroundColor = 0;

		sgray = json.stringOf(L"configuration.backgroundcard");
		if (json.get_LastMethodSuccess() == true) {
			int ngray = (int)ceil(255.0*_wtof(sgray));
			nBackgroundCard = PALETTERGB(ngray, ngray, ngray);
		}
		else
			nBackgroundCard = PALETTERGB(0x22, 0x22, 0x22);



		CkJsonArrayW* nudges = json.ArrayOf(L"nudges");

		if (nudges != NULL)
		{
			int nNudgets = nudges->get_Size();
			for (int i = 0; i < nNudgets; i++)
			{
				//int n = nudges->TypeAt(i);
				if (nudges->TypeAt(i) == 3)
				{
					CkJsonObjectW* nudge = nudges->ObjectAt(i);
					g_nudges->Add(new CNudge(nudge->IntOf(L"id"), nudge->IntOf(L"x"), nudge->IntOf(L"y"), nudge->IntOf(L"sc")));
					delete nudge;
				}
			}
			delete nudges;
		}


		json.put_EmitCrLf(true);
		json.put_EmitCompact(false);
		ATLTRACE(_T("%s\n"), json.emit());

		return true;

	}
	MessageBoxW(NULL, L"Configuration file not available", L"InstrumentApp", MB_OK);
	return false;

}

void SaveNudges()
{
	theInstrumentPanel->UpdateNudges(g_nudges);
	WriteConfiguration(hInst);
}

//
//  FUNCTION: InstrumentAppRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM InstrumentAppRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_INSTRUMENTPANELAPP));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	if (dwWindowMode == WS_POPUP)
		wcex.lpszMenuName = NULL;  //Set to null
	else
		wcex.lpszMenuName = MAKEINTRESOURCE(IDC_INSTRUMENTPANELAPP);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	InitialiseContext();

	// FLOAT ratio = 768.0f / 1366.0f;

	theInstrumentPanel = new CInstrumentPanel(useTurnAndSlip,g_nudges, g_dBrightness);
	theInstrumentPanel->Initialise();
	theInstrumentPanel->GetDesktopDpi(dpiX, dpiY);


	HWND hWnd = CreateWindow(szWindowClass, szTitle,
		dwWindowMode, //WS_POPUP, //WS_OVERLAPPEDWINDOW, //| WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		static_cast<UINT>(ceil(1366.f * dpiX / 96.f)),
		static_cast<UINT>(ceil(768.f * dpiY / 96.f)),
		NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	//Create the Renderer object.
	theInstrumentPanel->SetWindow(hWnd);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

bool InitialiseContext()
{
	if (bDesigntime == false) {
		ZeroMemory(&g_pSim_Data, sizeof(FSX_SIM_DATA));
		g_pSim_Data.oiltemp = 491.67f;  //O degrees in Rankine

#ifdef _SAMPLE_SETTINGS
	//Sample settings 
		g_pSim_Data.indicated_altitude = 602.0;
		g_pSim_Data.kohlsman_setting = 1013.0005666;
		g_pSim_Data.rpm = 2400.0;
		g_pSim_Data.heading = (-210.0) * 0.01745329251994329576923690768489;
		g_pSim_Data.airspeed_true = 80;
		g_pSim_Data.pitch_degrees = (-10.0) * 0.01745329251994329576923690768489;
		g_pSim_Data.bank_degrees = (0.0) * 0.01745329251994329576923690768489;
		g_pSim_Data.vertical_speed = -500;
		g_pSim_Data.heading_rate = (5.0) * 0.01745329251994329576923690768489;
		g_pSim_Data.turnball = -0.5;
#endif

		g_pSim_Context.dataSet = &g_pSim_Data;
		g_pSim_Context.stopListeningHandle = CreateEvent(NULL, TRUE, FALSE, NULL);
		InitializeCriticalSection(&g_pSim_Context.clientDataInterlock);
		wcscpy_s(g_pSim_Context.serveraddress, 64, szServerName); //L"localhost");
		g_pSim_Context.hReader = (HANDLE)_beginthreadex(NULL, 0, Reader, &g_pSim_Context, 0, NULL);
		//std::wcout << L"FSX Server Running - press Q to exit" << L"\r\n";
	}
	return true;
}

void UninitialiseContext()
{
	if (bDesigntime == false) {
		SetEvent(g_pSim_Context.stopListeningHandle);

		HANDLE handles[1];
		handles[0] = g_pSim_Context.hReader;
		WaitForSingleObject(g_pSim_Context.hReader, 700);
		CloseHandle(g_pSim_Context.stopListeningHandle);
		CloseHandle(g_pSim_Context.hReader);
		DeleteCriticalSection(&g_pSim_Context.clientDataInterlock);
	}


}



//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
POINT ptLast;
POINT ptLastNudge;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_KEYDOWN:
	
	{
		if (bDesigntime == true && (wParam == TCHAR('S') || wParam == TCHAR('s')))
		{
			theInstrumentPanel->SetAllInstrumentFocus(false);
			theInstrumentPanel->SetAllInstrumentFocus(true);
		}


	}
	break;
	case WM_KEYUP:
	{
		if (bDesigntime == true && (wParam == TCHAR('S') || wParam == TCHAR('s')))
		{
			theInstrumentPanel->SetAllInstrumentFocus(false);
			SaveNudges();
		}
		if (wParam == TCHAR('B') || wParam == TCHAR('b'))
		{
			bDesigntime = bDesigntime == false ? true : false;
		}
		if (bDesigntime == true && (wParam == TCHAR('W') || wParam == TCHAR('w')))
		{
			SaveNudges();
			PostMessage(hWnd, WM_QUIT, 0, 0L);
		}
		if (wParam == TCHAR('X') || wParam == TCHAR('x'))
		{
			PostMessage(hWnd, WM_QUIT, 0, 0L);
		}

	}
	break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_LBUTTONDOWN:

		if (bDesigntime == true) {

			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);

			POINT pt;
			pt.x = xPos; // -(boundedInstrument->m_xPosition - boundedInstrument->m_xCaseOffset);
			pt.y = yPos; // -(boundedInstrument->m_yPosition - boundedInstrument->m_yCaseOffset);
			pt.x = static_cast<INT>(ceil((float)xPos * dpiX / 96.f));
			pt.y = static_cast<UINT>(ceil((float)yPos * dpiY / 96.f));

			CInstrument* boundedInstrument = theInstrumentPanel->FindBoundedInstrument(pt);
			if (boundedInstrument != NULL && boundedInstrument->m_bHasFocus == false)
			{
				RECT r;
				boundedInstrument->GetBounds(&r);
				bool b = boundedInstrument->PtInBounds(pt);
				if (b == true)
				{

					ptLastNudge.x = (LONG)boundedInstrument->m_nudge_x; // -boundedInstrument->m_xCaseOffset);
					ptLastNudge.y = (LONG)boundedInstrument->m_nudge_y;
					ptLast.x = pt.x;
					ptLast.y = pt.y;
					//boundedInstrument->Nudge((float)(pt.x), (float)(pt.y));
					boundedInstrument->m_bHasFocus = true;
					SetCapture(hWnd);
				}
			}

		}
		break;
	case WM_LBUTTONUP:
		if (bDesigntime == true) {
			CInstrument* boundedInstrument = theInstrumentPanel->FindFocussedInstrument();
			if (boundedInstrument != NULL)
			{
				
					boundedInstrument->m_bHasFocus = false;
				
			}
			else
				ATLTRACE(_T("Error - not the HSI instrument\n"));
			ReleaseCapture();


		}
		break;
	case WM_MOUSEMOVE:
		if (bDesigntime == true) {
			CInstrument* boundedInstrument = theInstrumentPanel->FindFocussedInstrument();
			if (boundedInstrument != NULL)
			{
				POINT pt;
				pt.x = GET_X_LPARAM(lParam);
				pt.y = GET_Y_LPARAM(lParam);
				//ScreenToClient(hWnd, &pt);

				boundedInstrument->m_nudge_x = (float)(ptLastNudge.x + (pt.x - ptLast.x));
				boundedInstrument->m_nudge_y = (float)(ptLastNudge.y + (pt.y - ptLast.y));
				//boundedInstrument->m_nudge_y = ptLast.y + pt.y;
				ATLTRACE(_T("%f %f\n"), (float)pt.x, (float)(pt.y));



			}

		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


unsigned int __stdcall Reader(void* context)
{

	FSX_CLIENT_CONTEXT* pContext = (FSX_CLIENT_CONTEXT*)context;

	CkSocketW socket;
	bool success;
	success = socket.UnlockComponent(L"PSYTEC.CB40717_KrvC5VzKoLml");
	if (success != true) {
		//std::cout << socket.lastErrorText() << "\r\n";
		return 0;
	}

	bool ssl = false;
	int maxWaitMillisec = 2000; //Reduced to improve catch up time when running and FSX restarts.



	while (WaitForSingleObject(pContext->stopListeningHandle, 10) != WAIT_OBJECT_0)
	{
		//  Connect to the SSL server:
		bool success = socket.Connect(pContext->serveraddress, uPort, false, maxWaitMillisec);
		if (success != true) {
			continue;
		}
		else {  //Connection - start receiving
			socket.put_MaxReadIdleMs(1380);  //wifi needs longer than 380ms
			socket.put_MaxSendIdleMs(380);

			while (WaitForSingleObject(pContext->stopListeningHandle, 10) != WAIT_OBJECT_0)
			{
				CkByteData dataBytes;
				dataBytes.clear();
				if (socket.ReceiveBytesN(sizeof(FSX_SIM_DATA), dataBytes) == false)
				{
					socket.Close(maxWaitMillisec); //No data
					EnterCriticalSection(&pContext->clientDataInterlock);
					ZeroMemory(pContext->dataSet, sizeof(FSX_SIM_DATA));
					LeaveCriticalSection(&pContext->clientDataInterlock);
					break;
				}
				else
				{
					EnterCriticalSection(&pContext->clientDataInterlock);
					memcpy_s(pContext->dataSet, sizeof(FSX_SIM_DATA), (FSX_SIM_DATA*)dataBytes.getRange(0, sizeof(FSX_SIM_DATA)), sizeof(FSX_SIM_DATA));
					LeaveCriticalSection(&pContext->clientDataInterlock);

				}



			} //end inner while - we no longer have a socket

		}
	}
	return 0;
}



/*******************************************************
WIN32 command line parser function
********************************************************/
int ParseCommandline()
{
	LPWSTR *szArglist;
	int nArgs;
	int i;

	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if (NULL == szArglist)
	{
		//wprintf(L"CommandLineToArgvW failed\n");
		return 0;
	}
	else
	{
		for (i = 1; i < nArgs; i++)
		{
			if (wcscmp(szArglist[i], L"/Design") == 0  || wcscmp(szArglist[i], L"-Design") == 0)
			{
				bDesigntime = true;
			}
		
			ATLTRACE("%d: %ws\n", i, szArglist[i]);
		}
	}

	// Free memory allocated for CommandLineToArgvW arguments.

	LocalFree(szArglist);

	return nArgs;

}
