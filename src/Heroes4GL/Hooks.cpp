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
#include "Window.h"

#define BASE_ADDRESS 0x00400000
#define STYLE_FULL_OLD (WS_VISIBLE | WS_POPUP)
#define STYLE_FULL_NEW (WS_VISIBLE | WS_POPUP | WS_SYSMENU)

#define STYLE_WIN_OLD (WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
#define STYLE_WIN_NEW (WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX | WS_MAXIMIZEBOX)

AddressSpace* hookSpace;

AddressSpace addressArray[] = {
	// === RUS ======================================================================================================================================
#pragma region RUS
	0x00844A4D, 0x00844B35, 0x00401D18, 0x90909090, 0x0084497C, 0x006D59BB, 0x00843736, 0x00843F0E, LNG_RUSSIAN, IDD_HELP_ABOUT_RUSSIAN_1_0, // Heroes IV - 1.0
	"Герои Меча и Магии IV", //"Heroes of Might and Magic IV",

	0x008C6411, 0x008C64F9, 0x00000000, 0x00000000, 0x008C6340, 0x00736E79, 0x008C4EEE, 0x008C58B2, LNG_RUSSIAN, IDD_HELP_ABOUT_RUSSIAN_2_2_GS, // Heroes IV - 2.2GS
	"Герои Меча и Магии IV: Грядущая Буря",

	0x008D38B1, 0x008D3999, 0x00000000, 0x00000000, 0x008D37E0, 0x0073D780, 0x008D238E, 0x008D2D52, LNG_RUSSIAN, IDD_HELP_ABOUT_RUSSIAN_3_0_WOW, // Heroes IV - 3.0WoW
	"Герои Меча и Магии IV: Вихри Войны",
#pragma endregion 

	// === ENG ======================================================================================================================================
#pragma region ENG
	0x00844A4D, 0x00844B35, 0x00000000, 0x00000000, 0x0084497C, 0x006D59BB, 0x00843736, 0x00843F0E, LNG_ENGLISH, IDD_HELP_ABOUT_ENGLISH_1_0, // Heroes IV - 1.0
	"Heroes of Might and Magic IV",

	0x0084C0FD, 0x0084C1E5, 0x00000000, 0x00000000, 0x0084C02C, 0x006DAC3B, 0x0084AD96, 0x0084B5BE, LNG_ENGLISH, IDD_HELP_ABOUT_ENGLISH_1_2, // Heroes IV - 1.2
	"Heroes of Might and Magic IV",

	0x008533C1, 0x008534A9, 0x00000000, 0x00000000, 0x008532F0, 0x006DF4F5, 0x0085201E, 0x00852862, LNG_ENGLISH, IDD_HELP_ABOUT_ENGLISH_1_3, // Heroes IV - 1.3
	"Heroes of Might and Magic IV",

	0x008C5F51, 0x008C6039, 0x00000000, 0x00000000, 0x008C5E80, 0x007362D9, 0x008C4A2E, 0x008C53F2, LNG_ENGLISH, IDD_HELP_ABOUT_ENGLISH_2_0, // Heroes IV - 2.0
	"Heroes of Might and Magic IV",

	0x008C63F1, 0x008C64D9, 0x00000000, 0x00000000, 0x008C6320, 0x007369F9, 0x008C4ECE, 0x008C5892, LNG_ENGLISH, IDD_HELP_ABOUT_ENGLISH_2_2, // Heroes IV - 2.2
	"Heroes of Might and Magic IV",

	0x008D3881, 0x008D3969, 0x00000000, 0x00000000, 0x008D37B0, 0x0073D9B0, 0x008D235E, 0x008D2D22, LNG_ENGLISH, IDD_HELP_ABOUT_ENGLISH_3_0, // Heroes IV - 3.0
	"Heroes of Might and Magic IV",

	// ---------------------------------------------------------------------------------------------------------------------

	0x008C5F51, 0x008C6039, 0x00401448, 0x00961278, 0x008C5E80, 0x007362D9, 0x008C4A2E, 0x008C53F2, LNG_ENGLISH, IDD_HELP_ABOUT_ENGLISH_2_0_GS, // Heroes IV - 2.0GS
	"Heroes of Might and Magic IV: The Gathering Storm",

	0x008C63F1, 0x008C64D9, 0x00401448, 0x00961278, 0x008C6320, 0x007369F9, 0x008C4ECE, 0x008C5892, LNG_ENGLISH, IDD_HELP_ABOUT_ENGLISH_2_2_GS, // Heroes IV - 2.2GS
	"Heroes of Might and Magic IV: The Gathering Storm",

	0x008D3881, 0x008D3969, 0x00401448, 0x00970278, 0x008D37B0, 0x0073D9B0, 0x008D235E, 0x008D2D22, LNG_ENGLISH, IDD_HELP_ABOUT_ENGLISH_3_0_GS, // Heroes IV - 3.0GS
	"Heroes of Might and Magic IV: The Gathering Storm",

	// ---------------------------------------------------------------------------------------------------------------------

	0x008D3881, 0x008D3969, 0x00401448, 0x00970200, 0x008D37B0, 0x0073D9B0, 0x008D235E, 0x008D2D22, LNG_ENGLISH, IDD_HELP_ABOUT_ENGLISH_3_0_WOW, // Heroes IV - 3.0WoW
	"Heroes of Might and Magic IV: Winds of War"
#pragma endregion 
};

UINT menuIds[] = { IDM_FILT_OFF, IDM_FILT_LINEAR, IDM_FILT_CUBIC, IDM_ASPECT_RATIO, IDM_VSYNC, IDM_HELP_WRAPPER,
	IDM_FILT_XRBZ_LINEAR, IDM_FILT_XRBZ_CUBIC, IDM_FILT_XRBZ_2X, IDM_FILT_XRBZ_3X, IDM_FILT_XRBZ_4X, IDM_FILT_XRBZ_5X, IDM_FILT_XRBZ_6X,
	IDM_FILT_SCALEHQ_LINEAR, IDM_FILT_SCALEHQ_CUBIC, IDM_FILT_SCALEHQ_2X, IDM_FILT_SCALEHQ_4X,
	IDM_FILT_XSAL_LINEAR, IDM_FILT_XSAL_CUBIC, IDM_FILT_XSAL_2X,
	IDM_FILT_EAGLE_LINEAR, IDM_FILT_EAGLE_CUBIC, IDM_FILT_EAGLE_2X,
	IDM_FILT_SCALENX_LINEAR, IDM_FILT_SCALENX_CUBIC, IDM_FILT_SCALENX_2X, IDM_FILT_SCALENX_3X,
	IDM_PATCH_CPU
};

namespace Hooks
{
	HMODULE hModule;
	INT baseOffset;

#pragma region Hook helpers
	BOOL __fastcall PatchRedirect(DWORD addr, VOID* hook, BYTE instruction)
	{
		DWORD address = addr + baseOffset;

		DWORD old_prot;
		if (VirtualProtect((VOID*)address, 5, PAGE_EXECUTE_READWRITE, &old_prot))
		{
			BYTE* jump = (BYTE*)address;
			*jump = instruction;
			++jump;
			*(DWORD*)jump = (DWORD)hook - (DWORD)address - 5;

			VirtualProtect((VOID*)address, 5, old_prot, &old_prot);

			return TRUE;
		}
		return FALSE;
	}

	BOOL __fastcall PatchHook(DWORD addr, VOID* hook)
	{
		return PatchRedirect(addr, hook, 0xE9);
	}

	BOOL __fastcall PatchCall(DWORD addr, VOID* hook)
	{
		return PatchRedirect(addr, hook, 0xE8);
	}

	BOOL __fastcall PatchNop(DWORD addr, DWORD size)
	{
		DWORD address = addr + baseOffset;

		DWORD old_prot;
		if (VirtualProtect((VOID*)address, size, PAGE_EXECUTE_READWRITE, &old_prot))
		{
			MemorySet((VOID*)address, 0x90, size);
			VirtualProtect((VOID*)address, size, old_prot, &old_prot);

			return TRUE;
		}
		return FALSE;
	}

	BOOL __fastcall PatchBlock(DWORD addr, VOID* block, DWORD size)
	{
		DWORD address = addr + baseOffset;

		DWORD old_prot;
		if (VirtualProtect((VOID*)address, size, PAGE_EXECUTE_READWRITE, &old_prot))
		{
			switch (size)
			{
			case 4:
				*(DWORD*)address = *(DWORD*)block;
				break;
			case 2:
				*(WORD*)address = *(WORD*)block;
				break;
			case 1:
				*(BYTE*)address = *(BYTE*)block;
				break;
			default:
				MemoryCopy((VOID*)address, block, size);
				break;
			}

			VirtualProtect((VOID*)address, size, old_prot, &old_prot);

			return TRUE;
		}
		return FALSE;
	}

	BOOL __fastcall ReadBlock(DWORD addr, VOID* block, DWORD size)
	{
		DWORD address = addr + baseOffset;

		DWORD old_prot;
		if (VirtualProtect((VOID*)address, size, PAGE_READONLY, &old_prot))
		{
			switch (size)
			{
			case 4:
				*(DWORD*)block = *(DWORD*)address;
				break;
			case 2:
				*(WORD*)block = *(WORD*)address;
				break;
			case 1:
				*(BYTE*)block = *(BYTE*)address;
				break;
			default:
				MemoryCopy(block, (VOID*)address, size);
				break;
			}

			VirtualProtect((VOID*)address, size, old_prot, &old_prot);

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

	DWORD __fastcall PatchFunction(HMODULE hModule, const CHAR* function, VOID* addr)
	{
		DWORD res = NULL;
		HANDLE hFile = NULL;
		DWORD baseEx;
		DWORD base = (DWORD)hModule;
		PIMAGE_NT_HEADERS headNT = (PIMAGE_NT_HEADERS)(base + ((PIMAGE_DOS_HEADER)hModule)->e_lfanew);

		PIMAGE_DATA_DIRECTORY dataDir = &headNT->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
		if (dataDir->Size)
		{
			PIMAGE_IMPORT_DESCRIPTOR imports = (PIMAGE_IMPORT_DESCRIPTOR)(base + dataDir->VirtualAddress);
			for (DWORD idx = 0; imports->Name; ++idx, ++imports)
			{
				CHAR* libraryName = (CHAR*)(base + imports->Name);

				PIMAGE_THUNK_DATA addressThunk = (PIMAGE_THUNK_DATA)(base + imports->FirstThunk);
				PIMAGE_THUNK_DATA nameThunk;
				if (imports->OriginalFirstThunk)
					nameThunk = (PIMAGE_THUNK_DATA)(base + imports->OriginalFirstThunk);
				else
				{
					if (!hFile)
					{
						CHAR filePath[MAX_PATH];
						GetModuleFileName(hModule, filePath, sizeof(filePath) - 1);
						hFile = CreateFile(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
						HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
						baseEx = (DWORD)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
					}

					nameThunk = (PIMAGE_THUNK_DATA)(baseEx + imports->FirstThunk);
				}

				for (; nameThunk->u1.AddressOfData; ++nameThunk, ++addressThunk)
				{
					PIMAGE_IMPORT_BY_NAME name = PIMAGE_IMPORT_BY_NAME(base + nameThunk->u1.AddressOfData);

					WORD hint;
					if (ReadWord((INT)name - baseOffset, &hint) && !StrCompare((CHAR*)name->Name, function))
					{
						if (ReadDWord((INT)&addressThunk->u1.AddressOfData - baseOffset, &res))
							PatchDWord((INT)&addressThunk->u1.AddressOfData - baseOffset, (DWORD)addr);

						goto Exit;
					}
				}
			}
		}

		if (hFile)
			Exit: CloseHandle(hFile);

		return res;
	}
#pragma endregion

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

		OpenDraw* ddraw = Main::FindOpenDrawByWindow(hWnd);
		if (ddraw)
		{
			ddraw->ResetDisplayMode(adjustSize.cx, adjustSize.cy);

			if (ddraw->windowState != WinStateWindowed)
			{
				RECT rect = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
				AdjustWindowRect(&rect, GetWindowLong(hWnd, GWL_STYLE), TRUE);
				return MoveWindow(hWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, bRepaint);
			}
		}

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
					HMENU hNew = LoadMenu(hDllModule, MAKEINTRESOURCE(config.language == LNG_ENGLISH ? IDM_ENGLISH : IDM_RUSSIAN));
					if (hNew)
					{
						for (DWORD j = 0; hSub = GetSubMenu(hNew, j); ++j)
						{
							GetMenuString(hNew, j, buffer, 20, MF_BYPOSITION);
							InsertMenu(hMenu, i + j, MF_BYPOSITION | MF_POPUP, (UINT_PTR)hSub, buffer);
						}

						Window::CheckMenu(hMenu);
					}

					return;
				}
			}
		}
	}

	HWND __stdcall CreateWindowExHook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, INT X, INT Y, INT nWidth, INT nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
	{
		if (!config.isNoGL)
			dwStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX | WS_MAXIMIZEBOX;

		HWND hWnd = CreateWindow(lpClassName, hookSpace->windowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
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

	INT __stdcall MessageBoxHook(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
	{
		INT res;
		ULONG_PTR cookie = NULL;
		if (hActCtx && hActCtx != INVALID_HANDLE_VALUE && !ActivateActCtxC(hActCtx, &cookie))
			cookie = NULL;

		res = MessageBox(hWnd, lpText, lpCaption, uType);

		if (cookie)
			DeactivateActCtxC(0, cookie);

		return res;
	}

	BOOL __stdcall GetClientRectHook(HWND hWnd, LPRECT lpRect)
	{
		if (GetClientRect(hWnd, lpRect))
		{
			OpenDraw* ddraw = Main::FindOpenDrawByWindow(hWnd);
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
			OpenDraw* ddraw = Main::FindOpenDrawByWindow(hWnd);
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
		OpenDraw* ddraw = Main::FindOpenDrawByWindow(hWnd);
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
			Window::CheckMenu(hMenu);
			return TRUE;
		}

		return FALSE;
	}

	BOOL __stdcall EnableMenuItemHook(HMENU hMenu, UINT uIDEnableItem, UINT uEnable)
	{
		BOOL found = FALSE;
		DWORD count = sizeof(menuIds) / sizeof(*menuIds);
		UINT* menu = menuIds;
		do
		{
			if (*menu++ == uIDEnableItem)
			{
				found = TRUE;
				break;
			}
		} while (--count);

		return !found && EnableMenuItem(hMenu, uIDEnableItem, uEnable);
	}

	BOOL __stdcall PeekMessageHook(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
	{
		if (PeekMessage(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg))
			return TRUE;

		if (config.coldCPU)
			Sleep(1);

		return FALSE;
	}

#pragma region Registry
	LSTATUS __stdcall RegCreateKeyExHook(HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass, DWORD dwOptions, REGSAM samDesired, const LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition)
	{
		return config.isExist ? ERROR_SUCCESS : RegCreateKeyEx(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
	}

	LSTATUS __stdcall RegOpenKeyExHook(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
	{
		return config.isExist ? ERROR_SUCCESS : RegOpenKeyEx(hKey, lpSubKey, ulOptions, samDesired, phkResult);
	}

	LSTATUS __stdcall RegCloseKeyHook(HKEY hKey)
	{
		return config.isExist ? ERROR_SUCCESS : RegCloseKey(hKey);
	}

	LSTATUS __stdcall RegQueryValueExHook(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
	{
		DWORD size = *lpcbData;

		if (!config.isExist)
		{
			LSTATUS res = RegQueryValueEx(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);

			if (size == sizeof(DWORD))
				Config::Set(CONFIG_APP, lpValueName, *(INT*)lpData);
			else
				Config::Set(CONFIG_APP, lpValueName, (CHAR*)lpData);

			return res;
		}
		else
		{
			if (size == sizeof(DWORD))
			{
				*(INT*)lpData = Config::Get(CONFIG_APP, lpValueName, *(INT*)lpData);
				if (lpType)
					*lpType = REG_DWORD;
			}
			else
			{
				Config::Get(CONFIG_APP, lpValueName, "", (CHAR*)lpData, *lpcbData);
				if (lpType)
					*lpType = *lpcbData ? REG_BINARY : REG_SZ;
			}

			return ERROR_SUCCESS;
		}
	}

	LSTATUS __stdcall RegSetValueExHook(HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, const BYTE* lpData, DWORD cbData)
	{
		if (dwType == REG_DWORD)
			Config::Set(CONFIG_APP, lpValueName, *(INT*)lpData);
		else
			Config::Set(CONFIG_APP, lpValueName, (CHAR*)lpData);

		return ERROR_SUCCESS;
	}
#pragma endregion

	BOOL Load()
	{
		hookSpace = addressArray;
		hModule = GetModuleHandle(NULL);
		baseOffset = (INT)hModule - BASE_ADDRESS;

		AddressSpace* defaultSpace = NULL;
		AddressSpace* equalSpace = NULL;

		DWORD hookCount = sizeof(addressArray) / sizeof(AddressSpace);
		do
		{
			DWORD check1, check2, equal;
			if (ReadDWord(hookSpace->check_1 + 1, &check1) && check1 == STYLE_FULL_OLD &&
				ReadDWord(hookSpace->check_2 + 1, &check2) && check2 == STYLE_FULL_OLD)
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
		} while (--hookCount);

		hookSpace = equalSpace ? equalSpace : defaultSpace;
		if (hookSpace)
		{
			Config::Load(hModule, hookSpace);

			{
				PatchFunction(hModule, "CreateWindowExA", CreateWindowExHook);
				PatchFunction(hModule, "MessageBoxA", MessageBoxHook);

				PatchFunction(hModule, "LoadMenuA", LoadMenuHook);
				PatchFunction(hModule, "SetMenu", SetMenuHook);
				PatchFunction(hModule, "EnableMenuItem", EnableMenuItemHook);
				PatchFunction(hModule, "PeekMessageA", PeekMessageHook);

				PatchFunction(hModule, "RegCreateKeyExA", RegCreateKeyExHook);
				PatchFunction(hModule, "RegOpenKeyExA", RegOpenKeyExHook);
				PatchFunction(hModule, "RegCloseKey", RegCloseKeyHook);
				PatchFunction(hModule, "RegQueryValueExA", RegQueryValueExHook);
				PatchFunction(hModule, "RegSetValueExA", RegSetValueExHook);

				PatchFunction(hModule, "DirectDrawCreateEx", Main::DirectDrawCreateEx);

				if (!config.isNoGL)
				{
					PatchFunction(hModule, "SetWindowLongA", SetWindowLongHook);
					PatchFunction(hModule, "AdjustWindowRectEx", AdjustWindowRectExHook);
					PatchFunction(hModule, "MoveWindow", MoveWindowHook);

					PatchFunction(hModule, "GetWindowRect", GetWindowRectHook);
					PatchFunction(hModule, "GetClientRect", GetClientRectHook);
					PatchFunction(hModule, "GetCursorPos", GetCursorPosHook);
				}
			}

			// windowed limitations
			if (!config.isNoGL)
			{
				PatchNop(hookSpace->fullscr_nop1, 20);
				PatchNop(hookSpace->fullscr_nop2, 4);

				PatchNop(hookSpace->clientrect_nop1, 2);
				PatchNop(hookSpace->clientrect_nop2, 2);
			}

			return TRUE;
		}

		return FALSE;
	}
}