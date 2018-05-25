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

DirectDrawSurface::DirectDrawSurface(LPDIRECTDRAW lpDD, DWORD index)
{
	this->ddraw = lpDD;
	this->index = index;
	this->last = (DirectDrawSurface*)((DirectDraw*)this->ddraw)->surfaceEntries;

	this->attachedPallete = NULL;
	this->attachedClipper = NULL;
}

ULONG DirectDrawSurface::Release()
{
	DirectDraw* mdraw = (DirectDraw*)this->ddraw;
	if (mdraw->attachedSurface == this)
	{
		mdraw->isFinish = TRUE;
		SetEvent(mdraw->hDrawEvent);
		WaitForSingleObject(mdraw->hDrawThread, INFINITE);
		mdraw->hDrawThread = NULL;
		mdraw->attachedSurface = NULL;
	}

	if (mdraw->surfaceEntries == this)
		mdraw->surfaceEntries = NULL;
	else
	{
		DirectDrawSurface* entry = (DirectDrawSurface*)mdraw->surfaceEntries;
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
	DirectDraw* mdraw = (DirectDraw*)this->ddraw;
	RECT rect;
	GetClientRect(mdraw->hWnd, &rect);

	POINT p = { lpDestRect->left, lpDestRect->top };
	ScreenToClient(mdraw->hWnd, &p);

	DWORD px = (FLOAT)640 * (FLOAT)p.x / (FLOAT)(rect.right - rect.left);
	DWORD py = (FLOAT)480 * (FLOAT)p.y / (FLOAT)(rect.bottom - rect.top);

	DirectDrawSurface* surface = (DirectDrawSurface*)lpDDSrcSurface;

	DWORD width = lpSrcRect->right - lpSrcRect->left;
	DWORD height = lpSrcRect->bottom - lpSrcRect->top;

	if (width == 640 && height == 480)
	{
		DWORD* src = (DWORD*)surface->indexBuffer;
		DWORD* dest = (DWORD*)this->indexBuffer;
		DWORD count = 640 * 480 / 4;
		do
		{
			*dest++ = *src++;
		} while (--count);
	}
	else if (!(width % 4))
	{
		for (DWORD j = lpSrcRect->top, y = py; j < lpSrcRect->bottom; ++j, ++y)
		{
			DWORD* src = (DWORD*)(surface->indexBuffer + j * 640 + lpSrcRect->left);
			DWORD* dest = (DWORD*)(this->indexBuffer + y * 640 + px);
			for (DWORD i = lpSrcRect->left; i < lpSrcRect->right; i += sizeof(DWORD))
				*dest++ = *src++;
		}
	}
	else if (!(width % 2))
	{
		for (DWORD j = lpSrcRect->top, y = py; j < lpSrcRect->bottom; ++j, ++y)
		{
			WORD* src = (WORD*)(surface->indexBuffer + j * 640 + lpSrcRect->left);
			WORD* dest = (WORD*)(this->indexBuffer + y * 640 + px);
			for (DWORD i = lpSrcRect->left; i < lpSrcRect->right; i += sizeof(WORD))
				*dest++ = *src++;
		}
	}
	else
	{
		for (DWORD j = lpSrcRect->top, y = py; j < lpSrcRect->bottom; ++j, ++y)
		{
			BYTE* src = surface->indexBuffer + j * 640 + lpSrcRect->left;
			BYTE* dest = this->indexBuffer + y * 640 + px;
			for (DWORD i = lpSrcRect->left; i < lpSrcRect->right; ++i)
				*dest++ = *src++;
		}
	}

	SetEvent(mdraw->hDrawEvent);

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