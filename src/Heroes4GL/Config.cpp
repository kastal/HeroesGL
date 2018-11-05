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

const CHAR* configKey;
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

namespace Config
{
	DWORD __fastcall Get(const CHAR* name, DWORD def)
	{
		HKEY regKey;
		if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, configKey, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &regKey, NULL) == ERROR_SUCCESS)
		{
			BYTE data[256];
			DWORD size = 256;
			if (RegQueryValueEx(regKey, name, NULL, 0, data, &size) == ERROR_SUCCESS)
				def = *(DWORD*)data;

			RegCloseKey(regKey);
		}

		return def;
	}

	BOOL __fastcall Set(const CHAR* name, DWORD value)
	{
		BOOL res = FALSE;

		HKEY regKey;
		if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, configKey, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &regKey, NULL) == ERROR_SUCCESS)
		{
			res = RegSetValueEx(regKey, name, NULL, REG_DWORD, (BYTE*)&value, sizeof(DWORD)) == ERROR_SUCCESS;
			RegCloseKey(regKey);
		}

		return res;
	}
}