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

class DirectDraw : IDirectDraw
{
public:
	VOID * operator new(size_t size) { return MemoryAlloc(size); };
	VOID operator delete(VOID *p) { MemoryFree(p); };

	DirectDraw* last;

	DirectDrawSurface* surfaceEntries;
	DirectDrawPalette* paletteEntries;
	DirectDrawClipper* clipperEntries;

	DirectDrawSurface* attachedSurface;

	HWND hWnd;
	HWND hDraw;
	HDC hDc;

	DWORD width;
	DWORD height;

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

	VOID RenderOld();
	VOID RenderNew();

	// Inherited via  IDirectDraw
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID * ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall Compact();
	HRESULT __stdcall CreateClipper(DWORD, LPDIRECTDRAWCLIPPER *, IUnknown *);
	HRESULT __stdcall CreatePalette(DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE *, IUnknown *);
	HRESULT __stdcall CreateSurface(LPDDSURFACEDESC, LPDIRECTDRAWSURFACE *, IUnknown *);
	HRESULT __stdcall DuplicateSurface(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE *);
	HRESULT __stdcall EnumDisplayModes(DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK);
	HRESULT __stdcall EnumSurfaces(DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMSURFACESCALLBACK);
	HRESULT __stdcall FlipToGDISurface();
	HRESULT __stdcall GetCaps(LPDDCAPS, LPDDCAPS);
	HRESULT __stdcall GetDisplayMode(LPDDSURFACEDESC);
	HRESULT __stdcall GetFourCCCodes(LPDWORD, LPDWORD);
	HRESULT __stdcall GetGDISurface(LPDIRECTDRAWSURFACE *);
	HRESULT __stdcall GetMonitorFrequency(LPDWORD);
	HRESULT __stdcall GetScanLine(LPDWORD);
	HRESULT __stdcall GetVerticalBlankStatus(LPBOOL);
	HRESULT __stdcall Initialize(GUID *);
	HRESULT __stdcall RestoreDisplayMode();
	HRESULT __stdcall SetCooperativeLevel(HWND, DWORD);
	HRESULT __stdcall SetDisplayMode(DWORD, DWORD, DWORD);
	HRESULT __stdcall WaitForVerticalBlank(DWORD, HANDLE);
};