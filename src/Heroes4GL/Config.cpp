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
#include "Config.h"

DWORD configLanguage;
DWORD configDialog;
HICON configIcon;
HFONT configFont;

BOOL configColdCPU;

BOOL configImageAspect;
BOOL configImageVSync;

ImageFilter configImageFilter;
DWORD configImageScaleNx;
DWORD configImageXSal;
DWORD configImageEagle;
DWORD configImageScaleHQ;
DWORD configImageXBRZ;

BOOL configIsExist;
CHAR configFile[MAX_PATH];

namespace Config
{
	VOID __fastcall Load(HMODULE hModule, AddressSpace* hookSpace)
	{
		GetModuleFileName(hModule, configFile, MAX_PATH - 1);
		CHAR* p = StrLastChar(configFile, '\\');
		*p = NULL;
		StrCopy(p, "\\config.ini");

		FILE* file = FileOpen(configFile, "rb");
		if (file)
		{
			configIsExist = TRUE;
			FileClose(file);
		}

		configLanguage = hookSpace->resLanguage;
		configIcon = LoadIcon(hModule, MAKEINTRESOURCE(RESOURCE_ICON));
		configFont = (HFONT)CreateFont(16, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
			OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, TEXT("MS Shell Dlg"));

		configColdCPU = (BOOL)Config::Get(CONFIG_WRAPPER, "ColdCPU", FALSE);
		if (!isNoGL)
		{
			configImageAspect = (BOOL)Config::Get(CONFIG_WRAPPER, "ImageAspect", TRUE);
			configImageVSync = (BOOL)Config::Get(CONFIG_WRAPPER, "ImageVSync", TRUE);
			configImageFilter = (ImageFilter)Config::Get(CONFIG_WRAPPER, "ImageFilter", FilterNearest);
			configImageScaleNx = (DWORD)Config::Get(CONFIG_WRAPPER, "ImageScaleNx", 2);
			configImageScaleHQ = (DWORD)Config::Get(CONFIG_WRAPPER, "ImageScaleHQ", 2);
			configImageXBRZ = (DWORD)Config::Get(CONFIG_WRAPPER, "ImageXBRZ", 2);
		}
		else
		{
			configImageAspect = FALSE;
			configImageVSync = FALSE;
			configImageFilter = FilterNearest;
			configImageScaleNx = 2;
			configImageScaleHQ = 2;
			configImageXBRZ = 2;
		}
	}

	INT __fastcall Get(const CHAR* app, const CHAR* key, INT default)
	{
		return GetPrivateProfileInt(app, key, (INT)default, configFile);
	}

	DWORD __fastcall Get(const CHAR* app, const CHAR* key, CHAR* default, CHAR* returnString, DWORD nSize)
	{
		return GetPrivateProfileString(app, key, default, returnString, nSize, configFile);
	}

	BOOL __fastcall Set(const CHAR* app, const CHAR* key, INT value)
	{
		CHAR res[20];
		StrPrint(res, "%d", value);
		return WritePrivateProfileString(app, key, res, configFile);
	}

	BOOL __fastcall Set(const CHAR* app, const CHAR* key, CHAR* value)
	{
		return WritePrivateProfileString(app, key, value, configFile);
	}
}