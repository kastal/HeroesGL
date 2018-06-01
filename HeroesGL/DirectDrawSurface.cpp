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

#include "stdafx.h"
#include "GLib.h"
#include "DirectDrawSurface.h"
#include "DirectDrawClipper.h"
#include "DirectDraw.h"

#pragma region Not Implemented
HRESULT DirectDrawSurface::QueryInterface(REFIID riid, LPVOID* ppvObj) { return DD_OK; }
ULONG DirectDrawSurface::AddRef() { return 0; }
HRESULT DirectDrawSurface::AddAttachedSurface(LPDIRECTDRAWSURFACE) { return DD_OK; }
HRESULT DirectDrawSurface::AddOverlayDirtyRect(LPRECT) { return DD_OK; }
HRESULT DirectDrawSurface::BltBatch(LPDDBLTBATCH, DWORD, DWORD) { return DD_OK; }
HRESULT DirectDrawSurface::BltFast(DWORD, DWORD, LPDIRECTDRAWSURFACE, LPRECT, DWORD) { return DD_OK; }
HRESULT DirectDrawSurface::DeleteAttachedSurface(DWORD, LPDIRECTDRAWSURFACE) { return DD_OK; }
HRESULT DirectDrawSurface::EnumAttachedSurfaces(LPVOID, LPDDENUMSURFACESCALLBACK) { return DD_OK; }
HRESULT DirectDrawSurface::EnumOverlayZOrders(DWORD, LPVOID, LPDDENUMSURFACESCALLBACK) { return DD_OK; }
HRESULT DirectDrawSurface::Flip(LPDIRECTDRAWSURFACE, DWORD) { return DD_OK; }
HRESULT DirectDrawSurface::GetAttachedSurface(LPDDSCAPS, LPDIRECTDRAWSURFACE *) { return DD_OK; }
HRESULT DirectDrawSurface::GetBltStatus(DWORD) { return DD_OK; }
HRESULT DirectDrawSurface::GetCaps(LPDDSCAPS) { return DD_OK; }
HRESULT DirectDrawSurface::GetClipper(LPDIRECTDRAWCLIPPER *) { return DD_OK; }
HRESULT DirectDrawSurface::GetColorKey(DWORD, LPDDCOLORKEY) { return DD_OK; }
HRESULT DirectDrawSurface::GetFlipStatus(DWORD) { return DD_OK; }
HRESULT DirectDrawSurface::GetOverlayPosition(LPLONG, LPLONG) { return DD_OK; }
HRESULT DirectDrawSurface::GetPalette(LPDIRECTDRAWPALETTE *) { return DD_OK; }
HRESULT DirectDrawSurface::GetSurfaceDesc(LPDDSURFACEDESC) { return DD_OK; }
HRESULT DirectDrawSurface::Initialize(LPDIRECTDRAW, LPDDSURFACEDESC) { return DD_OK; }
HRESULT DirectDrawSurface::IsLost() { return DD_OK; }
HRESULT DirectDrawSurface::ReleaseDC(HDC) { return DD_OK; }
HRESULT DirectDrawSurface::Restore() { return DD_OK; }
HRESULT DirectDrawSurface::SetColorKey(DWORD, LPDDCOLORKEY) { return DD_OK; }
HRESULT DirectDrawSurface::SetOverlayPosition(LONG, LONG) { return DD_OK; }
HRESULT DirectDrawSurface::UpdateOverlay(LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDOVERLAYFX) { return DD_OK; }
HRESULT DirectDrawSurface::UpdateOverlayDisplay(DWORD) { return DD_OK; }
HRESULT DirectDrawSurface::UpdateOverlayZOrder(DWORD, LPDIRECTDRAWSURFACE) { return DD_OK; }
HRESULT DirectDrawSurface::GetPixelFormat(LPDDPIXELFORMAT) { return DD_OK; }
HRESULT DirectDrawSurface::GetDC(HDC* hDc) { return DD_OK; }
HRESULT DirectDrawSurface::Unlock(LPVOID) { return DD_OK; }
#pragma endregion

DirectDrawSurface::DirectDrawSurface(DirectDraw* lpDD, DWORD index)
{
	this->ddraw = lpDD;
	this->last = lpDD->surfaceEntries;
	this->index = index;
	this->pixelBuffer = index ? NULL : (DWORD*)malloc(640 * 480 * sizeof(DWORD));

	this->attachedPallete = NULL;
	this->attachedClipper = NULL;
}

DirectDrawSurface::~DirectDrawSurface()
{
	if (this->pixelBuffer)
		free(this->pixelBuffer);
}

ULONG DirectDrawSurface::Release()
{
	if (this->ddraw->attachedSurface == this)
	{
		this->ddraw->RenderStop();
		this->ddraw->attachedSurface = NULL;
	}

	if (this->ddraw->surfaceEntries == this)
		this->ddraw->surfaceEntries = NULL;
	else
	{
		DirectDrawSurface* entry = this->ddraw->surfaceEntries;
		while (entry)
		{
			if (entry->last == this)
			{
				entry->last = this->last;
				break;
			}

			entry = entry->last;
		}
	}

	delete this;
	return 0;
}

HRESULT DirectDrawSurface::Blt(LPRECT lpDestRect, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
{
	RECT rect;
	GetClientRect(this->ddraw->hWnd, &rect);

	POINT p = { lpDestRect->left, lpDestRect->top };
	ScreenToClient(this->ddraw->hWnd, &p);

	DWORD px = DWORD((FLOAT)640 * (FLOAT)p.x / (FLOAT)(rect.right - rect.left));
	DWORD py = DWORD((FLOAT)480 * (FLOAT)p.y / (FLOAT)(rect.bottom - rect.top));

	DirectDrawSurface* surface = (DirectDrawSurface*)lpDDSrcSurface;

	BYTE* source = surface->indexBuffer + lpSrcRect->top * 640 + lpSrcRect->left;
	BYTE* destination = this->indexBuffer + py * 640 + px;
	DWORD* pixels = this->pixelBuffer + py * 640 + px;

	DWORD width = lpSrcRect->right - lpSrcRect->left;
	DWORD height = lpSrcRect->bottom - lpSrcRect->top;

	do
	{
		BYTE* src = source;
		source += 640;

		BYTE* dest = destination;
		destination += 640;

		DWORD* pix = pixels;
		pixels += 640;
			
		DWORD count = width;
		do
		{
			*pix++ = *(DWORD*)&this->attachedPallete->entries[*src];
			*dest++ = *src++;
		}
		while (--count);
	} while (--height);
	
	SetEvent(this->ddraw->hDrawEvent);

	return DD_OK;
}

HRESULT DirectDrawSurface::Lock(LPRECT lpDestRect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent)
{
	lpDDSurfaceDesc->lpSurface = this->indexBuffer;
	return DD_OK;
}

HRESULT DirectDrawSurface::SetClipper(LPDIRECTDRAWCLIPPER lpDDClipper)
{
	this->attachedClipper = (DirectDrawClipper*)lpDDClipper;
	return DD_OK;
}

HRESULT DirectDrawSurface::SetPalette(LPDIRECTDRAWPALETTE lpDDPalette)
{
	this->attachedPallete = (DirectDrawPalette*)lpDDPalette;
	return DD_OK;
}