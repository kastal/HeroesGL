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
#include "Main.h"
#include "Config.h"
#include "Resource.h"

#define STYLE_FULL_OLD (WS_VISIBLE | WS_POPUP)
#define STYLE_FULL_NEW (WS_VISIBLE | WS_POPUP | WS_SYSMENU)

#define STYLE_WIN_OLD (WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
#define STYLE_WIN_NEW (WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX | WS_MAXIMIZEBOX)

AddressSpace* hookSpace;

AddressSpace addressArray[] = {
	// check 1 & 2,         renderNop,              cpu patch

	0x00844A4D, 0x00844B35, 0x00401D18, 0x90909090, 0x00842790, IDM_RUSSIAN, // Heroes IV - 1.0
	"Герои Меча и Магии IV", //"Heroes of Might and Magic IV",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic IV\\1.0",

	0x008C6411, 0x008C64F9, 0x00000000, 0x00000000, 0x008C3EF0, IDM_RUSSIAN, // Heroes IV - 2.2GS
	"Герои Меча и Магии IV: Грядущая Буря",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic IV\\1.0",

	0x008D38B1, 0x008D3999, 0x00000000, 0x00000000, 0x008D1390, IDM_RUSSIAN, // Heroes IV - 3.0WoW
	"Герои Меча и Магии IV: Вихри Войны",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic IV\\1.0",

	// =====================================================================================================================

	0x00844A4D, 0x00844B35, 0x00000000, 0x00000000, 0x00842790, IDM_ENGLISH, // Heroes IV - 1.0
	"Heroes of Might and Magic IV",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic IV\\1.0",

	0x0084C0FD, 0x0084C1E5, 0x00000000, 0x00000000, 0x00849DF0, IDM_ENGLISH, // Heroes IV - 1.2
	"Heroes of Might and Magic IV",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic IV\\1.0",

	0x008533C1, 0x008534A9, 0x00000000, 0x00000000, 0x00851010, IDM_ENGLISH, // Heroes IV - 1.3
	"Heroes of Might and Magic IV",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic IV\\1.0",

	0x008C5F51, 0x008C6039, 0x00000000, 0x00000000, 0x008C3A30, IDM_ENGLISH, // Heroes IV - 2.0
	"Heroes of Might and Magic IV",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic IV\\1.0",

	0x008C63F1, 0x008C64D9, 0x00000000, 0x00000000, 0x008C3ED0, IDM_ENGLISH, // Heroes IV - 2.2
	"Heroes of Might and Magic IV",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic IV\\1.0",

	0x008D3881, 0x008D3969, 0x00000000, 0x00000000, 0x008D1360, IDM_ENGLISH, // Heroes IV - 3.0
	"Heroes of Might and Magic IV",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic IV\\1.0",

	// ---------------------------------------------------------------------------------------------------------------------

	0x008C5F51, 0x008C6039, 0x00401448, 0x00961278, 0x008C3A30, IDM_ENGLISH, // Heroes IV - 2.0GS
	"Heroes of Might and Magic IV: The Gathering Storm",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic IV\\1.0",

	0x008C63F1, 0x008C64D9, 0x00401448, 0x00961278, 0x008C3ED0, IDM_ENGLISH, // Heroes IV - 2.2GS
	"Heroes of Might and Magic IV: The Gathering Storm",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic IV\\1.0",

	0x008D3881, 0x008D3969, 0x00401448, 0x00970278, 0x008D1360, IDM_ENGLISH, // Heroes IV - 3.0GS
	"Heroes of Might and Magic IV: The Gathering Storm",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic IV\\1.0",

	// ---------------------------------------------------------------------------------------------------------------------

	0x008D3881, 0x008D3969, 0x00401448, 0x00970200, 0x008D1360, IDM_ENGLISH, // Heroes IV - 3.0WoW
	"Heroes of Might and Magic IV: Winds of War",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic IV\\1.0",

	NULL
};

namespace Hooks
{
	BOOL __fastcall PatchHook(DWORD addr, VOID* hook)
	{
		DWORD old_prot;
		if (VirtualProtect((VOID*)addr, 5, PAGE_EXECUTE_READWRITE, &old_prot))
		{
			BYTE* jump = (BYTE*)addr;
			*jump = 0xE9;
			++jump;
			*(DWORD*)jump = (DWORD)hook - (DWORD)addr - 5;

			VirtualProtect((VOID*)addr, 5, old_prot, &old_prot);

			return TRUE;
		}
		return FALSE;
	}

	BOOL __fastcall PatchNop(DWORD addr, DWORD size)
	{
		DWORD old_prot;
		if (VirtualProtect((VOID*)addr, size, PAGE_EXECUTE_READWRITE, &old_prot))
		{
			memset((VOID*)addr, 0x90, size);
			VirtualProtect((VOID*)addr, size, old_prot, &old_prot);

			return TRUE;
		}
		return FALSE;
	}

	BOOL __fastcall PatchBlock(DWORD addr, VOID* block, DWORD size)
	{
		DWORD old_prot;
		if (VirtualProtect((VOID*)addr, size, PAGE_EXECUTE_READWRITE, &old_prot))
		{
			switch (size)
			{
			case 4:
				*(DWORD*)addr = *(DWORD*)block;
				break;
			case 2:
				*(WORD*)addr = *(WORD*)block;
				break;
			case 1:
				*(BYTE*)addr = *(BYTE*)block;
				break;
			default:
				memcpy((VOID*)addr, block, size);
				break;
			}

			VirtualProtect((VOID*)addr, size, old_prot, &old_prot);

			return TRUE;
		}
		return FALSE;
	}

	BOOL __fastcall ReadBlock(DWORD addr, VOID* block, DWORD size)
	{
		DWORD old_prot;
		if (VirtualProtect((VOID*)addr, size, PAGE_READONLY, &old_prot))
		{
			switch (size)
			{
			case 4:
				*(DWORD*)block = *(DWORD*)addr;
				break;
			case 2:
				*(WORD*)block = *(WORD*)addr;
				break;
			case 1:
				*(BYTE*)block = *(BYTE*)addr;
				break;
			default:
				memcpy(block, (VOID*)addr, size);
				break;
			}

			VirtualProtect((VOID*)addr, size, old_prot, &old_prot);

			return TRUE;
		}
		return FALSE;
	}

	BOOL __fastcall PatchWord(DWORD addr, WORD value)
	{
		return PatchBlock(addr, &value, sizeof(value));
	}

	BOOL __fastcall PatchDWord(DWORD addr, DWORD value)
	{
		return PatchBlock(addr, &value, sizeof(value));
	}

	BOOL __fastcall PatchByte(DWORD addr, BYTE value)
	{
		return PatchBlock(addr, &value, sizeof(value));
	}

	BOOL __fastcall ReadWord(DWORD addr, WORD* value)
	{
		return ReadBlock(addr, value, sizeof(*value));
	}

	BOOL __fastcall ReadDWord(DWORD addr, DWORD* value)
	{
		return ReadBlock(addr, value, sizeof(*value));
	}

	DWORD __fastcall PatchFunction(HMODULE hModule, const CHAR* function, DWORD addr)
	{
		PIMAGE_DOS_HEADER headDOS = (PIMAGE_DOS_HEADER)hModule;
		PIMAGE_NT_HEADERS headNT = (PIMAGE_NT_HEADERS)((BYTE*)headDOS + headDOS->e_lfanew);

		PIMAGE_IMPORT_DESCRIPTOR imports = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)headDOS + headNT->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
		for (; imports->Name; ++imports)
		{
			CHAR* libraryName = (CHAR*)((DWORD)headDOS + imports->Name);
			PIMAGE_THUNK_DATA nameThunk = (PIMAGE_THUNK_DATA)((DWORD)headDOS + imports->OriginalFirstThunk);
			PIMAGE_THUNK_DATA addressThunk = (PIMAGE_THUNK_DATA)((DWORD)headDOS + imports->FirstThunk);
			for (; nameThunk->u1.AddressOfData; ++nameThunk, ++addressThunk)
			{
				DWORD name = (DWORD)headDOS + nameThunk->u1.AddressOfData;
				CHAR* funcName = (CHAR*)(name + 2);

				WORD indent = 0;
				if (ReadWord(name, (WORD*)&indent) && !strcmp(funcName, function))
				{
					DWORD res;
					if (ReadDWord((DWORD)&addressThunk->u1.AddressOfData, &res) && PatchDWord((DWORD)&addressThunk->u1.AddressOfData, addr))
						return res;
					return NULL;
				}
			}
		}

		return NULL;
	}

	// ===============================================================
	SIZE adjustSize;

	BOOL __stdcall AdjustWindowRectExHook(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle)
	{
		adjustSize.cx = lpRect->right - lpRect->left;
		adjustSize.cy = lpRect->bottom - lpRect->top;

		switch (dwStyle)
		{
		case STYLE_FULL_OLD:
			dwStyle = STYLE_FULL_NEW;
			break;
		case STYLE_WIN_OLD:
			dwStyle = STYLE_WIN_NEW;
			break;
		default:
			break;
		}

		return AdjustWindowRectEx(lpRect, dwStyle, bMenu, dwExStyle);
	}

	BOOL __stdcall MoveWindowHook(HWND hWnd, INT X, INT Y, INT nWidth, INT nHeight, BOOL bRepaint)
	{
		if (GetWindowLong(hWnd, GWL_STYLE) & WS_MAXIMIZE)
			ShowWindow(hWnd, SW_SHOWNORMAL);

		DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
		if (ddraw)
		{
			if (!ddraw->mode || ddraw->mode->width != adjustSize.cx)
			{
				DisplayMode* mode = modesList;
				DWORD count = 3;
				do
				{
					if (mode->width == adjustSize.cx && mode->height == adjustSize.cy && mode->bpp)
					{
						ddraw->mode = mode;
						ddraw->RenderStop();
						ddraw->RenderStart();
						break;
					}

					++mode;
				} while (--count);
			}

			if (ddraw->windowState != WinStateWindowed)
			{
				RECT rect = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
				AdjustWindowRect(&rect, GetWindowLong(hWnd, GWL_STYLE), TRUE);
				return MoveWindow(hWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, bRepaint);
			}
			else
				return MoveWindow(hWnd, X, Y, nWidth, nHeight, bRepaint);
		}
		else
			return MoveWindow(hWnd, X, Y, nWidth, nHeight, bRepaint);
	}

	VOID __fastcall LoadNewMenu(HMENU hMenu)
	{
		if (hMenu)
		{
			HMENU hSub;
			CHAR buffer[20];
			for (DWORD i = 0; hSub = GetSubMenu(hMenu, i); ++i)
			{
				if (GetMenuItemID(hSub, 0) == IDM_FILE_NEW_GAME && DeleteMenu(hMenu, i, MF_BYPOSITION))
				{
					HMENU hNew = LoadMenu(hDllModule, MAKEINTRESOURCE(hookSpace->resMenu));
					if (hNew)
					{
						for (DWORD j = 0; hSub = GetSubMenu(hNew, j); ++j)
						{
							GetMenuString(hNew, j, buffer, 20, MF_BYPOSITION);
							InsertMenu(hMenu, i + j, MF_BYPOSITION | MF_POPUP, (UINT_PTR)hSub, buffer);
						}

						DestroyMenu(hNew);
					}

					return;
				}
			}
		}
	}

	HWND __stdcall CreateWindowExHook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, INT X, INT Y, INT nWidth, INT nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
	{
		dwStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX | WS_MAXIMIZEBOX;

		HWND hWnd = CreateWindowEx(dwExStyle, lpClassName, hookSpace->windowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
		if (hWnd)
			LoadNewMenu(GetMenu(hWnd));
		return hWnd;
	}

	LONG __stdcall SetWindowLongHook(HWND hWnd, INT nIndex, LONG dwNewLong)
	{
		if (nIndex == GWL_STYLE)
		{
			switch (dwNewLong)
			{
			case STYLE_FULL_OLD:
				dwNewLong = STYLE_FULL_NEW;
				break;
			case STYLE_WIN_OLD:
				dwNewLong = STYLE_WIN_NEW;
				break;
			default:
				break;
			}
		}

		return SetWindowLong(hWnd, nIndex, dwNewLong);
	}

	BOOL __stdcall GetClientRectHook(HWND hWnd, LPRECT lpRect)
	{
		if (GetClientRect(hWnd, lpRect))
		{
			DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
			if (ddraw && ddraw->mode)
			{
				lpRect->right = ddraw->mode->width;
				lpRect->bottom = ddraw->mode->height;
			}

			return TRUE;
		}

		return FALSE;
	}

	BOOL __stdcall GetCursorPosHook(LPPOINT lpPoint)
	{
		if (GetCursorPos(lpPoint))
		{
			HWND hWnd = WindowFromPoint(*lpPoint);
			DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
			if (ddraw && ddraw->mode)
			{
				ScreenToClient(hWnd, lpPoint);
				ddraw->ScaleMouse(lpPoint);
				ClientToScreen(hWnd, lpPoint);
			}

			return TRUE;
		}

		return FALSE;
	}

	BOOL __stdcall GetWindowRectHook(HWND hWnd, LPRECT lpRect)
	{
		DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
		if (ddraw && ddraw->mode)
		{
			lpRect->left = 0;
			lpRect->top = 0;
			ClientToScreen(hWnd, (LPPOINT)&lpRect->left);
			lpRect->right = lpRect->left + ddraw->mode->width;
			lpRect->bottom = lpRect->top + ddraw->mode->height;
			AdjustWindowRect(lpRect, GetWindowLong(hWnd, GWL_STYLE), TRUE);
			return TRUE;
		}
		else
			return GetWindowRect(hWnd, lpRect);
	}

	HMENU __stdcall LoadMenuHook(HINSTANCE hInstance, LPCTSTR lpMenuName)
	{
		HMENU hMenu = LoadMenu(hInstance, lpMenuName);
		LoadNewMenu(hMenu);
		return hMenu;
	}

	BOOL __stdcall SetMenuHook(HWND hWnd, HMENU hMenu)
	{
		if (SetMenu(hWnd, hMenu))
		{
			DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
			if (ddraw)
				ddraw->CheckMenu(hMenu);

			return TRUE;
		}

		return FALSE;
	}


	HMODULE hModule;
	VOID __stdcall StartHook()
	{
		PatchFunction(hModule, "AdjustWindowRectEx", (DWORD)AdjustWindowRectExHook);
		PatchFunction(hModule, "CreateWindowExA", (DWORD)CreateWindowExHook);
		PatchFunction(hModule, "SetWindowLongA", (DWORD)SetWindowLongHook);

		PatchFunction(hModule, "GetWindowRect", (DWORD)GetWindowRectHook);
		PatchFunction(hModule, "GetClientRect", (DWORD)GetClientRectHook);
		PatchFunction(hModule, "GetCursorPos", (DWORD)GetCursorPosHook);

		PatchFunction(hModule, "MoveWindow", (DWORD)MoveWindowHook);

		PatchFunction(hModule, "LoadMenuA", (DWORD)LoadMenuHook);
		PatchFunction(hModule, "SetMenu", (DWORD)SetMenuHook);

		PatchFunction(hModule, "DirectDrawCreateEx", (DWORD)Main::DirectDrawCreateEx);
	}

	DWORD back_cpu;
	VOID __declspec(naked) hook_cpu()
	{
		__asm
		{
			PUSH	0x1
			CALL	DWORD PTR[Sleep]
			SUB		ESP, 0x1C
			LEA		EAX, [ESP]
			JMP		back_cpu
		}
	}

	DWORD start_back;
	VOID __declspec(naked) start_hook()
	{
		__asm
		{
			CALL	StartHook
			PUSH    EBP
			MOV     EBP, ESP
			PUSH    0xFFFFFFFF
			JMP		start_back
		}
	}

	BOOL Load()
	{
		AddressSpace* defaultSpace = NULL;
		AddressSpace* equalSpace = NULL;

		hookSpace = addressArray;
		do
		{
			DWORD check1, check2, equal;
			if (ReadDWord(hookSpace->check_1 + 1, &check1) && check1 == STYLE_FULL_OLD && ReadDWord(hookSpace->check_2 + 1, &check2) && check2 == STYLE_FULL_OLD)
			{
				if (!hookSpace->equal_address)
					defaultSpace = hookSpace;
				else if (ReadDWord(hookSpace->equal_address, &equal) && equal == hookSpace->equal_value)
				{
					equalSpace = hookSpace;
					break;
				}
			}

			++hookSpace;
		} while (hookSpace->check_1);

		hookSpace = equalSpace ? equalSpace : defaultSpace;
		if (hookSpace) {
			hModule = GetModuleHandle(NULL);
			PIMAGE_DOS_HEADER headDOS = (PIMAGE_DOS_HEADER)hModule;
			PIMAGE_NT_HEADERS headNT = (PIMAGE_NT_HEADERS)((DWORD)headDOS + headDOS->e_lfanew);

			DWORD address = (DWORD)headDOS + headNT->OptionalHeader.AddressOfEntryPoint;
			start_back = address + 5;
			PatchHook(address, start_hook);

			PatchHook(hookSpace->cpu_hook, hook_cpu);
			back_cpu = hookSpace->cpu_hook + 7;

			configKey = hookSpace->configKey;

			return TRUE;
		}

		return FALSE;
	}
}