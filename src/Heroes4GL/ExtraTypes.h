/*
	MIT License

	Copyright (c) 2019 Oleksiy Ryabchun

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

#pragma once
#include "windows.h"
#include "GLib.h"

struct Rect
{
	INT x;
	INT y;
	INT width;
	INT height;
};

struct VecSize
{
	INT width;
	INT height;
};

struct TexSize
{
	FLOAT width;
	FLOAT height;
};

struct Frame
{
	GLuint id;
	Rect rect;
	VecSize vSize;
	TexSize tSize;
};

struct Viewport
{
	BOOL refresh;
	INT width;
	INT height;
	Rect rectangle;
	POINTFLOAT viewFactor;
	POINTFLOAT clipFactor;
};

enum WindowState
{
	WinStateNone = 0,
	WinStateFullScreen,
	WinStateWindowed
};

enum ImageFilter
{
	FilterNearest = 0,
	FilterLinear = 1,
	FilterCubic = 2,
	FilterXRBZ = 3,
	FilterScaleHQ = 4,
	FilterXSal = 5,
	FilterEagle = 6,
	FilterScaleNx = 7
};

struct FilterType
{
	WORD value;
	WORD type;
};

struct DisplayMode
{
	DWORD width;
	DWORD height;
	DWORD bpp;
};

enum FpsState
{
	FpsDisabled = 0,
	FpsNormal,
	FpsBenchmark
};

struct UpdateRect
{
	RECT rect;
	BOOL isActive;
};

struct ShaderProgram
{
	GLuint id;
	const CHAR* version;
	DWORD vertexName;
	DWORD fragmentName;
	GLfloat* mvp;
	struct {
		GLint location;
		DWORD value;
	} texSize;
};

struct AddressSpace
{
	DWORD check_1;
	DWORD check_2;
	DWORD equal_address;
	DWORD equal_value;
	DWORD fullscr_nop[2];
	DWORD clientrect_nop[2];
	DWORD updateWindow_nop[4];
	WORD resLanguage;
	WORD resDialog;
	const CHAR* windowName;
};

struct RGNRECTDATA
{
	RGNDATAHEADER   rdh;
	RECT            rect;
};

struct ConfigItems
{
	BOOL isNoGL;
	DWORD language;
	DWORD dialog;
	HICON icon;
	HFONT font;

	BOOL singleThread;
	BOOL singleWindow;
	BOOL coldCPU;

	struct {
		BOOL aspect;
		BOOL vSync;
		ImageFilter filter;
		FilterType scaleNx;
		FilterType xSal;
		FilterType eagle;
		FilterType scaleHQ;
		FilterType xBRz;
	} image;

	struct {
		BYTE fpsCounter;
		BYTE imageFilter;
		BYTE windowedMode;
		BYTE aspectRatio;
		BYTE vSync;
	} keys;

	BOOL isExist;
	CHAR file[MAX_PATH];
};

struct MappedFile
{
	HMODULE hModule;
	HANDLE hFile;
	HANDLE hMap;
	VOID* address;
};