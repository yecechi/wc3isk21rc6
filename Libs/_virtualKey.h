//////////////////////////////////////////////////////////////////////////
//	virtualKey.h
//
//	Author:		Cee'z
//	Date:		Feb-13-2011
//	Descript:	Header Contain struct and defination for KEYBOARD HOOK
//////////////////////////////////////////////////////////////////////////
#ifdef _VIRTUALKEY
#else

#define _VIRTUALKEY


#define WH_KEYBOARD_LL	13	//	13 is low level keyboard hook.
#define WH_MOUSE_LL		14		//	14 is low level mouse hook.
#define WH_MOUSE		7


//#define LLKHF_EXTENDED  0x01	// Low level Key Hook dwFlag - Key down extended
//#define LLKHF_ALTDOWN	0x20	// Low level Key Hook dwFlag - Alt key down

#define APPLY_KEYBOARD MULTIMEDIA_KEYBOARD
#define MULTIMEDIA_KEYBOARD 116		// 116 buttons
#define STANDARD_KEYBOARD	104		// 104 buttons

#define WM_MOUSEWHEEL		0x020A
// #define WM_XBUTTONDOWN	0x020B
// #define WM_XBUTTONUP		0x020C
// #define WM_XBUTTONDBLCLK	0x020D
#define WD_WHEELDATA_UP		0x0078	// 120 in dec		//WHEELDATA_UP
#define WD_WHEELDATA_DOWN	0xFF88	// 65416 in dec		//WHEELDATA_DOWN
#define WD_XBUTTON_BACK		1
#define WD_XBUTTON_NEXT		2

#define KEYDOWN				0x0001
#define KEYREPEAT			0x0001
#define KEYUP				0x0002
#define KEYDOWNTHENUP		0x0003


//Macro for get MOUSE DATA
#define _GET_MOUSEDATA(x) HIWORD(x) 

#define VK_LALT				0x00A4	// 164
#define VK_RALT				0x00A5	// 156
#define VK_PRTSCR			0x002C	// 44
#define VK_ALT				VK_MENU // I am not familiar with the name Menu for Alt key
#define VK_PAGEUP			VK_PRIOR // same as above
#define VK_PAGEDOWN			VK_NEXT // same as above
#define VK_CTXMENU			VK_APPS // same as above
#define VK_BACKSLASH		VK_OEM_5		// | \    - 220
#define VK_SINGLEQUOTE		VK_OEM_7		// " '
#define VK_SQRQUOTE_STR		VK_OEM_4		// } ]
#define VK_SQRQUOTE_END		VK_OEM_6		// { [
#define VK_EMPHASISE		VK_OEM_3		// ~ `
#define VK_MINUS			VK_OEM_MINUS	// - _
#define VK_EQUAL			VK_OEM_PLUS		// = +
#define VK_SEMICOLON		VK_OEM_1		// : ;
#define VK_SLASH			VK_OEM_2		// ? /
#define VK_PERIOD			VK_OEM_PERIOD	// > .
#define VK_COMMA			VK_OEM_COMMA	// < ,
#define VK_ENTER			VK_RETURN		// Enter button
#define VK_BACKSPC			VK_BACK
#define VK_NUMPERIOD		VK_DECIMAL


//BEGIN define VKname
#define VK_NAME_ENTER	"Enter" 
#define VK_NAME_SPACE	"Space" 
#define VK_NAME_ESCAPE	"Escape" 
#define VK_NAME_TAB		"Tab" 
#define VK_NAME_CAPITAL "Caps Lock" 
#define VK_NAME_BACKSPC	"Back Space" 
#define VK_NAME_DELETE	"Delete" 
#define VK_NAME_INSERT	"Insert" 
#define VK_NAME_HOME	"Home" 
#define VK_NAME_END		"End" 
#define VK_NAME_PAGEUP	"Page Up" 
#define VK_NAME_PAGEDOWN "Page Down" 
#define VK_NAME_RALT	"R ALT" 
#define VK_NAME_RSHIFT	"R Shift" 
#define VK_NAME_RCONTROL "R Ctrl" 
#define VK_NAME_LALT	"L ALT" 
#define VK_NAME_LCONTROL "L Ctrl" 
#define VK_NAME_LSHIFT	"L Shift" 
#define VK_NAME_UP		"Up" 
#define VK_NAME_DOWN	"Down" 
#define VK_NAME_LEFT	"Left" 
#define VK_NAME_RIGHT	"Right" 
#define VK_NAME_NUMLOCK	"Num Lock" 
#define VK_NAME_PRTSCR	"Print Screen" 
#define VK_NAME_SCROLL	"Scroll Lock" 
#define VK_NAME_PAUSE	"Pause Break" 
#define VK_NAME_LWIN	"L Win" 
#define VK_NAME_RWIN	"R Win" 
#define VK_NAME_CTXMENU	"Menu" 
#define VK_NAME_NUMPAD0 "Numpad 0" 
#define VK_NAME_NUMPAD1 "Numpad 1" 
#define VK_NAME_NUMPAD2 "Numpad 2" 
#define VK_NAME_NUMPAD3 "Numpad 3" 
#define VK_NAME_NUMPAD4 "Numpad 4" 
#define VK_NAME_NUMPAD5 "Numpad 5" 
#define VK_NAME_NUMPAD6 "Numpad 6" 
#define VK_NAME_NUMPAD7 "Numpad 7" 
#define VK_NAME_NUMPAD8 "Numpad 8" 
#define VK_NAME_NUMPAD9 "Numpad 9" 
#define VK_NAME_NUMPERIOD "Numpad ." 
#define VK_NAME_NUMADD	"Numpad +" 
#define VK_NAME_NUMSUB	"Numpad -" 
#define VK_NAME_NUMMUL	"Numpad *" 
#define VK_NAME_NUMDIV	"Numpad /" 
#define VK_NAME_F1		"F1" 
#define VK_NAME_F2		"F2" 
#define VK_NAME_F3		"F3" 
#define VK_NAME_F4		"F4" 
#define VK_NAME_F5		"F5" 
#define VK_NAME_F6		"F6" 
#define VK_NAME_F7		"F7" 
#define VK_NAME_F8		"F8" 
#define VK_NAME_F9		"F9" 
#define VK_NAME_F10		"F10" 
#define VK_NAME_F11		"F11" 
#define VK_NAME_F12		"F12" 
#define VK_NAME_BACKSLASH "\\" 
#define VK_NAME_EMPHASISE "`" 
#define VK_NAME_MINUS	"-" 
#define VK_NAME_EQUAL	"=" 
#define VK_NAME_SQRQUOTE_STR "[" 
#define VK_NAME_SQRQUOTE_END "]" 
#define VK_NAME_SEMICOLON ";" 
#define VK_NAME_SINGLEQUOTE "'" 
#define VK_NAME_COMMA	"," 
#define VK_NAME_PERIOD	"." 
#define VK_NAME_SLASH	"/" 
#define VK_NAME_A		"A" 
#define VK_NAME_B		"B" 
#define VK_NAME_C		"C" 
#define VK_NAME_D		"D" 
#define VK_NAME_E		"E" 
#define VK_NAME_F		"F" 
#define VK_NAME_G		"G"
#define VK_NAME_H		"H" 
#define VK_NAME_I		"I" 
#define VK_NAME_J		"J" 
#define VK_NAME_K		"K" 
#define VK_NAME_L		"L" 
#define VK_NAME_M		"M" 
#define VK_NAME_N		"N" 
#define VK_NAME_O		"O" 
#define VK_NAME_P		"P" 
#define VK_NAME_Q		"Q" 
#define VK_NAME_R		"R" 
#define VK_NAME_S		"S" 
#define VK_NAME_T		"T" 
#define VK_NAME_U		"U" 
#define VK_NAME_V		"V" 
#define VK_NAME_W		"W" 
#define VK_NAME_X		"X" 
#define VK_NAME_Y		"Y" 
#define VK_NAME_Z		"Z" 
#define VK_NAME_ZERO	"0" 
#define VK_NAME_ONE		"1" 
#define VK_NAME_TWO		"2" 
#define VK_NAME_THREE	"3" 
#define VK_NAME_FOUR	"4" 
#define VK_NAME_FIVE	"5" 
#define VK_NAME_SIX		"6" 
#define VK_NAME_SEVEN	"7" 
#define VK_NAME_EIGHT	"8" 
#define VK_NAME_NINE	"9" 
#define VK_NAME_MEDIA_EMAIL "Email" 
#define VK_NAME_MEDIA_PLAYER "Media" 
#define VK_NAME_MEDIA_PREVIOUS "Previous" 
#define VK_NAME_MEDIA_NEXT "Next" 
#define VK_NAME_MEDIA_VOLUP "Volume up" 
#define VK_NAME_MEDIA_VOLDOWN "Volume down" 
#define VK_NAME_MEDIA_PLAY "Play" 
#define VK_NAME_MEDIA_PAUSE "Pause" 
#define VK_NAME_MEDIA_MUTE "Mute" 
#define VK_NAME_MEDIA_SEARCH "Search" 
#define VK_NAME_MEDIA_FAVORITE "Favorite" 
#define VK_NAME_MEDIA_HOMEPAGE "Homepage" 
#define VK_NAME_SPECIAL "Special Key"

#define TEXT_MOUSEDATA_NODATA	"Not set"
#define TEXT_MOUSEDATA_MOUSE1	"Mouse 1"
#define TEXT_MOUSEDATA_MOUSE2	"Mouse 2"
#define TEXT_MOUSEDATA_MOUSE3	"Mouse 3"
#define TEXT_MOUSEDATA_MOUSE4	"Mouse 4"
#define TEXT_MOUSEDATA_MOUSE5	"Mouse 5"
#define TEXT_MOUSEDATA_MOUSE6	"Mouse 6"
#define TEXT_MOUSEDATA_MOUSE7	"Mouse 7"
#define TEXT_MOUSEDATA_MOUSE8	"Mouse 8"
#define TEXT_MOUSEDATA_WHEELUP	"Wheel Up"
#define TEXT_MOUSEDATA_WHEELDOWN "Wheel Down"
#define TEXT_MOUSEDATA_UNKNOW	"Unknow"

//END define VKey name

// VK_LBUTTON (0x01)
// VK_RBUTTON (0x02)
// VK_MBUTTON (0x04)
// VK_XBUTTON1 (0x05)
// VK_XBUTTON2 (0x06)

//MouseBtnData : extract from msData of MSHOOKSTRUCT
enum MouseData 
{
	MOUSEDATA_NODATA=0, 
	MOUSEDATA_MOUSE1=1, 
	MOUSEDATA_MOUSE2=2, 
	MOUSEDATA_MOUSE3=3, 
	MOUSEDATA_MOUSE4=4, 
	MOUSEDATA_MOUSE5=5,
	MOUSEDATA_MOUSE6=6,
	MOUSEDATA_MOUSE7=7,
	MOUSEDATA_MOUSE8=8,
	MOUSEDATA_WHEELUP=9,
	MOUSEDATA_WHEELDOWN=10,
	MOUSEDATA_UNKNOW=11
};
//BEGIN DECLARE STRUCT

//Structure used by WH_KEYBOARD_LL
typedef struct KBHOOKSTRUCT {
	DWORD   dwKeyCode;  //they usually use vkCode, for virtual key code
	DWORD   dwScanCode;
	DWORD   dwFlags;
	DWORD   dwTime;
	ULONG_PTR dwExtraInfo;
} KBHOOKSTRUCT;

//Structure used by WH_MOUSE_LL
typedef struct MSHOOKSTRUCT {
	POINT   pt;
	DWORD   dwMsData;
	DWORD   dwFlags;
	DWORD   dwTime;
	ULONG_PTR dwExtraInfo;
} MSHOOKSTRUCT;

typedef struct KEYSTROKENAME {
	DWORD dwKeyCode;
	CHAR *chrsKeyName;
} KeyStrokeName;
//END DECLARE STRUCT


// Constant array of KeyStrokeName
//116 for Multimedia keyboard, 104 for include menu, and winkey , 102 for standard keyboard.
const KeyStrokeName keysNames[MULTIMEDIA_KEYBOARD]=
{ 
	{VK_ENTER,		VK_NAME_ENTER},
	{VK_SPACE,		VK_NAME_SPACE},
	{VK_ESCAPE,		VK_NAME_ESCAPE},
	{VK_TAB,		VK_NAME_TAB},
	{VK_CAPITAL,	VK_NAME_CAPITAL},
	{VK_BACKSPC,	VK_NAME_BACKSPC},
	{VK_DELETE,		VK_NAME_DELETE},
	{VK_INSERT,		VK_NAME_INSERT},
	{VK_HOME,		VK_NAME_HOME},
	{VK_END,		VK_NAME_END},
	{VK_PAGEUP,		VK_NAME_PAGEUP},
	{VK_PAGEDOWN,	VK_NAME_PAGEDOWN},
	{VK_RALT,		VK_NAME_RALT},
	{VK_RSHIFT,		VK_NAME_RSHIFT},
	{VK_RCONTROL,	VK_NAME_RCONTROL},
	{VK_LALT,		VK_NAME_LALT},
	{VK_LCONTROL,	VK_NAME_LCONTROL},
	{VK_LSHIFT,		VK_NAME_LSHIFT},
	{VK_UP,			VK_NAME_UP},
	{VK_DOWN,		VK_NAME_DOWN},
	{VK_LEFT,		VK_NAME_LEFT},
	{VK_RIGHT,		VK_NAME_RIGHT},
	{VK_NUMLOCK,	VK_NAME_NUMLOCK},
	{VK_PRTSCR,		VK_NAME_PRTSCR},
	{VK_SCROLL,		VK_NAME_SCROLL},
	{VK_PAUSE,		VK_NAME_PAUSE},
	{VK_LWIN,		VK_NAME_LWIN},
	{VK_RWIN,		VK_NAME_RWIN},
	{VK_CTXMENU,	VK_NAME_CTXMENU},
	{VK_NUMPAD0,	VK_NAME_NUMPAD0},
	{VK_NUMPAD1,	VK_NAME_NUMPAD1},
	{VK_NUMPAD2,	VK_NAME_NUMPAD2},
	{VK_NUMPAD3,	VK_NAME_NUMPAD3},
	{VK_NUMPAD4,	VK_NAME_NUMPAD4},
	{VK_NUMPAD5,	VK_NAME_NUMPAD5},
	{VK_NUMPAD6,	VK_NAME_NUMPAD6},
	{VK_NUMPAD7,	VK_NAME_NUMPAD7},
	{VK_NUMPAD8,	VK_NAME_NUMPAD8},
	{VK_NUMPAD9,	VK_NAME_NUMPAD9},
	{VK_NUMPERIOD,	VK_NAME_NUMPERIOD},
	{VK_ADD,		VK_NAME_NUMADD},
	{VK_SUBTRACT,	VK_NAME_NUMSUB},
	{VK_MULTIPLY,	VK_NAME_NUMMUL},
	{VK_DIVIDE,		VK_NAME_NUMDIV},
	{VK_F1,			VK_NAME_F1},
	{VK_F2,			VK_NAME_F2},
	{VK_F3,			VK_NAME_F3},
	{VK_F4,			VK_NAME_F4},
	{VK_F5,			VK_NAME_F5},
	{VK_F6,			VK_NAME_F6},
	{VK_F7,			VK_NAME_F7},
	{VK_F8,			VK_NAME_F8},
	{VK_F9,			VK_NAME_F9},
	{VK_F10,		VK_NAME_F10},
	{VK_F11,		VK_NAME_F11},
	{VK_F12,		VK_NAME_F12},
	{VK_BACKSLASH,	VK_NAME_BACKSLASH},
	{VK_EMPHASISE,	VK_NAME_EMPHASISE},
	{VK_MINUS,		VK_NAME_MINUS},
	{VK_EQUAL,		VK_NAME_EQUAL},
	{VK_SQRQUOTE_STR,VK_NAME_SQRQUOTE_STR},
	{VK_SQRQUOTE_END,VK_NAME_SQRQUOTE_END},
	{VK_SEMICOLON,	VK_NAME_SEMICOLON},
	{VK_SINGLEQUOTE,VK_NAME_SINGLEQUOTE},
	{VK_COMMA,		VK_NAME_COMMA},
	{VK_PERIOD,		VK_NAME_PERIOD},
	{VK_SLASH,		VK_NAME_SLASH},
	{65,VK_NAME_A},
	{66,VK_NAME_B},
	{67,VK_NAME_C},
	{68,VK_NAME_D},
	{69,VK_NAME_E},
	{70,VK_NAME_F},
	{71,VK_NAME_G},
	{72,VK_NAME_H},
	{73,VK_NAME_I},
	{74,VK_NAME_J},
	{75,VK_NAME_K},
	{76,VK_NAME_L},
	{77,VK_NAME_M},
	{78,VK_NAME_N},
	{79,VK_NAME_O},
	{80,VK_NAME_P},
	{81,VK_NAME_Q},
	{82,VK_NAME_R},
	{83,VK_NAME_S},
	{84,VK_NAME_T},
	{85,VK_NAME_U},
	{86,VK_NAME_V},
	{87,VK_NAME_W},
	{88,VK_NAME_X},
	{89,VK_NAME_Y},
	{90,VK_NAME_Z},
	{48,VK_NAME_ZERO},
	{49,VK_NAME_ONE},
	{50,VK_NAME_TWO},
	{51,VK_NAME_THREE},
	{52,VK_NAME_FOUR},
	{53,VK_NAME_FIVE},
	{54,VK_NAME_SIX},
	{55,VK_NAME_SEVEN},
	{56,VK_NAME_EIGHT},
	{57,VK_NAME_NINE},
	{180,VK_NAME_MEDIA_EMAIL},
	{181,VK_NAME_MEDIA_PLAYER},
	{177,VK_NAME_MEDIA_PREVIOUS},
	{176,VK_NAME_MEDIA_NEXT},
	{175,VK_NAME_MEDIA_VOLUP},
	{174,VK_NAME_MEDIA_VOLDOWN},
	{179,VK_NAME_MEDIA_PLAY},
	{178,VK_NAME_MEDIA_PAUSE},
	{173,VK_NAME_MEDIA_MUTE},
	{170,VK_NAME_MEDIA_SEARCH},
	{171,VK_NAME_MEDIA_FAVORITE},
	{172,VK_NAME_MEDIA_HOMEPAGE},
};


BOOL IsWinKey(DWORD keyCode)
{
	switch(keyCode)
	{	
	case VK_LWIN:
	case VK_RWIN:
		return TRUE;
	}
	return FALSE;
}

// Key that should not pass
// F1 - F12, WIN
BOOL IsKeyNoPass(DWORD keyCode)
{
	// kill trigger key if trigger key is F1 - F12
	if (keyCode >= VK_F1 && keyCode <= VK_F12) return TRUE;
	if (keyCode == VK_EMPHASISE) return TRUE; //if [`] with chicken
	if (IsWinKey(keyCode)) return TRUE;
	return FALSE;
}

//Return value
// 0: key is allow, 
// 1: this is pass through key (include modifier)
BOOL IsKeyNotAllowed(DWORD keyCode)
{
	switch (keyCode)
	{
		// ESC, F7 are allow
		// ESC for escape from assigning
		// F7 for hotkey tool on/off
	case VK_NUMPAD1:
	case VK_NUMPAD2:
	case VK_NUMPAD4:
	case VK_NUMPAD5:
	case VK_NUMPAD7:
	case VK_NUMPAD8:
	case VK_LSHIFT:
	case VK_RSHIFT:
	case VK_RCONTROL:
	case VK_LCONTROL:
	case VK_LALT:
	case VK_RALT:
	case VK_BACKSPC:
	case VK_ENTER:
	case VK_SPACE:  //this is special button, work on this one later
		return TRUE;
	}
	return FALSE;
}


//keyStrokeCode is know as virtual key code or vKcode
LPSTR GetKbbtName(DWORD kbBtn)
{
	for (int i=0;i<=APPLY_KEYBOARD;i++)
	{
		if (keysNames[i].dwKeyCode == kbBtn)
			return keysNames[i].chrsKeyName;
	}
	return VK_NAME_SPECIAL;
};

LPSTR GetMsbtName(MouseData msBtn)
{
	switch (msBtn)
	{
	case MOUSEDATA_NODATA:
		return TEXT_MOUSEDATA_NODATA;
	case MOUSEDATA_MOUSE3:
		return TEXT_MOUSEDATA_MOUSE3;
	case MOUSEDATA_MOUSE4:
		return TEXT_MOUSEDATA_MOUSE4;
	case MOUSEDATA_MOUSE5:
		return TEXT_MOUSEDATA_MOUSE5;
	case MOUSEDATA_WHEELUP:
		return TEXT_MOUSEDATA_WHEELUP;
	case MOUSEDATA_WHEELDOWN:
		return TEXT_MOUSEDATA_WHEELDOWN;
	case MOUSEDATA_UNKNOW:
		return TEXT_MOUSEDATA_NODATA;
	}
	return TEXT_MOUSEDATA_NODATA;
};

MouseData getMouseData(DWORD data)
{
	switch (_GET_MOUSEDATA(data))
	{
	case WD_XBUTTON_BACK:
		return MOUSEDATA_MOUSE4;
	case WD_XBUTTON_NEXT:
		return MOUSEDATA_MOUSE5;
	case WD_WHEELDATA_DOWN:
		return MOUSEDATA_WHEELDOWN;
	case WD_WHEELDATA_UP:
		return MOUSEDATA_WHEELUP;
	}
	return MOUSEDATA_NODATA;
}


#endif // endif _virtual