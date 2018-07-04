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
#include "Hooks.h"
#include "GLib.h"

BOOL __stdcall DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		LoadMsvCRT();
		if (Hooks::Load())
		{
			LoadKernel32();
			hDllModule = hModule;

			ACTCTX actCtx;
			MemoryZero(&actCtx, sizeof(ACTCTX));
			actCtx.cbSize = sizeof(actCtx);
			actCtx.hModule = hDllModule;
			actCtx.lpResourceName = MAKEINTRESOURCE(2);
			actCtx.dwFlags = ACTCTX_FLAG_HMODULE_VALID | ACTCTX_FLAG_RESOURCE_NAME_VALID;

			if (CreateActCtxC)
				hActCtx = CreateActCtxC(&actCtx);
		}
		else
			LoadDDraw();

		break;

	case DLL_PROCESS_DETACH:
		if (hDllModule)
		{
			if (hActCtx && hActCtx != INVALID_HANDLE_VALUE)
				ReleaseActCtxC(hActCtx);

			GL::Free();
			ClipCursor(NULL);
		}

		break;

	default: break;
	}
	return TRUE;
}