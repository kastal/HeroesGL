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

AddressSpace* hookSpace;

AddressSpace addressArray[] = {
//	style,      style,      version,    DDRAW.dll,  scrToClt,   load menu,  set menu,   meth_2 NOP,    meth_2 JMP,        mode NOP,      mode hook,  call 1,     call 2,     call 3,    
	0x00442EFF, 0x00443C6D, 0x00000001, 0x0046815E, 0x0048A208, 0x0048A228, 0x0048A2C0, IDM_RUSSIAN, 0x0046827C, 2, 0x004684AA, TRUE,  0x0046F8EC, 6, 0x0046833C, 0x004683DA, 0x004677A0, 0x00000000, // Heroes I - Buka
	"SOFTWARE\\Buka\\3DO\\Heroes of Might and Magic Platinum\\1.000", "HMM1 ",
	0x0040CBFA, 0x0040D97E, 0x00000001, 0x0041AEA4, 0x00438274, 0x004381BC, 0x004381C4, IDM_RUSSIAN, 0x0041AFC2, 2, 0x0041B1EA, TRUE,  0x0042170C, 6, 0x0041B082, 0x0041B120, 0x0041A51D, 0x00000000, // Heroes I Editor - Buka
	"SOFTWARE\\Buka\\3DO\\Heroes of Might and Magic Platinum\\1.000", "HMM1 ",
	
	0x004710BE, 0x00471E18, 0x00000002, 0x004B17FD, 0x004EA228, 0x004EA208, 0x004EA244, IDM_RUSSIAN, 0x004B193C, 2, 0x004B1BB8, TRUE,  0x004D46A1, 6, 0x004B1A25, 0x004B1AE0, 0x004B0E4D, 0x004BD200, // Heroes II Gold - Buka
	"SOFTWARE\\Buka\\3DO\\Heroes of Might and Magic Platinum\\1.000", "HMM2POL ",
	0x0041535E, 0x004160D3, 0x00000002, 0x0042CB19, 0x0045B284, 0x0045B1C8, 0x0045B1D0, IDM_RUSSIAN, 0x0042CC58, 2, 0x0042CED4, TRUE,  0x00447E81, 6, 0x0042CD41, 0x0042CDFC, 0x0042C19D, 0x00431530, // Heroes II Gold Editor - Buka
	"SOFTWARE\\Buka\\3DO\\Heroes of Might and Magic Platinum\\1.000", "HMM2POL ",
	
	// ----------------------------------------------------------------------------
	
	0x0045B9AA, 0x0045C7C6, 0x00000001, 0x004053C2, 0x004D65E4, 0x004D6678, 0x004D6668, IDM_ENGLISH, 0x00405547, 6, 0x0040583D, FALSE, 0x004734D3, 2, 0x0040566A, 0x00405754, 0x0040490E, 0x00000000, // Heroes I v1.0
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic\\1.0", "",
	0x0041C62A, 0x0041D469, 0x00000001, 0x0041BDEB, 0x0044D54C, 0x0044D48C, 0x0044D4B0, IDM_ENGLISH, 0x0041BF70, 6, 0x0041C266, FALSE, 0x00421743, 2, 0x0041C093, 0x0041C17D, 0x0041B398, 0x00000000, // Heroes I Editor v1.0
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic\\1.0", "",

	0x0045BAAA, 0x0045C8C6, 0x00000001, 0x004053C2, 0x004D75E4, 0x004D7678, 0x004D7668, IDM_ENGLISH, 0x00405547, 6, 0x0040583D, FALSE, 0x00473AA3, 2, 0x0040566A, 0x00405754, 0x0040490E, 0x00000000, // Heroes I v1.1
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic\\1.0", "",
	0x0041C64A, 0x0041D489, 0x00000001, 0x0041BE0B, 0x0044D54C, 0x0044D48C, 0x0044D4B0, IDM_ENGLISH, 0x0041BF90, 6, 0x0041C286, FALSE, 0x00421773, 2, 0x0041C0B3, 0x0041C19D, 0x0041B3B8, 0x00000000, // Heroes I Editor v1.1
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic\\1.0", "",

	0x00432B2A, 0x00433946, 0x00000001, 0x00436CE2, 0x004D95F0, 0x004D9610, 0x004D9624, IDM_ENGLISH, 0x00436E67, 6, 0x0043715D, FALSE, 0x00475CC3, 2, 0x00436F8A, 0x00437074, 0x0043622E, 0x00000000, // Heroes I v1.2
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic\\1.0", "",
	0x004012BA, 0x004020F8, 0x00000001, 0x0041122A, 0x0044E568, 0x0044E510, 0x0044E53C, IDM_ENGLISH, 0x004113AF, 6, 0x004116A5, FALSE, 0x00420CD3, 2, 0x004114D2, 0x004115BC, 0x004107D8, 0x00000000, // Heroes I Editor v1.2
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic\\1.0", "",

	// ----------------------------------------------------------------------------

	0x00409017, 0x00409E2D, 0x00000002, 0x004B3109, 0x00528654, 0x005286D4, 0x005286EC, IDM_ENGLISH, 0x004B32A0, 6, 0x004B35F5, FALSE, 0x004CDFAC, 2, 0x004B33E8, 0x004B34E8, 0x004B262A, 0x004BF5A0, // Heroes II v1.0
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",
	0x004028B7, 0x004036EF, 0x00000002, 0x00421FC9, 0x0048A590, 0x0048A4DC, 0x0048A4E8, IDM_ENGLISH, 0x00422160, 6, 0x004224AF, FALSE, 0x0043914C, 2, 0x004222A8, 0x004223A8, 0x00421546, 0x00432040, // Heroes II Editor v1.0
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",

	0x00408FE7, 0x00409DFD, 0x00000002, 0x004B37DD, 0x00526640, 0x005266A4, 0x005266BC, IDM_ENGLISH, 0x004B398B, 6, 0x004B3CF2, FALSE, 0x004CE8CC, 2, 0x004B3AD3, 0x004B3BE5, 0x004B2D00, 0x004BFD70, // Heroes II v1.1
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",
	0x00402987, 0x004037BF, 0x00000002, 0x00421F8D, 0x0048E548, 0x0048E490, 0x0048E498, IDM_ENGLISH, 0x0042213B, 6, 0x0042249B, FALSE, 0x0043CF6C, 2, 0x00422283, 0x00422395, 0x0042150C, 0x00435F00, // Heroes II Editor v1.1
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",

	0x0045DE87, 0x0045EC9D, 0x00000002, 0x004A383D, 0x005266A0, 0x00526674, 0x00526688, IDM_ENGLISH, 0x004A39EB, 6, 0x004A3D52, FALSE, 0x004D2C7C, 2, 0x004A3B33, 0x004A3C45, 0x004A2D60, 0x004C3440, // Heroes II v1.2
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",
	0x00426667, 0x0042749F, 0x00000002, 0x0042179D, 0x0048E550, 0x0048E494, 0x0048E49C, IDM_ENGLISH, 0x0042194B, 6, 0x00421CAC, FALSE, 0x0043CA5C, 2, 0x00421A93, 0x00421BA5, 0x00420D1C, 0x004368F0, // Heroes II Editor v1.2
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",

	0x00434317, 0x0043512D, 0x00000002, 0x00437299, 0x00528670, 0x005286B4, 0x005286E0, IDM_ENGLISH, 0x00437447, 6, 0x004377AE, FALSE, 0x004CF75C, 2, 0x0043758F, 0x004376A1, 0x004367BA, 0x004C5AF0, // Heroes II v1.3
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",
	0x00426667, 0x0042749F, 0x00000002, 0x0042179D, 0x0048E550, 0x0048E494, 0x0048E49C, IDM_ENGLISH, 0x0042194B, 6, 0x00421CAC, FALSE, 0x0043CA5C, 2, 0x00421A93, 0x00421BA5, 0x00420D1C, 0x004368F0, // Heroes II Editor v1.3
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",

	0x0041BFB7, 0x0041CDCD, 0x00000002, 0x0043718D, 0x0053A6FC, 0x0053A648, 0x0053A6E0, IDM_ENGLISH, 0x0043733B, 6, 0x004376A2, FALSE, 0x004D85DC, 2, 0x00437483, 0x00437595, 0x004366B0, 0x004CE990, // Heroes II v2.0
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",
	0x0040FCD7, 0x00410B10, 0x00000002, 0x0042C8CD, 0x0049354C, 0x004934C4, 0x00493500, IDM_ENGLISH, 0x0042CA64, 6, 0x0042CDB2, FALSE, 0x00439E6C, 2, 0x0042CBAC, 0x0042CCAC, 0x0042BE4C, 0x00434350, // Heroes II Editor v2.0
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",

	0x00484DC1, 0x00485C1C, 0x00000002, 0x0049CD06, 0x0053967C, 0x005396B0, 0x005396C0, IDM_ENGLISH, 0x0049CEC3, 6, 0x0049D26D, FALSE, 0x004D88CC, 2, 0x0049D030, 0x0049D160, 0x0049C1E0, 0x004D1480, // Heroes II v2.1
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",
	0x00423EF7, 0x00424D2F, 0x00000002, 0x00418637, 0x00494580, 0x00494568, 0x00494544, IDM_ENGLISH, 0x004187CE, 6, 0x00418B1D, FALSE, 0x0043F20C, 2, 0x00418916, 0x00418A16, 0x00417BB6, 0x00437C90, // Heroes II Editor v2.1
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0", "",

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

	BOOL __fastcall ReadDWord(DWORD addr, DWORD* value)
	{
		return ReadBlock(addr, value, sizeof(value));
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

	HMENU __stdcall LoadMenuHook(HINSTANCE hInstance, LPCTSTR lpMenuName)
	{
		HMENU hMenu = LoadMenu(hInstance, lpMenuName);
		if (hMenu)
		{
			CHAR buffer[20];
			if (GetMenuString(hMenu, 1, buffer, 20, MF_BYPOSITION) && DeleteMenu(hMenu, 1, MF_BYPOSITION))
			{
				HMENU hDisplay = LoadMenu(hDllModule, MAKEINTRESOURCE(hookSpace->resMenu));
				if (hDisplay)
				{
					InsertMenu(hMenu, 1, MF_BYPOSITION | MF_POPUP, (UINT_PTR)hDisplay, buffer);
					DestroyMenu(hDisplay);
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

	BOOL Load()
	{
		hookSpace = addressArray;

		do
		{
			DWORD check;
			if (ReadDWord(hookSpace->style_check + 6, &check))
			{
				if (check == (WS_VISIBLE | WS_CLIPSIBLINGS))
				{
					PatchDWord(hookSpace->style_check + 6, (WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU));
					PatchDWord(hookSpace->style_icon + 6, WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU);

					PatchDWord(hookSpace->ddraw_dll + 1, (DWORD)"WING32.dll");

					PatchNop(hookSpace->method2_nop, hookSpace->method2_nop_size);
					if (hookSpace->method2_jmp_short)
						PatchByte(hookSpace->method2_jmp, 0xEB);
					else
						PatchWord(hookSpace->method2_jmp, 0xE990);

					PatchNop(hookSpace->mode_nop, hookSpace->mode_nop_size);
					PatchHook(hookSpace->mode_hook, hookSpace->game_version == 2 ? hook_mode_v2 : hook_mode_v1);

					PatchDWord(hookSpace->screenToClient, (DWORD)ScreenToClientHook);
					PatchDWord(hookSpace->loadMenu, (DWORD)LoadMenuHook);
					PatchDWord(hookSpace->setMenu, (DWORD)SetMenuHook);

					configKey = hookSpace->configKey;
					configPrefix = hookSpace->configPrefix;

					return TRUE;
				}
			}

			hookSpace++;
		} while (hookSpace->style_check);

		return FALSE;
	}
}