/*//////////////////////////////////////////////////////////////////////////
Warcraft III - Inventory Short Key (Item hot-key)
wc3isk.cpp

Version:	v2.1 RC6
Author:		Cee'z
Date:		Feb-xx-2011
Descriptions:	Using self inject technique - Disadvantage is both Hook 
function and main function is carrying by the same thread. 
When SYSTEM freeze the GUI (hit the close button), it may 
slow down the CPU a lot and make the cursor very lag. This 
can be fix by using a fake title bar with a close button.

---------------------
Structure:

3 Dialog from resource, 2 pop-up style, and 1 is a child
-Key Assign Dialog Message - no message handling
-Config Container Dialog - has its owned message handling
+Child Config Dialog - has its owned message handling

2 HookProcess : WH_KEYBOARD_LL, WH_MOUSE_LL
Some GUI handling function
Send Key Stroke function

The trick in Chat Support function: 
-Interference key message
-Send Numpad-key
-Send VK_BACK or VK_END

This will work on DoTA but not with some other map, because
VK_BackSpace and VK_End use for the hot-key that center screen and focus to main base.
This is the mechanism for Chat Support feature.

Note:
ALT + F,R,A,T,G is combination key to mini-map (except F for overriding creep Formation)
CTRL + C,M,S for music, sfx, center screen
Those Alt combination is useless anyway.

SHIFT + <command> for way-point action. Script action.
ALT + Fn : Do not process hot-key (especially Alt-F4)

After finding a way to eliminate unwanted character that show up on the chat MSG and find
out that my engine on detect combination is not good at all, I decide to remove combination
hot-key.

To do:
Remember to change IsWar3Active back to normal
//////////////////////////////////////////////////////////////////////////*/


// Must define it here, or it wont affect 
#define WINVER 0x0500			
#define _WIN32_IE 0x0500		
#define _WIN32_WINNT 0x0500		
#define CURRENTVERSION 2109 // v2.1 RC6
//#define _DEBUG 1 // to use trace

//begin header
#pragma once

#include <OleCtl.h>	// OleCtl.h must be include before stdafx.h, how odd!
#include "..\Libs\stdafx.h"
#include "..\Libs\_virtualKey.h"
#include "..\Libs\_ipicture.h"
#include "..\Libs\_urlStatic.h"
#include "..\Libs\_trace.h"
#include "..\Resouce\resource.h"
#include "..\Libs\_text_7.h"


//Macro for check box
#define cbGetCheck(x) SendMessage(x, BM_GETCHECK, 0, 0)		// Get checked status of Check box
#define cbChecked(x) SendMessage(x, BM_SETCHECK, BST_CHECKED, 0)	// Set checked status of Check box
#define cbUnChecked(x) SendMessage(x, BM_SETCHECK, BST_UNCHECKED, 0)	// Set checked status of Check box

//define all thing here
#define TRAY_ICON_ID		1			//	ID number for the Notify Icon
#define SWM_TRAYMSG			WM_APP		//	the message ID sent to our window

#define SM_SHOWCONFIG		WM_APP + 1	//	show the window
#define SM_HIDECONFIG		WM_APP + 2	//	hide the window
#define SM_DESTROY			WM_APP + 3	//	close the window
#define SM_SEPARATOR		WM_APP + 4	//	SEPARATOR
#define SM_SWITCHACTIVE		WM_APP + 5	//	ACTIVE HOOK or not
#define SM_ABOUT			WM_APP + 6	//	About
#define SHIFTTIMEOUT		WM_APP + 7


#define THREAD_KBHOOK_ID	NULL	//	NULL => current Thread ID, this Thread
#define THREAD_MSHOOK_ID	NULL	//	NULL => current Thread ID, this Thread

// struct to hold KEY DATA - This is look like a Record in a table
typedef struct SLOTKEYDATA {
	HWND btnHWnd;				// Handle to certain BUTTON
	DWORD btnID;				// BUTTON Resource ID
	DWORD vkTrigger;			// vKeyCode that we will monitor for
	MouseData msBtnTrigger;	// MouseBtnData that we will monitor for
	DWORD vkNumpad;				// Read only, init on starting, do not modify at runtime
} SlotKeyData;

// Using for Write to binary file or Read from binary file
typedef struct STOREDATA{
	INT version;
	DWORD vkCodes[6];
	MouseData mbtCodes[6];
	DWORD vkCodeKbHkOff;
	BOOL mouseEnable;
	BOOL wheelEnable;
	BOOL winkeyDisable; // 1 = disable, 0 = enable
	BOOL disableTip;	// 1 = disable, 0 = enable
	BOOL showConfigOnStart; //default = 0 (disable)
}StoreDataStruct;

// This will load hConfigDialog in shared common data segment
#pragma comment(linker, "/SECTION:.shr,RWS")
#pragma data_seg(".shr")
HWND hConfigDialog = NULL;		// handle to Config dialog
#pragma data_seg()

// Global Variables:
HINSTANCE		hInst;	// The current instance
NOTIFYICONDATA	niData;	// Storing notify icon data
static BOOL bIsKeyboardOff; //temporary Off, turn it on by a hot-key.
static BOOL isKBHookOn;	// KB HOOK status
static BOOL isMSHookOn;	// MS HOOK status
static BOOL isAtMouseTab;	// If we are open MOUSE CONFIG tab
static BOOL isToolActive;	// Status of tool
static BOOL isDestroy;		// Should post quit message or not
static BOOL bWheelSupport = FALSE; // Disable Scroll Wheel or NOT 
static BOOL bWinkeyDisable = FALSE; // Disable Win key or not
static BOOL bMSHookEnable;	// Disable MOUSE HOOK or NOT
static BOOL bShowCfgOnFirstRun = TRUE;
static BOOL bDisableTip = FALSE;
static BOOL bShiftDown = FALSE;
//static BOOL bLShiftDown = FALSE;
//static BOOL bRShiftDown = FALSE;
static BOOL bShowConfigOnStart = FALSE;

HHOOK kbhHook;		// Handle to our keyboard global hook
HHOOK mshHook;		// Handle to our mouse global hook

SlotKeyData sKeyDATA[6];	// Mini "database table" - capacity: 6 records - We work on this table
MouseData mbtMBCodes[6];	// MouseBtnData table - buffer which be written to keyset.dat

DWORD dwAssignBtn;	// slot as a button, on the config form.
DWORD dwVkCodeKbHkOff;	// vkCode of keyboard ON/OFF hot-key.
static HWND hAssignMessage;	// handle to Assign dialog
static HWND hAssignMSBTMessage;	// handle to Assign mouse hotkey dialog
static HWND hTabControl;		// handle to our tab control
static HWND hViewMsKbTab;	// handle to child dialog that put inside config dialog
static HWND hViewAboutTab;		// handle to about tab
static HWND hWheelCB;			// handle to check box wheel disable
static HWND hMouseCB;			// handle to check box mouse disable
static HWND hWinkeyDisableCB;		// handle to check box WIN key disable
static HWND hStaticTextKBOff;		// handle to static about VK_F7 toggle on/off
static HWND hStaticHotkeyKbOff;	// handle to static about VK_F7 toggle on/off
static HWND hAboutNameText;		// handle to static name in info tab
static HWND hAboutHomeLink;		// This is the homepage Static that me make it into hyper link
static HWND hDisbaleTipCB;			// handle to the the Disable Balloon Tip combo box in about tab
static HWND hStatusText;
static HWND hShowConfigOnStart;
WCHAR wc_wc3ClassName[12];	// buffer to hold WC3 class name
TCITEM tabItem;				// Store tabbed information

// For _ipicture to work
HBRUSH hBrush;
LPPICTURE iPicGlobal;

// Forward declarations of functions included in this code module:
// Remember: Functions are special variables
BOOL InitInstance(HINSTANCE, int);
BOOL OnInitDialog(HWND);
void ShowContextMenu(HWND);
DWORD GetNumpad(MouseData);
void TurnOnOff(void);
void TurnHookOff(void);
void TurnOnKBHook(void);
void TurnOnMSHook(void);
void TurnOffKBHook(void);
void TurnOffMSHook(void);
void showKBAssign(void);
void ResetAssignText();
INT	 DoWriteSetting(void);
void InitSetting (void);
void SetButtonText(HWND hWnd, LPSTR text);
void SetButtonText(SlotKeyData slotKey);
BOOL DoReadSetting(void);
BOOL SendKeyStroke(SlotKeyData);
BOOL SendKEY(DWORD keyCode);
BOOL SendKEY_event(DWORD keyCode, DWORD keyEvent);
BOOL SendMouseNumpad(MouseData msBtn);
void DoInitButton(BOOL isStartUp);
void DoResetButton(void);
void RedrawButtonName(void);
BOOL IsWar3Active(void);
void GetHwndAgain(void);
BOOL RemoveConflictKey(INT currentSlot,DWORD vKeyTrigger, MouseData mouseButton);
BOOL ShowBalloonTip (CHAR* text, UINT timeout = 100);
BOOL KeyboardSet(KBHOOKSTRUCT kbhs, WPARAM wParam);
BOOL KeyboardCall(KBHOOKSTRUCT kbhs, WPARAM wParam);
BOOL MouseSet(MSHOOKSTRUCT mshs, WPARAM wParam);
BOOL MouseCall(MSHOOKSTRUCT mshs, WPARAM wParam);

INT_PTR CALLBACK	ConfigDlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	ViewPaneProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	AboutPaneProc(HWND, UINT, WPARAM, LPARAM);

__declspec(dllexport) LRESULT CALLBACK KBHookProc ( int nCode,    WPARAM wParam,  LPARAM lParam);
__declspec(dllexport) LRESULT CALLBACK MSHookProc ( int nCode,    WPARAM wParam,  LPARAM lParam);



//Start Main program as an console application
int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;
	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) return FALSE;
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_CFG_ACC);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)||
			!IsDialogMessage(msg.hwnd,&msg) ) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int) msg.wParam;
}

// Initialize the window and tray icon
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	// Check if there is another instance has been loaded
	if (hConfigDialog)
	{
		MessageBox(NULL,TEXT_LOADED_ALERT,_T("Notify"),MB_OK);
		return FALSE;  // Quit here
	}

	// Store instance handle and create dialog
	hInst = hInstance;

	// Create a modeless dialog that ready to show up.
	hAssignMessage = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_ASSIGN_DLG),0,0);
	hConfigDialog = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_CONFIG_DLG),0,(DLGPROC)ConfigDlgProc);

	if (!hConfigDialog || !hAssignMessage) return FALSE;

	hBrush = CreateSolidBrush(COLOR_BTNFACE);

	// Fill the NOTIFYICONDATA structure and call Shell_NotifyIcon
	// zero the structure - note:	Some Windows functions require this but I can't be 
	//								bothered which ones do and which ones don't.


	// get Shell32 version number and set the size of the structure
	//		note:	the MSDN documentation about this is a little
	//				dubious and I'm not at all sure if the method
	//				bellow is correct
	//ULONGLONG ullVersion = GetDllVersion(_T("Shell32.dll"));
	//if(ullVersion >= MAKEDLLVERULL(5, 0,0,0))


	ZeroMemory(&niData,sizeof(NOTIFYICONDATA));
	niData.cbSize = sizeof(NOTIFYICONDATA);
	niData.uID = TRAY_ICON_ID;
	niData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;

	// load the icon
	niData.hIcon = (HICON)LoadImage(hInstance,MAKEINTRESOURCE(IDI_ACTIVE),
		IMAGE_ICON, GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),
		LR_DEFAULTCOLOR);

	// the window to send messages to and the message to send
	//		note:	the message value should be in the
	//				range of WM_APP through 0xBFFF
	niData.hWnd = hConfigDialog;
	niData.uCallbackMessage = SWM_TRAYMSG;

	// tooltip message
	lstrcpyn(niData.szTip, _T(TEXT_WC3ISK_ON), sizeof(niData.szTip)/sizeof(TCHAR));

	Shell_NotifyIcon(NIM_ADD,&niData);

	// free icon handle
	if(niData.hIcon && DestroyIcon(niData.hIcon))
		niData.hIcon = NULL;

	//load JPEG as stream data
	DWORD dwStreamSize;
	IStream* pstream = CreateStreamOnResource(MAKEINTRESOURCE(IDR_BIN1),_T("BIN"),dwStreamSize);
	HRESULT hr = ::OleLoadPicture(pstream, dwStreamSize, FALSE, IID_IPicture, (LPVOID *)&iPicGlobal);

	hStatusText = GetDlgItem(hAssignMessage,IDC_STATUS_TEXT);
	ResetAssignText();

	// call ShowWindow here to make the dialog initially visible
	if (bShowCfgOnFirstRun || bShowConfigOnStart) 
	{
		ShowWindow(hConfigDialog,SW_RESTORE);
		isDestroy =! isDestroy;
		isAtMouseTab = FALSE;
	}
	//ShowBalloonTip(TEXT_WC3ISK_ON); Display tip in first RUN ? No
	return TRUE;
}

void GetHwndAgain()
{
	hWheelCB = GetDlgItem(hViewMsKbTab, IDC_CB_WHEELENABLE);
	hMouseCB = GetDlgItem(hViewMsKbTab,IDC_CB_MOUSEENABLE);
	hWinkeyDisableCB = GetDlgItem(hViewMsKbTab,IDC_CB_WINKEYDISABLE);
	hStaticTextKBOff = GetDlgItem(hViewMsKbTab,IDC_STATIC_TEXT_KBDISABLE);
	hStaticHotkeyKbOff = GetDlgItem(hViewMsKbTab,IDC_STATIC_HOTKEY_KBDISABLE);
}

BOOL OnInitDialog(HWND hWnd)
{
	// Prepare tab control
	hTabControl = GetDlgItem(hWnd,IDC_TABCTRL);
	tabItem.mask = TCIF_TEXT;  
	tabItem.pszText = TEXT_KEYBOARD;
	TabCtrl_InsertItem(hTabControl, 0,&tabItem);
	tabItem.pszText = TEXT_MOUSE;
	TabCtrl_InsertItem(hTabControl, 1,&tabItem);
	tabItem.pszText = TEXT_ABOUT;
	TabCtrl_InsertItem(hTabControl, 2,&tabItem);

	// Prepare Rect inside Tab Control
	RECT rcTabButton,rcTab,rcView;
	TabCtrl_GetItemRect(hTabControl, 0, &rcTabButton);
	GetClientRect(hTabControl, &rcTab);

	rcView.left = rcTabButton.left+2;
	rcView.top = rcTabButton.bottom +4;
	rcView.right = rcTab.right -6 - rcView.left;
	rcView.bottom = rcTab.bottom -6 - rcView.top;

	// Prepare Config tab
	hViewMsKbTab = CreateDialog(hInst,MAKEINTRESOURCE(IDD_CONFIGTAB),hTabControl, (DLGPROC) ViewPaneProc);
	SetWindowPos(hViewMsKbTab,NULL,rcView.left,rcView.top,rcView.right,rcView.bottom,NULL);
	ShowWindow(hViewMsKbTab,WM_SHOWWINDOW);

	// Prepare About tab
	hViewAboutTab = CreateDialog(hInst,MAKEINTRESOURCE(IDD_ABOUTTAB),hTabControl, (DLGPROC) AboutPaneProc);
	SetWindowPos(hViewAboutTab,NULL,rcView.left,rcView.top,rcView.right,rcView.bottom,NULL);
	ShowWindow(hViewAboutTab,SW_HIDE);

	// Prepare SYSTEM MENU
	HMENU hMenu = GetSystemMenu(hWnd,FALSE);
	if (hMenu)
	{
		AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
		AppendMenu(hMenu, MF_STRING, IDM_ABOUT, TEXT_ABOUT);
		AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
		AppendMenu(hMenu, MF_STRING, SM_DESTROY, TEXT_EXIT);
	}

	HICON hIcon = (HICON)LoadImage
		(	
		hInst,
		MAKEINTRESOURCE(IDI_WC3ISK),
		IMAGE_ICON, 0,0, LR_SHARED|LR_DEFAULTSIZE
		);

	SendMessage(hWnd,WM_SETICON,ICON_BIG,(LPARAM)hIcon);
	SendMessage(hWnd,WM_SETICON,ICON_SMALL,(LPARAM)hIcon);

	// Config is hidden now, so close message will Exit.
	isDestroy=TRUE; 

	// Set Title of this tool
	SetWindowText(hWnd,_T(TEXT_TITLE));

	// Load setting if we may have.
	isAtMouseTab = FALSE;
	InitSetting();

	ShowWindow(hStaticHotkeyKbOff, SW_SHOW);
	ShowWindow(hStaticTextKBOff, SW_SHOW);
	ShowWindow(hWinkeyDisableCB, SW_SHOW);

	return TRUE;
}

// Init all setting if we may have
void InitSetting (void)
{
	//turn on hooking
	TurnOnKBHook();

	DoInitButton(TRUE);
	isToolActive = TRUE; //indicate program status is ACTIVATE
	if (DoReadSetting() == TRUE) bShowCfgOnFirstRun = FALSE;
	if (bMSHookEnable) TurnOnMSHook();
}

// Name says it all
void showKBAssign()
{
	ShowWindow(hAssignMessage,SW_RESTORE);
}

// Name says it all
void showMSAssign()
{
	ShowWindow(hAssignMSBTMessage,SW_RESTORE);
}

void ResetAssignText()
{
	SetButtonText(hStatusText,TEXT_PRESS_A_KEY);
}

void ShowKBAssignErrorText()
{
	SetButtonText(hStatusText,TEXT_ASSIGN_ERROR);
	//ShowWindow(hAssignMessage, SW_INVALIDATE);
}

void TurnHookOff(void)
{
	isToolActive = FALSE;
	TurnOffKBHook();
	TurnOffMSHook();
}

void TurnOnOff(void)
{
	isToolActive=!isToolActive;
	bIsKeyboardOff = FALSE;
	if (bDisableTip) niData.uFlags = NIF_ICON; 
	else niData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;

	if (!isToolActive) {				// Just turn off hook - active = FALSE;
		niData.hIcon = (HICON)LoadImage(
			hInst,
			MAKEINTRESOURCE(IDI_INACTIVE),
			IMAGE_ICON, 
			GetSystemMetrics(SM_CXSMICON),
			GetSystemMetrics(SM_CYSMICON),
			LR_DEFAULTCOLOR);
		niData.dwInfoFlags = IDI_INACTIVE;
		lstrcpyn(niData.szTip, _T(TEXT_WC3ISK_OFF), sizeof(niData.szTip)/sizeof(TCHAR));
		Shell_NotifyIcon(NIM_MODIFY,&niData);

		// do end hook here.
		TurnOffKBHook();
		TurnOffMSHook();
		if (!bDisableTip) ShowBalloonTip(TEXT_WC3ISK_OFF);
	}	
	else 
	{						// Just turn on hook
		niData.hIcon = (HICON)LoadImage(
			hInst,
			MAKEINTRESOURCE(IDI_ACTIVE),
			IMAGE_ICON, 
			GetSystemMetrics(SM_CXSMICON),
			GetSystemMetrics(SM_CYSMICON),
			LR_DEFAULTCOLOR);
		niData.dwInfoFlags = IDI_ACTIVE;
		lstrcpyn(niData.szTip, _T(TEXT_WC3ISK_ON), sizeof(niData.szTip)/sizeof(TCHAR));
		Shell_NotifyIcon(NIM_MODIFY,&niData);

		//do start hook here.
		TurnOnKBHook();
		TurnOnMSHook();
		if (!bDisableTip) ShowBalloonTip(TEXT_WC3ISK_ON);
	}
}

void TurnOnKBHook(void)
{
	if (!isKBHookOn)
	{
		kbhHook = SetWindowsHookEx( WH_KEYBOARD_LL, (HOOKPROC) KBHookProc, hInst, THREAD_KBHOOK_ID);
		//ThreadID = NULL , that is the current thread that contain "KBHookProc"
		isKBHookOn = TRUE;
	}
}

void TurnOnMSHook(void)
{
	if (bMSHookEnable || isAtMouseTab)
		if (!isMSHookOn)
		{
			mshHook = SetWindowsHookEx( WH_MOUSE_LL, (HOOKPROC) MSHookProc, hInst, THREAD_MSHOOK_ID);
			//ThreadID = NULL , that is the current thread that contain "MSHookProc"
			isMSHookOn = TRUE;
		}

}

void TurnOffKBHook(void)
{
	if (isKBHookOn)
	{
		isKBHookOn=FALSE;
		UnhookWindowsHookEx(kbhHook);
	}
}

void TurnOffMSHook(void)
{
	BOOL ret;
	if (isMSHookOn)
	{
		ret = UnhookWindowsHookEx(mshHook);
		isMSHookOn = FALSE;
	}
}

void SetButtonText(HWND hWnd, LPSTR text) {SendMessageA(hWnd,WM_SETTEXT, 0, (LPARAM)text );}

void SetButtonText(SlotKeyData slotKey)
{
	if (!isAtMouseTab) SetButtonText(slotKey.btnHWnd, GetKbbtName(slotKey.vkTrigger));
	else SetButtonText(slotKey.btnHWnd, GetMsbtName(slotKey.msBtnTrigger));
}

void DoInitButton(BOOL isStartUp)
{
	DWORD IDBl[6]	=	{IDC_B1, IDC_B2, IDC_B4, IDC_B5, IDC_B7, IDC_B8};
	DWORD KSl[6]	=	{VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD7, VK_NUMPAD8};

	//get hwnd for all button slot and ready for change text later.
	for (INT i=0;i<6;i++)
	{
		if (isStartUp)
		{
			sKeyDATA[i].btnID = IDBl[i];
			sKeyDATA[i].vkNumpad = KSl[i];
			sKeyDATA[i].vkTrigger = KSl[i];
			sKeyDATA[i].btnHWnd = GetDlgItem(hViewMsKbTab,sKeyDATA[i].btnID);
			sKeyDATA[i].msBtnTrigger = MOUSEDATA_NODATA;
			mbtMBCodes[i] = MOUSEDATA_NODATA;

			// set variable for check box by default
			bMSHookEnable = FALSE;
			bWheelSupport = FALSE;
			cbUnChecked(hMouseCB);		//disable by default
			cbUnChecked(hWheelCB);		//disable by default
			cbUnChecked(hWinkeyDisableCB);
			dwVkCodeKbHkOff = VK_F7;
		}
		else
		{
			if(!isAtMouseTab)	// At keyboard tab
			{
				sKeyDATA[i].btnID = IDBl[i];
				sKeyDATA[i].vkNumpad = KSl[i];
				sKeyDATA[i].vkTrigger = KSl[i];
				dwVkCodeKbHkOff = VK_F7;
				cbUnChecked(hWinkeyDisableCB);
				bWinkeyDisable = FALSE;
				SetButtonText(hStaticHotkeyKbOff,GetKbbtName(dwVkCodeKbHkOff));
			}
			else	// at mouse tab
			{
				sKeyDATA[i].msBtnTrigger = MOUSEDATA_NODATA;
				mbtMBCodes[i] = MOUSEDATA_NODATA;
				bWheelSupport = FALSE;
				bMSHookEnable = FALSE;
				cbUnChecked(hWheelCB);
				cbUnChecked(hMouseCB);
				TurnOffMSHook();
			}
		}
	}	
}

void DoResetButton(void)
{
	DoInitButton(FALSE); //reset buttons' settings
	RedrawButtonName();
	DoWriteSetting();
}

INT DoWriteSetting()
{
	STOREDATA sdata;
	INT n_size;
	ZeroMemory(&sdata, sizeof (STOREDATA));
	
	for (int i=0;i<6;i++)
	{
		sdata.mbtCodes[i] = mbtMBCodes[i];
		sdata.vkCodes[i] = sKeyDATA[i].vkTrigger;
	}

	sdata.version = CURRENTVERSION;
	sdata.wheelEnable = bWheelSupport;
	sdata.vkCodeKbHkOff = dwVkCodeKbHkOff;
	sdata.mouseEnable = bMSHookEnable;
	sdata.winkeyDisable = bWinkeyDisable;
	sdata.disableTip = bDisableTip;
	sdata.showConfigOnStart = bShowConfigOnStart;

	HANDLE hfile = CreateFile ((LPCWSTR)KEYSETFILE, FILE_WRITE_DATA, 0, NULL,CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(hfile, (LPCVOID)(&sdata), sizeof sdata, (LPDWORD)&n_size, NULL);
	CloseHandle (hfile);
	return n_size;
}

BOOL DoReadSetting()
{
	DoInitButton(FALSE);
	INT n_size,i;
	STOREDATA sdata;
	ZeroMemory(&sdata, sizeof (STOREDATA));
	HANDLE hfile = CreateFile ((LPCWSTR)KEYSETFILE,GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hfile)
	{
		ReadFile(hfile, (LPVOID)&sdata, sizeof sdata, (LPDWORD)&n_size, NULL);
		CloseHandle (hfile);	
		if (n_size != sizeof sdata || sdata.version!= CURRENTVERSION) 
			return FALSE;
		for (i=0;i<6;i++)
		{
			sKeyDATA[i].vkTrigger = sdata.vkCodes[i];
			mbtMBCodes[i] = sdata.mbtCodes[i];
			sKeyDATA[i].msBtnTrigger = mbtMBCodes[i];
		}
		bMSHookEnable = sdata.mouseEnable;
		bWheelSupport = sdata.wheelEnable;
		dwVkCodeKbHkOff = sdata.vkCodeKbHkOff;
		bWinkeyDisable = sdata.winkeyDisable;
		bDisableTip = sdata.disableTip;
		bShowConfigOnStart = sdata.showConfigOnStart;
		SetButtonText(hStaticHotkeyKbOff,GetKbbtName(dwVkCodeKbHkOff));
	}

	RedrawButtonName();
	if (bMSHookEnable) cbChecked(hMouseCB);
	else {cbUnChecked(hMouseCB);TurnOffMSHook();}

	if (bWheelSupport) cbChecked(hWheelCB);
	else cbUnChecked(hWheelCB);

	if (bDisableTip) cbChecked(hDisbaleTipCB);
	else cbUnChecked(hDisbaleTipCB);

	if (bShowConfigOnStart) cbChecked(hShowConfigOnStart);
	else cbUnChecked(hShowConfigOnStart);

	if (bWinkeyDisable) cbChecked(hWinkeyDisableCB);
	else cbUnChecked(hWinkeyDisableCB);

	return TRUE;
}


BOOL RemoveConflictKey(INT currentSlot, DWORD vKeyTrigger, MouseData mouseButton)
{
	BOOL isKeyConflict = FALSE;

	for (int i=0;i<6;i++)
	{ 
		if (currentSlot < 0 || currentSlot > 6)		// check assigning KB OFF HK - default VK_F7
		{
			if (vKeyTrigger == sKeyDATA[i].vkNumpad || vKeyTrigger == sKeyDATA[i].vkTrigger) 
			{
				dwVkCodeKbHkOff = VK_F7;
				SetButtonText(hStaticHotkeyKbOff,GetKbbtName(dwVkCodeKbHkOff));
				isKeyConflict= TRUE;
			}
		}
		else										// check assigning 6 slot
		{
			// ignore the current slot!
			if (i == currentSlot) continue;

			// conflict with keyboard off hot-key or numpad original hot-key, reset the current slot.
			if (vKeyTrigger == dwVkCodeKbHkOff || vKeyTrigger == sKeyDATA[i].vkNumpad)
			{
				sKeyDATA[currentSlot].vkTrigger = sKeyDATA[currentSlot].vkNumpad;
				SetButtonText(sKeyDATA[currentSlot].btnHWnd,GetKbbtName(sKeyDATA[currentSlot].vkTrigger));
				isKeyConflict= TRUE;
			}

			// conflict with other hot-key, reset other hot-key
			if (vKeyTrigger == sKeyDATA[i].vkTrigger)
			{
				// sKeyDATA[i] = other hot-key
				// sKeyDATA[currentSlot] = current assigning hot-key
				sKeyDATA[i].vkTrigger = sKeyDATA[i].vkNumpad;
				SetButtonText(sKeyDATA[i].btnHWnd,GetKbbtName(sKeyDATA[i].vkTrigger));
				isKeyConflict= TRUE;
			}

			// mouse button conflict, reset other hot-key
			if (mouseButton == sKeyDATA[i].msBtnTrigger)
			{
				sKeyDATA[i].msBtnTrigger = MOUSEDATA_NODATA;
				mbtMBCodes[i] = MOUSEDATA_NODATA;
				SetButtonText(sKeyDATA[i].btnHWnd,GetMsbtName(MOUSEDATA_NODATA));
				isKeyConflict= TRUE;
			}
		}
	}
	return isKeyConflict;
}

void RedrawButtonName(void)
{
	if (!isAtMouseTab)
		for (int i=0;i<6;i++) 
			SetButtonText(sKeyDATA[i]);
	else
		for (int i=0;i<6;i++) 
			SetButtonText(sKeyDATA[i]);
}
void ShowContextMenu(HWND hWnd)
{
	POINT pt;
	GetCursorPos(&pt);
	HMENU hMenu = CreatePopupMenu();
	if(hMenu)
	{
		if (isToolActive)	InsertMenu(hMenu,-1, MF_BYPOSITION, SM_SWITCHACTIVE,_T("Un-&activate"));
		else InsertMenu(hMenu,-1, MF_BYPOSITION, SM_SWITCHACTIVE,_T("&Activate"));
		InsertMenu(hMenu, -1, MF_BYPOSITION, SM_SHOWCONFIG, _T(TEXT_CONFIG));
		InsertMenu(hMenu, -1, MF_SEPARATOR, SM_SEPARATOR, NULL);
		InsertMenu(hMenu,-1, MF_BYPOSITION, SM_ABOUT,TEXT_ABOUT);
		InsertMenu(hMenu, -1, MF_SEPARATOR, SM_SEPARATOR, NULL);
		InsertMenu(hMenu, -1, MF_BYPOSITION, SM_DESTROY, TEXT_EXIT);

		// note:	must set window to the foreground or the
		//			menu won't disappear when it should
		SetForegroundWindow(hWnd);

		TrackPopupMenu(hMenu, TPM_BOTTOMALIGN,
			pt.x, pt.y, 0, hWnd, NULL );
		DestroyMenu(hMenu);
	}
}

// Message handler for the app
INT_PTR CALLBACK ConfigDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	LPNMHDR lpnmhdr;

	switch (message) 
	{
	case WM_INITDIALOG:
		// ConfigDialog = hWnd;		// already set in "inits"
		return OnInitDialog(hWnd);

	case WM_NOTIFY:
		lpnmhdr = (LPNMHDR)lParam;  // get the tab message from lParam
		if (lpnmhdr->code == TCN_SELCHANGE)
		{
			GetHwndAgain();
			int iPage = TabCtrl_GetCurSel(hTabControl);
			switch (iPage)
			{
			case 0: //first tab = TEXT_KEYBOARD
				isAtMouseTab = FALSE;
				ShowWindow(hWinkeyDisableCB, SW_SHOW);
				ShowWindow(hStaticHotkeyKbOff, SW_SHOW);
				ShowWindow(hStaticTextKBOff, SW_SHOW);
				ShowWindow(hWheelCB, SW_HIDE);
				ShowWindow(hMouseCB, SW_HIDE);
				RedrawButtonName();
				ShowWindow(hViewMsKbTab, SW_SHOW);
				ShowWindow(hViewAboutTab, SW_HIDE);
				return 0;
			case 1: //second tab = TEXT_MOUSE
				isAtMouseTab = TRUE;
				ShowWindow(hWinkeyDisableCB, SW_HIDE);
				ShowWindow(hStaticHotkeyKbOff, SW_HIDE);
				ShowWindow(hStaticTextKBOff, SW_HIDE);
				ShowWindow(hWheelCB, SW_SHOW);
				ShowWindow(hMouseCB, SW_SHOW);
				RedrawButtonName();
				ShowWindow(hViewMsKbTab, SW_SHOW);
				ShowWindow(hViewAboutTab, SW_HIDE);
				return 0;
			case 2: //third tab = TEXT_ABOUT
				ShowWindow(hViewMsKbTab, SW_HIDE);
				ShowWindow(hViewAboutTab, SW_SHOW);
				return 0;
			default:
				break;
			}
		}
		return 0;
	//case SHIFTTIMEOUT:
	//	ShiftTimeOut();
	//	return 0;
	case SWM_TRAYMSG:
		switch(lParam)
		{
		case WM_LBUTTONDOWN:
			// active or un-active hook here
			TurnOnOff();
			break;
		case WM_RBUTTONDOWN:
		case WM_CONTEXTMENU:
			ShowContextMenu(hWnd);
			break;
		}
		break;

	case WM_SYSCOMMAND:
		if((wParam & 0xFFF0) == SC_MINIMIZE)
		{
			return 1;
		}
		else 
			if(wParam == IDM_ABOUT)
			{
				isDestroy=FALSE;
				ShowWindow(hConfigDialog,SW_SHOW);
				TabCtrl_SetCurSel(hTabControl,2);
				ShowWindow(hViewAboutTab,SW_SHOW);
				ShowWindow(hViewMsKbTab,SW_HIDE);
			}
			if(wParam == SM_DESTROY)
			{
				niData.uFlags = 0;
				Shell_NotifyIcon(NIM_DELETE,&niData);
				if (iPicGlobal) 
					iPicGlobal->Release();		// we must free this memory or it will be memory leak
				TurnOffKBHook();
				TurnOffMSHook();
				PostQuitMessage(0);
				break;
			}
			break;

	case WM_CLOSE:
		if (isDestroy) DestroyWindow(hWnd);
		else 
		{
			// Animate minimize to system tray
			RECT systrayRect,thisWindowRect;
			HWND hShellTrayWnd = FindWindowEx(NULL,NULL,_T("Shell_TrayWnd"),NULL);
			HWND hTrayNotifyWnd = FindWindowEx(hShellTrayWnd,NULL,TEXT("TrayNotifyWnd"),NULL);
			if(hTrayNotifyWnd)
				GetWindowRect(hTrayNotifyWnd,&systrayRect);
			else 
				GetWindowRect(GetDesktopWindow(),&systrayRect);
			GetWindowRect(hWnd,&thisWindowRect);

			// Set the destination rect to the lower right corner of the screen
			systrayRect.left = systrayRect.right;
			systrayRect.top = systrayRect.bottom;

			// Animate
			DrawAnimatedRects(hWnd,IDANI_CAPTION,&thisWindowRect,&systrayRect);

			isDestroy=TRUE;
			ShowWindow(hWnd, SW_HIDE);
			if (!bDisableTip) ShowBalloonTip(isToolActive?TEXT_WC3ISK_ON:TEXT_WC3ISK_OFF);
		}
		break;

	case WM_DESTROY:
		niData.uFlags = 0;
		Shell_NotifyIcon(NIM_DELETE,&niData);
		if (iPicGlobal) 
			iPicGlobal->Release();		// we must free this memory or it will be memory leak
		TurnOffKBHook();
		TurnOffMSHook();
		PostQuitMessage(0);
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam); 

		//catch system menu on the dialog
		switch (wmId)
		{
		case SM_SHOWCONFIG:
			isDestroy=FALSE;
			ShowWindow(hWnd, SW_RESTORE);
			TabCtrl_SetCurSel(hTabControl,0);
			isAtMouseTab = FALSE;
			ShowWindow(hWinkeyDisableCB, SW_SHOW);
			ShowWindow(hStaticHotkeyKbOff, SW_SHOW);
			ShowWindow(hStaticTextKBOff, SW_SHOW);
			ShowWindow(hWheelCB, SW_HIDE);
			ShowWindow(hMouseCB, SW_HIDE);
			RedrawButtonName();
			ShowWindow(hViewMsKbTab, SW_SHOW);
			ShowWindow(hViewAboutTab, SW_HIDE);
			return 0;
		case SM_HIDECONFIG: //catch minimize button
		case IDOK:
			ShowWindow(hWnd, SW_HIDE);
			isDestroy=TRUE;
			break;
		case SM_DESTROY:
			DestroyWindow(hWnd);	
			break;
		case SM_SWITCHACTIVE:
			TurnOnOff();
			break;
		case SM_ABOUT:
		case IDM_ABOUT:
			isDestroy=FALSE;
			ShowWindow(hConfigDialog,SW_SHOW);
			TabCtrl_SetCurSel(hTabControl,2);
			ShowWindow(hViewAboutTab,SW_SHOW);
			ShowWindow(hViewMsKbTab,SW_HIDE);
			break;
		}
		return 1; //end this case and also end this function.	
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK AboutPaneProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND aboutTBhWnd = GetDlgItem(hDlg,IDC_TB_ABOUT);
	switch (message)
	{
	case WM_INITDIALOG:
		// Prepare handles for About Tab
		hAboutNameText = GetDlgItem(hDlg,IDC_STATIC_NAME);
		hAboutHomeLink = GetDlgItem(hDlg, IDC_STATIC_ABOUT_LINK);
		hDisbaleTipCB = GetDlgItem(hDlg, IDC_CB_DISABLETIP);
		hShowConfigOnStart = GetDlgItem(hDlg, IDC_CB_SHOWCFGONSTART);
		SetWindowText(hAboutHomeLink, TEXT_ABOUT_HOMEPAGE);
		SetWindowText(hAboutNameText, TEXT_APPNAME_PLATFORM);
		SetButtonText(hStaticTextKBOff, TEXT_KBHKOFF);

		setStaticUnderline(hAboutHomeLink);	// Make it underlining
		urlStaticProc  = (WNDPROC)SetWindowLong(hAboutHomeLink, GWL_WNDPROC, (LONG)StaticSubProc);
		SetWindowText(aboutTBhWnd,TEXT_ABOUT_LONG);
		//SetWindowText(GetDlgIftem(hDlg,IDC_STATIC_NAME),TEXT(TEXT_APPNAME_PLATFORM));
		return TRUE;

	case WM_SETCURSOR:	// cursor "hand" for hyper-link
		setStaticCursor(hAboutHomeLink);
		return 0; // return FALSE to prevent effect to other Static, return TRUE will effect to other control 

	case WM_CTLCOLORSTATIC:		// color blue for hyper-link
		if ((HWND)lParam == hAboutHomeLink)
			return (INT_PTR) setStaticColor(wParam, RGB(0,0,255), GetSysColor(COLOR_3DFACE));
		return 0; // return FALSE to prevent effect to other Static, return TRUE will effect to other Static

	case WM_COMMAND:		// handle hyper-link mouse click
		if((DWORD)wParam == IDC_STATIC_ABOUT_LINK)
			ShellExecute(NULL, L"open", TEXT_ABOUT_LINK, NULL, NULL, SW_SHOWNORMAL);
		
		if ((DWORD)wParam == IDC_CB_DISABLETIP)
		{
			bDisableTip = (cbGetCheck(hDisbaleTipCB) == BST_CHECKED)? TRUE : FALSE;		
			DoWriteSetting();
		}

		if ((DWORD)wParam == IDC_CB_SHOWCFGONSTART)
		{
			bShowConfigOnStart = (cbGetCheck(hShowConfigOnStart) == BST_CHECKED)? TRUE : FALSE;		
			DoWriteSetting();
		}
		break;
	}
	return FALSE;
}


// Message handler for the app
INT_PTR CALLBACK ViewPaneProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT bitmapRc;
	BOOL doAction = FALSE;
	switch (message) 
	{
	case WM_INITDIALOG:

		// Get Handles of ViewPane (sometime out side this init won't work, I don't understand)
		hWheelCB = GetDlgItem(hWnd, IDC_CB_WHEELENABLE);
		hMouseCB = GetDlgItem(hWnd, IDC_CB_MOUSEENABLE);
		hWinkeyDisableCB = GetDlgItem(hWnd,IDC_CB_WINKEYDISABLE);
		hStaticTextKBOff = GetDlgItem(hWnd, IDC_STATIC_TEXT_KBDISABLE);
		hStaticHotkeyKbOff = GetDlgItem(hWnd, IDC_STATIC_HOTKEY_KBDISABLE);

		//setStaticUnderline(hStaticHotkeyKbOff);	// Make it underlining
		urlStaticProc  = (WNDPROC)SetWindowLong(hStaticHotkeyKbOff, GWL_WNDPROC, (LONG)StaticSubProc);
		return 0;

	case WM_SETCURSOR:
		setStaticCursor(hStaticHotkeyKbOff);
		return 0;

	case WM_CTLCOLORSTATIC:
		if ((HWND)lParam == hStaticHotkeyKbOff )
			return (INT_PTR) setStaticColor(wParam, RGB(0,100,255), GetSysColor(COLOR_3DFACE)); 
		return 0; // return FALSE to prevent effect to other Static, return TRUE will effect to other Static

	case WM_PAINT:
		// get width and height of picture
		bitmapRc = Rect(10,12,150,225);

		// display picture
		hdc = BeginPaint(hWnd, &ps);
		iPictureOnPaint(hWnd, ps, hdc, iPicGlobal, bitmapRc);//return BOOL  //check if we can paint or not!
		EndPaint(hWnd, &ps);
		return 0;

	case WM_COMMAND:
		//wmId    = LOWORD(wParam);
		//wmEvent = HIWORD(wParam); 

		//catch the slot button on config dialog
		switch(wParam) 
		{
		case IDC_CB_WINKEYDISABLE:
			if (cbGetCheck(hWinkeyDisableCB) == BST_CHECKED)
				bWinkeyDisable = TRUE;
			else bWinkeyDisable = FALSE;
			DoWriteSetting();
			break;

		case IDC_CB_WHEELENABLE:
			if (cbGetCheck(hWheelCB) == BST_CHECKED)
				bWheelSupport = TRUE;
			else bWheelSupport = FALSE;
			DoWriteSetting();
			return 1;

		case IDC_CB_MOUSEENABLE:
			if (cbGetCheck(hMouseCB) == BST_CHECKED)
			{
				bMSHookEnable = TRUE;
				if (isToolActive && !isMSHookOn) TurnOnMSHook();
			}
			else 
			{
				bMSHookEnable = FALSE;
				TurnOffMSHook();
			}
			DoWriteSetting();
			return 1;

			// Assign hot-key here
		case IDC_STATIC_HOTKEY_KBDISABLE: //Hotkey off - F7
			dwAssignBtn = IDC_STATIC_HOTKEY_KBDISABLE;
			doAction = TRUE;
			break;
		case IDC_B1:
			dwAssignBtn = IDC_B1;
			doAction = TRUE;
			break;
		case IDC_B2:
			dwAssignBtn = IDC_B2;
			doAction = TRUE;
			break;
		case IDC_B4:
			dwAssignBtn = IDC_B4;
			doAction = TRUE;
			break;
		case IDC_B5:
			dwAssignBtn = IDC_B5;
			doAction = TRUE;
			break;
		case IDC_B7:
			dwAssignBtn = IDC_B7;
			doAction = TRUE;
			break;
		case IDC_B8:
			dwAssignBtn = IDC_B8;
			doAction = TRUE;
			break;
		case IDC_BR:
			DoResetButton();
			dwAssignBtn = IDC_NULLBTN;
			return 1;
		default:
			dwAssignBtn = IDC_NULLBTN;
		}
	}
	if (doAction)
	{
		showKBAssign();
		if (!isToolActive) //If currently hook is off
		{
			// Turn it on temporary for capture a new key then turn off in callback function
			TurnOnKBHook();
			TurnOnMSHook();
		}
		if (isToolActive) //if HOOK IS ON 
		{
			if (isAtMouseTab) // BUT MS HOOK IS DISABLE
				TurnOnMSHook();
		}
	}

	return 0;
}


// Check if current window is Warcraft III
BOOL IsWar3Active(void)
{

/* 
#ifdef _DEBUG
 		return TRUE;
#endif 
*/

	HWND currentHWnd = GetForegroundWindow();
	GetClassName(currentHWnd, wc_wc3ClassName, sizeof(wc_wc3ClassName));

	// compare 2 string current HWND ClassName and "Warcraft III"
	return (wcscmp(wc_wc3ClassName, TEXT_WC3_CLASSNAME) == 0);  // equal return 1 (TRUE), not-equal return 0 (FALSE)
}

BOOL ShowBalloonTip (CHAR* text, UINT timeout)
{
	WCHAR tmpWChar[256];

	niData.cbSize = sizeof ( NOTIFYICONDATA );								// Size of structure
	niData.hWnd = hConfigDialog;											// Handle to parent window
	//niData.uID = TRAY_ICON_ID;												// Id of the icon
	niData.uFlags = NIF_INFO;												// Options: show balloon
	//niData.dwInfoFlags = IDI_ACTIVE;										// Icon for the balloon
	niData.uTimeout = timeout;												// Timeout when the balloon disappear

	tmpWChar[0] = NULL;														// Clear buffer
	mbstowcs(tmpWChar,TEXT_TITLE,sizeof (TEXT_TITLE));						// Convert from TCHAR to WCHAR
	lstrcpyn ( niData.szInfoTitle, tmpWChar, sizeof ( niData.szInfoTitle ) );	// Set Title

	tmpWChar[0] = NULL;														// Clear buffer
	mbstowcs(tmpWChar,text, sizeof (niData.szInfo));						// Convert from TCHAR to WCHAR
	lstrcpyn ( niData.szInfo, tmpWChar, sizeof ( niData.szInfo ) );			// Set Message

	return Shell_NotifyIcon ( NIM_MODIFY, &niData );						// Show balloon
}


//////////////////////////////////////////////////////////////////////////
BOOL KeyboardSet(KBHOOKSTRUCT kbhs, WPARAM wParam)
{
	//if (kbh.dwKeyCode == VK_F4 && kbh.dwFlags & 0x20) return FALSE;// Ignore this return to other key hook chain

	// catch KEYUP event only for those button are being assign value
	if  (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
	{
		// Key is not allow
		if (IsKeyNotAllowed(kbhs.dwKeyCode) || (kbhs.dwKeyCode == VK_F7 && dwAssignBtn != IDC_STATIC_HOTKEY_KBDISABLE))  
		{
			ShowKBAssignErrorText();
			return TRUE;
		}

		// From here the Assign dialog is out 
		// if we Return TRUE, that mean we don't accept the key.
		ShowWindow(hAssignMessage, SW_HIDE);

		// We got the key, now turn off the hook if tool is inactive
		if (!isToolActive) TurnOffKBHook(); 

		ResetAssignText();

		// Escape should not be caught
		if (kbhs.dwKeyCode == VK_ESCAPE)	
			return TRUE;

		if (dwAssignBtn == IDC_STATIC_HOTKEY_KBDISABLE) // if hot-key off - F7		
		{
			dwVkCodeKbHkOff = kbhs.dwKeyCode;
			RemoveConflictKey(-1, dwVkCodeKbHkOff, MOUSEDATA_UNKNOW);
			SetButtonText(hStaticHotkeyKbOff, GetKbbtName(dwVkCodeKbHkOff));
			DoWriteSetting();
		}

		// All slot button, exclude button Reset, hot-key off, and our null button
		if (dwAssignBtn != IDC_STATIC_HOTKEY_KBDISABLE && dwAssignBtn != IDC_NULLBTN)
		{	
			SHORT i;
			for (i=0;i<=6;i++) if (sKeyDATA[i].btnID == dwAssignBtn) break;

			// Because variable i stop at 6 and 6 is the NULL value of sKeyData[], it must be valid
			if (i<6)	
			{	// assign key code to our slot.
				sKeyDATA[i].vkTrigger = kbhs.dwKeyCode;
				RemoveConflictKey(i, sKeyDATA[i].vkTrigger, MOUSEDATA_UNKNOW);
				SetButtonText(sKeyDATA[i]);
				DoWriteSetting();
			}
		}

		// End doing assign, I need keep to it point to non-exist button after assign
		dwAssignBtn = IDC_NULLBTN;
		
		return TRUE; // kill the keyboard event message.
	}

	// kill all the other message event
	return TRUE;
}

BOOL HotKeyOnOffToggle()
{
	bIsKeyboardOff = (!bIsKeyboardOff)?TRUE:FALSE;
	niData.hIcon = (HICON)LoadImage(
		hInst,
		MAKEINTRESOURCE((bIsKeyboardOff)?IDI_INACTIVE:IDI_ACTIVE),
		IMAGE_ICON, 
		GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CYSMICON),
		LR_DEFAULTCOLOR);
	lstrcpyn(niData.szTip, _T(TEXT_WC3ISK_OFF), sizeof(niData.szTip)/sizeof(TCHAR));
	Shell_NotifyIcon(NIM_MODIFY,&niData);
	return TRUE;	
}

BOOL KeyboardCall(KBHOOKSTRUCT kbhs, WPARAM wParam)
{
	BOOL bKillKey = FALSE;

	// let those key go through
	if (IsKeyNotAllowed(kbhs.dwKeyCode))													return FALSE;
	if ((wParam == WM_SYSKEYDOWN || wParam == WM_SYSKEYUP) && kbhs.dwKeyCode == VK_F4)	return FALSE;
	
	// kill WIN key if needed
	if (bWinkeyDisable && IsWinKey(kbhs.dwKeyCode)) bKillKey = TRUE; 
	
	SHORT i=0;
	//process all message, but take action on DOWN only
	if (wParam == WM_KEYDOWN || wParam == WM_KEYUP || wParam == WM_SYSKEYDOWN || wParam == WM_SYSKEYUP) 
	{
		if (kbhs.dwKeyCode == dwVkCodeKbHkOff && wParam == WM_KEYDOWN)	//If F7 is pressed ?
			return HotKeyOnOffToggle();  // Toggle On/Off and kill F7 ?

		if (bIsKeyboardOff) return FALSE; // if currently off for a while;

		// OK we check key and send key here, we must not let i goes up to 6 coz' index start with 0 
		for (i=0;i<=5;i++)
			if (kbhs.dwKeyCode == sKeyDATA[i].vkTrigger)
			{
				kbhs.dwKeyCode = sKeyDATA[i].vkNumpad;			// Replace the specific key or combination keys with Numpad key
				// send key if it is DOWN, we don't send if it is UP
				if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) SendKeyStroke(sKeyDATA[i]); 
				
				bKillKey = IsKeyNoPass(sKeyDATA[i].vkTrigger);
				// return TRUE to kill the key trigger 
				// if it is Letter we should pass it return FALSE
				break;		// with bKillKey = FALSE pass through
			}
	}
	return bKillKey;
}



DWORD GetNumpad(MouseData mbt)
{
	for (int i=0;i<6;i++)
	{
		if (mbtMBCodes[i] == mbt) 
			return sKeyDATA[i].vkNumpad;
	}	
	return MOUSEDATA_NODATA;
}


// Send keystroke to specific program
BOOL SendKEY(DWORD keyCode)
{
	keybd_event((BYTE) keyCode, NULL, KEYDOWN, 0);
	keybd_event((BYTE) keyCode, NULL, KEYREPEAT | KEYUP, 0);
	return TRUE;
}

// BOOL SendKEY_event(DWORD keyCode, DWORD keyEvent)
// {
// 	keybd_event((BYTE) keyCode, NULL, KEYDOWN, 0);
// 	if (keyEvent == KEYDOWNTHENUP) keybd_event((BYTE) keyCode, NULL, KEYUP, 0);
// 	return TRUE;
// }

BOOL SendKeyStroke(SlotKeyData slotKey)
{
	if (slotKey.vkNumpad == NULL) return TRUE; // kill trigger key if errors
	SendKEY(slotKey.vkNumpad);

#ifdef _DEBUG
	TRACELN(TEXT("Send BACKSPACE OR END."));
#endif

	if (bShiftDown) SendKEY(VK_END);
	else SendKEY(VK_BACK);
	
	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
// Hook process of Keyboard hook
__declspec(dllexport) LRESULT CALLBACK KBHookProc (int nCode, WPARAM wParam, LPARAM lParam)
{
	KBHOOKSTRUCT kbh = *(KBHOOKSTRUCT*) lParam;
	BOOL doKillKey = FALSE;

	if (nCode == HC_ACTION)
	{
		/*/ doing Process Shift key
		if (kbh.dwKeyCode == VK_LSHIFT || kbh.dwKeyCode == VK_RSHIFT)
		{
			if (kbh.dwKeyCode == VK_LSHIFT) bLShiftDown = (wParam == WM_KEYUP)? FALSE : TRUE;
			if (kbh.dwKeyCode == VK_RSHIFT) bRShiftDown = (wParam == WM_KEYUP)? FALSE : TRUE;
			bShiftDown = (bLShiftDown||bRShiftDown)? TRUE : FALSE;
			
			if (bShiftDown) SetTimer(hConfigDialog, SHIFTTIMEOUT, 8000, NULL);
			return CallNextHookEx(kbhHook, nCode, wParam, (LPARAM)(&kbh)); // pass Shift to war3
		}*/

		// doing assign keys 
		if (IsWindowVisible(hAssignMessage) && !isAtMouseTab)  
		{	
			doKillKey = KeyboardSet(kbh, wParam);
		}

		// doing swap keys
		else if (wParam == WM_KEYDOWN || wParam == WM_KEYUP || wParam == WM_SYSKEYDOWN || wParam == WM_SYSKEYUP) 
				if (IsWar3Active())
				{
					doKillKey = KeyboardCall(kbh,wParam);
				}
	}

	if (doKillKey) return 1;  // Kill the key.
	else return CallNextHookEx(kbhHook, nCode, wParam, (LPARAM)(&kbh)); // Return the keyboard event messages to the system to allow others hooks.
}



BOOL SendMouseNumpad(MouseData msBtn)
{
	DWORD numkey = GetNumpad(msBtn);
	if (numkey) {
		SendKEY(numkey);
		SendKEY(VK_BACK); //delete that numpad character
	}
	return TRUE;
}


BOOL MouseSet(MSHOOKSTRUCT mshs, WPARAM wParam)
{
	MouseData mouseData = getMouseData(mshs.dwMsData);
	
	ShowWindow(hAssignMessage,SW_HIDE);
	if (!bMSHookEnable) TurnOffMSHook();
	
	SHORT i;
	for (i=0;i<=6;i++) {
		if (sKeyDATA[i].btnID == dwAssignBtn) break; //got press btnID now
	}
	
	if (wParam == WM_MBUTTONDOWN)
		mouseData = MOUSEDATA_MOUSE3;
	
	sKeyDATA[i].msBtnTrigger = mouseData;
	mbtMBCodes[i] = sKeyDATA[i].msBtnTrigger;
	RemoveConflictKey(i,sKeyDATA[i].vkTrigger,sKeyDATA[i].msBtnTrigger);
	SetButtonText(sKeyDATA[i].btnHWnd,GetMsbtName(sKeyDATA[i].msBtnTrigger));
	DoWriteSetting();

	return FALSE; // don't need to kill the trigger
}

// Return FALSE to pass the key, TRUE to kill the key
BOOL MouseCall(MSHOOKSTRUCT mshs, WPARAM wParam)
{
	// if xbutton or wheel
	MouseData mouseData = getMouseData(mshs.dwMsData);

	// if mid button
	if (wParam == WM_MBUTTONDOWN)
		mouseData = MOUSEDATA_MOUSE3;
		
	// pass the key through if no processing
	if (mouseData == MOUSEDATA_NODATA || wParam == WM_XBUTTONUP || wParam == WM_MBUTTONUP) 
		return FALSE;

	// if wheel is on action
	if (wParam == WM_MOUSEWHEEL)
		if (!bWheelSupport) return FALSE; // pass the wheel message

#ifdef _DEBUG
	_traceln(_T("Hit sendMouseNumpad with mousedata: %d"), mouseData);
#endif

	SendMouseNumpad(mouseData);
	if (wParam == WM_MOUSEWHEEL)
		if (bWheelSupport) 
			return TRUE; //kill the wheel message if wheel enable
	if (wParam == WM_MBUTTONDOWN || wParam == WM_MBUTTONDOWN) 
		return TRUE;
	return FALSE; //pass all key through
}


// Hook process of Mouse hook
__declspec(dllexport) LRESULT CALLBACK MSHookProc ( int nCode,WPARAM wParam,LPARAM lParam)
{
	BOOL ret = FALSE;
	MSHOOKSTRUCT mshs = *((MSHOOKSTRUCT*)lParam);
	if (nCode >= 0 && wParam != WM_MOUSEMOVE )
	{
		if (IsWindowVisible(hAssignMessage) && isAtMouseTab) 
		{
			// on tab assign mouse button
			if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN)
			{
				ShowWindow(hAssignMessage,SW_HIDE);
				if (!bMSHookEnable) TurnOffMSHook();
			}
			else 
				if (dwAssignBtn != IDC_NULLBTN) MouseSet(mshs,wParam); 
			// we don't need to kill the key after assigning
			// end catch button
		} 
		else
		{
			if (wParam == WM_LBUTTONDOWN || wParam == WM_LBUTTONUP || wParam == WM_RBUTTONDOWN || wParam == WM_RBUTTONUP)
			return CallNextHookEx(mshHook, nCode, wParam, lParam);
			// on send key
			if (IsWar3Active()){
				if (!MouseCall(mshs, wParam))  // if return is FALSE pass the key
					return CallNextHookEx(mshHook, nCode, wParam, lParam);
				return TRUE; // bug fix! kill key only when war3 is active!
			}
			// kill the key if needed (wheel up, down) 
			// return TRUE; // --> bug mouse key is locked in and outside game
			}
	}
	return CallNextHookEx(mshHook, nCode, wParam, lParam);
}
//////////////////////////////////////////////////////////////////////////
// End of the code
//////////////////////////////////////////////////////////////////////////