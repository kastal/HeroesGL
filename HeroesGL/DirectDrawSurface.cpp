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

	this->pixelBuffer = index ? NULL : (DWORD*)MemoryAlloc(RES_WIDTH * RES_HEIGHT * sizeof(DWORD));
	this->clipsList = !this->index ? (UpdateRect*)MemoryAlloc(STENCIL_COUNT * sizeof(UpdateRect)) : NULL;
	this->endClip = this->clipsList + (!this->index ? STENCIL_COUNT : 0);
	this->poinetrClip = this->currentClip = this->clipsList;

	this->attachedPallete = NULL;
	this->attachedClipper = NULL;
}

DirectDrawSurface::~DirectDrawSurface()
{
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

	if (this->pixelBuffer)
		MemoryFree(this->pixelBuffer);

	if (this->clipsList)
		MemoryFree(this->clipsList);
}

ULONG DirectDrawSurface::Release()
{
	if (this->ddraw->attachedSurface == this)
		this->ddraw->attachedSurface = NULL;

	delete this;
	return 0;
}

HRESULT DirectDrawSurface::Blt(LPRECT lpDestRect, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
{
	if (lpSrcRect->right == lpSrcRect->left || lpSrcRect->bottom == lpSrcRect->top)
		return DD_OK;

	if (this->attachedClipper)
		ScreenToClient(this->attachedClipper->hWnd, (POINT*)&lpDestRect->left);

	RECT rect;
	GetClientRect(this->ddraw->hWnd, &rect);

	DWORD left = lpDestRect->left * RES_WIDTH / (rect.right - rect.left);
	DWORD top = lpDestRect->top * RES_HEIGHT / (rect.bottom - rect.top);
	DWORD width = lpSrcRect->right - lpSrcRect->left;
	DWORD height = lpSrcRect->bottom - lpSrcRect->top;

	DirectDrawSurface* surface = (DirectDrawSurface*)lpDDSrcSurface;

	BYTE* source = surface->indexBuffer + lpSrcRect->top * RES_WIDTH + lpSrcRect->left;
	BYTE* destination = this->indexBuffer + top * RES_WIDTH + left;
	DWORD* pixels = this->pixelBuffer + top * RES_WIDTH + left;

	DWORD copyHeight = height;
	do
	{
		BYTE* src = source;
		source += RES_WIDTH;

		BYTE* dest = destination;
		destination += RES_WIDTH;

		DWORD* pix = pixels;
		pixels += RES_WIDTH;

		DWORD copyWidth = width;
		do
		{
			*pix++ = *(DWORD*)&this->attachedPallete->entries[*src];
			*dest++ = *src++;
		} while (--copyWidth);
	} while (--copyHeight);

	this->currentClip->rect.left = left;
	this->currentClip->rect.top = top;
	this->currentClip->rect.right = left + width;
	this->currentClip->rect.bottom = top + height;
	this->currentClip->isActive = TRUE;

	if (width == this->ddraw->width &&
		height == this->ddraw->height)
		this->poinetrClip = this->currentClip;
	else
	{
		UpdateRect* oldClip = surface->poinetrClip;
		UpdateRect* currClip = surface->currentClip;

		while (oldClip != currClip)
		{
			if (oldClip->isActive)
			{
				if (oldClip->rect.left >= currClip->rect.left &&
					oldClip->rect.top >= currClip->rect.top &&
					oldClip->rect.right <= currClip->rect.right &&
					oldClip->rect.bottom <= currClip->rect.bottom)
					oldClip->isActive = FALSE;
				else if (currClip->rect.left >= oldClip->rect.left &&
					currClip->rect.top >= oldClip->rect.top &&
					currClip->rect.right <= oldClip->rect.right &&
					currClip->rect.bottom <= oldClip->rect.bottom)
				{
					currClip->isActive = FALSE;
					break;
				}
			}

			if (++oldClip == surface->endClip)
				oldClip = surface->clipsList;
		}
	}

	this->currentClip = this->currentClip + 1 != this->endClip ? this->currentClip + 1 : this->clipsList;

	SetEvent(this->ddraw->hDrawEvent);
	Sleep(0);

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