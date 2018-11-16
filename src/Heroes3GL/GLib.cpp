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
#include "Main.h"
#include "GLib.h"

#define PREFIX_GL "gl"
#define PREFIX_WGL "wgl"

WGLGETPROCADDRESS WGLGetProcAddress;
WGLMAKECURRENT WGLMakeCurrent;
WGLCREATECONTEXT WGLCreateContext;
WGLDELETECONTEXT WGLDeleteContext;
WGLCREATECONTEXTATTRIBSARB WGLCreateContextAttribs;
WGLSWAPINTERVAL WGLSwapInterval;

GLGETSTRING GLGetString;
GLVERTEX2S GLVertex2s;
GLCOLOR4UBV GLColor4ubv;
GLTEXCOORD2F GLTexCoord2f;
GLBEGIN GLBegin;
GLEND GLEnd;
GLVIEWPORT GLViewport;
GLMATRIXMODE GLMatrixMode;
GLLOADIDENTITY GLLoadIdentity;
GLORTHO GLOrtho;
GLFINISH GLFinish;
GLENABLE GLEnable;
GLDISABLE GLDisable;
GLBINDTEXTURE GLBindTexture;
GLDELETETEXTURES GLDeleteTextures;
GLTEXPARAMETERI GLTexParameteri;
GLTEXENVI GLTexEnvi;
GLGETTEXIMAGE GLGetTexImage;
GLTEXSUBIMAGE1D GLTexSubImage1D;
GLTEXIMAGE2D GLTexImage2D;
GLTEXSUBIMAGE2D GLTexSubImage2D;
GLGENTEXTURES GLGenTextures;
GLGETINTEGERV GLGetIntegerv;
GLCLEAR GLClear;
GLCLEARCOLOR GLClearColor;
GLCOLORMASK GLColorMask;
GLSTENCILFUNC GLStencilFunc;
GLSTENCILOP GLStencilOp;

#ifdef _DEBUG
GLGETERROR GLGetError;
#endif

GLACTIVETEXTURE GLActiveTexture;
GLGENBUFFERS GLGenBuffers;
GLDELETEBUFFERS GLDeleteBuffers;
GLBINDBUFFER GLBindBuffer;
GLBUFFERDATA GLBufferData;
GLBUFFERSUBDATA GLBufferSubData;
GLDRAWARRAYS GLDrawArrays;

GLENABLEVERTEXATTRIBARRAY GLEnableVertexAttribArray;
GLVERTEXATTRIBPOINTER GLVertexAttribPointer;

GLCREATESHADER GLCreateShader;
GLDELETESHADER GLDeleteShader;
GLDELETEPROGRAM GLDeleteProgram;
GLCREATEPROGRAM GLCreateProgram;
GLSHADERSOURCE GLShaderSource;
GLCOMPILESHADER GLCompileShader;
GLATTACHSHADER GLAttachShader;
GLDETACHSHADER GLDetachShader;
GLLINKPROGRAM GLLinkProgram;
GLUSEPROGRAM GLUseProgram;
GLGETSHADERIV GLGetShaderiv;
GLGETSHADERINFOLOG GLGetShaderInfoLog;

GLGETATTRIBLOCATION GLGetAttribLocation;
GLGETUNIFORMLOCATION GLGetUniformLocation;

GLUNIFORM1I GLUniform1i;
GLUNIFORM2F GLUniform2f;
GLUNIFORMMATRIX4FV GLUniformMatrix4fv;

GLGENVERTEXARRAYS GLGenVertexArrays;
GLBINDVERTEXARRAY GLBindVertexArray;
GLDELETEVERTEXARRAYS GLDeleteVertexArrays;

GLGENFRAMEBUFFERS GLGenFramebuffers;
GLDELETEFRAMEBUFFERS GLDeleteFramebuffers;
GLBINDFRAMEBUFFER GLBindFramebuffer;
GLFRAMEBUFFERTEXTURE2D GLFramebufferTexture2D;

GLGENRENDERBUFFERS GLGenRenderbuffers;
GLDELETERENDERBUFFERS GLDeleteRenderbuffers;
GLBINDRENDERBUFFER GLBindRenderbuffer;
GLRENDERBUFFERSTORAGE GLRenderbufferStorage;
GLFRAMEBUFFERRENDERBUFFER GLFramebufferRenderbuffer;

HMODULE hModule;

DWORD glVersion;
DWORD glPixelFormat;
DWORD glCapsClampToEdge;
BOOL glCapsBGRA;

BOOL isDummyRegistered;

namespace GL
{
	BOOL __fastcall Load()
	{
		if (!hModule)
			hModule = LoadLibrary("OPENGL32.dll");

		if (!hModule)
			return FALSE;

		WGLGetProcAddress = (WGLGETPROCADDRESS)GetProcAddress(hModule, "wglGetProcAddress");
		WGLMakeCurrent = (WGLMAKECURRENT)GetProcAddress(hModule, "wglMakeCurrent");
		WGLCreateContext = (WGLCREATECONTEXT)GetProcAddress(hModule, "wglCreateContext");
		WGLDeleteContext = (WGLDELETECONTEXT)GetProcAddress(hModule, "wglDeleteContext");

		return TRUE;
	}

	VOID __fastcall Free()
	{
		if (isDummyRegistered)
			UnregisterClass("dummyclass", hDllModule);

		if (FreeLibrary(hModule))
			hModule = NULL;
	}

	VOID __fastcall LoadGLFunction(CHAR* buffer, const CHAR* prefix, const CHAR* name, PROC* func, const CHAR* sufix = NULL)
	{
		StrCopy(buffer, prefix);
		StrCat(buffer, name);

		if (sufix)
			StrCat(buffer, sufix);

		if (WGLGetProcAddress)
			*func = WGLGetProcAddress(buffer);

		if ((INT)*func >= -1 && (INT)*func <= 3)
			*func = GetProcAddress(hModule, buffer);

		if (!sufix && !*func)
		{
			LoadGLFunction(buffer, prefix, name, func, "EXT");
			if (!*func)
				LoadGLFunction(buffer, prefix, name, func, "ARB");
		}
	}

	BOOL __fastcall GetContext(HDC hDc, HGLRC* lpHRc, BOOL showError, DWORD* wglAttributes)
	{
		HGLRC hRc = WGLCreateContextAttribs(hDc, NULL, wglAttributes);
		if (hRc)
		{
			WGLMakeCurrent(hDc, hRc);
			WGLDeleteContext(*lpHRc);
			*lpHRc = hRc;

			return TRUE;
		}
		else if (showError)
		{
			DWORD errorCode = GetLastError();
			if (errorCode == ERROR_INVALID_VERSION_ARB)
				Main::ShowError("Invalid ARB version", __FILE__, __LINE__);
			else if (errorCode == ERROR_INVALID_PROFILE_ARB)
				Main::ShowError("Invalid ARB profile", __FILE__, __LINE__);
		}

		return FALSE;
	}

	BOOL __fastcall GetContext(HDC hDc, HGLRC* lpHRc, DWORD major, DWORD minor, BOOL showError)
	{
		DWORD wglAttributes[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, major,
			WGL_CONTEXT_MINOR_VERSION_ARB, minor,
			0
		};

		return GetContext(hDc, lpHRc, showError, wglAttributes);
	}

	VOID __fastcall CreateContextAttribs(HDC hDc, HGLRC* hRc)
	{
		CHAR buffer[256];
		LoadGLFunction(buffer, PREFIX_WGL, "CreateContextAttribs", (PROC*)&WGLCreateContextAttribs, "ARB");

		if (WGLCreateContextAttribs)
		{
			if (!GetContext(hDc, hRc, 3, 0, FALSE))
				GetContext(hDc, hRc, 1, 4, TRUE);
		}

		LoadGLFunction(buffer, "wgl", "SwapInterval", (PROC*)&WGLSwapInterval, "EXT");

		LoadGLFunction(buffer, PREFIX_GL, "GetString", (PROC*)&GLGetString);
		LoadGLFunction(buffer, PREFIX_GL, "TexCoord2f", (PROC*)&GLTexCoord2f);
		LoadGLFunction(buffer, PREFIX_GL, "Vertex2s", (PROC*)&GLVertex2s);
		LoadGLFunction(buffer, PREFIX_GL, "Color4ubv", (PROC*)&GLColor4ubv);
		LoadGLFunction(buffer, PREFIX_GL, "Begin", (PROC*)&GLBegin);
		LoadGLFunction(buffer, PREFIX_GL, "End", (PROC*)&GLEnd);
		LoadGLFunction(buffer, PREFIX_GL, "Viewport", (PROC*)&GLViewport);
		LoadGLFunction(buffer, PREFIX_GL, "MatrixMode", (PROC*)&GLMatrixMode);
		LoadGLFunction(buffer, PREFIX_GL, "LoadIdentity", (PROC*)&GLLoadIdentity);
		LoadGLFunction(buffer, PREFIX_GL, "Ortho", (PROC*)&GLOrtho);
		LoadGLFunction(buffer, PREFIX_GL, "Finish", (PROC*)&GLFinish);
		LoadGLFunction(buffer, PREFIX_GL, "Enable", (PROC*)&GLEnable);
		LoadGLFunction(buffer, PREFIX_GL, "Disable", (PROC*)&GLDisable);
		LoadGLFunction(buffer, PREFIX_GL, "BindTexture", (PROC*)&GLBindTexture);
		LoadGLFunction(buffer, PREFIX_GL, "DeleteTextures", (PROC*)&GLDeleteTextures);
		LoadGLFunction(buffer, PREFIX_GL, "TexParameteri", (PROC*)&GLTexParameteri);
		LoadGLFunction(buffer, PREFIX_GL, "TexEnvi", (PROC*)&GLTexEnvi);
		LoadGLFunction(buffer, PREFIX_GL, "GetTexImage", (PROC*)&GLGetTexImage);
		LoadGLFunction(buffer, PREFIX_GL, "TexSubImage1D", (PROC*)&GLTexSubImage1D);
		LoadGLFunction(buffer, PREFIX_GL, "TexImage2D", (PROC*)&GLTexImage2D);
		LoadGLFunction(buffer, PREFIX_GL, "TexSubImage2D", (PROC*)&GLTexSubImage2D);
		LoadGLFunction(buffer, PREFIX_GL, "GenTextures", (PROC*)&GLGenTextures);
		LoadGLFunction(buffer, PREFIX_GL, "GetIntegerv", (PROC*)&GLGetIntegerv);
		LoadGLFunction(buffer, PREFIX_GL, "Clear", (PROC*)&GLClear);
		LoadGLFunction(buffer, PREFIX_GL, "ClearColor", (PROC*)&GLClearColor);
		LoadGLFunction(buffer, PREFIX_GL, "ColorMask", (PROC*)&GLColorMask);
		LoadGLFunction(buffer, PREFIX_GL, "StencilFunc", (PROC*)&GLStencilFunc);
		LoadGLFunction(buffer, PREFIX_GL, "StencilOp", (PROC*)&GLStencilOp);

#ifdef _DEBUG
		LoadGLFunction(buffer, PREFIX_GL, "GetError", (PROC*)&GLGetError);
#endif

		LoadGLFunction(buffer, PREFIX_GL, "ActiveTexture", (PROC*)&GLActiveTexture);
		LoadGLFunction(buffer, PREFIX_GL, "GenBuffers", (PROC*)&GLGenBuffers);
		LoadGLFunction(buffer, PREFIX_GL, "DeleteBuffers", (PROC*)&GLDeleteBuffers);
		LoadGLFunction(buffer, PREFIX_GL, "BindBuffer", (PROC*)&GLBindBuffer);
		LoadGLFunction(buffer, PREFIX_GL, "BufferData", (PROC*)&GLBufferData);
		LoadGLFunction(buffer, PREFIX_GL, "BufferSubData", (PROC*)&GLBufferSubData);
		LoadGLFunction(buffer, PREFIX_GL, "DrawArrays", (PROC*)&GLDrawArrays);

		LoadGLFunction(buffer, PREFIX_GL, "EnableVertexAttribArray", (PROC*)&GLEnableVertexAttribArray);
		LoadGLFunction(buffer, PREFIX_GL, "VertexAttribPointer", (PROC*)&GLVertexAttribPointer);

		LoadGLFunction(buffer, PREFIX_GL, "CreateShader", (PROC*)&GLCreateShader);
		LoadGLFunction(buffer, PREFIX_GL, "DeleteShader", (PROC*)&GLDeleteShader);
		LoadGLFunction(buffer, PREFIX_GL, "CreateProgram", (PROC*)&GLCreateProgram);
		LoadGLFunction(buffer, PREFIX_GL, "DeleteProgram", (PROC*)&GLDeleteProgram);
		LoadGLFunction(buffer, PREFIX_GL, "ShaderSource", (PROC*)&GLShaderSource);
		LoadGLFunction(buffer, PREFIX_GL, "CompileShader", (PROC*)&GLCompileShader);
		LoadGLFunction(buffer, PREFIX_GL, "AttachShader", (PROC*)&GLAttachShader);
		LoadGLFunction(buffer, PREFIX_GL, "DetachShader", (PROC*)&GLDetachShader);
		LoadGLFunction(buffer, PREFIX_GL, "LinkProgram", (PROC*)&GLLinkProgram);
		LoadGLFunction(buffer, PREFIX_GL, "UseProgram", (PROC*)&GLUseProgram);
		LoadGLFunction(buffer, PREFIX_GL, "GetShaderiv", (PROC*)&GLGetShaderiv);
		LoadGLFunction(buffer, PREFIX_GL, "GetShaderInfoLog", (PROC*)&GLGetShaderInfoLog);

		LoadGLFunction(buffer, PREFIX_GL, "GetAttribLocation", (PROC*)&GLGetAttribLocation);
		LoadGLFunction(buffer, PREFIX_GL, "GetUniformLocation", (PROC*)&GLGetUniformLocation);

		LoadGLFunction(buffer, PREFIX_GL, "Uniform1i", (PROC*)&GLUniform1i);
		LoadGLFunction(buffer, PREFIX_GL, "Uniform2f", (PROC*)&GLUniform2f);
		LoadGLFunction(buffer, PREFIX_GL, "UniformMatrix4fv", (PROC*)&GLUniformMatrix4fv);

		LoadGLFunction(buffer, PREFIX_GL, "GenVertexArrays", (PROC*)&GLGenVertexArrays);
		LoadGLFunction(buffer, PREFIX_GL, "BindVertexArray", (PROC*)&GLBindVertexArray);
		LoadGLFunction(buffer, PREFIX_GL, "DeleteVertexArrays", (PROC*)&GLDeleteVertexArrays);

		LoadGLFunction(buffer, PREFIX_GL, "GenFramebuffers", (PROC*)&GLGenFramebuffers);
		LoadGLFunction(buffer, PREFIX_GL, "DeleteFramebuffers", (PROC*)&GLDeleteFramebuffers);
		LoadGLFunction(buffer, PREFIX_GL, "BindFramebuffer", (PROC*)&GLBindFramebuffer);
		LoadGLFunction(buffer, PREFIX_GL, "FramebufferTexture2D", (PROC*)&GLFramebufferTexture2D);

		LoadGLFunction(buffer, PREFIX_GL, "GenRenderbuffers", (PROC*)&GLGenRenderbuffers);
		LoadGLFunction(buffer, PREFIX_GL, "DeleteRenderbuffers", (PROC*)&GLDeleteRenderbuffers);
		LoadGLFunction(buffer, PREFIX_GL, "BindRenderbuffer", (PROC*)&GLBindRenderbuffer);
		LoadGLFunction(buffer, PREFIX_GL, "RenderbufferStorage", (PROC*)&GLRenderbufferStorage);
		LoadGLFunction(buffer, PREFIX_GL, "FramebufferRenderbuffer", (PROC*)&GLFramebufferRenderbuffer);

		const GLubyte* extensions = GLGetString(GL_EXTENSIONS);
		if (GLGetString)
		{
			glVersion = 0;
			CHAR* strVer = (CHAR*)GLGetString(GL_VERSION);
			if (strVer)
			{
				BOOL shift = FALSE;
				WORD* val = (WORD*)&glVersion;
				for (CHAR* p = strVer; ; ++p)
				{
					CHAR ch = *p;
					if (ch <= '9' && ch >= '0')
						*val = *val * 10 + (ch - '0');
					else
					{
						if (!glVersion)
							break;

						if (shift)
							break;
						
						shift = TRUE;
						glVersion <<= 16;

						if (ch != '.')
							break;
					}
				}
			}
			else
				glVersion = GL_VER_1_1;

			if (glVersion < GL_VER_1_2)
			{
				if (extensions)
				{
					glCapsClampToEdge = (StrStr((const CHAR*)extensions, "GL_EXT_texture_edge_clamp") || StrStr((const CHAR*)extensions, "GL_SGIS_texture_edge_clamp")) ? GL_CLAMP_TO_EDGE : GL_CLAMP;
					glCapsBGRA = StrStr((const CHAR*)extensions, "GL_EXT_bgra") || StrStr((const CHAR*)extensions, "GL_EXT_texture_format_BGRA8888") || StrStr((const CHAR*)extensions, "GL_APPLE_texture_format_BGRA8888");
				}
				else
					glVersion = GL_VER_1_1;
			}
			else
			{
				glCapsClampToEdge = GL_CLAMP_TO_EDGE;
				glCapsBGRA = TRUE;
			}
		}
		else
			glVersion = GL_VER_1_1;

		if (!glVersion)
			glVersion = GL_VER_1_1;
	}

	VOID __fastcall PreparePixelFormatDescription(PIXELFORMATDESCRIPTOR* pfd)
	{
		DEVMODE devMode;
		MemoryZero(&devMode, sizeof(DEVMODE));
		devMode.dmSize = sizeof(DEVMODE);

		DWORD bpp;
		if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode) && devMode.dmBitsPerPel >= 16 && devMode.dmBitsPerPel <= 32)
			bpp = devMode.dmBitsPerPel;
		else
			bpp = 32;

		MemoryZero(pfd, sizeof(PIXELFORMATDESCRIPTOR));
		pfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd->nVersion = 1;
		pfd->dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DEPTH_DONTCARE | PFD_STEREO_DONTCARE | PFD_SWAP_EXCHANGE;
		pfd->iPixelType = PFD_TYPE_RGBA;
		pfd->cColorBits = bpp;
		pfd->cDepthBits = 0;
		pfd->cStencilBits = 8;
		pfd->iLayerType = PFD_MAIN_PLANE;
	}

	BOOL __fastcall PreparePixelFormat(PIXELFORMATDESCRIPTOR* pfd)
	{
		BOOL res = FALSE;

		if (!isDummyRegistered)
		{
			WNDCLASS wc;
			MemoryZero(&wc, sizeof(WNDCLASS));
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
			wc.lpfnWndProc = DefWindowProc;
			wc.hInstance = hDllModule;
			wc.lpszClassName = "dummyclass";

			isDummyRegistered = RegisterClass(&wc);
		}

		if (isDummyRegistered)
		{
			HWND hWnd = CreateWindowEx(
				WS_EX_APPWINDOW,
				"dummyclass",
				"DUMMY",
				WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
				0, 0,
				1, 1,
				NULL,
				NULL,
				hDllModule,
				NULL
			);

			if (hWnd)
			{
				HDC hDc = GetDC(hWnd);
				if (hDc)
				{
					if (!glPixelFormat)
						glPixelFormat = ChoosePixelFormat(hDc, pfd);

					if (glPixelFormat)
					{
						if (SetPixelFormat(hDc, glPixelFormat, pfd))
						{
							HGLRC hRc = WGLCreateContext(hDc);
							if (hRc)
							{
								if (WGLMakeCurrent(hDc, hRc))
								{
									WGLCHOOSEPIXELFORMATARB WGLChoosePixelFormatARB = (WGLCHOOSEPIXELFORMATARB)WGLGetProcAddress("wglChoosePixelFormatARB");
									if (WGLChoosePixelFormatARB)
									{
										INT piFormats[128];
										UINT nNumFormats;

										INT glAttributes[] = {
											WGL_DRAW_TO_WINDOW_ARB, (pfd->dwFlags & PFD_DRAW_TO_WINDOW) ? GL_TRUE : GL_FALSE,
											WGL_SUPPORT_OPENGL_ARB, (pfd->dwFlags & PFD_SUPPORT_OPENGL) ? GL_TRUE : GL_FALSE,
											WGL_DOUBLE_BUFFER_ARB, (pfd->dwFlags & PFD_DOUBLEBUFFER) ? GL_TRUE : GL_FALSE,
											WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
											WGL_COLOR_BITS_ARB, pfd->cColorBits,
											WGL_DEPTH_BITS_ARB, pfd->cDepthBits,
											WGL_STENCIL_BITS_ARB, pfd->cStencilBits,
											WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
											WGL_SWAP_METHOD_ARB, (pfd->dwFlags & PFD_SWAP_EXCHANGE) ? WGL_SWAP_EXCHANGE_ARB : WGL_SWAP_COPY_ARB,
											0
										};

										if (WGLChoosePixelFormatARB(hDc, glAttributes, NULL, sizeof(piFormats) / sizeof(INT), piFormats, &nNumFormats) && nNumFormats)
											glPixelFormat = piFormats[0];
									}
									res = TRUE;

									WGLMakeCurrent(NULL, NULL);
								}

								WGLDeleteContext(hRc);
							}
						}
					}

					ReleaseDC(hWnd, hDc);
				}

				DestroyWindow(hWnd);
			}
		}

		return res;
	}

	GLuint __fastcall CompileShaderSource(DWORD name, GLenum type)
	{
		HRSRC hResource = FindResource(hDllModule, MAKEINTRESOURCE(name), RT_RCDATA);
		if (!hResource)
			Main::ShowError("FindResource failed", __FILE__, __LINE__);

		HGLOBAL hResourceData = LoadResource(hDllModule, hResource);
		if (!hResourceData)
			Main::ShowError("LoadResource failed", __FILE__, __LINE__);

		LPVOID pData = LockResource(hResourceData);
		if (!pData)
			Main::ShowError("LockResource failed", __FILE__, __LINE__);

		GLuint shader = GLCreateShader(type);

		const GLchar* source[] = { static_cast<const GLchar*>(pData) };
		const GLint lengths[] = { SizeofResource(hDllModule, hResource) };
		GLShaderSource(shader, 1, source, lengths);

		GLint result;
		GLCompileShader(shader);
		GLGetShaderiv(shader, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE)
		{
			GLGetShaderiv(shader, GL_INFO_LOG_LENGTH, &result);

			if (result == 0)
				Main::ShowError("Compile shader failed", __FILE__, __LINE__);
			else
			{
				CHAR data[360];
				GLGetShaderInfoLog(shader, sizeof(data), &result, data);
				Main::ShowError(data, __FILE__, __LINE__);
			}
		}

		return shader;
	}

	DWORD __stdcall ResetThread(LPVOID lpParameter)
	{
		PIXELFORMATDESCRIPTOR pfd;
		GL::PreparePixelFormatDescription(&pfd);
		GL::PreparePixelFormat(&pfd);

		return NULL;
	}

	VOID __fastcall ResetContext()
	{
		DWORD threadId;
		SECURITY_ATTRIBUTES sAttribs;
		MemoryZero(&sAttribs, sizeof(SECURITY_ATTRIBUTES));
		sAttribs.nLength = sizeof(SECURITY_ATTRIBUTES);
		HANDLE hDummy = CreateThread(&sAttribs, NULL, ResetThread, NULL, NORMAL_PRIORITY_CLASS, &threadId);
		WaitForSingleObject(hDummy, INFINITE);
		CloseHandle(hDummy);
	}
}