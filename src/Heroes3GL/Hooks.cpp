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
#define STYLE_FULL_NEW (WS_VISIBLE | WS_POPUP | WS_SYSMENU | WS_CLIPSIBLINGS)

#define STYLE_WIN_OLD (WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
#define STYLE_WIN_NEW (WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX | WS_CLIPSIBLINGS)

#define STYLE_DIALOG (DS_MODALFRAME | WS_POPUP)
#define RESOURCE_ICON 115

AddressSpace* hookSpace;

AddressSpace addressArray[] = {
	// check 1 & 2,         cpu patch,  renderNop                                                                                     move time,  move value, move hook

	// === RUS ======================================================================================================================================
#pragma region RUS
	0x004D4A5F, 0x0059308B, 0x00000000, 0x00000000, 0x00592D72, 0x004E6BAF, 0x004E6C1D, 0x004E6C22, 0x005B88F0, 0x005B8904, 0x004759BD, LNG_RUSSIAN, // Heroes III Erathia - v1.0 Buka
	"Ãåðîè Ìå÷à è Ìàãèè III: Âîçðîæäåíèå Ýðàôèè",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	0x004F2533, 0x005F9649, 0x00000000, 0x00000000, 0x005F9336, 0x005074CF, 0x0050753B, 0x00507540, 0x0063569C, 0x00635688, 0x0047DFF6, LNG_RUSSIAN, // Heroes III Armageddon - v2.1 Buka
	"Ãåðîè Ìå÷à è Ìàãèè III: Êëèíîê Àðìàãåääîíà",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	0x004F7EB3, 0x00602379, 0x00000000, 0x00000000, 0x00602066, 0x0050D58F, 0x0050D5FB, 0x0050D600, 0x0063C6BC, 0x0063C6A8, 0x0048051E, LNG_RUSSIAN, // Heroes III Shadow - Buka
	"Ãåðîè Ìå÷à è Ìàãèè III: Äûõàíèå Cìåðòè",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	0x004F7EB3, 0x006021A9, 0x00000000, 0x00000000, 0x00601E96, 0x0050DB1F, 0x0050DB8B, 0x0050DB90, 0x0063E6EC, 0x0063E6D8, 0x0048016E, LNG_RUSSIAN, // Heroes III Complete - v4.0 Buka
	"Ãåðîè Ìå÷à è Ìàãèè III: Ïîëíîå Ñîáðàíèå",
	"SOFTWARE\\Buka\\3DO\\Heroes of Might and Magic III Complete Platinum",

	// ----------------------------------------------------------------------------------------------------------------------------------------------

	0x004EBA34, 0x005AF329, 0x00000000, 0x00000000, 0x005AF016, 0x004FFFAF, 0x0050001B, 0x00500020, 0x005EB1CC, 0x005EB1B8, 0x0047AF86, LNG_RUSSIAN, // Heroes Chronicles Warlords & Underworld & Elements & Dragons - v1.0
	"Õðîíèêè ãåðîåâ",
	"SOFTWARE\\New World Computing\\Heroes Chronicles\\1.0",

	0x004EB494, 0x005AF2D9, 0x004027E9, 0x00643234, 0x005AEFC6, 0x004FF98F, 0x004FF9FB, 0x004FFA00, 0x005EB1CC, 0x005EB1B8, 0x0047AF36, LNG_RUSSIAN, // Heroes Chronicles Beastmaster - v1.0
	"Õðîíèêè ãåðîåâ: Âîññòàíèå",
	"SOFTWARE\\New World Computing\\Heroes Chronicles\\1.0",

	0x004EB494, 0x005AF2D9, 0x004027E9, 0x0064322C, 0x005AEFC6, 0x004FF98F, 0x004FF9FB, 0x004FFA00, 0x005EB1CC, 0x005EB1B8, 0x0047AF36, LNG_RUSSIAN, // Heroes Chronicles Sword - v1.0
	"Õðîíèêè ãåðîåâ: Ëåäÿíîé Êëèíîê",
	"SOFTWARE\\New World Computing\\Heroes Chronicles\\1.0",
#pragma endregion 

	// === ENG ======================================================================================================================================
#pragma region ENG
	0x004D3363, 0x0058E558, 0x00000000, 0x00000000, 0x0058E246, 0x004E568F, 0x004E56FB, 0x004E5700, 0x005B58B4, 0x005B58A0, 0x004753D2, LNG_ENGLISH, // Heroes III Erathia - v1.0
	"Heroes of Might and Magic III: The Restoration of Erathia",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	0x0041E573, 0x004210B8, 0x00000000, 0x00000000, 0x00420DA6, 0x004179AF, 0x00417A1B, 0x00417A20, 0x005B7D64, 0x005B7D50, 0x0048B7A2, LNG_ENGLISH, // Heroes III Erathia - v1.1
	"Heroes of Might and Magic III: The Restoration of Erathia",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	0x0041E523, 0x00421078, 0x00000000, 0x00000000, 0x00420D66, 0x0041797F, 0x004179EB, 0x004179F0, 0x005B6D54, 0x005B6D40, 0x0048BB12, LNG_ENGLISH, // Heroes III Erathia - v1.2
	"Heroes of Might and Magic III: The Restoration of Erathia",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	0x004F58F3, 0x005D9679, 0x00000000, 0x00000000, 0x005D9366, 0x0050ABEF, 0x0050AC5B, 0x0050AC60, 0x00613644, 0x00613630, 0x0047F81E, LNG_ENGLISH, // Heroes III Erathia - v1.3
	"Heroes of Might and Magic III: The Restoration of Erathia",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	0x004F5583, 0x005D8F69, 0x00000000, 0x00000000, 0x005D8C56, 0x0050AAAF, 0x0050AB1B, 0x0050AB20, 0x00613644, 0x00613630, 0x0047FC3E, LNG_ENGLISH, // Heroes III Erathia - v1.4
	"Heroes of Might and Magic III: The Restoration of Erathia",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	// ----------------------------------------------------------------------------------------------------------------------------------------------

	0x004EB283, 0x005EEFD8, 0x00000000, 0x00000000, 0x005EECC6, 0x0050041F, 0x0050048B, 0x00500490, 0x00628F28, 0x00628F14, 0x0047A472, LNG_ENGLISH, // Heroes III Armageddon - v2.0
	"Heroes of Might and Magic III: Armageddon’s Blade",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	0x004F5C43, 0x00600299, 0x00000000, 0x00000000, 0x005FFF86, 0x0050B6AF, 0x0050B71B, 0x0050B720, 0x0063B68C, 0x0063B678, 0x0048023E, LNG_ENGLISH, // Heroes III Armageddon - v2.1
	"Heroes of Might and Magic III: Armageddon’s Blade",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	0x004F5963, 0x005FFBF9, 0x00000000, 0x00000000, 0x005FF8E6, 0x0050B1BF, 0x0050B22B, 0x0050B230, 0x0063B68C, 0x0063B678, 0x0048052E, LNG_ENGLISH, // Heroes III Armageddon - v2.2
	"Heroes of Might and Magic III: Armageddon’s Blade",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	// ----------------------------------------------------------------------------------------------------------------------------------------------

	0x004F7D73, 0x006027E9, 0x00000000, 0x00000000, 0x006024D6, 0x0050D93F, 0x0050D9AB, 0x0050D9B0, 0x0063E6DC, 0x0063E6C8, 0x004802DE, LNG_ENGLISH, // Heroes III Shadow - v3.0
	"Heroes of Might and Magic III: The Shadow of Death",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	0x004F85B3, 0x006027E9, 0x00000000, 0x00000000, 0x006024D6, 0x0050DE4F, 0x0050DEBB, 0x0050DEC0, 0x0063E6DC, 0x0063E6C8, 0x004802EE, LNG_ENGLISH, // Heroes III Shadow - v3.1
	"Heroes of Might and Magic III: The Shadow of Death",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	0x004F8193, 0x00602149, 0x00000000, 0x00000000, 0x00601E36, 0x0050D8CF, 0x0050D93B, 0x0050D940, 0x0063D6DC, 0x0063D6C8, 0x0047FE9E, LNG_ENGLISH, // Heroes III Shadow - v3.2
	"Heroes of Might and Magic III: The Shadow of Death",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	// ----------------------------------------------------------------------------------------------------------------------------------------------

	0x004F7B03, 0x00601B89, 0x00000000, 0x00000000, 0x00601876, 0x0050D6CF, 0x0050D73B, 0x0050D740, 0x0063D6DC, 0x0063D6C8, 0x0048057E, LNG_ENGLISH, // Heroes III Complete - v4.0
	"Heroes of Might and Magic III: Complete",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	// ==============================================================================================================================================

	0x004F0033, 0x005B5129, 0x004022FD, 0x0063B6C0, 0x005B4E16, 0x00504C8F, 0x00504CFB, 0x00504D00, 0x005EFE3C, 0x005EFE28, 0x0047DF4E, LNG_ENGLISH, // Heroes Chronicles Warlords - v1.0
	"Heroes Chronicles: Warlords of the Wasteland",
	"SOFTWARE\\New World Computing\\Heroes Chronicles\\1.0",

	0x004F0033, 0x005B5129, 0x004022FD, 0x0063B6B8, 0x005B4E16, 0x00504C8F, 0x00504CFB, 0x00504D00, 0x005EFE3C, 0x005EFE28, 0x0047DF4E, LNG_ENGLISH, // Heroes Chronicles Underworld - v1.0
	"Heroes Chronicles: Conquest of the Underworld",
	"SOFTWARE\\New World Computing\\Heroes Chronicles\\1.0",

	0x004EFE04, 0x005B5469, 0x004022FD, 0x0063B6C0, 0x005B5156, 0x00504B6F, 0x00504BDB, 0x00504BE0, 0x005EFE3C, 0x005EFE28, 0x0047DA0E, LNG_ENGLISH, // Heroes Chronicles Elements - v1.0
	"Heroes Chronicles: Masters of the Elements",
	"SOFTWARE\\New World Computing\\Heroes Chronicles\\1.0",

	0x004EFE04, 0x005B5469, 0x004022FD, 0x0063B6B8, 0x005B5156, 0x00504B6F, 0x00504BDB, 0x00504BE0, 0x005EFE3C, 0x005EFE28, 0x0047DA0E, LNG_ENGLISH, // Heroes Chronicles Dragons - v1.0
	"Heroes Chronicles: Clash of the Dragons",
	"SOFTWARE\\New World Computing\\Heroes Chronicles\\1.0",

	0x004EFA84, 0x005B51B9, 0x00000000, 0x00000000, 0x005B4EA6, 0x00504B6F, 0x00504BDB, 0x00504BE0, 0x005EFE3C, 0x005EFE28, 0x0047D8DE, LNG_ENGLISH, // Heroes Chronicles WorldTree - v1.0
	"Heroes Chronicles: The World Tree",
	"SOFTWARE\\New World Computing\\Heroes Chronicles\\1.0",

	0x004EF824, 0x005B5249, 0x00000000, 0x00000000, 0x005B4F36, 0x0050418F, 0x005041FB, 0x00504200, 0x005F11BC, 0x005F11A8, 0x0047D36E, LNG_ENGLISH, // Heroes Chronicles FieryMoon - v1.0
	"Heroes Chronicles: The Fiery Moon",
	"SOFTWARE\\New World Computing\\Heroes Chronicles\\1.0",

	0x004EF874, 0x005B4C09, 0x00401050, 0x00636868, 0x005B48F6, 0x0050460F, 0x0050467B, 0x00504680, 0x005F11CC, 0x005F11B8, 0x0047D5FE, LNG_ENGLISH, // Heroes Chronicles Beastmaster - v1.0
	"Heroes Chronicles: Revolt of the Beastmasters",
	"SOFTWARE\\New World Computing\\Heroes Chronicles\\1.0",

	0x004EF874, 0x005B4C09, 0x00401050, 0x00636878, 0x005B48F6, 0x0050460F, 0x0050467B, 0x00504680, 0x005F11CC, 0x005F11B8, 0x0047D5FE, LNG_ENGLISH, // Heroes Chronicles Sword - v1.0
	"Heroes Chronicles: The Sword of Frost",
	"SOFTWARE\\New World Computing\\Heroes Chronicles\\1.0",
#pragma endregion

	// === GER ======================================================================================================================================
#pragma region GER
	0x004D5253, 0x00591B29, 0x00000000, 0x00000000, 0x00591816, 0x004E714F, 0x004E71BB, 0x004E71C0, 0x005B88B4, 0x005B88A0, 0x004761CE, LNG_ENGLISH, // Heroes III Erathia - v1.2
	"Heroes of Might and Magic III: The Restoration of Erathia",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",

	// ==============================================================================================================================================

	0x004EFA04, 0x005B51C9, 0x00000000, 0x00000000, 0x005B4EB6, 0x0050475F, 0x005047CB, 0x005047D0, 0x005F11BC, 0x005F11A8, 0x0047D25E, LNG_ENGLISH, // Heroes Chronicles Dragons - GOG - v1.0
	"Heroes Chronicles: Clash of the Dragons",
	"SOFTWARE\\New World Computing\\Heroes Chronicles\\1.0",
#pragma endregion

	// ==============================================================================================================================================
#pragma region Others
	0x004F8193, 0x00602149, 0x0067FEB2, 0x00352E33, 0x00601E36, 0x0050D8CF, 0x0050D93B, 0x0050D940, 0x0063D6DC, 0x0063D6C8, 0x0047FE9E, LNG_ENGLISH, // Heroes III WoG - v3.58f
	"Heroes of Might and Magic III: In the Wake of Gods",
	"SOFTWARE\\New Life of Heroes\\Heroes of Might and Magic III\\3.5",

	0x004F8193, 0x00602149, 0x0067FEB2, 0x00506F4D, 0x00601E36, 0x0050D8CF, 0x0050D93B, 0x0050D940, 0x0063D6DC, 0x0063D6C8, 0x0047FE9E, LNG_ENGLISH, // Heroes III MoP - v3.7.2.7
	"Heroes of Might and Magic III: Master of Puppets",
	"SOFTWARE\\New Life of Heroes\\Heroes of Might and Magic III\\MoP",

	0x004F8193, 0x00602149, 0x00639C01, 0x00639BF4, 0x00601E36, 0x0050D8CF, 0x0050D93B, 0x0050D940, 0x0063D6DC, 0x0063D6C8, 0x0047FE9E, LNG_ENGLISH, // Heroes III HotA - v1.5.3
	"Heroes of Might and Magic III: Horn of the Abyss",
	"SOFTWARE\\New World Computing\\Heroes of Might and Magic® III\\1.0",
#pragma endregion

	NULL
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

		return CreateWindow(lpClassName, hookSpace->windowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
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
			OpenDraw* ddraw = Main::FindOpenDrawByWindow(hWnd);
			if (ddraw)
				ddraw->ScaleMouse(lpPoint);

			return TRUE;
		}

		return FALSE;
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
					if (itemId == IDM_FILE_QUIT || itemId == IDM_RES_FULL_SCREEN || itemId == IDM_HELP_MANUAL
						|| itemId == IDM_HELP_ABOUT) // for GOG releases
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
			RECT rect, offset;
			GetClientRect(hDlg, &rect);
			GetWindowRect(hDlg, &offset);
			OffsetRect(&rect, offset.left, offset.top);
			AdjustWindowRect(&rect, STYLE_DIALOG, FALSE);
			SetWindowLong(hDlg, GWL_STYLE, STYLE_DIALOG);
			SetWindowLong(hDlg, GWL_EXSTYLE, NULL);
			MoveWindow(hDlg, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
			EnumChildWindows(hDlg, EnumChildProc, NULL);
		}
		return OldDialogProc(hDlg, uMsg, wParam, lParam);
	}

	INT_PTR __stdcall DialogBoxParamHook(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
	{
		OldDialogProc = lpDialogFunc;

		INT_PTR res;
		ULONG_PTR cookie = NULL;
		if (hActCtx && hActCtx != INVALID_HANDLE_VALUE && !ActivateActCtxC(hActCtx, &cookie))
			cookie = NULL;

		res = DialogBoxParam(hInstance, lpTemplateName, hWndParent, (DLGPROC)AboutProc, dwInitParam);

		if (cookie)
			DeactivateActCtxC(0, cookie);

		return res;
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

	DWORD posIndex;
	DWORD moveCounter;
	VOID __stdcall CalcRunPos(VOID* obj)
	{
		if (posIndex % moveCounter)
		{
			DWORD* pos = (DWORD*)obj + 128;
			if (*pos)
				--*pos;
			else
				*pos = 7;
		}

		++posIndex;
	}

	DWORD back_move;
	VOID __declspec(naked) hook_move()
	{
		__asm
		{
			PUSH ECX
			PUSH EAX

			PUSH ESI
			CALL CalcRunPos

			POP EAX
			POP ECX
			PUSH 544
			JMP back_move
		}
	}

	BOOL __stdcall PeekMessageHook(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
	{
		if (PeekMessage(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg))
			return TRUE;

		if (configColdCPU)
			Sleep(1);

		return FALSE;
	}

	BOOL Load()
	{
		hookSpace = addressArray;
		hModule = GetModuleHandle(NULL);
		baseOffset = (INT)hModule - BASE_ADDRESS;

		AddressSpace* defaultSpace = NULL;
		AddressSpace* equalSpace = NULL;

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
		} while (hookSpace->check_1);

		hookSpace = equalSpace ? equalSpace : defaultSpace;
		if (hookSpace)
		{
			{
				PatchFunction(hModule, "MessageBoxA", MessageBoxHook);

				PatchFunction(hModule, "LoadMenuA", LoadMenuHook);
				PatchFunction(hModule, "SetMenu", SetMenuHook);
				PatchFunction(hModule, "EnableMenuItem", EnableMenuItemHook);

				PatchFunction(hModule, "Sleep", SleepHook);
				PatchFunction(hModule, "DialogBoxParamA", DialogBoxParamHook);
				PatchFunction(hModule, "PeekMessageA", PeekMessageHook);

				PatchFunction(hModule, "DirectDrawCreate", Main::DirectDrawCreate);

				AIL_waveOutOpen = (AIL_WAVEOUTOPEN)PatchFunction(hModule, "_AIL_waveOutOpen@16", AIL_waveOutOpenHook);

				if (!isNoGL)
				{
					PatchFunction(hModule, "AdjustWindowRect", AdjustWindowRectHook);
					PatchFunction(hModule, "AdjustWindowRectEx", AdjustWindowRectExHook);
					PatchFunction(hModule, "CreateWindowExA", CreateWindowExHook);
					PatchFunction(hModule, "SetWindowLongA", SetWindowLongHook);

					PatchFunction(hModule, "GetDeviceCaps", GetDeviceCapsHook);
					PatchFunction(hModule, "ScreenToClient", ScreenToClientHook);
				}
			}

			if (!isNoGL)
				PatchNop(hookSpace->renderNop, 5); // prevent on WM_PAINT

			DEVMODE devMode;
			MemoryZero(&devMode, sizeof(DEVMODE));
			devMode.dmSize = sizeof(DEVMODE);
			DWORD cursorTime;
			if (EnumDisplaySettings(NULL, ENUM_REGISTRY_SETTINGS, &devMode) && devMode.dmDisplayFrequency)
				cursorTime = 1000 / devMode.dmDisplayFrequency;
			else
				cursorTime = 16;

			// ==========================================================
			PatchByte(hookSpace->cursor_time_1 + 2, (BYTE)cursorTime);
			PatchByte(hookSpace->cursor_time_2 + 2, (BYTE)cursorTime);
			PatchDWord(hookSpace->cursor_time_3 + 1, cursorTime);

			// ==========================================================
			moveCounter = DWORD(50.0f / cursorTime);

			PatchDWord(hookSpace->move_timeout + 4, cursorTime);
			PatchDWord(hookSpace->move_timeout + 8, cursorTime);
			PatchDWord(hookSpace->move_timeout + 12, cursorTime);

			PatchDWord(hookSpace->move_distance + 4, DWORD((FLOAT)cursorTime / 50.0f * 8.0f));
			PatchDWord(hookSpace->move_distance + 8, DWORD((FLOAT)cursorTime / 50.0f * 12.0f));
			PatchDWord(hookSpace->move_distance + 12, DWORD((FLOAT)cursorTime / 50.0f * 16.0f));

			PatchHook(hookSpace->move_address, hook_move);
			back_move = hookSpace->move_address + 5;
			// ==========================================================

			configKey = hookSpace->configKey;
			configLanguage = hookSpace->resLanguage;
			configIcon = LoadIcon(hModule, MAKEINTRESOURCE(RESOURCE_ICON));
			configFont = (HFONT)CreateFont(16, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
				OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_DONTCARE, TEXT("MS Shell Dlg"));

			configColdCPU = Config::Get("ColdCPU", FALSE);
			if (!isNoGL)
			{
				configImageAspect = Config::Get("ImageAspect", TRUE);
				configImageVSync = Config::Get("ImageVSync", TRUE);
				configImageFilter = (ImageFilter)Config::Get("ImageFilter", FilterNearest);
				configImageScaleNx = Config::Get("ImageScaleNx", 2);
				configImageScaleHQ = Config::Get("ImageScaleHQ", 2);
				configImageXBRZ = Config::Get("ImageXBRZ", 2);
			}
			else
			{
				configImageAspect = FALSE;
				configImageVSync = FALSE;
				configImageFilter = FilterNearest;
				configImageScaleNx = 1;
				configImageScaleHQ = 1;
				configImageXBRZ = 1;
			}

			return TRUE;
		}

		return FALSE;
	}
}