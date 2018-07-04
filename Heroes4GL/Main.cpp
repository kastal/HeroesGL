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
#include "stdio.h"
#include "math.h"
#include "Main.h"

DirectDraw* ddrawList;
DisplayMode modesList[3] = {
	800, 600, FALSE,
	1024, 768, FALSE,
	1280, 1024, FALSE
};

namespace Main
{
	HRESULT __stdcall DirectDrawCreateEx(GUID* lpGuid, LPVOID* lplpDD, REFIID iid, IUnknown* pUnkOuter)
	{
		ddrawList = new DirectDraw(ddrawList);
		*(DirectDraw**)lplpDD = ddrawList;
		return DD_OK;
	}

	DirectDraw* __fastcall FindDirectDrawByWindow(HWND hWnd)
	{
		DirectDraw* ddraw = ddrawList;
		while (ddraw)
		{
			if (ddraw->hWnd == hWnd || ddraw->hDraw == hWnd)
				return ddraw;

			ddraw = ddraw->last;
		}

		return NULL;
	}

	INT __fastcall Round(FLOAT number)
	{
		FLOAT floorVal = MathFloor(number);
		return INT(floorVal + 0.5f > number ? floorVal : MathCeil(number));
	}

	VOID __fastcall ShowError(CHAR* message, CHAR* file, DWORD line)
	{
		CHAR dest[400];
		StrPrint(dest, "%s\n\n\nFILE %s\nLINE %d", message, file, line);

		if (hActCtx && hActCtx != INVALID_HANDLE_VALUE)
		{
			ULONG_PTR cookie;
			ActivateActCtxC(hActCtx, &cookie);
			MessageBox(NULL, dest, "Error", MB_OK | MB_ICONERROR);
			DeactivateActCtxC(0, cookie);
		}
		else
			MessageBox(NULL, dest, "Error", MB_OK | MB_ICONERROR);
	}

#ifdef _DEBUG
	VOID __fastcall CheckError(CHAR* file, DWORD line)
	{
		DWORD statusCode = GLGetError();

		CHAR* message;

		if (statusCode != GL_NO_ERROR)
		{
			switch (statusCode)
			{
			case GL_INVALID_ENUM:
				message = "GL_INVALID_ENUM";
				break;

			case GL_INVALID_VALUE:
				message = "GL_INVALID_VALUE";
				break;

			case GL_INVALID_OPERATION:
				message = "GL_INVALID_OPERATION";
				break;

			case GL_INVALID_FRAMEBUFFER_OPERATION:
				message = "GL_INVALID_FRAMEBUFFER_OPERATION";
				break;

			case GL_OUT_OF_MEMORY:
				message = "GL_OUT_OF_MEMORY";
				break;

			case GL_STACK_UNDERFLOW:
				message = "GL_STACK_UNDERFLOW";
				break;

			case GL_STACK_OVERFLOW:
				message = "GL_STACK_OVERFLOW";
				break;

			default:
				message = "GL_UNDEFINED";
				break;
			}

			ShowError(message, file, line);
		}
	}
#endif
}