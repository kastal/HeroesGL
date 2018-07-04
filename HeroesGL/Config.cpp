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
const CHAR* configPrefix;
DWORD configLanguage;
HICON configIcon;
HFONT configFont;

namespace Config
{
	DWORD __fastcall Get(const CHAR* name, DWORD def)
	{
		HKEY regKeyGame;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, configKey, NULL, KEY_READ, &regKeyGame) == ERROR_SUCCESS)
		{
			CHAR valName[50];
			StrPrint(valName, "%s%s", configPrefix, name);

			BYTE data[256];
			DWORD size = 256;
			if (RegQueryValueEx(regKeyGame, valName, NULL, 0, data, &size) == ERROR_SUCCESS)
				def = *(DWORD*)data;

			RegCloseKey(regKeyGame);
		}

		return def;
	}

	BOOL __fastcall Set(const CHAR* name, DWORD value)
	{
		BOOL res = FALSE;

		HKEY regKeyGame;
		if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, configKey, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &regKeyGame, NULL) == ERROR_SUCCESS)
		{
			CHAR valName[50];
			StrPrint(valName, "%s%s", configPrefix, name);

			res = RegSetValueEx(regKeyGame, valName, NULL, REG_DWORD, (BYTE*)&value, sizeof(DWORD)) == ERROR_SUCCESS;

			RegCloseKey(regKeyGame);
		}

		return res;
	}
}