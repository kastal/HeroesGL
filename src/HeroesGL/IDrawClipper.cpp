/*
	MIT License

	Copyright (c) 2019 Oleksiy Ryabchun

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
#include "IDrawClipper.h"
#include "IDraw.h"

IDrawClipper::~IDrawClipper()
{
	if (this->ddraw->clipperEntries == this)
		this->ddraw->clipperEntries = NULL;
	else
	{
		IDrawClipper* entry = this->ddraw->clipperEntries;
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
}

HRESULT __stdcall IDrawClipper::QueryInterface(REFIID, LPVOID*) { return DD_OK; }
ULONG __stdcall IDrawClipper::AddRef() { return 0; }
ULONG __stdcall IDrawClipper::Release() { return 0; }
HRESULT __stdcall IDrawClipper::GetClipList(LPRECT, LPRGNDATA, LPDWORD) { return DD_OK; }
HRESULT __stdcall IDrawClipper::GetHWnd(HWND*) { return DD_OK; }
HRESULT __stdcall IDrawClipper::Initialize(LPDIRECTDRAW, DWORD) { return DD_OK; }
HRESULT __stdcall IDrawClipper::IsClipListChanged(BOOL*) { return DD_OK; }
HRESULT __stdcall IDrawClipper::SetClipList(LPRGNDATA, DWORD) { return DD_OK; }
HRESULT __stdcall IDrawClipper::SetHWnd(DWORD, HWND) { return DD_OK; }