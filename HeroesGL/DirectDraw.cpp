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
#include "Hooks.h"
#include "DirectDraw.h"
#include "FpsCounter.h"
#include "Config.h"
#include "CommCtrl.h"
#include "Shellapi.h"

#define VK_I 0x49
#define VK_F 0x46

#define MIN_WIDTH 240
#define MIN_HEIGHT 180

WNDPROC OldWindowProc, OldPanelProc;
BOOL isVSync;

#pragma region Not Implemented
ULONG DirectDraw::AddRef() { return 0; }
HRESULT DirectDraw::Compact() { return DD_OK; }
HRESULT DirectDraw::EnumSurfaces(DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMSURFACESCALLBACK) { return DD_OK; }
HRESULT DirectDraw::FlipToGDISurface(void) { return DD_OK; }
HRESULT DirectDraw::GetCaps(LPDDCAPS, LPDDCAPS) { return DD_OK; }
HRESULT DirectDraw::GetDisplayMode(LPDDSURFACEDESC) { return DD_OK; }
HRESULT DirectDraw::GetFourCCCodes(LPDWORD, LPDWORD) { return DD_OK; }
HRESULT DirectDraw::GetGDISurface(LPDIRECTDRAWSURFACE *) { return DD_OK; }
HRESULT DirectDraw::GetMonitorFrequency(LPDWORD) { return DD_OK; }
HRESULT DirectDraw::GetScanLine(LPDWORD) { return DD_OK; }
HRESULT DirectDraw::GetVerticalBlankStatus(LPBOOL) { return DD_OK; }
HRESULT DirectDraw::Initialize(GUID *) { return DD_OK; }
HRESULT DirectDraw::RestoreDisplayMode() { return DD_OK; }
HRESULT DirectDraw::DuplicateSurface(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE *) { return DD_OK; }
HRESULT DirectDraw::WaitForVerticalBlank(DWORD, HANDLE) { return DD_OK; }
HRESULT DirectDraw::QueryInterface(REFIID riid, LPVOID* ppvObj) { return DD_OK; }
HRESULT DirectDraw::EnumDisplayModes(DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK) { return DD_OK; }
#pragma endregion

LRESULT __stdcall AboutProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		SetWindowLong(hDlg, GWL_EXSTYLE, NULL);
		EnumChildWindows(hDlg, Hooks::EnumChildProc, NULL);

		CHAR email[50];
		GetDlgItemText(hDlg, IDC_LNK_EMAIL, email, sizeof(email) - 1);
		CHAR anchor[256];
		StrPrint(anchor, "<A HREF=\"mailto:%s\">%s</A>", email, email);
		SetDlgItemText(hDlg, IDC_LNK_EMAIL, anchor);

		break;
	}

	case WM_NOTIFY:
	{
		if (((NMHDR*)lParam)->code == NM_CLICK && wParam == IDC_LNK_EMAIL)
		{
			NMLINK* pNMLink = (NMLINK*)lParam;
			LITEM iItem = pNMLink->item;

			CHAR url[256];
			StrToAnsi(url, pNMLink->item.szUrl, sizeof(url) - 1);

			SHELLEXECUTEINFO shExecInfo;
			MemoryZero(&shExecInfo, sizeof(SHELLEXECUTEINFO));
			shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
			shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
			shExecInfo.lpFile = url;
			shExecInfo.nShow = SW_SHOW;

			ShellExecuteEx(&shExecInfo);
		}

		break;
	}

	case WM_COMMAND:
	{
		if (wParam == IDC_BTN_OK)
			EndDialog(hDlg, TRUE);
		break;
	}

	default:
		break;
	}

	return DefWindowProc(hDlg, uMsg, wParam, lParam);
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
		if (ddraw && ddraw->imageAspect)
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
		case IDM_HELP_WRAPPER:
		{
			INT_PTR res;
			if (hActCtx && hActCtx != INVALID_HANDLE_VALUE)
			{
				ULONG_PTR cookie;
				ActivateActCtxC(hActCtx, &cookie);
				res = DialogBoxParam(hDllModule, MAKEINTRESOURCE(configLanguage == LNG_ENGLISH ? IDD_ENGLISH : IDD_RUSSIAN), hWnd, (DLGPROC)AboutProc, NULL);
				DeactivateActCtxC(0, cookie);
			}
			else
				res = DialogBoxParam(hDllModule, MAKEINTRESOURCE(configLanguage == LNG_ENGLISH ? IDD_ENGLISH : IDD_RUSSIAN), hWnd, (DLGPROC)AboutProc, NULL);

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

	case WM_ACTIVATEAPP:
	{
		DirectDraw* ddraw = Main::FindDirectDrawByWindow(hWnd);
		if (ddraw && ddraw->windowState != WinStateWindowed)
		{
			if ((BOOL)wParam)
				ddraw->RenderStart();
			else
				ddraw->RenderStop();
		}

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

VOID DirectDraw::ScaleMouse(LPPOINT p)
{
	p->x = (LONG)MathRound((FLOAT)(p->x - this->viewport.rectangle.x) * (FLOAT)this->viewport.width / (FLOAT)this->viewport.rectangle.width);
	p->y = (LONG)MathRound((FLOAT)(p->y - this->viewport.rectangle.y) * (FLOAT)this->viewport.height / (FLOAT)this->viewport.rectangle.height);
}

VOID DirectDraw::CheckMenu(HMENU hMenu)
{
	if (!hMenu)
		hMenu = GetMenu(this->hWnd);

	if (!hMenu) return;

	CheckMenuItem(hMenu, IDM_ASPECT_RATIO, MF_BYCOMMAND | (this->imageAspect ? MF_CHECKED : MF_UNCHECKED));

	EnableMenuItem(hMenu, IDM_VSYNC, MF_BYCOMMAND | (!glVersion || WGLSwapInterval ? MF_ENABLED : MF_DISABLED));
	CheckMenuItem(hMenu, IDM_VSYNC, MF_BYCOMMAND | (this->imageVSync && (!glVersion || WGLSwapInterval) ? MF_CHECKED : MF_UNCHECKED));

	EnableMenuItem(hMenu, IDM_FILT_XRBZ, MF_BYCOMMAND | (!glVersion || glVersion >= GL_VER_3_0 ? MF_ENABLED : MF_DISABLED));

	switch (this->imageFilter)
	{
	case FilterLinear:
		CheckMenuItem(hMenu, IDM_FILT_OFF, MF_BYCOMMAND | MF_UNCHECKED);
		CheckMenuItem(hMenu, IDM_FILT_LINEAR, MF_BYCOMMAND | MF_CHECKED);
		CheckMenuItem(hMenu, IDM_FILT_XRBZ, MF_BYCOMMAND | MF_UNCHECKED);
		break;
	case FilterXRBZ:
		if (!glVersion || glVersion >= GL_VER_3_0)
		{
			CheckMenuItem(hMenu, IDM_FILT_OFF, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(hMenu, IDM_FILT_LINEAR, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(hMenu, IDM_FILT_XRBZ, MF_BYCOMMAND | MF_CHECKED);
		}
		else
		{
			CheckMenuItem(hMenu, IDM_FILT_OFF, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(hMenu, IDM_FILT_LINEAR, MF_BYCOMMAND | MF_CHECKED);
			CheckMenuItem(hMenu, IDM_FILT_XRBZ, MF_BYCOMMAND | MF_UNCHECKED);
		}
		break;
	default:
		CheckMenuItem(hMenu, IDM_FILT_OFF, MF_BYCOMMAND | MF_CHECKED);
		CheckMenuItem(hMenu, IDM_FILT_LINEAR, MF_BYCOMMAND | MF_UNCHECKED);
		CheckMenuItem(hMenu, IDM_FILT_XRBZ, MF_BYCOMMAND | MF_UNCHECKED);
		break;
	}
}

DWORD __stdcall RenderThread(LPVOID lpParameter)
{
	DirectDraw* ddraw = (DirectDraw*)lpParameter;
	ddraw->hDc = ::GetDC(ddraw->hDraw);
	{
		PIXELFORMATDESCRIPTOR pfd;
		GL::PreparePixelFormatDescription(&pfd);
		if (!glPixelFormat && !GL::PreparePixelFormat(&pfd))
		{
			glPixelFormat = ChoosePixelFormat(ddraw->hDc, &pfd);
			if (!glPixelFormat)
				Main::ShowError("ChoosePixelFormat failed", __FILE__, __LINE__);
			else if (pfd.dwFlags & PFD_NEED_PALETTE)
				Main::ShowError("Needs palette", __FILE__, __LINE__);
		}

		if (!SetPixelFormat(ddraw->hDc, glPixelFormat, &pfd))
			Main::ShowError("SetPixelFormat failed", __FILE__, __LINE__);

		MemoryZero(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		if (DescribePixelFormat(ddraw->hDc, glPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd) == NULL)
			Main::ShowError("DescribePixelFormat failed", __FILE__, __LINE__);

		if ((pfd.iPixelType != PFD_TYPE_RGBA) ||
			(pfd.cRedBits < 5) || (pfd.cGreenBits < 5) || (pfd.cBlueBits < 5))
			Main::ShowError("Bad pixel type", __FILE__, __LINE__);

		do
		{
			WaitForSingleObject(ddraw->hDrawEvent, INFINITE);
			if (ddraw->width)
			{
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
							DWORD maxSize = ddraw->width > ddraw->height ? ddraw->width : ddraw->height;

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
		} while (!ddraw->isFinish);
	}
	::ReleaseDC(ddraw->hDraw, ddraw->hDc);
	ddraw->hDc = NULL;

	return NULL;
}

VOID DirectDraw::RenderOld(DWORD glMaxTexSize)
{
	if (this->imageFilter == FilterXRBZ)
		this->imageFilter = FilterLinear;

	if (glMaxTexSize < 256)
		glMaxTexSize = 256;

	DWORD size = this->width > this->height ? this->width : this->height;
	DWORD maxAllow = 1;
	while (maxAllow < size)
		maxAllow <<= 1;

	DWORD maxTexSize = maxAllow < glMaxTexSize ? maxAllow : glMaxTexSize;
	DWORD glFilter = this->imageFilter == FilterNearest ? GL_NEAREST : GL_LINEAR;

	DWORD framePerWidth = this->width / maxTexSize + (this->width % maxTexSize ? 1 : 0);
	DWORD framePerHeight = this->height / maxTexSize + (this->height % maxTexSize ? 1 : 0);
	DWORD frameCount = framePerWidth * framePerHeight;
	Frame* frames = (Frame*)MemoryAlloc(frameCount * sizeof(Frame));
	{
		Frame* frame = frames;
		for (DWORD y = 0; y < this->height; y += maxTexSize)
		{
			DWORD height = this->height - y;
			if (height > maxTexSize)
				height = maxTexSize;

			for (DWORD x = 0; x < this->width; x += maxTexSize, ++frame)
			{
				DWORD width = this->width - x;
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
				GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilter);
				GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilter);

				GLTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

				GLTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, maxTexSize, maxTexSize, GL_NONE, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			}
		}

		GLMatrixMode(GL_PROJECTION);
		GLLoadIdentity();
		GLOrtho(0.0, (GLdouble)this->width, (GLdouble)this->height, 0.0, 0.0, 1.0);
		GLMatrixMode(GL_MODELVIEW);
		GLLoadIdentity();

		GLEnable(GL_TEXTURE_2D);
		GLClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		this->viewport.refresh = TRUE;

		VOID* frameBuffer = MemoryAlloc(maxTexSize * maxTexSize * sizeof(DWORD));
		{
			DWORD fpsQueue[FPS_COUNT];
			DWORD tickQueue[FPS_COUNT];

			DWORD fpsIdx = -1;
			DWORD fpsTotal = 0;
			DWORD fpsCount = 0;
			INT fpsSum = 0;
			MemoryZero(fpsQueue, sizeof(fpsQueue));
			MemoryZero(tickQueue, sizeof(tickQueue));

			BOOL isVSync = FALSE;
			if (WGLSwapInterval)
				WGLSwapInterval(0);

			DWORD clear = TRUE;
			do
			{
				if (this->attachedSurface)
				{
					DirectDrawSurface* surface = this->attachedSurface;
					if (surface->attachedPallete)
					{
						DirectDrawPalette* palette = surface->attachedPallete;

						if (WGLSwapInterval)
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
								MemoryZero(fpsQueue, sizeof(fpsQueue));
								MemoryZero(tickQueue, sizeof(tickQueue));
							}

							++fpsTotal;
							if (fpsCount < FPS_COUNT)
								++fpsCount;

							++fpsIdx;
							if (fpsIdx == FPS_COUNT)
								fpsIdx = 0;

							DWORD diff = tick - tickQueue[fpsTotal != fpsCount ? fpsIdx : 0];
							tickQueue[fpsIdx] = tick;

							DWORD fps = diff ? (DWORD)MathRound(1000.0f / diff * fpsCount) : 9999;

							DWORD* queue = &fpsQueue[fpsIdx];
							fpsSum -= *queue - fps;
							*queue = fps;
						}
						else if (isFpsChanged)
						{
							isFpsChanged = FALSE;
							clear = TRUE;
						}

						if (this->CheckView())
						{
							clear = TRUE;
							GLViewport(this->viewport.rectangle.x, this->viewport.rectangle.y, this->viewport.rectangle.width, this->viewport.rectangle.height);
						}

						glFilter = 0;
						if (this->isStateChanged)
						{
							this->isStateChanged = FALSE;
							clear = TRUE;
							glFilter = this->imageFilter == FilterNearest ? GL_NEAREST : GL_LINEAR;
						}

						if (palette->isChanged)
						{
							palette->isChanged = FALSE;
							clear = TRUE;
						}

						RECT* updateClip = surface->poinetrClip;
						RECT* finClip = surface->currentClip;
						surface->poinetrClip = finClip;

						if (clear)
						{
							if (clear & 1)
								++clear;
							else
								clear = FALSE;

							updateClip = (finClip == surface->clipsList ? surface->endClip : finClip) - 1;
							updateClip->left = 0;
							updateClip->top = 0;
							updateClip->right = this->width;
							updateClip->bottom = this->height;

							GLClear(GL_COLOR_BUFFER_BIT);
						}

						DWORD count = frameCount;
						frame = frames;
						while (count--)
						{
							if (frameCount == 1)
							{
								if (glFilter)
								{
									GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilter);
									GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilter);
								}

								while (updateClip != finClip)
								{
									RECT update = *updateClip;
									DWORD texWidth = update.right - update.left;
									DWORD texHeight = update.bottom - update.top;

									if (texWidth == this->width)
										GLTexSubImage2D(GL_TEXTURE_2D, 0, 0, update.top, texWidth, texHeight, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixelBuffer + update.top * texWidth);
									else
									{
										DWORD* source = surface->pixelBuffer + update.top * this->width + update.left;
										DWORD* dest = (DWORD*)frameBuffer;
										DWORD copyWidth = texWidth;
										DWORD copyHeight = texHeight;
										do
										{
											DWORD* src = (DWORD*)source;
											source += this->width;

											DWORD count = copyWidth;
											do
												*dest++ = *src++;
											while (--count);
										} while (--copyHeight);

										GLTexSubImage2D(GL_TEXTURE_2D, 0, update.left, update.top, texWidth, texHeight, GL_RGBA, GL_UNSIGNED_BYTE, frameBuffer);
									}

									if (++updateClip == surface->endClip)
										updateClip = surface->clipsList;
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

								Rect* rect = &frame->rect;
								INT rect_right = rect->x + rect->width;
								INT rect_bottom = rect->y + rect->height;

								RECT* update = updateClip;
								while (update != finClip)
								{
									RECT clip = {
										rect->x > update->left ? rect->x : update->left,
										rect->y > update->top ? rect->y : update->top,
										rect_right < update->right ? rect_right : update->right,
										rect_bottom < update->bottom ? rect_bottom : update->bottom
									};

									INT clipWidth = clip.right - clip.left;
									INT clipHeight = clip.bottom - clip.top;
									if (clipWidth > 0 && clipHeight > 0)
									{
										if (clipWidth & 1)
										{
											++clipWidth;
											if (clip.left != rect->x)
												--clip.left;
											else
												++clip.right;
										}

										DWORD* source = surface->pixelBuffer + clip.top * this->width + clip.left;
										DWORD* dest = (DWORD*)frameBuffer;
										DWORD copyWidth = clipWidth;
										DWORD copyHeight = clipHeight;
										do
										{
											DWORD* src = (DWORD*)source;
											source += this->width;

											DWORD count = copyWidth;
											do
												*dest++ = *src++;
											while (--count);
										} while (--copyHeight);

										GLTexSubImage2D(GL_TEXTURE_2D, 0, clip.left - rect->x, clip.top - rect->y, clipWidth, clipHeight, GL_RGBA, GL_UNSIGNED_BYTE, frameBuffer);
									}

									if (++update == surface->endClip)
										update = surface->clipsList;
								}
							}

							if (fpsState && frame == frames)
							{
								DWORD fps = (DWORD)MathRound((FLOAT)fpsSum / fpsCount);
								DWORD digCount = 0;
								DWORD current = fps;
								do
								{
									++digCount;
									current = current / 10;
								} while (current);

								DWORD fpsColor = fpsState == FpsBenchmark ? 0xFF00FFFF : 0xFFFFFFFF;
								DWORD dcount = digCount;
								do
								{
									bool* lpDig = (bool*)counters + FPS_WIDTH * FPS_HEIGHT * (fps % 10);

									for (DWORD y = 0; y < FPS_HEIGHT; ++y)
									{
										DWORD* idx = surface->pixelBuffer + (FPS_Y + y) * this->width +
											FPS_X + (FPS_STEP + FPS_WIDTH) * (dcount - 1);

										DWORD* pix = (DWORD*)frameBuffer + y * (FPS_STEP + FPS_WIDTH) * 4 +
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

									fps = fps / 10;
								} while (--dcount);

								dcount = 4;
								while (dcount != digCount)
								{
									for (DWORD y = 0; y < FPS_HEIGHT; ++y)
									{
										DWORD* idx = surface->pixelBuffer + (FPS_Y + y) * this->width +
											FPS_X + (FPS_STEP + FPS_WIDTH) * (dcount - 1);

										DWORD* pix = (DWORD*)frameBuffer + y * (FPS_STEP + FPS_WIDTH) * 4 +
											(FPS_STEP + FPS_WIDTH) * (dcount - 1);

										DWORD width = FPS_STEP + FPS_WIDTH;
										do
											*pix++ = *idx++;
										while (--width);
									}

									--dcount;
								}

								GLTexSubImage2D(GL_TEXTURE_2D, 0, FPS_X, FPS_Y, (FPS_STEP + FPS_WIDTH) * 4, FPS_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, frameBuffer);
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

						SwapBuffers(this->hDc);
						if (fpsState != FpsBenchmark)
						{
							WaitForSingleObject(this->hDrawEvent, INFINITE);
							ResetEvent(this->hDrawEvent);
						}
						GLFinish();
					}
				}
			} while (!this->isFinish);
		}
		MemoryFree(frameBuffer);

		frame = frames;
		DWORD count = frameCount;
		while (count--)
		{
			GLDeleteTextures(1, &frame->id);
			++frame;
		}
	}
	MemoryFree(frames);
}

VOID DirectDraw::RenderNew()
{
	DWORD maxSize = this->width > this->height ? this->width : this->height;
	DWORD maxTexSize = 1;
	while (maxTexSize < maxSize) maxTexSize <<= 1;
	FLOAT texWidth = this->width == maxTexSize ? 1.0f : (FLOAT)this->width / maxTexSize;
	FLOAT texHeight = this->height == maxTexSize ? 1.0f : (FLOAT)this->height / maxTexSize;

	FLOAT buffer[8][4] = {
		{ 0.0f, 0.0f, 0.0f, 0.0f },
		{ (FLOAT)this->width, 0.0f, texWidth, 0.0f },
		{ (FLOAT)this->width, (FLOAT)this->height, texWidth, texHeight },
		{ 0.0f, (FLOAT)this->height, 0.0f, texHeight },
		{ 0.0f, 0.0f, 0.0f, texHeight },
		{ (FLOAT)this->width, 0.0f, texWidth, texHeight },
		{ (FLOAT)this->width, (FLOAT)this->height, texWidth, 0.0f },
		{ 0.0f, (FLOAT)this->height, 0.0f, 0.0f }
	};

	FLOAT mvpMatrix[4][4] = {
		{ 2.0f / this->width, 0.0f, 0.0f, 0.0f },
		{ 0.0f, -2.0f / this->height, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 2.0f, 0.0f },
		{ -1.0f, 1.0f, -1.0f, 1.0f }
	};

	FLOAT* stencil = (FLOAT*)MemoryAlloc(STENCIL_COUNT * 4 * 2);
	{
		GLuint shProgramStencil = GLCreateProgram();
		{
			GLuint vStencil = GL::CompileShaderSource(IDR_STENCIL_VERTEX, GL_VERTEX_SHADER);
			GLuint fStencil = GL::CompileShaderSource(IDR_STENCIL_FRAGMENT, GL_FRAGMENT_SHADER);
			{

				GLAttachShader(shProgramStencil, vStencil);
				GLAttachShader(shProgramStencil, fStencil);
				{
					GLLinkProgram(shProgramStencil);
				}
				GLDetachShader(shProgramStencil, fStencil);
				GLDetachShader(shProgramStencil, vStencil);
			}
			GLDeleteShader(fStencil);
			GLDeleteShader(vStencil);

			GLUseProgram(shProgramStencil);
			{
				GLUniformMatrix4fv(GLGetUniformLocation(shProgramStencil, "mvp"), 1, GL_FALSE, (GLfloat*)mvpMatrix);

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

								GLuint inSize = GLGetUniformLocation(shProgramXRBZ, "inSize");
								GLUniform2f(inSize, (GLfloat)maxTexSize, (GLfloat)maxTexSize);
								GLuint outSize = GLGetUniformLocation(shProgramXRBZ, "outSize");
								GLUniform2f(outSize, (GLfloat)maxTexSize, (GLfloat)maxTexSize);

								GLUseProgram(shProgramStencil);
								GLuint stArrayName;
								GLGenVertexArrays(1, &stArrayName);
								{
									GLBindVertexArray(stArrayName);
									{
										GLuint stBufferName;
										GLGenBuffers(1, &stBufferName);
										{
											GLBindBuffer(GL_ARRAY_BUFFER, stBufferName);
											{
												GLBufferData(GL_ARRAY_BUFFER, STENCIL_COUNT * 4 * 2 * sizeof(FLOAT), NULL, GL_STREAM_DRAW);

												GLint attrCoordsLoc = GLGetAttribLocation(shProgramStencil, "vCoord");
												GLEnableVertexAttribArray(attrCoordsLoc);
												GLVertexAttribPointer(attrCoordsLoc, 2, GL_FLOAT, GL_FALSE, 8, (GLvoid*)0);

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
																	DWORD filter = this->imageFilter == FilterLinear ? GL_LINEAR : GL_NEAREST;
																	GLActiveTexture(GL_TEXTURE0);
																	GLBindTexture(GL_TEXTURE_2D, textureId);
																	GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glCapsClampToEdge);
																	GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glCapsClampToEdge);
																	GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
																	GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
																	GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
																	GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
																	GLTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, maxTexSize, maxTexSize, GL_NONE, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

																	GLuint fboId;
																	GLGenFramebuffers(1, &fboId);
																	{
																		DWORD uniSize = 0;
																		GLuint rboId = 0, tboId = 0;
																		{
																			GLClearColor(0.0f, 0.0f, 0.0f, 1.0f);

																			VOID* frameBuffer = MemoryAlloc(this->width * this->height * sizeof(DWORD));
																			{
																				this->isStateChanged = TRUE;

																				DWORD fpsQueue[FPS_COUNT];
																				DWORD tickQueue[FPS_COUNT];

																				DWORD fpsIdx = -1;
																				DWORD fpsTotal = 0;
																				DWORD fpsCount = 0;
																				INT fpsSum = 0;
																				MemoryZero(fpsQueue, sizeof(fpsQueue));
																				MemoryZero(tickQueue, sizeof(tickQueue));

																				BOOL isVSync = FALSE;
																				if (WGLSwapInterval)
																					WGLSwapInterval(0);

																				DWORD  clear = TRUE;
																				do
																				{
																					if (this->attachedSurface)
																					{
																						DirectDrawSurface* surface = this->attachedSurface;
																						if (surface->attachedPallete)
																						{
																							DirectDrawPalette* palette = surface->attachedPallete;

																							if (WGLSwapInterval)
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
																							}

																							if (this->isStateChanged)
																							{
																								this->viewport.refresh = TRUE;
																								isFpsChanged = TRUE;
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
																									MemoryZero(fpsQueue, sizeof(fpsQueue));
																									MemoryZero(tickQueue, sizeof(tickQueue));
																								}

																								++fpsTotal;
																								if (fpsCount < FPS_COUNT)
																									++fpsCount;

																								++fpsIdx;
																								if (fpsIdx == FPS_COUNT)
																									fpsIdx = 0;

																								DWORD diff = tick - tickQueue[fpsTotal != fpsCount ? fpsIdx : 0];
																								tickQueue[fpsIdx] = tick;

																								DWORD fps = diff ? (DWORD)MathRound(1000.0f / diff * fpsCount) : 9999;

																								DWORD* queue = &fpsQueue[fpsIdx];
																								fpsSum -= *queue - fps;
																								*queue = fps;
																							}

																							RECT* updateClip = surface->poinetrClip;
																							RECT* finClip = surface->currentClip;
																							surface->poinetrClip = finClip;

																							if (this->imageFilter == FilterXRBZ)
																							{
																								if (this->isStateChanged)
																								{
																									this->isStateChanged = FALSE;
																									GLUseProgram(shProgramXRBZ);
																								}

																								if (isFpsChanged)
																								{
																									isFpsChanged = FALSE;
																									clear = TRUE;
																								}

																								GLBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboId);

																								if (this->CheckView())
																								{
																									clear = TRUE;

																									DWORD sw = this->viewport.rectangle.width;
																									DWORD sh = this->viewport.rectangle.height;

																									DWORD w = this->width;
																									while (w < sw) w <<= 1;
																									DWORD tw = 1;
																									while (tw < w) tw <<= 1;

																									DWORD h = this->height;
																									while (h < sh) h <<= 1;
																									DWORD th = 1;
																									while (th < h) th <<= 1;

																									DWORD newUniSize = tw > th ? tw : th;
																									if (newUniSize != uniSize)
																									{
																										if (!uniSize)
																										{
																											GLGenTextures(1, &tboId);
																											GLGenRenderbuffers(1, &rboId);
																										}

																										uniSize = newUniSize;
																										GLUniform2f(outSize, (FLOAT)uniSize, (FLOAT)uniSize);

																										// Gen texture
																										GLBindTexture(GL_TEXTURE_2D, tboId);
																										GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glCapsClampToEdge);
																										GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glCapsClampToEdge);
																										GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
																										GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
																										GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
																										GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
																										GLTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, uniSize, uniSize, GL_NONE, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

																										// Get storage
																										GLBindRenderbuffer(GL_RENDERBUFFER, rboId);
																										GLRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, uniSize, uniSize);
																										GLBindRenderbuffer(GL_RENDERBUFFER, NULL);

																										GLFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tboId, 0);
																										GLFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboId);
																									}
																								}

																								DWORD k = uniSize / maxTexSize;
																								GLViewport(0, 0, this->width * k, this->height * k);

																								// Clear and stencil
																								if (clear)
																								{
																									clear = FALSE;

																									SwapBuffers(this->hDc);
																									GLFinish();
																									GLClear(GL_COLOR_BUFFER_BIT);

																									palette->isChanged = FALSE;
																									updateClip = (finClip == surface->clipsList ? surface->endClip : finClip) - 1;
																									updateClip->left = 0;
																									updateClip->top = 0;
																									updateClip->right = this->width;
																									updateClip->bottom = this->height;
																								}
																								else
																								{
																									if (palette->isChanged)
																									{
																										palette->isChanged = FALSE;
																										updateClip = (finClip == surface->clipsList ? surface->endClip : finClip) - 1;
																										updateClip->left = 0;
																										updateClip->top = 0;
																										updateClip->right = this->width;
																										updateClip->bottom = this->height;
																									}

																									GLEnable(GL_STENCIL_TEST);
																									GLClear(GL_STENCIL_BUFFER_BIT);

																									GLUseProgram(shProgramStencil);
																									{
																										GLColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
																										GLStencilFunc(GL_ALWAYS, 0x01, 0x01);
																										GLStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
																										{
																											GLBindVertexArray(stArrayName);
																											GLBindBuffer(GL_ARRAY_BUFFER, stBufferName);
																											{
																												FLOAT* pointer = stencil;

																												RECT* clip = updateClip;
																												while (clip != finClip)
																												{
																													*pointer++ = (FLOAT)clip->left;  *pointer++ = (FLOAT)clip->top;
																													*pointer++ = (FLOAT)clip->right;  *pointer++ = (FLOAT)clip->top;
																													*pointer++ = (FLOAT)clip->right;  *pointer++ = (FLOAT)clip->bottom;

																													*pointer++ = (FLOAT)clip->left;  *pointer++ = (FLOAT)clip->top;
																													*pointer++ = (FLOAT)clip->right;  *pointer++ = (FLOAT)clip->bottom;
																													*pointer++ = (FLOAT)clip->left;  *pointer++ = (FLOAT)clip->bottom;

																													if (++clip == surface->endClip)
																														clip = surface->clipsList;
																												}

																												if (fpsState)
																												{
																													*pointer++ = (FLOAT)(FPS_X);  *pointer++ = (FLOAT)(FPS_Y);
																													*pointer++ = (FLOAT)(FPS_X + (FPS_STEP + FPS_WIDTH) * 4);  *pointer++ = (FLOAT)(FPS_Y);
																													*pointer++ = (FLOAT)(FPS_X + (FPS_STEP + FPS_WIDTH) * 4);  *pointer++ = (FLOAT)(FPS_Y + FPS_HEIGHT);

																													*pointer++ = (FLOAT)(FPS_X);  *pointer++ = (FLOAT)(FPS_Y);
																													*pointer++ = (FLOAT)(FPS_X + (FPS_STEP + FPS_WIDTH) * 4);  *pointer++ = (FLOAT)(FPS_Y + FPS_HEIGHT);
																													*pointer++ = (FLOAT)(FPS_X);  *pointer++ = (FLOAT)(FPS_Y + FPS_HEIGHT);
																												}

																												DWORD count = pointer - stencil;
																												if (count)
																												{
																													GLBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(FLOAT), stencil);
																													GLDrawArrays(GL_TRIANGLES, 0, count >> 1);
																												}
																											}
																											GLBindVertexArray(arrayName);
																											GLBindBuffer(GL_ARRAY_BUFFER, bufferName);
																										}
																										GLColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
																										GLStencilFunc(GL_EQUAL, 0x01, 0x01);
																										GLStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
																									}
																									GLUseProgram(shProgramXRBZ);
																								}

																								GLBindTexture(GL_TEXTURE_2D, textureId);
																								GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
																								GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
																							}
																							else
																							{
																								if (isFpsChanged)
																								{
																									isFpsChanged = FALSE;
																									clear = TRUE;
																								}

																								if (this->CheckView())
																								{
																									clear = TRUE;
																									GLViewport(this->viewport.rectangle.x, this->viewport.rectangle.y, this->viewport.rectangle.width, this->viewport.rectangle.height);
																								}

																								if (this->isStateChanged)
																								{
																									this->isStateChanged = FALSE;
																									clear = TRUE;
																									GLUseProgram(shProgramLinear);

																									if (uniSize)
																									{
																										GLDeleteTextures(1, &tboId);
																										GLDeleteRenderbuffers(1, &rboId);
																										uniSize = 0;
																									}

																									filter = this->imageFilter == FilterLinear ? GL_LINEAR : GL_NEAREST;
																									GLBindTexture(GL_TEXTURE_2D, textureId);
																									GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
																									GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
																								}

																								if (palette->isChanged)
																								{
																									palette->isChanged = FALSE;
																									clear = TRUE;
																								}

																								if (clear)
																								{
																									if (clear & 1)
																										++clear;
																									else
																										clear = FALSE;

																									updateClip = (finClip == surface->clipsList ? surface->endClip : finClip) - 1;
																									updateClip->left = 0;
																									updateClip->top = 0;
																									updateClip->right = this->width;
																									updateClip->bottom = this->height;

																									GLClear(GL_COLOR_BUFFER_BIT);
																								}
																							}

																							// NEXT UNCHANGED
																							{
																								// Update texture
																								while (updateClip != finClip)
																								{
																									RECT update = *updateClip;
																									DWORD texWidth = update.right - update.left;
																									DWORD texHeight = update.bottom - update.top;

																									if (texWidth == this->width)
																										GLTexSubImage2D(GL_TEXTURE_2D, 0, 0, update.top, texWidth, texHeight, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixelBuffer + update.top * texWidth);
																									else
																									{
																										DWORD* source = surface->pixelBuffer + update.top * this->width + update.left;
																										DWORD* dest = (DWORD*)frameBuffer;
																										DWORD copyWidth = texWidth;
																										DWORD copyHeight = texHeight;
																										do
																										{
																											DWORD* src = (DWORD*)source;
																											source += this->width;

																											DWORD copyCount = copyWidth;
																											do
																												*dest++ = *src++;
																											while (--copyCount);
																										} while (--copyHeight);

																										GLTexSubImage2D(GL_TEXTURE_2D, 0, update.left, update.top, texWidth, texHeight, GL_RGBA, GL_UNSIGNED_BYTE, frameBuffer);
																									}

																									if (++updateClip == surface->endClip)
																										updateClip = surface->clipsList;
																								}

																								// Update FPS
																								if (fpsState)
																								{
																									DWORD fps = (DWORD)MathRound((FLOAT)fpsSum / fpsCount);
																									DWORD digCount = 0;
																									DWORD current = fps;
																									do
																									{
																										++digCount;
																										current = current / 10;
																									} while (current);

																									DWORD fpsColor = fpsState == FpsBenchmark ? 0xFF00FFFF : 0xFFFFFFFF;
																									DWORD dcount = digCount;
																									do
																									{
																										bool* lpDig = (bool*)counters + FPS_WIDTH * FPS_HEIGHT * (fps % 10);

																										for (DWORD y = 0; y < FPS_HEIGHT; ++y)
																										{
																											DWORD* idx = surface->pixelBuffer + (FPS_Y + y) * this->width +
																												FPS_X + (FPS_STEP + FPS_WIDTH) * (dcount - 1);

																											DWORD* pix = (DWORD*)frameBuffer + y * (FPS_STEP + FPS_WIDTH) * 4 +
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

																										fps = fps / 10;
																									} while (--dcount);

																									dcount = 4;
																									while (dcount != digCount)
																									{
																										for (DWORD y = 0; y < FPS_HEIGHT; ++y)
																										{
																											DWORD* idx = surface->pixelBuffer + (FPS_Y + y) * this->width +
																												FPS_X + (FPS_STEP + FPS_WIDTH) * (dcount - 1);

																											DWORD* pix = (DWORD*)frameBuffer + y * (FPS_STEP + FPS_WIDTH) * 4 +
																												(FPS_STEP + FPS_WIDTH) * (dcount - 1);

																											DWORD width = FPS_STEP + FPS_WIDTH;
																											do
																												*pix++ = *idx++;
																											while (--width);
																										}

																										--dcount;
																									}

																									GLTexSubImage2D(GL_TEXTURE_2D, 0, FPS_X, FPS_Y, (FPS_STEP + FPS_WIDTH) * 4, FPS_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, frameBuffer);
																								}

																								GLDrawArrays(GL_TRIANGLE_FAN, 0, 4);
																							}

																							// Draw from FBO
																							if (this->imageFilter == FilterXRBZ)
																							{
																								GLDisable(GL_STENCIL_TEST);
																								//GLFinish();
																								GLBindFramebuffer(GL_DRAW_FRAMEBUFFER, NULL);

																								GLUseProgram(shProgramLinear);
																								{
																									GLViewport(this->viewport.rectangle.x, this->viewport.rectangle.y, this->viewport.rectangle.width, this->viewport.rectangle.height);

																									GLClear(GL_COLOR_BUFFER_BIT);

																									GLBindTexture(GL_TEXTURE_2D, tboId);
																									GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
																									GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

																									GLDrawArrays(GL_TRIANGLE_FAN, 4, 4);
																								}
																								GLUseProgram(shProgramXRBZ);
																							}

																							// Swap
																							{
																								SwapBuffers(this->hDc);
																								if (fpsState != FpsBenchmark)
																								{
																									WaitForSingleObject(this->hDrawEvent, INFINITE);
																									ResetEvent(this->hDrawEvent);
																								}
																								GLFinish();
																							}
																						}
																					}
																				} while (!this->isFinish);
																			}
																			MemoryFree(frameBuffer);
																		}

																		if (uniSize)
																		{
																			GLDeleteRenderbuffers(1, &rboId);
																			GLDeleteTextures(1, &tboId);
																		}
																	}
																	GLDeleteFramebuffers(1, &fboId);
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
										}
										GLDeleteBuffers(1, &stBufferName);
									}
								}
								GLDeleteVertexArrays(1, &stArrayName);
							}
							GLUseProgram(NULL);
						}
						GLDeleteProgram(shProgramXRBZ);
					}
				}
				GLDeleteProgram(shProgramLinear);
			}
		}
		GLDeleteProgram(shProgramStencil);
	}
	MemoryFree(stencil);
}

VOID DirectDraw::RenderStart()
{
	if (!this->isFinish || !this->hWnd)
		return;

	this->isFinish = FALSE;
	GL::Load();

	RECT rect;
	GetClientRect(this->hWnd, &rect);

	if (this->windowState != WinStateWindowed)
	{
		this->hDraw = CreateWindowEx(
			WS_EX_CONTROLPARENT | WS_EX_TOPMOST,
			WC_STATIC,
			NULL,
			WS_VISIBLE | WS_POPUP,
			rect.left, rect.top,
			rect.right - rect.left, rect.bottom - rect.top,
			this->hWnd,
			NULL,
			hDllModule,
			NULL);
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
	SECURITY_ATTRIBUTES sAttribs;
	MemoryZero(&sAttribs, sizeof(SECURITY_ATTRIBUTES));
	this->hDrawThread = CreateThread(&sAttribs, 262144, RenderThread, this, NORMAL_PRIORITY_CLASS, &threadId);
}

VOID DirectDraw::RenderStop()
{
	if (this->isFinish)
		return;

	this->isFinish = TRUE;
	SetEvent(this->hDrawEvent);
	WaitForSingleObject(this->hDrawThread, INFINITE);
	this->hDrawThread = NULL;

	BOOL wasFull = GetWindowLong(this->hDraw, GWL_STYLE) & WS_POPUP;
	if (DestroyWindow(this->hDraw))
		this->hDraw = NULL;

	if (wasFull)
		GL::ResetContext();

	ClipCursor(NULL);
}

BOOL DirectDraw::CheckView()
{
	if (this->viewport.refresh)
	{
		this->viewport.refresh = FALSE;

		this->viewport.rectangle.x = this->viewport.rectangle.y = 0;
		this->viewport.rectangle.width = this->viewport.width;
		this->viewport.rectangle.height = this->viewport.height;

		this->viewport.clipFactor.x = this->viewport.viewFactor.x = (FLOAT)this->viewport.width / this->width;
		this->viewport.clipFactor.y = this->viewport.viewFactor.y = (FLOAT)this->viewport.height / this->height;

		if (this->imageAspect && this->viewport.viewFactor.x != this->viewport.viewFactor.y)
		{
			if (this->viewport.viewFactor.x > this->viewport.viewFactor.y)
			{
				FLOAT fw = this->viewport.viewFactor.y * this->width;
				this->viewport.rectangle.width = (INT)MathRound(fw);
				this->viewport.rectangle.x = (INT)MathRound(((FLOAT)this->viewport.width - fw) / 2.0f);
				this->viewport.clipFactor.x = this->viewport.viewFactor.y;
			}
			else
			{
				FLOAT fh = this->viewport.viewFactor.x * this->height;
				this->viewport.rectangle.height = (INT)MathRound(fh);
				this->viewport.rectangle.y = (INT)MathRound(((FLOAT)this->viewport.height - fh) / 2.0f);
				this->viewport.clipFactor.y = this->viewport.viewFactor.x;
			}
		}

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

DirectDraw::DirectDraw(DirectDraw* lastObj)
{
	this->last = lastObj;

	this->surfaceEntries = NULL;
	this->paletteEntries = NULL;
	this->clipperEntries = NULL;

	this->attachedSurface = NULL;

	this->hWnd = NULL;
	this->hDraw = NULL;
	this->hDc = NULL;

	this->width = FALSE;
	this->height = FALSE;

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
	this->RenderStop();

	DirectDrawSurface* surfaceEntry = this->surfaceEntries;
	while (surfaceEntry)
	{
		DirectDrawSurface* curr = surfaceEntry->last;
		delete surfaceEntry;
		surfaceEntry = curr;
	}

	DirectDrawPalette* paletteEntry = this->paletteEntries;
	while (paletteEntry)
	{
		DirectDrawPalette* curr = paletteEntry->last;
		delete paletteEntry;
		paletteEntry = curr;
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
	if (this->hWnd != hWnd)
	{
		this->hDc = NULL;
		this->hWnd = hWnd;

		if (!OldWindowProc)
			OldWindowProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)WindowProc);
	}

	if (dwFlags & DDSCL_FULLSCREEN)
		this->windowState = WinStateFullScreen;
	else if (this->windowState != WinStateWindowed)
	{
		this->windowState = WinStateWindowed;
		this->RenderStop();
		this->RenderStart();
	}

	return DD_OK;
}

HRESULT DirectDraw::SetDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP)
{
	this->width = dwWidth;
	this->height = dwHeight;

	RECT rect = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
	AdjustWindowRect(&rect, GetWindowLong(this->hWnd, GWL_STYLE), FALSE);
	MoveWindow(this->hWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
	SetForegroundWindow(this->hWnd);

	this->RenderStop();
	this->RenderStart();

	return DD_OK;
}

HRESULT DirectDraw::CreatePalette(DWORD dwFlags, LPPALETTEENTRY lpDDColorArray, LPDIRECTDRAWPALETTE* lplpDDPalette, IUnknown* pUnkOuter)
{
	this->paletteEntries = new DirectDrawPalette(this);
	*lplpDDPalette = (LPDIRECTDRAWPALETTE)this->paletteEntries;

	this->paletteEntries->SetEntries(0, 0, 256, lpDDColorArray);

	return DD_OK;
}

HRESULT DirectDraw::CreateSurface(LPDDSURFACEDESC lpDDSurfaceDesc, LPDIRECTDRAWSURFACE* lplpDDSurface, IUnknown* pUnkOuter)
{
	BOOL isPrimary = lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE;

	this->surfaceEntries = new DirectDrawSurface(this, !isPrimary);
	*lplpDDSurface = (LPDIRECTDRAWSURFACE)this->surfaceEntries;

	if (lpDDSurfaceDesc->dwFlags & DDSD_WIDTH)
		this->width = lpDDSurfaceDesc->dwWidth;

	if (lpDDSurfaceDesc->dwFlags & DDSD_HEIGHT)
		this->height = lpDDSurfaceDesc->dwHeight;

	if (isPrimary)
		this->attachedSurface = this->surfaceEntries;

	return DD_OK;
}

HRESULT DirectDraw::CreateClipper(DWORD dwFlags, LPDIRECTDRAWCLIPPER* lplpDDClipper, IUnknown* pUnkOuter)
{
	this->clipperEntries = new DirectDrawClipper(this);
	*lplpDDClipper = (LPDIRECTDRAWCLIPPER)this->clipperEntries;

	return DD_OK;
}