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
#include "DirectDrawClipper.h"
#include "GLib.h"

class DirectDraw;

class DirectDrawSurface : IDirectDrawSurface7
{
public:
	DirectDrawSurface* last;
	DirectDraw* ddraw;
	DWORD index;
	DWORD width;
	DWORD height;

	DirectDrawClipper* attachedClipper;

	BYTE* indexBuffer;
	HBITMAP hBmp;
	HDC hDc;

	DirectDrawSurface(DirectDraw*, DWORD);
	~DirectDrawSurface();

	VOID CreateBuffer(DWORD, DWORD);
	VOID ReleaseBuffer();

	// Inherited via IDirectDrawSurface7
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID * ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall AddAttachedSurface(LPDIRECTDRAWSURFACE7);
	HRESULT __stdcall AddOverlayDirtyRect(LPRECT);
	HRESULT __stdcall Blt(LPRECT, LPDIRECTDRAWSURFACE7, LPRECT, DWORD, LPDDBLTFX);
	HRESULT __stdcall BltBatch(LPDDBLTBATCH, DWORD, DWORD);
	HRESULT __stdcall BltFast(DWORD, DWORD, LPDIRECTDRAWSURFACE7, LPRECT, DWORD);
	HRESULT __stdcall DeleteAttachedSurface(DWORD, LPDIRECTDRAWSURFACE7);
	HRESULT __stdcall EnumAttachedSurfaces(LPVOID, LPDDENUMSURFACESCALLBACK7);
	HRESULT __stdcall EnumOverlayZOrders(DWORD, LPVOID, LPDDENUMSURFACESCALLBACK7);
	HRESULT __stdcall Flip(LPDIRECTDRAWSURFACE7, DWORD);
	HRESULT __stdcall GetAttachedSurface(LPDDSCAPS2, LPDIRECTDRAWSURFACE7 *);
	HRESULT __stdcall GetBltStatus(DWORD);
	HRESULT __stdcall GetCaps(LPDDSCAPS2);
	HRESULT __stdcall GetClipper(LPDIRECTDRAWCLIPPER *);
	HRESULT __stdcall GetColorKey(DWORD, LPDDCOLORKEY);
	HRESULT __stdcall GetDC(HDC *);
	HRESULT __stdcall GetFlipStatus(DWORD);
	HRESULT __stdcall GetOverlayPosition(LPLONG, LPLONG);
	HRESULT __stdcall GetPalette(LPDIRECTDRAWPALETTE *);
	HRESULT __stdcall GetPixelFormat(LPDDPIXELFORMAT);
	HRESULT __stdcall GetSurfaceDesc(LPDDSURFACEDESC2);
	HRESULT __stdcall Initialize(LPDIRECTDRAW, LPDDSURFACEDESC2);
	HRESULT __stdcall IsLost();
	HRESULT __stdcall Lock(LPRECT, LPDDSURFACEDESC2, DWORD, HANDLE);
	HRESULT __stdcall ReleaseDC(HDC);
	HRESULT __stdcall Restore();
	HRESULT __stdcall SetClipper(LPDIRECTDRAWCLIPPER);
	HRESULT __stdcall SetColorKey(DWORD, LPDDCOLORKEY);
	HRESULT __stdcall SetOverlayPosition(LONG, LONG);
	HRESULT __stdcall SetPalette(LPDIRECTDRAWPALETTE);
	HRESULT __stdcall Unlock(LPRECT);
	HRESULT __stdcall UpdateOverlay(LPRECT, LPDIRECTDRAWSURFACE7, LPRECT, DWORD, LPDDOVERLAYFX);
	HRESULT __stdcall UpdateOverlayDisplay(DWORD);
	HRESULT __stdcall UpdateOverlayZOrder(DWORD, LPDIRECTDRAWSURFACE7);
	HRESULT __stdcall GetDDInterface(LPVOID *);
	HRESULT __stdcall PageLock(DWORD);
	HRESULT __stdcall PageUnlock(DWORD);
	HRESULT __stdcall SetSurfaceDesc(LPDDSURFACEDESC2, DWORD);
	HRESULT __stdcall SetPrivateData(REFGUID, LPVOID, DWORD, DWORD);
	HRESULT __stdcall GetPrivateData(REFGUID, LPVOID, LPDWORD);
	HRESULT __stdcall FreePrivateData(REFGUID);
	HRESULT __stdcall GetUniquenessValue(LPDWORD);
	HRESULT __stdcall ChangeUniquenessValue();
	HRESULT __stdcall SetPriority(DWORD);
	HRESULT __stdcall GetPriority(LPDWORD);
	HRESULT __stdcall SetLOD(DWORD);
	HRESULT __stdcall GetLOD(LPDWORD);
};