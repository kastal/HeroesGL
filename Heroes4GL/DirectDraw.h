/*
	MIT License

	Copyright (c) 2018 Oleksiy Ryabchun

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#pragma once
#include "ddraw.h"
#include "ExtraTypes.h"
#include "DirectDrawSurface.h"

class DirectDraw : IDirectDraw7
{
public:
	VOID * operator new(size_t size) { return MemoryAlloc(size); };
	VOID operator delete(VOID *p) { MemoryFree(p); };

	DirectDraw* last;

	DirectDrawSurface* surfaceEntries;
	DirectDrawClipper* clipperEntries;

	DirectDrawSurface* attachedSurface;

	HWND hWnd;
	HWND hDraw;
	HDC hDc;

	DisplayMode* mode;

	BOOL isFinish;

	HANDLE hDrawThread;
	HANDLE hDrawEvent;

	Viewport viewport;
	WindowState windowState;
	ImageFilter imageFilter;
	BOOL imageAspect;
	BOOL imageVSync;
	BOOL isStateChanged;

	DirectDraw(DirectDraw* lastObj);
	~DirectDraw();

	BOOL CheckView();
	VOID __fastcall ScaleMouse(LPPOINT p);
	VOID __fastcall CheckMenu(HMENU hMenu);

	VOID RenderStart();
	VOID RenderStop();

	VOID RenderOld(DWORD glMaxTexSize);
	VOID RenderNew();

	// Inherited via IDirectDraw7
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID * ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall Compact();
	HRESULT __stdcall CreateClipper(DWORD, LPDIRECTDRAWCLIPPER *, IUnknown *);
	HRESULT __stdcall CreatePalette(DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE *, IUnknown *);
	HRESULT __stdcall CreateSurface(LPDDSURFACEDESC2, LPDIRECTDRAWSURFACE7 *, IUnknown *);
	HRESULT __stdcall DuplicateSurface(LPDIRECTDRAWSURFACE7, LPDIRECTDRAWSURFACE7 *);
	HRESULT __stdcall EnumDisplayModes(DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMMODESCALLBACK2);
	HRESULT __stdcall EnumSurfaces(DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMSURFACESCALLBACK7);
	HRESULT __stdcall FlipToGDISurface();
	HRESULT __stdcall GetCaps(LPDDCAPS, LPDDCAPS);
	HRESULT __stdcall GetDisplayMode(LPDDSURFACEDESC2);
	HRESULT __stdcall GetFourCCCodes(LPDWORD, LPDWORD);
	HRESULT __stdcall GetGDISurface(LPDIRECTDRAWSURFACE7 *);
	HRESULT __stdcall GetMonitorFrequency(LPDWORD);
	HRESULT __stdcall GetScanLine(LPDWORD);
	HRESULT __stdcall GetVerticalBlankStatus(LPBOOL);
	HRESULT __stdcall Initialize(GUID *);
	HRESULT __stdcall RestoreDisplayMode();
	HRESULT __stdcall SetCooperativeLevel(HWND, DWORD);
	HRESULT __stdcall SetDisplayMode(DWORD, DWORD, DWORD, DWORD, DWORD);
	HRESULT __stdcall WaitForVerticalBlank(DWORD, HANDLE);
	HRESULT __stdcall GetAvailableVidMem(LPDDSCAPS2, LPDWORD, LPDWORD);
	HRESULT __stdcall GetSurfaceFromDC(HDC, LPDIRECTDRAWSURFACE7 *);
	HRESULT __stdcall RestoreAllSurfaces();
	HRESULT __stdcall TestCooperativeLevel();
	HRESULT __stdcall GetDeviceIdentifier(LPDDDEVICEIDENTIFIER2, DWORD);
	HRESULT __stdcall StartModeTest(LPSIZE, DWORD, DWORD);
	HRESULT __stdcall EvaluateMode(DWORD, DWORD *);
};