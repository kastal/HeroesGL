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
#include "DirectDrawPalette.h"
#include "DirectDrawSurface.h"
#include "DirectDraw.h"
#include "Glib.h"

#pragma region Not Implemented
HRESULT DirectDrawPalette::QueryInterface(REFIID riid, LPVOID * ppvObj) { return DD_OK; }
ULONG DirectDrawPalette::AddRef() { return 0; }
HRESULT DirectDrawPalette::GetCaps(LPDWORD) { return DD_OK; }
HRESULT DirectDrawPalette::GetEntries(DWORD, DWORD, DWORD, LPPALETTEENTRY) { return DD_OK; }
HRESULT DirectDrawPalette::Initialize(LPDIRECTDRAW, DWORD, LPPALETTEENTRY) { return DD_OK; }
#pragma endregion

DirectDrawPalette::DirectDrawPalette(DirectDraw* lpDD)
{
	this->ddraw = lpDD;
	this->last = lpDD->paletteEntries;
	this->isChanged = FALSE;
}

ULONG DirectDrawPalette::Release()
{
	if (this->ddraw->paletteEntries == this)
		this->ddraw->paletteEntries = NULL;
	else
	{
		DirectDrawPalette* entry = this->ddraw->paletteEntries;
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

HRESULT DirectDrawPalette::SetEntries(DWORD dwFlags, DWORD dwStartingEntry, DWORD dwCount, LPPALETTEENTRY lpEntries)
{
	BOOL changed = FALSE;

	DWORD* src = (DWORD*)lpEntries;
	DWORD* dst = (DWORD*)this->entries + dwStartingEntry;
	do
	{
		if (*dst != *src)
		{
			changed = TRUE;
			*dst++ = *src++;
			--dwCount;
			break;
		}

		++dst;
		++src;
	} while (--dwCount);

	if (changed)
	{
		while (dwCount--) *dst++ = *src++;

		DirectDrawSurface* surfaceEntry = this->ddraw->surfaceEntries;
		while (surfaceEntry)
		{
			if (surfaceEntry->attachedPallete == this)
			{
				BYTE* idx = surfaceEntry->indexBuffer;
				DWORD* pix = surfaceEntry->pixelBuffer;
				DWORD count = RES_WIDTH * RES_HEIGHT;
				do
					*pix++ = *(DWORD*)&this->entries[*idx++];
				while (--count);
			}

			surfaceEntry = surfaceEntry->last;
		}

		this->isChanged = TRUE;
		SetEvent(this->ddraw->hDrawEvent);
	}

	return DD_OK;
}