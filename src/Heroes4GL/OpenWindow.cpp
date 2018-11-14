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
#include "OpenWindow.h"
#include "Window.h"
#include "Windowsx.h"
#include "CommCtrl.h"
#include "Resource.h"
#include "Main.h"
#include "Config.h"
#include "FpsCounter.h"

namespace OpenWindow
{
	HHOOK OldWindowKeyHook;
	WNDPROC OldPanelProc;
	HBRUSH blackBrush;

	LRESULT __stdcall WindowKeyHook(INT nCode, WPARAM wParam, LPARAM lParam)
	{
		if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN))
		{
			KBDLLHOOKSTRUCT* phs = (KBDLLHOOKSTRUCT*)lParam;
			if (phs->vkCode == VK_SNAPSHOT)
			{
				HWND hWnd = GetActiveWindow();
				OpenDraw* ddraw = Main::FindOpenDrawByWindow(hWnd);
				if (ddraw && ddraw->windowState != WinStateWindowed)
				{
					ddraw->isTakeSnapshot = TRUE;
					if (ddraw)
						SetEvent(ddraw->hDrawEvent);
					MessageBeep(0);
					return TRUE;
				}
			}
		}

		return CallNextHookEx(OldWindowKeyHook, nCode, wParam, lParam);
	}

	VOID __fastcall FilterChanged(HWND hWnd)
	{
		Config::Set(CONFIG_WRAPPER, "ImageFilter", configImageFilter);
		Window::CheckMenu(hWnd);

		OpenDraw* ddraw = Main::FindOpenDrawByWindow(hWnd);
		if (ddraw)
		{
			ddraw->isStateChanged = TRUE;
			SetEvent(ddraw->hDrawEvent);
		}
	}

	VOID __fastcall SelectScaleNxMode(HWND hWnd, DWORD value, DWORD isMode)
	{
		configImageScaleNx = isMode ?
			(configImageScaleNx & 0xFFFF0000) | value :
			(configImageScaleNx & 0x0000FFFF) | (value << 16);
		Config::Set(CONFIG_WRAPPER, "ImageScaleNx", configImageScaleNx);

		configImageFilter = glVersion >= GL_VER_3_0 ? FilterScaleNx : FilterNearest;
		FilterChanged(hWnd);
	}

	VOID __fastcall SelectXSalMode(HWND hWnd, DWORD value, DWORD isMode)
	{
		configImageXSal = isMode ?
			(configImageXSal & 0xFFFF0000) | value :
			(configImageXSal & 0x0000FFFF) | (value << 16);
		Config::Set(CONFIG_WRAPPER, "ImageXSal", configImageXSal);

		configImageFilter = glVersion >= GL_VER_3_0 ? FilterXSal : FilterNearest;
		FilterChanged(hWnd);
	}

	VOID __fastcall SelectEagleMode(HWND hWnd, DWORD value, DWORD isMode)
	{
		configImageEagle = isMode ?
			(configImageEagle & 0xFFFF0000) | value :
			(configImageEagle & 0x0000FFFF) | (value << 16);
		Config::Set(CONFIG_WRAPPER, "ImageEagle", configImageEagle);

		configImageFilter = glVersion >= GL_VER_3_0 ? FilterEagle : FilterNearest;
		FilterChanged(hWnd);
	}

	VOID __fastcall SelectScaleHQMode(HWND hWnd, DWORD value, DWORD isMode)
	{
		configImageScaleHQ = isMode ?
			(configImageScaleHQ & 0xFFFF0000) | value :
			(configImageScaleHQ & 0x0000FFFF) | (value << 16);
		Config::Set(CONFIG_WRAPPER, "ImageScaleHQ", configImageScaleHQ);

		configImageFilter = glVersion >= GL_VER_3_0 ? FilterScaleHQ : FilterNearest;
		FilterChanged(hWnd);
	}

	VOID __fastcall SelectXBRZMode(HWND hWnd, DWORD value, DWORD isMode)
	{
		configImageXBRZ = isMode ?
			(configImageXBRZ & 0xFFFF0000) | value :
			(configImageXBRZ & 0x0000FFFF) | (value << 16);
		Config::Set(CONFIG_WRAPPER, "ImageXBRZ", configImageXBRZ);

		configImageFilter = glVersion >= GL_VER_3_0 ? FilterXRBZ : FilterNearest;
		FilterChanged(hWnd);
	}

	LRESULT __stdcall WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_PAINT:
		{
			OpenDraw* ddraw = Main::FindOpenDrawByWindow(hWnd);
			if (ddraw && ddraw->windowState == WinStateWindowed)
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
			return CallWindowProc(Window::OldWindowProc, hWnd, uMsg, wParam, lParam);
		}

		case WM_ERASEBKGND:
		{
			OpenDraw* ddraw = Main::FindOpenDrawByWindow(hWnd);
			if (ddraw && ddraw->windowState != WinStateWindowed)
			{
				RECT rc;
				GetClientRect(hWnd, &rc);
				FillRect((HDC)wParam, &rc, blackBrush);
				return TRUE;
			}
			return NULL;
		}

		case WM_MOVE:
		{
			OpenDraw* ddraw = Main::FindOpenDrawByWindow(hWnd);
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

			return CallWindowProc(Window::OldWindowProc, hWnd, uMsg, wParam, lParam);
		}

		case WM_SIZE:
		{
			OpenDraw* ddraw = Main::FindOpenDrawByWindow(hWnd);
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

			return CallWindowProc(Window::OldWindowProc, hWnd, uMsg, wParam, lParam);
		}

		case WM_GETMINMAXINFO:
		{
			OpenDraw* ddraw = Main::FindOpenDrawByWindow(hWnd);
			if (ddraw && ddraw->windowState == WinStateWindowed)
			{
				RECT rect = { 0, 0, MIN_WIDTH, MIN_HEIGHT };
				AdjustWindowRect(&rect, GetWindowLong(hWnd, GWL_STYLE), TRUE);

				MINMAXINFO* mmi = (MINMAXINFO*)lParam;
				mmi->ptMinTrackSize.x = rect.right - rect.left;
				mmi->ptMinTrackSize.y = rect.bottom - rect.top;
				mmi->ptMaxTrackSize.x = LONG_MAX >> 16;
				mmi->ptMaxTrackSize.y = LONG_MAX >> 16;
				mmi->ptMaxSize.x = LONG_MAX >> 16;
				mmi->ptMaxSize.y = LONG_MAX >> 16;

				return NULL;
			}

			return CallWindowProc(Window::OldWindowProc, hWnd, uMsg, wParam, lParam);
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
				OpenDraw* ddraw = Main::FindOpenDrawByWindow(hWnd);
				if (ddraw)
					SetEvent(ddraw->hDrawEvent);
				return NULL;
			}

			case VK_F3: // Filtering on/off
			{
				switch (configImageFilter)
				{
				case FilterLinear:
					configImageFilter = glVersion >= GL_VER_3_0 ? FilterCubic : FilterNearest;
					break;

				case FilterCubic:
					configImageFilter = glVersion >= GL_VER_3_0 ? FilterScaleNx : FilterNearest;
					break;

				case FilterScaleNx:
					configImageFilter = glVersion >= GL_VER_3_0 ? FilterEagle : FilterNearest;
					break;

				case FilterEagle:
					configImageFilter = glVersion >= GL_VER_3_0 ? FilterXSal : FilterNearest;
					break;

				case FilterXSal:
					configImageFilter = glVersion >= GL_VER_3_0 ? FilterScaleHQ : FilterNearest;
					break;

				case FilterScaleHQ:
					configImageFilter = glVersion >= GL_VER_3_0 ? FilterXRBZ : FilterNearest;
					break;

				case FilterXRBZ:
					configImageFilter = FilterNearest;
					break;

				default:
					configImageFilter = FilterLinear;
					break;
				}

				FilterChanged(hWnd);

				return NULL;
			}

			case VK_F9:
			{
				configImageAspect = !configImageAspect;
				Config::Set(CONFIG_WRAPPER, "ImageAspect", configImageAspect);
				Window::CheckMenu(hWnd);

				OpenDraw* ddraw = Main::FindOpenDrawByWindow(hWnd);
				if (ddraw)
				{
					ddraw->viewport.refresh = TRUE;
					SetEvent(ddraw->hDrawEvent);
				}

				return NULL;
			}

			default:
				return CallWindowProc(Window::OldWindowProc, hWnd, uMsg, wParam, lParam);
			}
		}

		case WM_ACTIVATE:
		{
			OpenDraw* ddraw = Main::FindOpenDrawByWindow(hWnd);
			if (ddraw && ddraw->windowState != WinStateWindowed)
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
			else
				return CallWindowProc(Window::OldWindowProc, hWnd, uMsg, wParam, lParam);
		}

		case WM_ACTIVATEAPP:
		{
			OpenDraw* ddraw = Main::FindOpenDrawByWindow(hWnd);
			if (ddraw && ddraw->windowState != WinStateWindowed)
			{
				if ((BOOL)wParam)
					ddraw->RenderStart();
				else
					ddraw->RenderStop();

				return DefWindowProc(hWnd, uMsg, wParam, lParam);
			}
			else
				return CallWindowProc(Window::OldWindowProc, hWnd, uMsg, wParam, lParam);
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
			OpenDraw* ddraw = Main::FindOpenDrawByWindow(hWnd);
			if (ddraw && ddraw->mode)
			{
				POINT p = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				ddraw->ScaleMouse(&p);
				lParam = MAKELONG(p.x, p.y);
			}

			return CallWindowProc(Window::OldWindowProc, hWnd, uMsg, wParam, lParam);
		}

		case WM_COMMAND:
		{
			switch (wParam)
			{
			case IDM_PATCH_CPU:
			{
				configColdCPU = !configColdCPU;
				Config::Set(CONFIG_WRAPPER, "ColdCPU", configColdCPU);
				Window::CheckMenu(hWnd);

				return NULL;
			}

			case IDM_RES_FULL_SCREEN:
			{
				SendMessage(hWnd, WM_KEYDOWN, VK_F4, NULL);
				return NULL;
			}

			case IDM_HELP_WRAPPER:
			{
				INT_PTR res;
				ULONG_PTR cookie = NULL;
				if (hActCtx && hActCtx != INVALID_HANDLE_VALUE && !ActivateActCtxC(hActCtx, &cookie))
					cookie = NULL;

				res = DialogBoxParam(hDllModule, MAKEINTRESOURCE(configLanguage == LNG_ENGLISH ? IDD_ENGLISH : IDD_RUSSIAN), hWnd, (DLGPROC)Window::AboutProc, NULL);

				if (cookie)
					DeactivateActCtxC(0, cookie);

				SetForegroundWindow(hWnd);
				return NULL;
			}

			case IDM_HELP_ABOUT:
			{
				INT_PTR res;
				ULONG_PTR cookie = NULL;
				if (hActCtx && hActCtx != INVALID_HANDLE_VALUE && !ActivateActCtxC(hActCtx, &cookie))
					cookie = NULL;

				res = DialogBoxParam(hDllModule, MAKEINTRESOURCE(configDialog), hWnd, (DLGPROC)Window::AboutAppProc, NULL);

				if (cookie)
					DeactivateActCtxC(0, cookie);

				SetForegroundWindow(hWnd);
				return NULL;
			}

			case IDM_ASPECT_RATIO:
			{
				configImageAspect = !configImageAspect;
				Config::Set(CONFIG_WRAPPER, "ImageAspect", configImageAspect);
				Window::CheckMenu(hWnd);

				OpenDraw* ddraw = Main::FindOpenDrawByWindow(hWnd);
				if (ddraw)
				{
					ddraw->viewport.refresh = TRUE;
					SetEvent(ddraw->hDrawEvent);
				}

				return NULL;
			}

			case IDM_VSYNC:
			{
				configImageVSync = !configImageVSync;
				Config::Set(CONFIG_WRAPPER, "ImageVSync", configImageVSync);
				Window::CheckMenu(hWnd);

				OpenDraw* ddraw = Main::FindOpenDrawByWindow(hWnd);
				if (ddraw)
					SetEvent(ddraw->hDrawEvent);

				return NULL;
			}

			case IDM_FILT_OFF:
			{
				configImageFilter = FilterNearest;
				FilterChanged(hWnd);
				return NULL;
			}

			case IDM_FILT_LINEAR:
			{
				configImageFilter = FilterLinear;
				FilterChanged(hWnd);
				return NULL;
			}

			case IDM_FILT_CUBIC:
			{
				configImageFilter = glVersion >= GL_VER_3_0 ? FilterCubic : FilterNearest;
				FilterChanged(hWnd);
				return NULL;
			}

			case IDM_FILT_SCALENX_LINEAR:
			{
				SelectScaleNxMode(hWnd, 0, FALSE);
				return NULL;
			}

			case IDM_FILT_SCALENX_CUBIC:
			{
				SelectScaleNxMode(hWnd, 1, FALSE);
				return NULL;
			}

			case IDM_FILT_SCALENX_2X:
			{
				SelectScaleNxMode(hWnd, 2, TRUE);
				return NULL;
			}

			case IDM_FILT_SCALENX_3X:
			{
				SelectScaleNxMode(hWnd, 3, TRUE);
				return NULL;
			}

			case IDM_FILT_XSAL_LINEAR:
			{
				SelectXSalMode(hWnd, 0, FALSE);
				return NULL;
			}

			case IDM_FILT_XSAL_CUBIC:
			{
				SelectXSalMode(hWnd, 1, FALSE);
				return NULL;
			}

			case IDM_FILT_XSAL_2X:
			{
				SelectXSalMode(hWnd, 2, TRUE);
				return NULL;
			}

			case IDM_FILT_EAGLE_LINEAR:
			{
				SelectEagleMode(hWnd, 0, FALSE);
				return NULL;
			}

			case IDM_FILT_EAGLE_CUBIC:
			{
				SelectEagleMode(hWnd, 1, FALSE);
				return NULL;
			}

			case IDM_FILT_EAGLE_2X:
			{
				SelectEagleMode(hWnd, 2, TRUE);
				return NULL;
			}

			case IDM_FILT_SCALEHQ_LINEAR:
			{
				SelectScaleHQMode(hWnd, 0, FALSE);
				return NULL;
			}

			case IDM_FILT_SCALEHQ_CUBIC:
			{
				SelectScaleHQMode(hWnd, 1, FALSE);
				return NULL;
			}

			case IDM_FILT_SCALEHQ_2X:
			{
				SelectScaleHQMode(hWnd, 2, TRUE);
				return NULL;
			}

			case IDM_FILT_SCALEHQ_4X:
			{
				SelectScaleHQMode(hWnd, 4, TRUE);
				return NULL;
			}

			case IDM_FILT_XRBZ_LINEAR:
			{
				SelectXBRZMode(hWnd, 0, FALSE);
				return NULL;
			}

			case IDM_FILT_XRBZ_CUBIC:
			{
				SelectXBRZMode(hWnd, 1, FALSE);
				return NULL;
			}

			case IDM_FILT_XRBZ_2X:
			{
				SelectXBRZMode(hWnd, 2, TRUE);
				return NULL;
			}

			case IDM_FILT_XRBZ_3X:
			{
				SelectXBRZMode(hWnd, 3, TRUE);
				return NULL;
			}

			case IDM_FILT_XRBZ_4X:
			{
				SelectXBRZMode(hWnd, 4, TRUE);
				return NULL;
			}

			case IDM_FILT_XRBZ_5X:
			{
				SelectXBRZMode(hWnd, 5, TRUE);
				return NULL;
			}

			case IDM_FILT_XRBZ_6X:
			{
				SelectXBRZMode(hWnd, 6, TRUE);
				return NULL;
			}

			default:
				return CallWindowProc(Window::OldWindowProc, hWnd, uMsg, wParam, lParam);
			}
		}

		default:
			return CallWindowProc(Window::OldWindowProc, hWnd, uMsg, wParam, lParam);
		}
	}

	LRESULT __stdcall PanelProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_PAINT:
		{
			OpenDraw* ddraw = Main::FindOpenDrawByWindow(hWnd);
			if (ddraw && ddraw->windowState == WinStateWindowed)
				return CallWindowProc(Window::OldWindowProc, hWnd, uMsg, wParam, lParam);
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
}