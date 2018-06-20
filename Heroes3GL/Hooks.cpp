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
#define STYLE_FULL_NEW (WS_VISIBLE | WS_POPUP | WS_SYSMENU | WS_CLIPSIBLINGS)

#define STYLE_WIN_OLD (WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
#define STYLE_WIN_NEW (WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_CLIPSIBLINGS)

AddressSpace* hookSpace;

AddressSpace addressArray[] = {
	// check 1 & 2,         cpu patch,  renderNop

	0x004D4A5F, 0x0059308B, 0x004D4EF0, 0x00592D72, IDM_RUSSIAN, // Heroes III Erathia - v1.0 Buka
	"Ãåðîè Ìå÷à è Ìàãèè III: Âîçðîæäåíèå Ýðàôèè",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	0x004F2533, 0x005F9649, 0x004F29C3, 0x005F9336, IDM_RUSSIAN, // Heroes III Armageddon - v2.1 Buka
	"Ãåðîè Ìå÷à è Ìàãèè III: Êëèíîê Àðìàãåääîíà",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	0x004F7EB3, 0x00602379, 0x004F8363, 0x00602066, IDM_RUSSIAN, // Heroes III Shadow - Buka
	"Ãåðîè Ìå÷à è Ìàãèè III: Äûõàíèå Cìåðòè",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	0x004F7EB3, 0x006021A9, 0x004F8363, 0x00601E96, IDM_RUSSIAN, // Heroes III Complete - v4.0 Buka
	"Ãåðîè Ìå÷à è Ìàãèè III: Ïîëíîå Ñîáðàíèå",
	"SOFTWARE\\Buka\\3DO\\Heroes of Might and Magic III Complete Platinum",

	// ----------------------------------------------------------------------------------------------------------------------------------------------

	0x004EBA34, 0x005AF329, 0x004EBEC3, 0x005AF016, IDM_RUSSIAN, // Heroes Chronicles Warlords & Underworld & Elements & Dragons - v1.0
	"Õðîíèêè ãåðîåâ",
	"SOFTWARE\\New World Computing\\Heroes Chronicles\\1.0",

	0x004EB494, 0x005AF2D9, 0x004EB923, 0x005AEFC6, IDM_RUSSIAN, // Heroes Chronicles Beastmaster & Sword - v1.0
	"Õðîíèêè ãåðîåâ",
	"SOFTWARE\\New World Computing\\Heroes Chronicles\\1.0",

	// ==============================================================================================================================================

	0x004D3363, 0x0058E558, 0x004D39F3, 0x0058E246, IDM_ENGLISH, // Heroes III Erathia - v1.0
	"Heroes of Might and Magic III: The Restoration of Erathia",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	0x0041E573, 0x004210B8, 0x0041EA03, 0x00420DA6, IDM_ENGLISH, // Heroes III Erathia - v1.1
	"Heroes of Might and Magic III: The Restoration of Erathia",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	0x0041E523, 0x00421078, 0x0041E9C3, 0x00420D66, IDM_ENGLISH, // Heroes III Erathia - v1.2
	"Heroes of Might and Magic III: The Restoration of Erathia",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	0x004F58F3, 0x005D9679, 0x004F5D83, 0x005D9366, IDM_ENGLISH, // Heroes III Erathia - v1.3
	"Heroes of Might and Magic III: The Restoration of Erathia",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	0x004F5583, 0x005D8F69, 0x004F5A13, 0x005D8C56, IDM_ENGLISH, // Heroes III Erathia - v1.4
	"Heroes of Might and Magic III: The Restoration of Erathia",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	// ----------------------------------------------------------------------------------------------------------------------------------------------

	0x004EB283, 0x005EEFD8, 0x004EB713, 0x005EECC6, IDM_ENGLISH, // Heroes III Armageddon - v2.0
	"Heroes of Might and Magic III: Armageddon’s Blade",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	0x004F5C43, 0x00600299, 0x004F60D3, 0x005FFF86, IDM_ENGLISH, // Heroes III Armageddon - v2.1
	"Heroes of Might and Magic III: Armageddon’s Blade",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	0x004F5963, 0x005FFBF9, 0x004F5DF3, 0x005FF8E6, IDM_ENGLISH, // Heroes III Armageddon - v2.2
	"Heroes of Might and Magic III: Armageddon’s Blade",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	// ----------------------------------------------------------------------------------------------------------------------------------------------

	0x004F7D73, 0x006027E9, 0x004F8223, 0x006024D6, IDM_ENGLISH, // Heroes III Shadow - v3.0
	"Heroes of Might and Magic III: The Shadow of Death",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	0x004F85B3, 0x006027E9, 0x004F8A63, 0x006024D6, IDM_ENGLISH, // Heroes III Shadow - v3.1
	"Heroes of Might and Magic III: The Shadow of Death",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	0x004F8193, 0x00602149, 0x004F8643, 0x00601E36, IDM_ENGLISH, // Heroes III Shadow - v3.2
	"Heroes of Might and Magic III: The Shadow of Death",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	// ----------------------------------------------------------------------------------------------------------------------------------------------

	0x004F7B03, 0x00601B89, 0x004F7FB3, 0x00601876, IDM_ENGLISH, // Heroes III Complete - v4.0
	"Heroes of Might and Magic III: Complete",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	// ==============================================================================================================================================

	0x004F0033, 0x005B5129, 0x004F04B3, 0x005B4E16, IDM_ENGLISH, // Heroes Chronicles Warlords & Underworld - v1.0
	"Heroes Chronicles",
	"SOFTWARE\\New World Computing\\Heroes Chronicles\\1.0",

	0x004EFE04, 0x005B5469, 0x004F0293, 0x005B5156, IDM_ENGLISH, // Heroes Chronicles Elements & Dragons - v1.0
	"Heroes Chronicles",
	"SOFTWARE\\New World Computing\\Heroes Chronicles\\1.0",

	0x004EFA84, 0x005B51B9, 0x004EFFF3, 0x005B4EA6, IDM_ENGLISH, // Heroes Chronicles WorldTree - v1.0
	"Heroes Chronicles",
	"SOFTWARE\\New World Computing\\Heroes Chronicles\\1.0",

	0x004EF824, 0x005B5249, 0x004EFD93, 0x005B4F36, IDM_ENGLISH, // Heroes Chronicles FieryMoon - v1.0
	"Heroes Chronicles",
	"SOFTWARE\\New World Computing\\Heroes Chronicles\\1.0",

	0x004EF874, 0x005B4C09, 0x004EFD03, 0x005B48F6, IDM_ENGLISH, // Heroes Chronicles Beastmaster & Sword - v1.0
	"Heroes Chronicles",
	"SOFTWARE\\New World Computing\\Heroes Chronicles\\1.0",

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

	BOOL __stdcall AdjustWindowRectHook(LPRECT lpRect, DWORD dwStyle, BOOL bMenu)
	{
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

		return AdjustWindowRect(lpRect, dwStyle, bMenu);
	}

	BOOL __stdcall AdjustWindowRectExHook(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle)
	{
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

	HWND __stdcall CreateWindowExHook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, INT X, INT Y, INT nWidth, INT nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
	{
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

		return CreateWindowEx(dwExStyle, lpClassName, hookSpace->windowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
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

	INT __stdcall GetDeviceCapsHook(HDC hdc, INT index)
	{
		return index != BITSPIXEL ? GetDeviceCaps(hdc, index) : 16;
	}

	BOOL __stdcall ScreenToClientHook(HWND hWnd, LPPOINT lpPoint)
	{
		if (ScreenToClient(hWnd, lpPoint))
		{
			DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
			if (ddraw)
				ddraw->ScaleMouse(lpPoint);

			return TRUE;
		}

		return FALSE;
	}

	HMENU __stdcall LoadMenuHook(HINSTANCE hInstance, LPCTSTR lpMenuName)
	{
		HMENU hMenu = LoadMenu(hInstance, lpMenuName);
		if (hMenu)
		{
			HMENU hNew = LoadMenu(hDllModule, MAKEINTRESOURCE(hookSpace->resMenu));
			if (hNew)
			{
				DWORD i, index = 0;

				HMENU hSub;
				for (i = 0; hSub = GetSubMenu(hMenu, i);)
				{
					DWORD itemId = GetMenuItemID(hSub, 0);
					if (itemId == IDM_FILE_QUIT || IDM_RES_FULL_SCREEN || IDM_HELP_MANUAL)
					{
						DeleteMenu(hMenu, i, MF_BYPOSITION);
						index = i;
					}
					else
						++i;
				}

				CHAR buffer[20];

				for (i = 0; hSub = GetSubMenu(hNew, i); ++i);
				for (; i; --i)
				{
					hSub = GetSubMenu(hNew, i - 1);

					GetMenuString(hNew, i - 1, buffer, 20, MF_BYPOSITION);
					InsertMenu(hMenu, index, MF_BYPOSITION | MF_POPUP, (UINT_PTR)hSub, buffer);
				}

				DestroyMenu(hNew);
			}
		}
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

	AIL_WAVEOUTOPEN AIL_waveOutOpen;
	DWORD __stdcall AIL_waveOutOpenHook(LPVOID driver, DWORD a1, DWORD a2, LPWAVEFORMATEX waveFormat)
	{
		waveFormat->wFormatTag = 1;
		waveFormat->nChannels = 2;
		waveFormat->nSamplesPerSec = 44100;
		waveFormat->nAvgBytesPerSec = 176400;
		waveFormat->nBlockAlign = 4;
		waveFormat->wBitsPerSample = 16;

		return AIL_waveOutOpen(driver, a1, a2, waveFormat);
	}

	DWORD back_cpu;
	VOID __declspec(naked) hook_cpu()
	{
		__asm
		{
			PUSH	0x1
			CALL	DWORD PTR[Sleep]
			SUB     ESP, 0x1C
			PUSH	EBX
			MOV		EBX, GetMessage
			JMP		back_cpu
		}
	}

	HMODULE hModule;
	VOID __stdcall StartHook()
	{
		PatchFunction(hModule, "AdjustWindowRect", (DWORD)AdjustWindowRectHook);
		PatchFunction(hModule, "AdjustWindowRectEx", (DWORD)AdjustWindowRectExHook);
		PatchFunction(hModule, "CreateWindowExA", (DWORD)CreateWindowExHook);
		PatchFunction(hModule, "SetWindowLongA", (DWORD)SetWindowLongHook);

		PatchFunction(hModule, "GetDeviceCaps", (DWORD)GetDeviceCapsHook);
		PatchFunction(hModule, "ScreenToClient", (DWORD)ScreenToClientHook);
		PatchFunction(hModule, "LoadMenuA", (DWORD)LoadMenuHook);
		PatchFunction(hModule, "SetMenu", (DWORD)SetMenuHook);

		PatchFunction(hModule, "DirectDrawCreate", (DWORD)Main::DirectDrawCreate);

		AIL_waveOutOpen = (AIL_WAVEOUTOPEN)PatchFunction(hModule, "_AIL_waveOutOpen@16", (DWORD)AIL_waveOutOpenHook);
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
		hookSpace = addressArray;
		do
		{
			DWORD check1, check2;
			if (ReadDWord(hookSpace->check_1 + 1, &check1) && check1 == STYLE_FULL_OLD &&
				ReadDWord(hookSpace->check_2 + 1, &check2) && check2 == STYLE_FULL_OLD)
			{
				hModule = GetModuleHandle(NULL);
				PIMAGE_DOS_HEADER headDOS = (PIMAGE_DOS_HEADER)hModule;
				PIMAGE_NT_HEADERS headNT = (PIMAGE_NT_HEADERS)((DWORD)headDOS + headDOS->e_lfanew);

				DWORD address = (DWORD)headDOS + headNT->OptionalHeader.AddressOfEntryPoint;
				start_back = address + 5;
				PatchHook(address, start_hook);

				PatchHook(hookSpace->cpu_hook, hook_cpu);
				back_cpu = hookSpace->cpu_hook + 10;

				PatchNop(hookSpace->renderNop, 5); // prevent on WM_PAINT

				configKey = hookSpace->configKey;

				return TRUE;
			}

			++hookSpace;
		} while (hookSpace->check_1);

		return FALSE;
	}
}