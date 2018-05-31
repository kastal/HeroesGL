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
#include "resource.h"
#include "math.h"
#include "Windowsx.h"
#include "Main.h"
#include "DirectDraw.h"
#include "DirectDrawSurface.h"
#include "DirectDrawClipper.h"
#include "FpsCounter.h"
#include "CommCtrl.h"

#define VK_I 0x49
#define VK_F 0x46
#define GL_FILTER GL_NEAREST

WNDPROC OldWindowProc, OldPanelProc;

#pragma region Not Implemented
HRESULT DirectDraw::QueryInterface(REFIID riid, LPVOID * ppvObj) { return DD_OK; }
ULONG DirectDraw::AddRef() { return 0; }
HRESULT DirectDraw::Compact() { return DD_OK; }
HRESULT DirectDraw::CreatePalette(DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE *, IUnknown *) { return DD_OK; }
HRESULT DirectDraw::DuplicateSurface(LPDIRECTDRAWSURFACE7, LPDIRECTDRAWSURFACE7 *) { return DD_OK; }
HRESULT DirectDraw::EnumSurfaces(DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMSURFACESCALLBACK7) { return DD_OK; }
HRESULT DirectDraw::FlipToGDISurface() { return DD_OK; }
HRESULT DirectDraw::GetCaps(LPDDCAPS, LPDDCAPS) { return DD_OK; }
HRESULT DirectDraw::GetDisplayMode(LPDDSURFACEDESC2) { return DD_OK; }
HRESULT DirectDraw::GetFourCCCodes(LPDWORD, LPDWORD) { return DD_OK; }
HRESULT DirectDraw::GetGDISurface(LPDIRECTDRAWSURFACE7 *) { return DD_OK; }
HRESULT DirectDraw::GetMonitorFrequency(LPDWORD) { return DD_OK; }
HRESULT DirectDraw::GetScanLine(LPDWORD) { return DD_OK; }
HRESULT DirectDraw::GetVerticalBlankStatus(LPBOOL) { return DD_OK; }
HRESULT DirectDraw::Initialize(GUID *) { return DD_OK; }
HRESULT DirectDraw::RestoreDisplayMode() { return DD_OK; }
HRESULT DirectDraw::WaitForVerticalBlank(DWORD, HANDLE) { return DD_OK; }
HRESULT DirectDraw::GetAvailableVidMem(LPDDSCAPS2, LPDWORD, LPDWORD) { return DD_OK; }
HRESULT DirectDraw::GetSurfaceFromDC(HDC, LPDIRECTDRAWSURFACE7 *) { return DD_OK; }
HRESULT DirectDraw::RestoreAllSurfaces() { return DD_OK; }
HRESULT DirectDraw::TestCooperativeLevel() { return DD_OK; }
HRESULT DirectDraw::GetDeviceIdentifier(LPDDDEVICEIDENTIFIER2, DWORD) { return DD_OK; }
HRESULT DirectDraw::StartModeTest(LPSIZE, DWORD, DWORD) { return DD_OK; }
HRESULT DirectDraw::EvaluateMode(DWORD, DWORD *) { return DD_OK; }
#pragma endregion

LRESULT __stdcall WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT:
	{
		DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
		if (ddraw && ddraw->windowState == WinStateWindowed)
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		return CallWindowProc(OldWindowProc, hWnd, uMsg, wParam, lParam);
	}

	case WM_MOVE:
	{
		DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
		if (ddraw && ddraw->hDraw)
		{
			DWORD stye = GetWindowLong(ddraw->hDraw, GWL_STYLE);
			if (stye & WS_POPUP)
			{
				POINT point = { LOWORD(lParam), HIWORD(lParam) };
				ScreenToClient(hWnd, &point);

				RECT rect;
				rect.left = point.x - LOWORD(lParam);
				rect.top = point.y - HIWORD(lParam);
				rect.right = rect.left + 256;
				rect.bottom = rect.left + 256;

				AdjustWindowRect(&rect, stye, FALSE);
				SetWindowPos(ddraw->hDraw, NULL, rect.left, rect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOREDRAW | SWP_NOREPOSITION | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
			}
			else
				SetWindowPos(ddraw->hDraw, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOREDRAW | SWP_NOREPOSITION | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
		}

		return CallWindowProc(OldWindowProc, hWnd, uMsg, wParam, lParam);
	}

	case WM_SIZE:
	{
		DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
		if (ddraw)
		{
			if (ddraw->hDraw)
				SetWindowPos(ddraw->hDraw, NULL, 0, 0, LOWORD(lParam), HIWORD(lParam), SWP_NOZORDER | SWP_NOMOVE | SWP_NOREDRAW | SWP_NOREPOSITION | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING);

			ddraw->viewport.width = LOWORD(lParam);
			ddraw->viewport.height = HIWORD(lParam);
			ddraw->viewport.refresh = TRUE;
			SetEvent(ddraw->hDrawEvent);
		}

		return CallWindowProc(OldWindowProc, hWnd, uMsg, wParam, lParam);
	}

	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_F2: // FPS counter on/off
		{
			switch (fpsState)
			{
			case FpsNormal:
				fpsState = FpsBenchmark;
				break;
			case FpsBenchmark:
				fpsState = FpsDisabled;
				break;
			default:
				fpsState = FpsNormal;
				break;
			}

			isFpsChanged = TRUE;
			DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
			if (ddraw)
				SetEvent(ddraw->hDrawEvent);
			return NULL;
		}

		default:
			return CallWindowProc(OldWindowProc, hWnd, uMsg, wParam, lParam);
		}
	}

	//case WM_SETFOCUS:
	//case WM_KILLFOCUS:
	//case WM_NCACTIVATE:
	case WM_ACTIVATE:
	{
		DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
		if (ddraw && ddraw->windowState != WinStateWindowed)
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		else
			return CallWindowProc(OldWindowProc, hWnd, uMsg, wParam, lParam);
	}

	case WM_ACTIVATEAPP:
	{
		DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
		if (ddraw && ddraw->windowState != WinStateWindowed)
		{
			if ((BOOL)wParam)
				ddraw->RenderStart();
			else
				ddraw->RenderStop();

			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		else
			return CallWindowProc(OldWindowProc, hWnd, uMsg, wParam, lParam);
	}

	default:
		return CallWindowProc(OldWindowProc, hWnd, uMsg, wParam, lParam);
	}
}

LRESULT __stdcall PanelProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT:
	{
		DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
		if (ddraw && ddraw->windowState == WinStateWindowed)
			return CallWindowProc(OldWindowProc, hWnd, uMsg, wParam, lParam);
		return CallWindowProc(OldPanelProc, hWnd, uMsg, wParam, lParam);
	}

	default:
		return CallWindowProc(OldPanelProc, hWnd, uMsg, wParam, lParam);
	}
}

DWORD __stdcall RenderThread(LPVOID lpParameter)
{
	DirectDraw* ddraw = (DirectDraw*)lpParameter;
	ddraw->hDc = ::GetDC(ddraw->hDraw);
	{
		PIXELFORMATDESCRIPTOR pfd = { NULL };
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cAlphaBits = 0;
		pfd.cAccumBits = 0;
		pfd.cDepthBits = 16;
		pfd.cStencilBits = 0;
		pfd.cAuxBuffers = 0;
		pfd.iLayerType = PFD_MAIN_PLANE;

		DWORD pfIndex;
		if (!GL::PreparePixelFormat(&pfd, &pfIndex))
		{
			pfIndex = ChoosePixelFormat(ddraw->hDc, &pfd);
			if (pfIndex == NULL)
				Main::ShowError("ChoosePixelFormat failed", __FILE__, __LINE__);
			else if (pfd.dwFlags & PFD_NEED_PALETTE)
				Main::ShowError("Needs palette", __FILE__, __LINE__);
		}

		if (!SetPixelFormat(ddraw->hDc, pfIndex, &pfd))
			Main::ShowError("SetPixelFormat failed", __FILE__, __LINE__);

		memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		if (DescribePixelFormat(ddraw->hDc, pfIndex, sizeof(PIXELFORMATDESCRIPTOR), &pfd) == NULL)
			Main::ShowError("DescribePixelFormat failed", __FILE__, __LINE__);

		if ((pfd.iPixelType != PFD_TYPE_RGBA) ||
			(pfd.cRedBits < 5) || (pfd.cGreenBits < 5) || (pfd.cBlueBits < 5))
			Main::ShowError("Bad pixel type", __FILE__, __LINE__);

		HGLRC hRc = WGLCreateContext(ddraw->hDc);
		if (hRc)
		{
			WGLMakeCurrent(ddraw->hDc, hRc);
			{
				GL::CreateContextAttribs(ddraw->hDc, &hRc);

				GLint glMaxTexSize;
				GLGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTexSize);

				if (glVersion >= GL_VER_3_0)
				{
					DWORD maxSize = ddraw->mode->width > ddraw->mode->height ? ddraw->mode->width : ddraw->mode->height;

					DWORD maxTexSize = 1;
					while (maxTexSize < maxSize)
						maxTexSize <<= 1;

					if (maxTexSize > glMaxTexSize)
						glVersion = GL_VER_1_1;
				}

				if (glVersion >= GL_VER_3_0)
					ddraw->RenderNew();
				else
					ddraw->RenderOld(glMaxTexSize);
			}
			WGLMakeCurrent(ddraw->hDc, NULL);
			WGLDeleteContext(hRc);
		}
	}
	::ReleaseDC(ddraw->hDraw, ddraw->hDc);
	ddraw->hDc = NULL;

	return NULL;
}

VOID DirectDraw::RenderOld(DWORD glMaxTexSize)
{
	if (glMaxTexSize < 256)
		glMaxTexSize = 256;

	DWORD size;
	if (glCapsClampToEdge == GL_CLAMP_TO_EDGE)
		size = this->mode->width < this->mode->height ? this->mode->width : this->mode->height;
	else
		size = this->mode->width > this->mode->height ? this->mode->width : this->mode->height;

	DWORD maxAllow = 1;
	while (maxAllow < size)
		maxAllow <<= 1;

	DWORD maxTexSize = maxAllow < glMaxTexSize ? maxAllow : glMaxTexSize;

	DWORD framePerWidth = this->mode->width / maxTexSize + (this->mode->width % maxTexSize ? 1 : 0);
	DWORD framePerHeight = this->mode->height / maxTexSize + (this->mode->height % maxTexSize ? 1 : 0);
	DWORD frameCount = framePerWidth * framePerHeight;
	Frame* frames = (Frame*)malloc(frameCount * sizeof(Frame));
	{
		Frame* frame = frames;
		for (DWORD y = 0; y < this->mode->height; y += maxTexSize)
		{
			DWORD height = this->mode->height - y;
			if (height > maxTexSize)
				height = maxTexSize;

			for (DWORD x = 0; x < this->mode->width; x += maxTexSize, ++frame)
			{
				DWORD width = this->mode->width - x;
				if (width > maxTexSize)
					width = maxTexSize;

				frame->rect.x = x;
				frame->rect.y = y;
				frame->rect.width = width;
				frame->rect.height = height;

				frame->vSize.width = x + width;
				frame->vSize.height = y + height;

				frame->tSize.width = width == maxTexSize ? 1.0f : (FLOAT)width / maxTexSize;
				frame->tSize.height = height == maxTexSize ? 1.0f : (FLOAT)height / maxTexSize;

				GLGenTextures(1, &frame->id);
				GLBindTexture(GL_TEXTURE_2D, frame->id);

				GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glCapsClampToEdge);
				GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glCapsClampToEdge);
				GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
				GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

				GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_FILTER);
				GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_FILTER);

				GLTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

				if (glVersion > GL_VER_1_1)
					GLTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, maxTexSize, maxTexSize, GL_NONE, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
				else
					GLTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, maxTexSize, maxTexSize, GL_NONE, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			}
		}

		GLClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		this->viewport.refresh = TRUE;

		GLMatrixMode(GL_PROJECTION);
		GLLoadIdentity();
		GLOrtho(0.0, (GLdouble)this->mode->width, (GLdouble)this->mode->height, 0.0, 0.0, 1.0);
		GLMatrixMode(GL_MODELVIEW);
		GLLoadIdentity();

		GLEnable(GL_TEXTURE_2D);

		VOID* pixelBuffer = malloc(maxTexSize * maxTexSize * (glVersion > GL_VER_1_1 ? sizeof(WORD) : sizeof(DWORD)));
		{
			DWORD fpsQueue[FPS_COUNT];
			DWORD tickQueue[FPS_COUNT];

			DWORD fpsIdx = -1;
			DWORD fpsTotal = 0;
			DWORD fpsCount = 0;
			INT fpsSum = 0;
			memset(fpsQueue, 0, sizeof(fpsQueue));
			memset(tickQueue, 0, sizeof(tickQueue));

			GLClear(GL_COLOR_BUFFER_BIT);
			do
			{
				if (this->attachedSurface)
				{
					if (fpsState)
					{
						DWORD tick = GetTickCount();

						if (isFpsChanged)
						{
							isFpsChanged = FALSE;
							fpsIdx = -1;
							fpsTotal = 0;
							fpsCount = 0;
							fpsSum = 0;
							memset(fpsQueue, 0, sizeof(fpsQueue));
							memset(tickQueue, 0, sizeof(tickQueue));
						}

						++fpsTotal;
						if (fpsCount < FPS_COUNT)
							++fpsCount;

						++fpsIdx;
						if (fpsIdx == FPS_COUNT)
							fpsIdx = 0;

						DWORD diff = tick - tickQueue[fpsTotal != fpsCount ? fpsIdx : 0];
						tickQueue[fpsIdx] = tick;

						DWORD fps = diff ? Main::Round(1000.0f / diff * fpsCount) : 9999;

						DWORD* queue = &fpsQueue[fpsIdx];
						fpsSum -= *queue - fps;
						*queue = fps;
					}

					this->CheckView();

					DWORD count = frameCount;
					frame = frames;
					while (count--)
					{
						GLBindTexture(GL_TEXTURE_2D, frame->id);

						if (glVersion > GL_VER_1_1)
						{
							if (frameCount > 1)
							{
								if (!(frame->rect.width % 2))
								{
									DWORD* pix = (DWORD*)pixelBuffer;
									for (DWORD y = frame->rect.y; y < frame->vSize.height; ++y)
									{
										DWORD* idx = (DWORD*)((WORD*)this->attachedSurface->indexBuffer + y * this->mode->width + frame->rect.x);
										for (DWORD x = frame->rect.x; x < frame->vSize.width; x += 2)
											*pix++ = *idx++;
									}
								}
								else
								{
									WORD* pix = (WORD*)pixelBuffer;
									for (DWORD y = frame->rect.y; y < frame->vSize.height; ++y)
									{
										WORD* idx = (WORD*)this->attachedSurface->indexBuffer + y * this->mode->width + frame->rect.x;
										for (DWORD x = frame->rect.x; x < frame->vSize.width; ++x)
											*pix++ = *idx++;
									}
								}
							}
							else
							{
								DWORD count = frame->rect.width * frame->rect.height;
								if (!(count % 2))
								{
									count >>= 1;
									DWORD* pix = (DWORD*)pixelBuffer;
									DWORD* idx = (DWORD*)this->attachedSurface->indexBuffer;
									do
										*pix++ = *idx++;
									while (--count);
								}
								else
								{
									WORD* pix = (WORD*)pixelBuffer;
									WORD* idx = (WORD*)this->attachedSurface->indexBuffer;
									do
										*pix++ = *idx++;
									while (--count);
								}
							}

							GLTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame->rect.width, frame->rect.height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, pixelBuffer);

							if (fpsState && count == frameCount - 1)
							{
								DWORD fps = Main::Round((FLOAT)fpsSum / fpsCount);

								DWORD offset = FPS_X;

								DWORD digCount = 0;
								DWORD current = fps;
								do
								{
									++digCount;
									current = current / 10;
								} while (current);

								WORD fpsColor = fpsState == FpsBenchmark ? 0xFFE0 : 0xFFFF;
								DWORD dcount = digCount;
								current = fps;
								do
								{
									DWORD digit = current % 10;
									bool* lpDig = (bool*)counters + FPS_WIDTH * FPS_HEIGHT * digit;

									for (DWORD y = 0; y < FPS_HEIGHT; ++y)
									{
										WORD* idx = (WORD*)this->attachedSurface->indexBuffer + (FPS_Y + y) * this->mode->width +
											FPS_X + (FPS_STEP + FPS_WIDTH) * (dcount - 1);

										WORD* pix = (WORD*)pixelBuffer + y * (FPS_STEP + FPS_WIDTH) * digCount +
											(FPS_STEP + FPS_WIDTH) * (dcount - 1);

										*(DWORD*)pix = *(DWORD*)idx;
										idx += 2;
										pix += 2;

										*(DWORD*)pix = *(DWORD*)idx;
										idx += 2;
										pix += 2;

										DWORD width = FPS_WIDTH;
										do
										{
											*pix++ = *lpDig++ ? fpsColor : *idx;
											++idx;
										} while (--width);
									}

									current = current / 10;
								} while (--dcount);

								GLPixelStorei(GL_UNPACK_ALIGNMENT, 2);
								GLTexSubImage2D(GL_TEXTURE_2D, 0, FPS_X, FPS_Y, (FPS_WIDTH + FPS_STEP) * digCount, FPS_HEIGHT, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, pixelBuffer);
								GLPixelStorei(GL_UNPACK_ALIGNMENT, 4);
							}
						}
						else
						{
							DWORD* pix = (DWORD*)pixelBuffer;
							for (DWORD y = frame->rect.y; y < frame->vSize.height; ++y)
							{
								WORD* idx = (WORD*)this->attachedSurface->indexBuffer + y * this->mode->width + frame->rect.x;
								for (DWORD x = frame->rect.x; x < frame->vSize.width; ++x)
								{
									WORD px = *idx++;
									*pix++ = ((px & 0xF800) >> 8) | ((px & 0x07E0) << 5) | ((px & 0x001F) << 19);
								}
							}

							GLTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame->rect.width, frame->rect.height, GL_RGBA, GL_UNSIGNED_BYTE, pixelBuffer);

							if (fpsState && count == frameCount - 1)
							{
								DWORD fps = Main::Round((FLOAT)fpsSum / fpsCount);

								DWORD offset = FPS_X;

								DWORD digCount = 0;
								DWORD current = fps;
								do
								{
									++digCount;
									current = current / 10;
								} while (current);

								DWORD fpsColor = fpsState == FpsBenchmark ? 0xFF00FFFF : 0xFFFFFFFF;
								DWORD dcount = digCount;
								current = fps;
								do
								{
									DWORD digit = current % 10;
									bool* lpDig = (bool*)counters + FPS_WIDTH * FPS_HEIGHT * digit;

									for (DWORD y = 0; y < FPS_HEIGHT; ++y)
									{
										WORD* idx = (WORD*)this->attachedSurface->indexBuffer + (FPS_Y + y) * this->mode->width +
											FPS_X + (FPS_STEP + FPS_WIDTH) * (dcount - 1);

										DWORD* pix = (DWORD*)pixelBuffer + y * (FPS_STEP + FPS_WIDTH) * digCount +
											(FPS_STEP + FPS_WIDTH) * (dcount - 1);

										DWORD width = FPS_STEP;
										do
										{
											*pix++ = ((*idx & 0xF800) >> 8) | ((*idx & 0x07E0) << 5) | ((*idx & 0x001F) << 19);
											++idx;
										} while (--width);

										width = FPS_WIDTH;
										do
										{
											*pix++ = *lpDig++ ? fpsColor : (((*idx & 0xF800) >> 8) | ((*idx & 0x07E0) << 5) | ((*idx & 0x001F) << 19));
											++idx;
										} while (--width);
									}

									current = current / 10;
								} while (--dcount);

								GLTexSubImage2D(GL_TEXTURE_2D, 0, FPS_X, FPS_Y, (FPS_WIDTH + FPS_STEP) * digCount, FPS_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixelBuffer);
							}
						}

						GLBegin(GL_TRIANGLE_FAN);
						{
							GLTexCoord2f(0.0f, 0.0f);
							GLVertex2s(frame->rect.x, frame->rect.y);

							GLTexCoord2f(frame->tSize.width, 0.0f);
							GLVertex2s(frame->vSize.width, frame->rect.y);

							GLTexCoord2f(frame->tSize.width, frame->tSize.height);
							GLVertex2s(frame->vSize.width, frame->vSize.height);

							GLTexCoord2f(0.0f, frame->tSize.height);
							GLVertex2s(frame->rect.x, frame->vSize.height);

						}
						GLEnd();
						++frame;
					}

					GLFlush();
					SwapBuffers(this->hDc);
					GLClear(GL_COLOR_BUFFER_BIT);

					if (fpsState != FpsBenchmark)
					{
						WaitForSingleObject(this->hDrawEvent, INFINITE);
						ResetEvent(this->hDrawEvent);
					}
				}
			} while (!this->isFinish);
			GLFinish();
		}

		frame = frames;
		DWORD count = frameCount;
		while (count--)
		{
			GLDeleteTextures(1, &frame->id);
			++frame;
		}
	}
	free(frames);
}

VOID DirectDraw::RenderNew()
{
	DWORD maxSize = this->mode->width > this->mode->height ? this->mode->width : this->mode->height;
	DWORD maxTexSize = 1;
	while (maxTexSize < maxSize) maxTexSize <<= 1;
	FLOAT texWidth = this->mode->width == maxTexSize ? 1.0f : (FLOAT)this->mode->width / maxTexSize;
	FLOAT texHeight = this->mode->height == maxTexSize ? 1.0f : (FLOAT)this->mode->height / maxTexSize;

	FLOAT buffer[4][4] = {
		{ 0.0f, 0.0f, 0.0f, 0.0f },
		{ (FLOAT)this->mode->width, 0.0f, texWidth, 0.0f },
		{ (FLOAT)this->mode->width, (FLOAT)this->mode->height, texWidth, texHeight },
		{ 0.0f, (FLOAT)this->mode->height, 0.0f, texHeight }
	};

	FLOAT mvpMatrix[4][4] = {
		{ 2.0f / this->mode->width, 0.0f, 0.0f, 0.0f },
		{ 0.0f, -2.0f / this->mode->height, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 2.0f, 0.0f },
		{ -1.0f, 1.0f, -1.0f, 1.0f }
	};

	GLuint shProgramLinear = GLCreateProgram();
	{
		GLuint vLinear = GL::CompileShaderSource(IDR_30_LINEAR_VERTEX, GL_VERTEX_SHADER);
		GLuint fLinear = GL::CompileShaderSource(IDR_30_LINEAR_FRAGMENT, GL_FRAGMENT_SHADER);
		{

			GLAttachShader(shProgramLinear, vLinear);
			GLAttachShader(shProgramLinear, fLinear);
			{
				GLLinkProgram(shProgramLinear);
			}
			GLDetachShader(shProgramLinear, fLinear);
			GLDetachShader(shProgramLinear, vLinear);
		}
		GLDeleteShader(fLinear);
		GLDeleteShader(vLinear);

		GLUseProgram(shProgramLinear);
		{
			GLUniformMatrix4fv(GLGetUniformLocation(shProgramLinear, "mvp"), 1, GL_FALSE, (GLfloat*)mvpMatrix);
			GLUniform1i(GLGetUniformLocation(shProgramLinear, "tex01"), 0);
			{
				GLuint arrayName;
				GLGenVertexArrays(1, &arrayName);
				GLBindVertexArray(arrayName);
				{
					GLuint bufferName;
					GLGenBuffers(1, &bufferName);
					{
						GLBindBuffer(GL_ARRAY_BUFFER, bufferName);
						{
							GLBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW);

							GLint attrCoordsLoc = GLGetAttribLocation(shProgramLinear, "vCoord");
							GLEnableVertexAttribArray(attrCoordsLoc);
							GLVertexAttribPointer(attrCoordsLoc, 2, GL_FLOAT, GL_FALSE, 16, (GLvoid*)0);

							GLint attrTexCoordsLoc = GLGetAttribLocation(shProgramLinear, "vTexCoord");
							GLEnableVertexAttribArray(attrTexCoordsLoc);
							GLVertexAttribPointer(attrTexCoordsLoc, 2, GL_FLOAT, GL_FALSE, 16, (GLvoid*)8);

							GLuint textureId;
							GLGenTextures(1, &textureId);
							{
								GLActiveTexture(GL_TEXTURE0);
								GLBindTexture(GL_TEXTURE_2D, textureId);
								GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glCapsClampToEdge);
								GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glCapsClampToEdge);
								GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
								GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
								GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_FILTER);
								GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_FILTER);
								GLTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, maxTexSize, maxTexSize, GL_NONE, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);

								GLClearColor(0.0f, 0.0f, 0.0f, 1.0f);
								this->viewport.refresh = TRUE;

								VOID* pixelBuffer = malloc(maxTexSize * maxTexSize * sizeof(WORD));
								{
									DWORD fpsQueue[FPS_COUNT];
									DWORD tickQueue[FPS_COUNT];

									DWORD fpsIdx = -1;
									DWORD fpsTotal = 0;
									DWORD fpsCount = 0;
									INT fpsSum = 0;
									memset(fpsQueue, 0, sizeof(fpsQueue));
									memset(tickQueue, 0, sizeof(tickQueue));

									GLClear(GL_COLOR_BUFFER_BIT);
									do
									{
										if (this->attachedSurface)
										{
											if (fpsState)
											{
												DWORD tick = GetTickCount();

												if (isFpsChanged)
												{
													isFpsChanged = FALSE;
													fpsIdx = -1;
													fpsTotal = 0;
													fpsCount = 0;
													fpsSum = 0;
													memset(fpsQueue, 0, sizeof(fpsQueue));
													memset(tickQueue, 0, sizeof(tickQueue));
												}

												++fpsTotal;
												if (fpsCount < FPS_COUNT)
													++fpsCount;

												++fpsIdx;
												if (fpsIdx == FPS_COUNT)
													fpsIdx = 0;

												DWORD diff = tick - tickQueue[fpsTotal != fpsCount ? fpsIdx : 0];
												tickQueue[fpsIdx] = tick;

												DWORD fps = diff ? Main::Round(1000.0f / diff * fpsCount) : 9999;

												DWORD* queue = &fpsQueue[fpsIdx];
												fpsSum -= *queue - fps;
												*queue = fps;
											}

											this->CheckView();

											GLTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->mode->width, this->mode->height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, this->attachedSurface->indexBuffer);

											if (fpsState)
											{
												DWORD fps = Main::Round((FLOAT)fpsSum / fpsCount);

												DWORD offset = FPS_X;

												DWORD digCount = 0;
												DWORD current = fps;
												do
												{
													++digCount;
													current = current / 10;
												} while (current);

												WORD fpsColor = fpsState == FpsBenchmark ? 0xFFE0 : 0xFFFF;
												DWORD dcount = digCount;
												current = fps;
												do
												{
													DWORD digit = current % 10;
													bool* lpDig = (bool*)counters + FPS_WIDTH * FPS_HEIGHT * digit;

													for (DWORD y = 0; y < FPS_HEIGHT; ++y)
													{
														WORD* idx = (WORD*)this->attachedSurface->indexBuffer + (FPS_Y + y) * this->mode->width +
															FPS_X + (FPS_STEP + FPS_WIDTH) * (dcount - 1);

														WORD* pix = (WORD*)pixelBuffer + y * (FPS_STEP + FPS_WIDTH) * digCount +
															(FPS_STEP + FPS_WIDTH) * (dcount - 1);

														*(DWORD*)pix = *(DWORD*)idx;
														idx += 2;
														pix += 2;

														*(DWORD*)pix = *(DWORD*)idx;
														idx += 2;
														pix += 2;

														DWORD width = FPS_WIDTH;
														do
														{
															*pix++ = *lpDig++ ? fpsColor : *idx;
															++idx;
														} while (--width);
													}

													current = current / 10;
												} while (--dcount);

												GLPixelStorei(GL_UNPACK_ALIGNMENT, 2);
												GLTexSubImage2D(GL_TEXTURE_2D, 0, FPS_X, FPS_Y, (FPS_WIDTH + FPS_STEP) * digCount, FPS_HEIGHT, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, pixelBuffer);
												GLPixelStorei(GL_UNPACK_ALIGNMENT, 4);
											}

											GLDrawArrays(GL_TRIANGLE_FAN, 0, 4);

											GLFlush();
											SwapBuffers(this->hDc);
											GLClear(GL_COLOR_BUFFER_BIT);

											if (fpsState != FpsBenchmark)
											{
												WaitForSingleObject(this->hDrawEvent, INFINITE);
												ResetEvent(this->hDrawEvent);
											}
										}
									} while (!this->isFinish);
									GLFinish();
								}
								free(pixelBuffer);
							}
							GLDeleteTextures(1, &textureId);
						}
						GLBindBuffer(GL_ARRAY_BUFFER, NULL);
					}
					GLDeleteBuffers(1, &bufferName);
				}
				GLBindVertexArray(NULL);
				GLDeleteVertexArrays(1, &arrayName);
			}
		}
		GLUseProgram(NULL);
	}
	GLDeleteProgram(shProgramLinear);
}

VOID DirectDraw::RenderStart()
{
	if (!this->isFinish)
		return;

	this->isFinish = FALSE;
	GL::Load();

	RECT rect;
	GetClientRect(this->hWnd, &rect);

	if (this->windowState != WinStateWindowed)
	{
		this->hDraw = CreateWindowEx(
			WS_EX_CONTROLPARENT,
			WC_STATIC,
			NULL,
			WS_VISIBLE | WS_POPUP,
			rect.left, rect.top,
			rect.right - rect.left, rect.bottom - rect.top,
			this->hWnd,
			NULL,
			hDllModule,
			NULL);

		//SetActiveWindow(this->hWnd);
	}
	else
	{
		this->hDraw = CreateWindowEx(
			WS_EX_CONTROLPARENT,
			WC_STATIC,
			NULL,
			WS_VISIBLE | WS_CHILD,
			rect.left, rect.top,
			rect.right - rect.left, rect.bottom - rect.top,
			this->hWnd,
			NULL,
			hDllModule,
			NULL);
	}

	OldPanelProc = (WNDPROC)SetWindowLongPtr(this->hDraw, GWLP_WNDPROC, (LONG_PTR)PanelProc);

	SetClassLongPtr(this->hDraw, GCLP_HBRBACKGROUND, NULL);
	RedrawWindow(this->hDraw, NULL, NULL, RDW_INVALIDATE);
	SetClassLongPtr(this->hWnd, GCLP_HBRBACKGROUND, NULL);
	RedrawWindow(this->hWnd, NULL, NULL, RDW_INVALIDATE);

	this->viewport.width = rect.right - rect.left;
	this->viewport.height = rect.bottom - rect.top;
	this->viewport.refresh = TRUE;

	DWORD threadId;
	SECURITY_ATTRIBUTES sAttribs = { NULL };
	this->hDrawThread = CreateThread(&sAttribs, NULL, RenderThread, this, NORMAL_PRIORITY_CLASS, &threadId);
}

VOID DirectDraw::RenderStop()
{
	if (this->isFinish)
		return;

	this->isFinish = TRUE;
	SetEvent(this->hDrawEvent);
	WaitForSingleObject(this->hDrawThread, INFINITE);
	this->hDrawThread = NULL;

	if (this->hDraw && DestroyWindow(this->hDraw))
		this->hDraw = NULL;
}

BOOL DirectDraw::CheckView()
{
	if (this->viewport.refresh)
	{
		this->viewport.refresh = FALSE;

		this->viewport.rectangle.x = this->viewport.rectangle.y = 0;
		this->viewport.rectangle.width = this->viewport.width;
		this->viewport.rectangle.height = this->viewport.height;

		this->viewport.clipFactor.x = this->viewport.viewFactor.x = (FLOAT)this->viewport.width / this->mode->width;
		this->viewport.clipFactor.y = this->viewport.viewFactor.y = (FLOAT)this->viewport.height / this->mode->height;

		GLViewport(this->viewport.rectangle.x, this->viewport.rectangle.y, this->viewport.rectangle.width, this->viewport.rectangle.height);

		return TRUE;
	}

	return FALSE;
}

DirectDraw::DirectDraw(DirectDraw* lastObj)
{
	this->last = lastObj;

	this->surfaceEntries = NULL;
	this->clipperEntries = NULL;

	this->attachedSurface = NULL;

	this->hWnd = NULL;
	this->hDraw = NULL;
	this->hDc = NULL;

	this->mode = NULL;

	this->isFinish = TRUE;

	this->hDrawEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

DirectDraw::~DirectDraw()
{
	DirectDrawSurface* surfaceEntry = this->surfaceEntries;
	while (surfaceEntry)
	{
		DirectDrawSurface* curr = surfaceEntry->last;
		delete surfaceEntry;
		surfaceEntry = curr;
	}

	DirectDrawClipper* clipperEntry = this->clipperEntries;
	while (clipperEntry)
	{
		DirectDrawClipper* curr = clipperEntry->last;
		delete clipperEntry;
		clipperEntry = curr;
	}

	CloseHandle(this->hDrawEvent);
	ChangeDisplaySettings(NULL, NULL);
}

ULONG DirectDraw::Release()
{
	if (ddrawList == this)
		ddrawList = NULL;
	else
	{
		DirectDraw* ddraw = ddrawList;
		while (ddraw)
		{
			if (ddraw->last == this)
			{
				ddraw->last = ddraw->last->last;
				break;
			}

			ddraw = ddraw->last;
		}
	}

	delete this;
	return 0;
}

HRESULT DirectDraw::SetCooperativeLevel(HWND hWnd, DWORD dwFlags)
{
	this->windowState = dwFlags & DDSCL_FULLSCREEN ? WinStateFullScreen : WinStateWindowed;

	if (this->hWnd != hWnd)
	{
		this->hDc = NULL;
		this->hWnd = hWnd;

		if (!OldWindowProc)
			OldWindowProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)WindowProc);
	}

	if (this->windowState == WinStateWindowed)
		ChangeDisplaySettings(NULL, NULL);

	return DD_OK;
}

HRESULT DirectDraw::EnumDisplayModes(DWORD dwFlags, LPDDSURFACEDESC2 lpDDSurfaceDesc, LPVOID lpContext, LPDDENUMMODESCALLBACK2 lpEnumModesCallback)
{
	DisplayMode* mode = modesList;
	DWORD count = 3;
	do
		(mode++)->isExists = FALSE;
	while (--count);

	DEVMODE devMode = { NULL };
	devMode.dmSize = sizeof(DEVMODE);

	DWORD bppCheck = 32;
	if (EnumDisplaySettings(NULL, ENUM_REGISTRY_SETTINGS, &devMode))
	{
		bppCheck = devMode.dmBitsPerPel;
		memset(&devMode, NULL, sizeof(DEVMODE));
		devMode.dmSize = sizeof(DEVMODE);
	}

	DWORD i;
	for (i = 0; EnumDisplaySettings(NULL, i, &devMode); ++i)
	{
		if (devMode.dmBitsPerPel == bppCheck)
		{
			mode = modesList;
			count = 3;
			do
			{
				if (devMode.dmPelsWidth == mode->width && devMode.dmPelsHeight == mode->height)
				{
					mode->bpp = 16;
					break;
				}

				++mode;
			} while (--count);
		}

		memset(&devMode, NULL, sizeof(DEVMODE));
		devMode.dmSize = sizeof(DEVMODE);
	}

	DDSURFACEDESC2 ddSurfaceDesc = { NULL };
	mode = modesList;
	count = 3;
	do
	{
		if (mode->isExists)
		{
			ddSurfaceDesc.dwWidth = mode->width;
			ddSurfaceDesc.dwHeight = mode->height;
			ddSurfaceDesc.ddpfPixelFormat.dwRGBBitCount = mode->bpp;
			if (!lpEnumModesCallback(&ddSurfaceDesc, NULL))
				break;
		}

		++mode;
	} while (--count);

	return DD_OK;
}

HRESULT DirectDraw::SetDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP, DWORD dwRefreshRate, DWORD dwFlags)
{
	this->mode = NULL;

	DisplayMode* mode = modesList;
	DWORD count = 3;
	do
	{
		if (mode->width == dwWidth && mode->height == dwHeight && mode->bpp == dwBPP)
		{
			this->mode = mode;
			break;
		}

		++mode;
	} while (--count);

	if (!this->mode)
		return DDERR_INVALIDMODE;

	DEVMODE devMode = { NULL };
	devMode.dmSize = sizeof(DEVMODE);
	EnumDisplaySettings(NULL, ENUM_REGISTRY_SETTINGS, &devMode);

	devMode.dmPelsWidth = dwWidth;
	devMode.dmPelsHeight = dwHeight;
	devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

	if (ChangeDisplaySettingsEx(NULL, &devMode, NULL, CDS_FULLSCREEN | CDS_TEST | CDS_RESET, NULL) == DISP_CHANGE_SUCCESSFUL)
	{
		ChangeDisplaySettingsEx(NULL, &devMode, NULL, CDS_FULLSCREEN | CDS_RESET, NULL);
		MoveWindow(this->hWnd, 0, 0, dwWidth, dwHeight, 0);
		return DD_OK;
	}
	else
		return DDERR_INVALIDMODE;
}

HRESULT DirectDraw::CreateSurface(LPDDSURFACEDESC2 lpDDSurfaceDesc, LPDIRECTDRAWSURFACE7* lplpDDSurface, IUnknown* pUnkOuter)
{
	BOOL isPrimary = lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE;

	this->surfaceEntries = new DirectDrawSurface(this, !isPrimary);
	*lplpDDSurface = (LPDIRECTDRAWSURFACE7)this->surfaceEntries;

	if (isPrimary)
		this->attachedSurface = this->surfaceEntries;

	DWORD width, height;
	if (lpDDSurfaceDesc->dwFlags & (DDSD_WIDTH | DDSD_HEIGHT))
	{
		width = lpDDSurfaceDesc->dwWidth;
		height = lpDDSurfaceDesc->dwHeight;
	}
	else
	{
		if (this->windowState != WinStateWindowed)
		{
			width = this->mode->width;
			height = this->mode->height;
		}
		else
		{
			width = GetSystemMetrics(SM_CXSCREEN);
			height = GetSystemMetrics(SM_CYSCREEN);
		}
	}

	this->surfaceEntries->CreateBuffer(width, height);

	return DD_OK;
}

HRESULT DirectDraw::CreateClipper(DWORD dwFlags, LPDIRECTDRAWCLIPPER* lplpDDClipper, IUnknown* pUnkOuter)
{
	this->clipperEntries = new DirectDrawClipper(this);
	*lplpDDClipper = (LPDIRECTDRAWCLIPPER)this->clipperEntries;

	return DD_OK;
}