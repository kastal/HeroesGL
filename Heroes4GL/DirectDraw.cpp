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
#include "FpsCounter.h"
#include "Config.h"
#include "CommCtrl.h"

#define VK_I 0x49
#define VK_F 0x46

#define MIN_WIDTH 240
#define MIN_HEIGHT 180

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

LRESULT __stdcall HelpProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
	{
		if (wParam == CMD_OK)
			EndDialog(hWnd, TRUE);

		break;
	}

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

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

			if (ddraw->mode)
				lParam = ddraw->mode->width | (ddraw->mode->height << 16);
		}

		return CallWindowProc(OldWindowProc, hWnd, uMsg, wParam, lParam);
	}

	case WM_GETMINMAXINFO:
	{
		DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
		if (ddraw && ddraw->windowState == WinStateWindowed)
		{
			RECT rect = { 0, 0, MIN_WIDTH, MIN_HEIGHT };
			AdjustWindowRect(&rect, GetWindowLong(hWnd, GWL_STYLE), TRUE);

			MINMAXINFO* mmi = (MINMAXINFO*)lParam;
			mmi->ptMinTrackSize.x = rect.right - rect.left;
			mmi->ptMinTrackSize.y = rect.bottom - rect.top;;

			return NULL;
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

		case VK_F3: // Filtering on/off
		{
			DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
			if (ddraw)
			{
				switch (ddraw->imageFilter)
				{
				case FilterLinear:
					if (glVersion >= GL_VER_3_0)
					{
						ddraw->imageFilter = FilterXRBZ;
						Config::Set("ImageFilter", 2);
					}
					else
					{
						ddraw->imageFilter = FilterNearest;
						Config::Set("ImageFilter", 0);
					}
					break;
				case FilterXRBZ:
					ddraw->imageFilter = FilterNearest;
					Config::Set("ImageFilter", 0);
					break;
				default:
					ddraw->imageFilter = FilterLinear;
					Config::Set("ImageFilter", 1);
					break;
				}

				ddraw->isStateChanged = TRUE;
				SetEvent(ddraw->hDrawEvent);
				ddraw->CheckMenu(NULL);
			}

			return NULL;
		}

		case VK_F5:
		{
			DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
			if (ddraw)
			{
				ddraw->imageAspect = !ddraw->imageAspect;
				Config::Set("ImageAspect", ddraw->imageAspect);
				ddraw->viewport.refresh = TRUE;
				SetEvent(ddraw->hDrawEvent);
				ddraw->CheckMenu(NULL);
			}

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

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MOUSEMOVE:
	{
		DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
		if (ddraw && ddraw->mode)
		{
			POINT p = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ddraw->ScaleMouse(&p);
			lParam = MAKELONG(p.x, p.y);
		}

		return CallWindowProc(OldWindowProc, hWnd, uMsg, wParam, lParam);
	}

	case WM_COMMAND:
	{
		switch (wParam)
		{
		case IDM_RES_FULL_SCREEN:
		{
			SendMessage(hWnd, WM_KEYDOWN, VK_F4, NULL);
			return NULL;
		}

		case IDM_HELP_ABOUT:
		{
			DialogBoxParam(NULL, MAKEINTRESOURCE(IDG_ABOUT), hWnd, (DLGPROC)HelpProc, NULL);
			SetForegroundWindow(hWnd);
			return NULL;
		}

		case IDM_ASPECT_RATIO:
		{
			DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
			if (ddraw)
			{
				ddraw->imageAspect = !ddraw->imageAspect;
				Config::Set("ImageAspect", ddraw->imageAspect);
				ddraw->viewport.refresh = TRUE;
				SetEvent(ddraw->hDrawEvent);
				ddraw->CheckMenu(NULL);
			}
			return NULL;
		}

		case IDM_VSYNC:
		{
			DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
			if (ddraw)
			{
				ddraw->imageVSync = !ddraw->imageVSync;
				Config::Set("ImageVSync", ddraw->imageVSync);
				SetEvent(ddraw->hDrawEvent);
				ddraw->CheckMenu(NULL);
			}
			return NULL;
		}

		case IDM_FILT_OFF:
		{
			DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
			if (ddraw)
			{
				ddraw->imageFilter = FilterNearest;
				Config::Set("ImageFilter", 0);
				ddraw->isStateChanged = TRUE;
				SetEvent(ddraw->hDrawEvent);
				ddraw->CheckMenu(NULL);
			}
			return NULL;
		}

		case IDM_FILT_LINEAR:
		{
			DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
			if (ddraw)
			{
				ddraw->imageFilter = FilterLinear;
				Config::Set("ImageFilter", 1);
				ddraw->isStateChanged = TRUE;
				SetEvent(ddraw->hDrawEvent);
				ddraw->CheckMenu(NULL);
			}
			return NULL;
		}

		case IDM_FILT_XRBZ:
		{
			DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
			if (ddraw)
			{
				if (glVersion >= GL_VER_3_0)
				{
					ddraw->imageFilter = FilterXRBZ;
					Config::Set("ImageFilter", 2);
				}
				else
				{
					ddraw->imageFilter = FilterNearest;
					Config::Set("ImageFilter", 0);
				}
				ddraw->isStateChanged = TRUE;
				SetEvent(ddraw->hDrawEvent);
				ddraw->CheckMenu(NULL);
			}
			return NULL;
		}

		default:
			return CallWindowProc(OldWindowProc, hWnd, uMsg, wParam, lParam);
		}
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

	case WM_SYSCOMMAND:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
	{
		DWORD stye = GetWindowLong(hWnd, GWL_STYLE);
		if (stye & WS_POPUP)
			return WindowProc(GetParent(hWnd), uMsg, wParam, lParam);

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

				DWORD glMaxTexSize;
				GLGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&glMaxTexSize);

				if (glVersion >= GL_VER_3_0)
				{
					DWORD maxSize = ddraw->mode->width > ddraw->mode->height ? ddraw->mode->width : ddraw->mode->height;

					DWORD maxTexSize = 1;
					while (maxTexSize < maxSize)
						maxTexSize <<= 1;

					if (maxTexSize > glMaxTexSize)
						glVersion = GL_VER_1_1;
				}

				ddraw->CheckMenu(NULL);
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

	DWORD size = this->mode->width > this->mode->height ? this->mode->width : this->mode->height;

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

				if (this->imageFilter == FilterNearest)
				{
					GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				}
				else
				{
					GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				}

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

			BOOL isVSync = FALSE;
			WGLSwapInterval(0);

			do
			{
				if (this->attachedSurface)
				{
					if (!isVSync)
					{
						if (this->imageVSync)
						{
							isVSync = TRUE;
							WGLSwapInterval(1);
						}
					}
					else
					{
						if (!this->imageVSync)
						{
							isVSync = FALSE;
							WGLSwapInterval(0);
						}
					}

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

					DWORD glFilter = 0;
					if (this->isStateChanged)
					{
						this->isStateChanged = FALSE;
						glFilter = this->imageFilter == FilterNearest ? GL_NEAREST : GL_LINEAR;
					}

					GLClear(GL_COLOR_BUFFER_BIT);

					DWORD count = frameCount;
					frame = frames;
					while (count--)
					{
						if (glVersion > GL_VER_1_1)
						{
							if (frameCount == 1)
							{
								if (glFilter)
								{
									GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilter);
									GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilter);
								}

								GLTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame->rect.width, frame->rect.height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, this->attachedSurface->indexBuffer);

								if (fpsState)
								{
									DWORD fps = Main::Round((FLOAT)fpsSum / fpsCount);
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
											WORD* idx = this->attachedSurface->indexBuffer + (FPS_Y + y) * this->mode->width +
												FPS_X + (FPS_STEP + FPS_WIDTH) * (dcount - 1);

											WORD* pix = (WORD*)pixelBuffer + y * (FPS_STEP + FPS_WIDTH) * digCount +
												(FPS_STEP + FPS_WIDTH) * (dcount - 1);

											DWORD width = FPS_STEP;
											do
												*pix++ = *idx++;
											while (--width);

											width = FPS_WIDTH;
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
								GLBindTexture(GL_TEXTURE_2D, frame->id);

								if (glFilter)
								{
									GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilter);
									GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilter);
								}

								WORD* indices = this->attachedSurface->indexBuffer + frame->rect.y * this->mode->width + frame->rect.x;
								DWORD height = frame->rect.height;
								if (frame->rect.width & 1)
								{
									DWORD width = frame->rect.width;
									WORD* pix = (WORD*)pixelBuffer;

									do
									{
										WORD* idx = indices;
										indices += this->mode->width;
										DWORD count = width;

										do
											*pix++ = *idx++;
										while (--count);
									} while (--height);
								}
								else
								{
									DWORD width = frame->rect.width >> 1;
									DWORD* pix = (DWORD*)pixelBuffer;

									do
									{
										DWORD* idx = (DWORD*)indices;
										indices += this->mode->width;
										DWORD count = width;

										do
											*pix++ = *idx++;
										while (--count);
									} while (--height);
								}

								if (fpsState && count == frameCount - 1)
								{
									DWORD fps = Main::Round((FLOAT)fpsSum / fpsCount);
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
											WORD* pix = (WORD*)pixelBuffer + (FPS_Y + y) * frame->rect.width +
												FPS_X + (FPS_STEP + FPS_WIDTH) * (dcount - 1) + FPS_STEP;

											DWORD width = FPS_WIDTH;
											do
											{
												if (*lpDig++)
													*pix = fpsColor;
												++pix;
											} while (--width);
										}

										current = current / 10;
									} while (--dcount);
								}

								GLTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame->rect.width, frame->rect.height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, pixelBuffer);
							}
						}
						else
						{
							GLBindTexture(GL_TEXTURE_2D, frame->id);

							if (glFilter)
							{
								GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilter);
								GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilter);
							}

							WORD* indices = this->attachedSurface->indexBuffer + frame->rect.y * this->mode->width + frame->rect.x;
							DWORD* pix = (DWORD*)pixelBuffer;

							DWORD width = frame->rect.width;
							DWORD height = frame->rect.height;

							do
							{
								WORD* idx = indices;
								indices += this->mode->width;
								DWORD count = width;

								do
								{
									WORD px = *idx++;
									*pix++ = ((px & 0xF800) >> 8) | ((px & 0x07E0) << 5) | ((px & 0x001F) << 19);
								} while (--count);
							} while (--height);

							if (fpsState && count == frameCount - 1)
							{
								DWORD fps = Main::Round((FLOAT)fpsSum / fpsCount);
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
										DWORD* pix = (DWORD*)pixelBuffer + (FPS_Y + y) * frame->rect.width +
											FPS_X + (FPS_STEP + FPS_WIDTH) * (dcount - 1) + FPS_STEP;

										DWORD width = FPS_WIDTH;
										do
										{
											if (*lpDig++)
												*pix = fpsColor;
											++pix;
										} while (--width);
									}

									current = current / 10;
								} while (--dcount);
							}

							GLTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame->rect.width, frame->rect.height, GL_RGBA, GL_UNSIGNED_BYTE, pixelBuffer);
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
					GLFinish();

					if (fpsState != FpsBenchmark)
					{
						WaitForSingleObject(this->hDrawEvent, INFINITE);
						ResetEvent(this->hDrawEvent);
					}
				}
			} while (!this->isFinish);
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
		GLuint vLinear = GL::CompileShaderSource(IDR_LINEAR_VERTEX, GL_VERTEX_SHADER);
		GLuint fLinear = GL::CompileShaderSource(IDR_LINEAR_FRAGMENT, GL_FRAGMENT_SHADER);
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

			GLuint shProgramXRBZ = GLCreateProgram();
			{
				GLuint vXRBZ = GL::CompileShaderSource(IDR_XBRZ_VERTEX, GL_VERTEX_SHADER);
				GLuint fXRBZ = GL::CompileShaderSource(IDR_XBRZ_FRAGMENT, GL_FRAGMENT_SHADER);
				{
					GLAttachShader(shProgramXRBZ, vXRBZ);
					GLAttachShader(shProgramXRBZ, fXRBZ);
					{
						GLLinkProgram(shProgramXRBZ);
					}
					GLDetachShader(shProgramXRBZ, fXRBZ);
					GLDetachShader(shProgramXRBZ, vXRBZ);
				}
				GLDeleteShader(fXRBZ);
				GLDeleteShader(vXRBZ);

				GLUseProgram(shProgramXRBZ);
				{
					GLUniformMatrix4fv(GLGetUniformLocation(shProgramXRBZ, "mvp"), 1, GL_FALSE, (GLfloat*)mvpMatrix);
					GLUniform1i(GLGetUniformLocation(shProgramXRBZ, "tex01"), 0);
					GLUniform2f(GLGetUniformLocation(shProgramXRBZ, "inSize"), (GLfloat)maxTexSize, (GLfloat)maxTexSize);
					GLuint outSize = GLGetUniformLocation(shProgramXRBZ, "outSize");
					GLUniform2f(outSize, this->viewport.clipFactor.x * maxTexSize, this->viewport.clipFactor.y * maxTexSize);

					GLuint arrayName;
					GLGenVertexArrays(1, &arrayName);
					{
						GLBindVertexArray(arrayName);
						{
							GLuint bufferName;
							GLGenBuffers(1, &bufferName);
							{
								GLBindBuffer(GL_ARRAY_BUFFER, bufferName);
								{
									GLBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW);

									GLint attrCoordsLoc = GLGetAttribLocation(shProgramXRBZ, "vCoord");
									GLEnableVertexAttribArray(attrCoordsLoc);
									GLVertexAttribPointer(attrCoordsLoc, 2, GL_FLOAT, GL_FALSE, 16, (GLvoid*)0);

									GLint attrTexCoordsLoc = GLGetAttribLocation(shProgramXRBZ, "vTexCoord");
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
										GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
										GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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

											BOOL isVSync = FALSE;
											WGLSwapInterval(0);

											do
											{
												if (this->attachedSurface)
												{
													if (!isVSync)
													{
														if (this->imageVSync)
														{
															isVSync = TRUE;
															WGLSwapInterval(1);
														}
													}
													else
													{
														if (!this->imageVSync)
														{
															isVSync = FALSE;
															WGLSwapInterval(0);
														}
													}

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

													if (this->CheckView() && this->imageFilter == FilterXRBZ)
														GLUniform2f(outSize, this->viewport.clipFactor.x * maxTexSize, this->viewport.clipFactor.y * maxTexSize);

													if (this->isStateChanged)
													{
														this->isStateChanged = FALSE;

														switch (this->imageFilter)
														{
														case FilterLinear:
															GLUseProgram(shProgramLinear);
															GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
															GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
															break;
														case FilterXRBZ:
															GLUseProgram(shProgramXRBZ);
															GLUniform2f(outSize, this->viewport.clipFactor.x * maxTexSize, this->viewport.clipFactor.y * maxTexSize);
															GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
															GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
															break;
														default:
															GLUseProgram(shProgramLinear);
															GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
															GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
															break;
														}
													}

													GLTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->mode->width, this->mode->height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, this->attachedSurface->indexBuffer);

													if (fpsState)
													{
														DWORD fps = Main::Round((FLOAT)fpsSum / fpsCount);
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
																WORD* idx = this->attachedSurface->indexBuffer + (FPS_Y + y) * this->mode->width +
																	FPS_X + (FPS_STEP + FPS_WIDTH) * (dcount - 1);

																WORD* pix = (WORD*)pixelBuffer + y * (FPS_STEP + FPS_WIDTH) * digCount +
																	(FPS_STEP + FPS_WIDTH) * (dcount - 1);

																DWORD width = FPS_STEP;
																do
																	*pix++ = *idx++;
																while (--width);

																width = FPS_WIDTH;
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

													GLClear(GL_COLOR_BUFFER_BIT);
													GLDrawArrays(GL_TRIANGLE_FAN, 0, 4);

													GLFlush();
													SwapBuffers(this->hDc);
													GLFinish();

													if (fpsState != FpsBenchmark)
													{
														WaitForSingleObject(this->hDrawEvent, INFINITE);
														ResetEvent(this->hDrawEvent);
													}
												}
											} while (!this->isFinish);
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
					}
					GLDeleteVertexArrays(1, &arrayName);
				}
				GLUseProgram(NULL);
			}
			GLDeleteProgram(shProgramXRBZ);
		}
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

		if (this->imageAspect && this->viewport.viewFactor.x != this->viewport.viewFactor.y)
		{
			if (this->viewport.viewFactor.x > this->viewport.viewFactor.y)
			{
				FLOAT fw = this->viewport.viewFactor.y * this->mode->width;
				this->viewport.rectangle.width = Main::Round(fw);
				this->viewport.rectangle.x = Main::Round(((FLOAT)this->viewport.width - fw) / 2.0f);
				this->viewport.clipFactor.x = this->viewport.viewFactor.y;
			}
			else
			{
				FLOAT fh = this->viewport.viewFactor.x * this->mode->height;
				this->viewport.rectangle.height = Main::Round(fh);
				this->viewport.rectangle.y = Main::Round(((FLOAT)this->viewport.height - fh) / 2.0f);
				this->viewport.clipFactor.y = this->viewport.viewFactor.x;
			}
		}

		GLViewport(this->viewport.rectangle.x, this->viewport.rectangle.y, this->viewport.rectangle.width, this->viewport.rectangle.height);

		if (this->imageAspect && this->windowState != WinStateWindowed)
		{
			RECT clipRect;
			GetClientRect(this->hWnd, &clipRect);

			clipRect.left = this->viewport.rectangle.x;
			clipRect.right = clipRect.left + this->viewport.rectangle.width;
			clipRect.bottom = clipRect.bottom - this->viewport.rectangle.y;
			clipRect.top = clipRect.bottom - this->viewport.rectangle.height;

			ClientToScreen(this->hWnd, (POINT*)&clipRect.left);
			ClientToScreen(this->hWnd, (POINT*)&clipRect.right);

			ClipCursor(&clipRect);
		}
		else
			ClipCursor(NULL);

		return TRUE;
	}

	return FALSE;
}

VOID DirectDraw::ScaleMouse(LPPOINT p)
{
	p->x = Main::Round((FLOAT)(p->x - this->viewport.rectangle.x) * (FLOAT)this->mode->width / (FLOAT)this->viewport.rectangle.width);
	p->y = Main::Round((FLOAT)(p->y - this->viewport.rectangle.y) * (FLOAT)this->mode->height / (FLOAT)this->viewport.rectangle.height);
}

VOID DirectDraw::CheckMenu(HMENU hMenu)
{
	if (!hMenu)
		hMenu = GetMenu(this->hWnd);

	if (!hMenu) return;

	CheckMenuItem(hMenu, IDM_ASPECT_RATIO, MF_BYCOMMAND | (this->imageAspect ? MF_CHECKED : MF_UNCHECKED));
	CheckMenuItem(hMenu, IDM_VSYNC, MF_BYCOMMAND | (this->imageVSync ? MF_CHECKED : MF_UNCHECKED));
	EnableMenuItem(hMenu, IDM_FILT_XRBZ, MF_BYCOMMAND | (!glVersion || glVersion >= GL_VER_3_0 ? MF_ENABLED : MF_DISABLED));

	switch (this->imageFilter)
	{
	case FilterLinear:
		CheckMenuItem(hMenu, IDM_FILT_OFF, MF_BYCOMMAND | MF_UNCHECKED);
		CheckMenuItem(hMenu, IDM_FILT_LINEAR, MF_BYCOMMAND | MF_CHECKED);
		CheckMenuItem(hMenu, IDM_FILT_XRBZ, MF_BYCOMMAND | MF_UNCHECKED);
		break;
	case FilterXRBZ:
		CheckMenuItem(hMenu, IDM_FILT_OFF, MF_BYCOMMAND | MF_UNCHECKED);
		CheckMenuItem(hMenu, IDM_FILT_LINEAR, MF_BYCOMMAND | MF_UNCHECKED);
		CheckMenuItem(hMenu, IDM_FILT_XRBZ, MF_BYCOMMAND | MF_CHECKED);
		break;
	default:
		CheckMenuItem(hMenu, IDM_FILT_OFF, MF_BYCOMMAND | MF_CHECKED);
		CheckMenuItem(hMenu, IDM_FILT_LINEAR, MF_BYCOMMAND | MF_UNCHECKED);
		CheckMenuItem(hMenu, IDM_FILT_XRBZ, MF_BYCOMMAND | MF_UNCHECKED);
		break;
	}
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

	DWORD filter = Config::Get("ImageFilter", 0);
	switch (filter)
	{
	case 1:
		this->imageFilter = FilterLinear;
		break;

	case 2:
		this->imageFilter = FilterXRBZ;
		break;

	default:
		this->imageFilter = FilterNearest;
		break;
	}

	this->imageAspect = Config::Get("ImageAspect", TRUE);
	this->imageVSync = Config::Get("ImageVSync", TRUE);

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
	ClipCursor(NULL);
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

	MoveWindow(this->hWnd, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), TRUE);

	SetForegroundWindow(this->hWnd);

	return DD_OK;
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