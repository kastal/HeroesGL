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
	DWORD vertexName;
	DWORD fragmentName;
	GLfloat* mvp;
};

struct ShaderProgramsList
{
	ShaderProgram stencil;
	ShaderProgram linear;
	ShaderProgram cubic;
	ShaderProgram xBRz_2x;
	ShaderProgram xBRz_3x;
	ShaderProgram xBRz_4x;
	ShaderProgram xBRz_5x;
	ShaderProgram xBRz_6x;
	ShaderProgram scaleHQ_2x;
	ShaderProgram scaleHQ_4x;
	ShaderProgram xSal_2x;
	ShaderProgram eagle_2x;
	ShaderProgram scaleNx_2x;
	ShaderProgram scaleNx_3x;
};

struct AddressSpace
{
	DWORD check_1;
	DWORD check_2;
	DWORD equal_address;
	DWORD equal_value;
	DWORD fullscr_nop1;
	DWORD fullscr_nop2;
	DWORD clientrect_nop1;
	DWORD clientrect_nop2;
	WORD resLanguage;
	WORD resDialog;
	const CHAR* windowName;
	const CHAR* configKey;
};

struct RGNRECTDATA
{
	RGNDATAHEADER   rdh;
	RECT            rect;
};