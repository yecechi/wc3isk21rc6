//////////////////////////////////////////////////////////////////////////
// _ipicture.h
// 5-6-2009
// By Cee'z
//
// This is support JEPG loading for WIN32-App, using OLE.
//
// How to use :
// Declare global variable : 
//
//		//our global picture stuff, and will be use every time window repaint
//		LPPICTURE iPicObject;	
//
// Add these line in InitInstance or OnInitDialog...
//
//		DWORD dwStreamSize;
//		IStream* pStream = CreateStreamOnResource(MAKEINTRESOURCE(IDR_IMAGE_ID),_T("IMAGE_RESOURCE_TYPE"),dwStreamSize);
//		HRESULT hr = ::OleLoadPicture(pStream, dwStreamSize, FALSE, IID_IPicture, (LPVOID *)&iPicObject);
//
// Now we got the stuff ready to draw out to the window
// Add WM_PAINT message in CALLBACK DlgProc():
//		
//		PAINTSTRUCT ps;
//		HDC hdc;
//		RECT bitmapRc;
//
//		switch (message) 
//		{
//		..........
//		case WM_PAINT:	// each time dialog is paint, we draw the picture
//			bitmapRc = Rect(xTop, yTop, wide, height);	// drawing rect, where the picture will be draw
//			hdc = BeginPaint(hWnd, &ps);
//			iPictureOnPaint(hWnd, ps, hdc, iPicObject, bitmapRc);
//			EndPaint(hWnd, &ps);
//			break;
//		..........
//		}
//////////////////////////////////////////////////////////////////////////

#ifdef _IPICTURE
#else

#define _IPICTURE
#include <OleCtl.h>

#define HIMETRIC_INCH	2540
#define MAP_LOGHIM_TO_PIX(x,ppli)   ( ((ppli)*(x) + HIMETRIC_INCH/2) / HIMETRIC_INCH )

typedef struct CeezRect
{
	INT    left;
	INT    top;
	INT    wide;
	INT    height;
} CZRECT;

IStream * CreateStreamOnResource(LPCTSTR lpName, LPCTSTR lpType, DWORD &dwStreamSize)
{
	// initialize return value
	IStream * ipStream = NULL;

	// find the resource
	HRSRC hrsrc = FindResource(NULL, lpName, lpType);
	if (hrsrc == NULL)
		goto Return;

	// load the resource
	DWORD dwResourceSize = SizeofResource(NULL, hrsrc);
	HGLOBAL hglbImage = LoadResource(NULL, hrsrc);
	if (hglbImage == NULL)
		goto Return;

	// lock the resource, getting a pointer to its data
	LPVOID pvSourceResourceData = LockResource(hglbImage);
	if (pvSourceResourceData == NULL)		
		goto Return;

	// allocate memory to hold the resource data
	HGLOBAL hgblResourceData = GlobalAlloc(GMEM_MOVEABLE, dwResourceSize);
	if (hgblResourceData == NULL)
		goto Return;

	// get a pointer to the allocated memory
	LPVOID pvResourceData = GlobalLock(hgblResourceData);
	if (pvResourceData == NULL)
		goto FreeData;

	// copy the data from the resource to the new memory block
	CopyMemory(pvResourceData, pvSourceResourceData, dwResourceSize);
	GlobalUnlock(hgblResourceData);

	// create a stream on the HGLOBAL containing the data
	if (SUCCEEDED(CreateStreamOnHGlobal(hgblResourceData, TRUE, &ipStream)))
	{
		dwStreamSize = dwResourceSize;
		goto Return;
	}

FreeData:
	// couldn't create stream; free the memory
	GlobalFree(hgblResourceData);

Return:
	// no need to unlock or free the resource
	return ipStream;
}

BOOL iPictureOnPaint(HWND hWnd, PAINTSTRUCT ps,HDC hdc,LPPICTURE _lpPicture,RECT rc)
{
	
	if (_lpPicture)
	{
		// get width and height of picture
		long hmWidth, hmHeight;
		_lpPicture->get_Width(&hmWidth);
		_lpPicture->get_Height(&hmHeight);
		// convert HIMETRIC to pixels
		int nWidth	= MulDiv(hmWidth, GetDeviceCaps(hdc, LOGPIXELSX), HIMETRIC_INCH);
		int nHeight	= MulDiv(hmHeight, GetDeviceCaps(hdc, LOGPIXELSY), HIMETRIC_INCH);
		// display picture using IPicture::Render
		_lpPicture->Render(hdc, rc.left, rc.top, nWidth, nHeight, 0, hmHeight, hmWidth, -hmHeight, &rc);
		return TRUE;
	}
	return FALSE;
}

RECT Rect(INT xTop,INT yTop,INT wide,INT height)
{
	RECT ret;
	ret.top = yTop;
	ret.left = xTop;
	ret.bottom = height + yTop;
	ret.right = wide + xTop;
	return ret;	
}
#endif