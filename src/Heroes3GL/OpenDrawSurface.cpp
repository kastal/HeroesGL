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
#include "OpenDrawSurface.h"
#include "OpenDraw.h"

OpenDrawSurface::OpenDrawSurface(IDraw* lpDD, DWORD index)
{
	this->ddraw = lpDD;
	this->last = lpDD->surfaceEntries;
	lpDD->surfaceEntries = this;

	this->attachedClipper = NULL;

	this->index = index;
	this->indexBuffer = NULL;

	this->width = 0;
	this->height = 0;
	this->scale = 1.0f;
	this->isSizeChanged = TRUE;

	this->colorKey = 0;

	this->clipsList = NULL;
}

OpenDrawSurface::~OpenDrawSurface()
{
	this->ReleaseBuffer();
}

VOID OpenDrawSurface::ReleaseBuffer()
{
	if (this->indexBuffer)
		MemoryFree(this->indexBuffer);

	if (this->clipsList)
		MemoryFree(this->clipsList);
}

VOID OpenDrawSurface::CreateBuffer(DWORD width, DWORD height)
{
	this->ReleaseBuffer();
	this->width = width;
	this->height = height;
	this->indexBuffer = (WORD*)MemoryAlloc(width * height * sizeof(WORD));
	MemoryZero(this->indexBuffer, width * height * sizeof(WORD));

	this->clipsList = !this->index ? (UpdateRect*)MemoryAlloc(STENCIL_COUNT * sizeof(UpdateRect)) : NULL;
	this->endClip = this->clipsList + (!this->index ? STENCIL_COUNT : 0);
	this->poinetrClip = this->currentClip = this->clipsList;
}

ULONG __stdcall OpenDrawSurface::Release()
{
	if (((OpenDraw*)this->ddraw)->attachedSurface == this)
		((OpenDraw*)this->ddraw)->attachedSurface = NULL;

	delete this;
	return 0;
}

HRESULT __stdcall OpenDrawSurface::GetPixelFormat(LPDDPIXELFORMAT lpDDPixelFormat)
{
	lpDDPixelFormat->dwRBitMask = 0xF800;
	lpDDPixelFormat->dwGBitMask = 0x07E0;
	lpDDPixelFormat->dwBBitMask = 0x001F;
	return DD_OK;
}

HRESULT __stdcall OpenDrawSurface::SetColorKey(DWORD dwFlags, LPDDCOLORKEY lpDDColorKey)
{
	this->colorKey = LOWORD(lpDDColorKey->dwColorSpaceLowValue);
	return DD_OK;
}

HRESULT __stdcall OpenDrawSurface::Lock(LPRECT lpDestRect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent)
{
	lpDDSurfaceDesc->dwWidth = this->width;
	lpDDSurfaceDesc->dwHeight = this->height;
	lpDDSurfaceDesc->lPitch = this->width * 2;
	lpDDSurfaceDesc->lpSurface = this->indexBuffer;
	return DD_OK;
}

HRESULT __stdcall OpenDrawSurface::SetClipper(LPDIRECTDRAWCLIPPER lpDDClipper)
{
	this->attachedClipper = (OpenDrawClipper*)lpDDClipper;
	return DD_OK;
}

HRESULT __stdcall OpenDrawSurface::GetSurfaceDesc(LPDDSURFACEDESC lpDDSurfaceDesc)
{
	lpDDSurfaceDesc->dwWidth = this->width;
	lpDDSurfaceDesc->dwHeight = this->height;
	return DD_OK;
}

HRESULT __stdcall OpenDrawSurface::Blt(LPRECT lpDestRect, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
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
		FLOAT currScale = (FLOAT)(lpSrcRect->right - lpSrcRect->left) / (lpDestRect->right - lpDestRect->left);

		OpenDrawSurface* surface = (OpenDrawSurface*)lpDDSrcSurface;

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

			sWidth = ((OpenDraw*)this->ddraw)->width;
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

			dWidth = ((OpenDraw*)this->ddraw)->width;
		}
		else
			dWidth = this->width;

		if (currScale != 1.0f)
			dWidth = DWORD(currScale * dWidth);

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
			do
			{
				MemoryCopy(destination, source, width << 1);
				source += sWidth;
				destination += dWidth;
			} while (--height);
		}

		if (this->scale != currScale)
		{
			this->scale = currScale;
			this->isSizeChanged = TRUE;
		}

		if (!this->index)
		{
			this->currentClip->rect = *lpDestRect;
			this->currentClip->isActive = TRUE;

			if (lpDestRect->right - lpDestRect->left == ((OpenDraw*)this->ddraw)->width &&
				lpDestRect->bottom - lpDestRect->top == ((OpenDraw*)this->ddraw)->height)
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
		}

		if (((OpenDraw*)this->ddraw)->attachedSurface == this)
		{
			SetEvent(((OpenDraw*)this->ddraw)->hDrawEvent);
			Sleep(0);
		}
	}

	return DD_OK;
}
