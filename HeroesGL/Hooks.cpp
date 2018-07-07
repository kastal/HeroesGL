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
#include "mmsystem.h"
#include "Hooks.h"
#include "Main.h"
#include "Config.h"
#include "Resource.h"

#define STYLE_FULL_OLD (WS_VISIBLE | WS_CLIPSIBLINGS)
#define STYLE_FULL_NEW (WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU)

AddressSpace* hookSpace;

AddressSpace addressArray[] = {
	//	style,      ver,                        DDRAW.dll,  cpu patch,               meth_2 NOP,    meth_2 JMP,        mode NOP,      mode hook,  call 1,     call 2,     call 3,    
		0x00442EFF, 1, 0, MAKEINTRESOURCE(109), 0x0046815E, 0x00443776, LNG_RUSSIAN, 0x0046827C, 2, 0x004684AA, TRUE,  0x0046F8EC, 6, 0x0046833C, 0x004683DA, 0x004677A0, 0x00000000, // Heroes I - Buka
		"Герои Меча и Магии",
		"SOFTWARE\\Buka\\3DO\\Heroes of Might and Magic Platinum\\1.000", "HMM1 ",
		0x0040CBFA, 1, 0, MAKEINTRESOURCE(109), 0x0041AEA4, 0x0040D487, LNG_RUSSIAN, 0x0041AFC2, 2, 0x0041B1EA, TRUE,  0x0042170C, 6, 0x0041B082, 0x0041B120, 0x0041A51D, 0x00000000, // Heroes I Editor - Buka
		"Герои Меча и Магии: Редактор",
		"SOFTWARE\\Buka\\3DO\\Heroes of Might and Magic Platinum\\1.000", "HMM1 ",

		0x004710BE, 2, 0, "HEROES", 0x004B17FD, 0x004718FA, LNG_RUSSIAN, 0x004B193C, 2, 0x004B1BB8, TRUE,  0x004D46A1, 6, 0x004B1A25, 0x004B1AE0, 0x004B0E4D, 0x004BD200, // Heroes II Gold - Buka
		"Герои Меча и Магии II: Цена Верности",
		"SOFTWARE\\Buka\\3DO\\Heroes of Might and Magic Platinum\\1.000", "HMM2POL ",
		0x0041535E, 2, 0, "EDITOR", 0x0042CB19, 0x00415BB5, LNG_RUSSIAN, 0x0042CC58, 2, 0x0042CED4, TRUE,  0x00447E81, 6, 0x0042CD41, 0x0042CDFC, 0x0042C19D, 0x00431530, // Heroes II Gold Editor - Buka
		"Герои Меча и Магии II: Редактор",
		"SOFTWARE\\Buka\\3DO\\Heroes of Might and Magic Platinum\\1.000", "HMM2POL ",

		// ----------------------------------------------------------------------------

		0x0045B9AA, 1, 6, "HEROES", 0x004053C2, 0x0045C207, LNG_ENGLISH, 0x00405547, 6, 0x0040583D, FALSE, 0x004734D3, 2, 0x0040566A, 0x00405754, 0x0040490E, 0x00000000, // Heroes I v1.0
		"Heroes of Might and Magic: A Strategic Quest",
		"SOFTWARE\\New World Computing\\Heroes of Might and Magic\\1.0", "",
		0x0041C62A, 1, 6, "EDITOR", 0x0041BDEB, 0x0041CEAA, LNG_ENGLISH, 0x0041BF70, 6, 0x0041C266, FALSE, 0x00421743, 2, 0x0041C093, 0x0041C17D, 0x0041B398, 0x00000000, // Heroes I Editor v1.0
		"Heroes of Might and Magic: Editor",
		"SOFTWARE\\New World Computing\\Heroes of Might and Magic\\1.0", "",

		0x0045BAAA, 1, 6, "HEROES", 0x004053C2, 0x0045C307, LNG_ENGLISH, 0x00405547, 6, 0x0040583D, FALSE, 0x00473AA3, 2, 0x0040566A, 0x00405754, 0x0040490E, 0x00000000, // Heroes I v1.1
		"Heroes of Might and Magic: A Strategic Quest",
		"SOFTWARE\\New World Computing\\Heroes of Might and Magic\\1.0", "",
		0x0041C64A, 1, 6, "EDITOR", 0x0041BE0B, 0x0041CECA, LNG_ENGLISH, 0x0041BF90, 6, 0x0041C286, FALSE, 0x00421773, 2, 0x0041C0B3, 0x0041C19D, 0x0041B3B8, 0x00000000, // Heroes I Editor v1.1
		"Heroes of Might and Magic: Editor",
		"SOFTWARE\\New World Computing\\Heroes of Might and Magic\\1.0", "",

		0x00432B2A, 1, 6, "HEROES", 0x00436CE2, 0x00433387, LNG_ENGLISH, 0x00436E67, 6, 0x0043715D, FALSE, 0x00475CC3, 2, 0x00436F8A, 0x00437074, 0x0043622E, 0x00000000, // Heroes I v1.2
		"Heroes of Might and Magic: A Strategic Quest",
		"SOFTWARE\\New World Computing\\Heroes of Might and Magic\\1.0", "",
		0x004012BA, 1, 6, "EDITOR", 0x0041122A, 0x00401B39, LNG_ENGLISH, 0x004113AF, 6, 0x004116A5, FALSE, 0x00420CD3, 2, 0x004114D2, 0x004115BC, 0x004107D8, 0x00000000, // Heroes I Editor v1.2
		"Heroes of Might and Magic: Editor",
		"SOFTWARE\\New World Computing\\Heroes of Might and Magic\\1.0", "",

		// ----------------------------------------------------------------------------

		0x00409017, 2, 6, "HEROES", 0x004B3109, 0x00409819, LNG_ENGLISH, 0x004B32A0, 6, 0x004B35F5, FALSE, 0x004CDFAC, 2, 0x004B33E8, 0x004B34E8, 0x004B262A, 0x004BF5A0, // Heroes II v1.0
		"Heroes of Might and Magic II: The Succession Wars",
		"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",
		0x004028B7, 2, 6, "EDITOR", 0x00421FC9, 0x004030DB, LNG_ENGLISH, 0x00422160, 6, 0x004224AF, FALSE, 0x0043914C, 2, 0x004222A8, 0x004223A8, 0x00421546, 0x00432040, // Heroes II Editor v1.0
		"Heroes of Might and Magic II: Editor",
		"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",

		0x00408FE7, 2, 6, "HEROES", 0x004B37DD, 0x004097E9, LNG_ENGLISH, 0x004B398B, 6, 0x004B3CF2, FALSE, 0x004CE8CC, 2, 0x004B3AD3, 0x004B3BE5, 0x004B2D00, 0x004BFD70, // Heroes II v1.1
		"Heroes of Might and Magic II: The Succession Wars",
		"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",
		0x00402987, 2, 6, "EDITOR", 0x00421F8D, 0x004031AB, LNG_ENGLISH, 0x0042213B, 6, 0x0042249B, FALSE, 0x0043CF6C, 2, 0x00422283, 0x00422395, 0x0042150C, 0x00435F00, // Heroes II Editor v1.1
		"Heroes of Might and Magic II: Editor",
		"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",

		0x0045DE87, 2, 6, "HEROES", 0x004A383D, 0x0045E689, LNG_ENGLISH, 0x004A39EB, 6, 0x004A3D52, FALSE, 0x004D2C7C, 2, 0x004A3B33, 0x004A3C45, 0x004A2D60, 0x004C3440, // Heroes II v1.2
		"Heroes of Might and Magic II: The Succession Wars",
		"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",
		0x00426667, 2, 6, "EDITOR", 0x0042179D, 0x00426E8B, LNG_ENGLISH, 0x0042194B, 6, 0x00421CAC, FALSE, 0x0043CA5C, 2, 0x00421A93, 0x00421BA5, 0x00420D1C, 0x004368F0, // Heroes II Editor v1.2
		"Heroes of Might and Magic II: Editor",
		"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",

		0x00434317, 2, 6, "HEROES", 0x00437299, 0x00434B19, LNG_ENGLISH, 0x00437447, 6, 0x004377AE, FALSE, 0x004CF75C, 2, 0x0043758F, 0x004376A1, 0x004367BA, 0x004C5AF0, // Heroes II v1.3
		"Heroes of Might and Magic II: The Succession Wars",
		"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",
		0x00426667, 2, 6, "EDITOR", 0x0042179D, 0x00426EA0, LNG_ENGLISH, 0x0042194B, 6, 0x00421CAC, FALSE, 0x0043CA5C, 2, 0x00421A93, 0x00421BA5, 0x00420D1C, 0x004368F0, // Heroes II Editor v1.3
		"Heroes of Might and Magic II: Editor",
		"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",

		0x0041BFB7, 2, 6, "HEROES", 0x0043718D, 0x0041C7B9, LNG_ENGLISH, 0x0043733B, 6, 0x004376A2, FALSE, 0x004D85DC, 2, 0x00437483, 0x00437595, 0x004366B0, 0x004CE990, // Heroes II v2.0
		"Heroes of Might and Magic II: The Price of Loyalty",
		"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",
		0x0040FCD7, 2, 6, "EDITOR", 0x0042C8CD, 0x004104FC, LNG_ENGLISH, 0x0042CA64, 6, 0x0042CDB2, FALSE, 0x00439E6C, 2, 0x0042CBAC, 0x0042CCAC, 0x0042BE4C, 0x00434350, // Heroes II Editor v2.0
		"Heroes of Might and Magic II: Editor",
		"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",

		0x00484DC1, 2, 6, "HEROES", 0x0049CD06, 0x004855E1, LNG_ENGLISH, 0x0049CEC3, 6, 0x0049D26D, FALSE, 0x004D88CC, 2, 0x0049D030, 0x0049D160, 0x0049C1E0, 0x004D1480, // Heroes II v2.1
		"Heroes of Might and Magic II: The Price of Loyalty",
		"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",
		0x00423EF7, 2, 6, "EDITOR", 0x00418637, 0x0042471B, LNG_ENGLISH, 0x004187CE, 6, 0x00418B1D, FALSE, 0x0043F20C, 2, 0x00418916, 0x00418A16, 0x00417BB6, 0x00437C90, // Heroes II Editor v2.1
		"Heroes of Might and Magic II: Editor",
		"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",

		NULL
};

UINT menuIds[] = { IDM_FILT_OFF, IDM_FILT_LINEAR, IDM_FILT_XRBZ, IDM_ASPECT_RATIO, IDM_VSYNC, IDM_HELP_WRAPPER };

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
			MemorySet((VOID*)addr, 0x90, size);
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

	DWORD __fastcall PatchFunction(HMODULE hModule, const CHAR* function, VOID* addr)
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
					if (ReadDWord((DWORD)&addressThunk->u1.AddressOfData, &res) && PatchDWord((DWORD)&addressThunk->u1.AddressOfData, (DWORD)addr))
						return res;
					return NULL;
				}
			}
		}

		return NULL;
	}

	// ==========================================================================

	VOID __cdecl SwitchMode_v1(DWORD isFullscreen)
	{
		((VOID(__cdecl *)(DWORD))hookSpace->mode_call1)(2);
		((VOID(__cdecl *)(DWORD))hookSpace->mode_call2)(isFullscreen);
	}

	VOID __stdcall SwitchMode_v2(DWORD isFullscreen)
	{
		DWORD call_1 = hookSpace->mode_call1;
		DWORD call_2 = hookSpace->mode_call2;
		DWORD call_3 = hookSpace->mode_call3;

		__asm {
			MOV ECX, 2
			CALL call_1

			MOV ECX, isFullscreen
			CALL call_2

			XOR ECX, ECX
			XOR EDX, EDX
			PUSH isFullscreen
			CALL call_3
		}
	}

	VOID __declspec(naked) hook_mode_v1()
	{
		_asm { JMP SwitchMode_v1 }
	}

	VOID __declspec(naked) hook_mode_v2()
	{
		_asm
		{
			PUSH ECX
			CALL SwitchMode_v2
			RETN
		}
	}

	BOOL __stdcall AdjustWindowRectHook(LPRECT lpRect, DWORD dwStyle, BOOL bMenu)
	{
		if (dwStyle == STYLE_FULL_OLD)
			dwStyle = STYLE_FULL_NEW;

		return AdjustWindowRect(lpRect, dwStyle, bMenu);
	}

	HWND __stdcall CreateWindowExHook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, INT X, INT Y, INT nWidth, INT nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
	{
		if (dwStyle == STYLE_FULL_OLD)
			dwStyle = STYLE_FULL_NEW;

		return CreateWindow(lpClassName, hookSpace->windowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	}

	LONG __stdcall SetWindowLongHook(HWND hWnd, INT nIndex, LONG dwNewLong)
	{
		if (dwNewLong == STYLE_FULL_OLD)
			dwNewLong = STYLE_FULL_NEW;

		return SetWindowLong(hWnd, nIndex, dwNewLong);
	}

	BOOL __stdcall ScreenToClientHook(HWND hWnd, LPPOINT lpPoint)
	{
		if (ScreenToClient(hWnd, lpPoint))
		{
			DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
			if (ddraw && ddraw->imageAspect)
				ddraw->ScaleMouse(lpPoint);

			return TRUE;
		}

		return FALSE;
	}

	INT __stdcall MessageBoxHook(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
	{
		INT res;

		if (hActCtx && hActCtx != INVALID_HANDLE_VALUE)
		{
			ULONG_PTR cookie;
			ActivateActCtxC(hActCtx, &cookie);
			res = MessageBox(hWnd, lpText, lpCaption, uType);
			DeactivateActCtxC(0, cookie);
		}
		else
			res = MessageBox(hWnd, lpText, lpCaption, uType);
	
		return res;
	}

	HMENU __stdcall LoadMenuHook(HINSTANCE hInstance, LPCTSTR lpMenuName)
	{
		HMENU hMenu = LoadMenu(hInstance, lpMenuName);
		if (hMenu)
		{
			HMENU hNew = LoadMenu(hDllModule, MAKEINTRESOURCE(configLanguage == LNG_ENGLISH ? IDM_ENGLISH : IDM_RUSSIAN));
			if (hNew)
			{
				DWORD i, index = 0;

				HMENU hSub;
				for (i = 0; hSub = GetSubMenu(hMenu, i);)
				{
					DWORD itemId = GetMenuItemID(hSub, 0);
					if (itemId == IDM_RES_640_480 || itemId == IDM_HELP_MANUAL)
					{
						DeleteMenu(hMenu, i, MF_BYPOSITION);
						index = i;
					}
					else
						++i;
				}

				CHAR buffer[20];
				for (i = GetMenuItemCount(hNew); i; --i)
				{
					hSub = GetSubMenu(hNew, i - 1);

					GetMenuString(hNew, i - 1, buffer, 20, MF_BYPOSITION);
					InsertMenu(hMenu, index, MF_BYPOSITION | MF_POPUP, (UINT_PTR)hSub, buffer);
				}
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

	VOID __stdcall SleepHook(DWORD dwMilliseconds)
	{
		if (dwMilliseconds != 3000)
			Sleep(dwMilliseconds);
	}

	BOOL __stdcall EnumChildProc(HWND hDlg, LPARAM lParam)
	{
		if ((GetWindowLong(hDlg, GWL_STYLE) & SS_ICON) == SS_ICON)
			SendMessage(hDlg, STM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)configIcon);
		else
			SendMessage(hDlg, WM_SETFONT, (WPARAM)configFont, TRUE);

		return TRUE;
	}

	DLGPROC OldDialogProc;
	LRESULT __stdcall AboutProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_INITDIALOG)
		{
			SetWindowLong(hDlg, GWL_EXSTYLE, NULL);
			EnumChildWindows(hDlg, EnumChildProc, NULL);
		}
		return OldDialogProc(hDlg, uMsg, wParam, lParam);
	}

	INT_PTR __stdcall DialogBoxParamHook(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
	{
		OldDialogProc = lpDialogFunc;

		INT_PTR res;
		if (hActCtx && hActCtx != INVALID_HANDLE_VALUE)
		{
			ULONG_PTR cookie;
			ActivateActCtxC(hActCtx, &cookie);
				res = DialogBoxParam(hInstance, lpTemplateName, hWndParent, (DLGPROC)AboutProc, dwInitParam);
			DeactivateActCtxC(0, cookie);
		}
		else
			res = DialogBoxParam(hInstance, lpTemplateName, hWndParent, (DLGPROC)AboutProc, dwInitParam);

		return res;
	}

	DWORD back_cpu;
	VOID __declspec(naked) hook_cpu()
	{
		__asm
		{
			PUSH	0x1
			CALL	DWORD PTR[Sleep]
			MOV     EBP, ESP
			SUB     ESP, 0x20
			JMP		back_cpu
		}
	}

	HMODULE hModule;
	VOID __stdcall StartHook()
	{
		PatchFunction(hModule, "AdjustWindowRect", AdjustWindowRectHook);
		PatchFunction(hModule, "CreateWindowExA", CreateWindowExHook);
		PatchFunction(hModule, "SetWindowLongA", SetWindowLongHook);
		PatchFunction(hModule, "ScreenToClient", ScreenToClientHook);
		PatchFunction(hModule, "MessageBoxA", MessageBoxHook);

		PatchFunction(hModule, "LoadMenuA", LoadMenuHook);
		PatchFunction(hModule, "SetMenu", SetMenuHook);
		PatchFunction(hModule, "EnableMenuItem", EnableMenuItemHook);

		PatchFunction(hModule, "Sleep", SleepHook);
		PatchFunction(hModule, "DialogBoxParamA", DialogBoxParamHook);
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
			DWORD check;
			if (ReadDWord(hookSpace->check + 6, &check) && check == STYLE_FULL_OLD)
			{
				hModule = GetModuleHandle(NULL);
				PIMAGE_DOS_HEADER headDOS = (PIMAGE_DOS_HEADER)hModule;
				PIMAGE_NT_HEADERS headNT = (PIMAGE_NT_HEADERS)((DWORD)headDOS + headDOS->e_lfanew);

				DWORD address = (DWORD)headDOS + headNT->OptionalHeader.AddressOfEntryPoint + hookSpace->start_offset;
				start_back = address + 5;
				PatchHook(address, start_hook);

				PatchDWord(hookSpace->ddraw_dll + 1, (DWORD)"WING32.dll");

				PatchHook(hookSpace->cpu_hook, hook_cpu);
				back_cpu = hookSpace->cpu_hook + 5;

				PatchNop(hookSpace->method2_nop, hookSpace->method2_nop_size);
				if (hookSpace->method2_jmp_short)
					PatchByte(hookSpace->method2_jmp, 0xEB);
				else
					PatchWord(hookSpace->method2_jmp, 0xE990);

				PatchNop(hookSpace->mode_nop, hookSpace->mode_nop_size);
				PatchHook(hookSpace->mode_hook, hookSpace->game_version == 2 ? hook_mode_v2 : hook_mode_v1);

				configKey = hookSpace->configKey;
				configPrefix = hookSpace->configPrefix;
				configLanguage = hookSpace->resLanguage;
				configIcon = LoadIcon(hModule, hookSpace->icon);
				configFont = (HFONT)CreateFont(16, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
					OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_DONTCARE, TEXT("MS Sans Serif"));

				return TRUE;
			}

			hookSpace++;
		} while (hookSpace->check);

		return FALSE;
	}
}