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
HRESULT DirectDrawSurface::Initialize(LPDIRECTDRAW, LPDDSURFACEDESC) { return DD_OK; }
HRESULT DirectDrawSurface::IsLost() { return DD_OK; }
HRESULT DirectDrawSurface::ReleaseDC(HDC) { return DD_OK; }
HRESULT DirectDrawSurface::Restore() { return DD_OK; }
HRESULT DirectDrawSurface::SetOverlayPosition(LONG, LONG) { return DD_OK; }
HRESULT DirectDrawSurface::UpdateOverlay(LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDOVERLAYFX) { return DD_OK; }
HRESULT DirectDrawSurface::UpdateOverlayDisplay(DWORD) { return DD_OK; }
HRESULT DirectDrawSurface::UpdateOverlayZOrder(DWORD, LPDIRECTDRAWSURFACE) { return DD_OK; }
HRESULT DirectDrawSurface::GetDC(HDC* hDc) { return DD_OK; }
HRESULT DirectDrawSurface::Unlock(LPVOID) { return DD_OK; }
HRESULT DirectDrawSurface::SetPalette(LPDIRECTDRAWPALETTE) { return DD_OK; }
#pragma endregion

DirectDrawSurface::DirectDrawSurface(DirectDraw* lpDD, DWORD index)
{
	this->ddraw = lpDD;
	this->last = lpDD->surfaceEntries;

	this->attachedClipper = NULL;

	this->index = index;
	this->indexBuffer = NULL;

	this->width = 0;
	this->height = 0;
	this->isDouble = FALSE;
	this->isSizeChanged = TRUE;

	this->colorKey = 0;

	this->clipsList = NULL;
}

DirectDrawSurface::~DirectDrawSurface()
{
	this->ReleaseBuffer();
}

VOID DirectDrawSurface::ReleaseBuffer()
{
	if (this->indexBuffer)
		MemoryFree(this->indexBuffer);

	if (this->clipsList)
		MemoryFree(this->clipsList);
}

VOID DirectDrawSurface::CreateBuffer(DWORD width, DWORD height)
{
	this->ReleaseBuffer();
	this->width = width;
	this->height = height;
	this->indexBuffer = (WORD*)MemoryAlloc(width * height * sizeof(WORD));

	this->clipsList = !this->index ? (RECT*)MemoryAlloc(STENCIL_COUNT * sizeof(RECT)) : NULL;
	this->endClip = this->clipsList + (!this->index ? STENCIL_COUNT : 0);
	this->poinetrClip = this->currentClip = this->clipsList;
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

HRESULT DirectDrawSurface::GetPixelFormat(LPDDPIXELFORMAT lpDDPixelFormat)
{
	lpDDPixelFormat->dwRBitMask = 0xF800;
	lpDDPixelFormat->dwGBitMask = 0x07E0;
	lpDDPixelFormat->dwBBitMask = 0x001F;
	return DD_OK;
}

HRESULT DirectDrawSurface::SetColorKey(DWORD dwFlags, LPDDCOLORKEY lpDDColorKey)
{
	this->colorKey = LOWORD(lpDDColorKey->dwColorSpaceLowValue);
	return DD_OK;
}

HRESULT DirectDrawSurface::Lock(LPRECT lpDestRect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent)
{
	ddraw->RenderStart();
	lpDDSurfaceDesc->dwWidth = this->width;
	lpDDSurfaceDesc->dwHeight = this->height;
	lpDDSurfaceDesc->lPitch = this->width * 2;
	lpDDSurfaceDesc->lpSurface = this->indexBuffer;
	return DD_OK;
}

HRESULT DirectDrawSurface::SetClipper(LPDIRECTDRAWCLIPPER lpDDClipper)
{
	this->attachedClipper = (DirectDrawClipper*)lpDDClipper;
	return DD_OK;
}

HRESULT DirectDrawSurface::GetSurfaceDesc(LPDDSURFACEDESC lpDDSurfaceDesc)
{
	lpDDSurfaceDesc->dwWidth = this->width;
	lpDDSurfaceDesc->dwHeight = this->height;
	return DD_OK;
}

HRESULT DirectDrawSurface::Blt(LPRECT lpDestRect, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
{
	if (dwFlags & DDBLT_COLORFILL)
	{
		DWORD count = this->width * this->height;
		if (count & 1)
		{
			WORD* dest = this->indexBuffer;
			WORD color = LOWORD(lpDDBltFx->dwFillColor);
			do
				*dest++ = color;
			while (--count);
		}
		else
		{
			count >>= 1;
			DWORD* dest = (DWORD*)this->indexBuffer;
			DWORD color = lpDDBltFx->dwFillColor | (lpDDBltFx->dwFillColor << 16);
			do
				*dest++ = color;
			while (--count);
		}
	}
	else
	{
		BOOL doubled = (lpDestRect->right - lpDestRect->left) == ((lpSrcRect->right - lpSrcRect->left) << 1) &&
			(lpDestRect->bottom - lpDestRect->top) == ((lpSrcRect->bottom - lpSrcRect->top) << 1);

		DirectDrawSurface* surface = (DirectDrawSurface*)lpDDSrcSurface;

		DWORD sWidth;
		if (surface->attachedClipper)
		{
			RECT clip;
			GetClientRect(surface->attachedClipper->hWnd, &clip);
			ClientToScreen(surface->attachedClipper->hWnd, (POINT*)&clip.left);

			lpSrcRect->left -= clip.left;
			lpSrcRect->top -= clip.top;
			lpSrcRect->right -= clip.left;
			lpSrcRect->bottom -= clip.top;

			sWidth = RES_WIDTH;
		}
		else
			sWidth = surface->width;

		DWORD dWidth;
		if (this->attachedClipper)
		{
			RECT clip;
			GetClientRect(this->attachedClipper->hWnd, &clip);
			ClientToScreen(this->attachedClipper->hWnd, (POINT*)&clip.left);

			lpDestRect->left -= clip.left;
			lpDestRect->top -= clip.top;
			lpDestRect->right -= clip.left;
			lpDestRect->bottom -= clip.top;

			dWidth = RES_WIDTH;
		}
		else
			dWidth = this->width;

		if (doubled)
			dWidth >>= 1;

		WORD* source = surface->indexBuffer + lpSrcRect->top * sWidth + lpSrcRect->left;
		WORD* destination = this->indexBuffer + lpDestRect->top * dWidth + lpDestRect->left;

		INT width = lpSrcRect->right - lpSrcRect->left;
		INT height = lpSrcRect->bottom - lpSrcRect->top;

		if (surface->colorKey)
		{
			do
			{
				WORD* src = source;
				WORD* dest = destination;
				source += sWidth;
				destination += dWidth;

				DWORD count = width;
				do
				{
					if (*src != surface->colorKey)
						*dest = *src;
					++src;
					++dest;
				} while (--count);
			} while (--height);
		}
		else
		{
			if (width & 1)
			{
				do
				{
					WORD* src = source;
					WORD* dest = destination;
					source += sWidth;
					destination += dWidth;

					DWORD count = width;
					do
						*dest++ = *src++;
					while (--count);
				} while (--height);
			}
			else
			{
				width >>= 1;
				do
				{
					DWORD* src = (DWORD*)source;
					DWORD* dest = (DWORD*)destination;
					source += sWidth;
					destination += dWidth;

					DWORD count = width;
					do
						*dest++ = *src++;
					while (--count);
				} while (--height);
			}
		}

		if (this->isDouble != doubled)
		{
			this->isDouble = doubled;
			this->isSizeChanged = TRUE;
		}

		if (!this->index)
		{
			*this->currentClip = *lpDestRect;

			if (lpDestRect->right - lpDestRect->left == this->ddraw->width &&
				lpDestRect->bottom - lpDestRect->top == this->ddraw->height)
				this->poinetrClip = this->currentClip;

			this->currentClip = this->currentClip + 1 != this->endClip ? this->currentClip + 1 : this->clipsList;
		}

		if (this->ddraw->attachedSurface == this)
			SetEvent(this->ddraw->hDrawEvent);
	}

	return DD_OK;
}
