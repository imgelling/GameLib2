#pragma once

#include <fstream>
#include <GL/gl.h>
#include <sstream>

#include "GameImageLoader.h"
#include "GameRendererBase.h"
#include "GameShader.h"
#include "GameTexture2D.h"

namespace game
{
	extern SystemInfo systemInfo;
// OpenGL defines (removed at end of RendererGL.cpp)

#define WGL_CONTEXT_DEBUG_BIT_ARB  0x0001

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_ACCELERATION_ARB 0x2003
#define WGL_SWAP_METHOD_ARB 0x2007
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_STEREO_ARB 0x2012
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_COLOR_BITS_ARB 0x2014
#define WGL_RED_BITS_ARB 0x2015
#define WGL_GREEN_BITS_ARB 0x2017
#define WGL_BLUE_BITS_ARB 0x2019
#define WGL_ALPHA_BITS_ARB 0x201B
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023
#define WGL_FULL_ACCELERATION_ARB 0x2027
#define WGL_SWAP_EXCHANGE_ARB 0x2028
#define WGL_SWAP_COPY_ARB 0x2029
#define WGL_TYPE_RGBA_ARB 0x202B
#define GL_TEXTURE_IMAGE_FORMAT 0x828F
#define GL_TEXTURE_IMAGE_TYPE 0x8290
#define GL_SHADING_LANGUAGE_VERSION 0x8B8C
#define GL_NUM_SHADING_LANGUAGE_VERSIONS 0x82E9
#define GL_NUM_EXTENSIONS 0x821D
#define GL_UNSIGNED_INT_8_8_8_8_REV 0x8367
#define GL_UNSIGNED_BYTE 0x1401
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE 0x8212
#define GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE 0x8213
#define GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE 0x8214
#define GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE 0x8215
#define GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE 0x8216
#define GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE 0x8217
#define GL_FRAMEBUFFER 0x8D40
#define GL_MAX_SAMPLES 0x8D57
#define WGL_SAMPLE_BUFFERS_ARB 0x2041
#define WGL_SAMPLES_ARB 0x2042
#define GL_MULTISAMPLE 0x809D
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_VALIDATE_STATUS 0x8B83
#define GL_DEBUG_OUTPUT 0x92E0
#define GL_DEBUG_OUTPUT_SYNCHRONOUS 0x8242
#define GLAPIENTRY __stdcall
#define GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB 0x8242
#define GL_DEBUG_SOURCE_API 0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM 0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER 0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY 0x8249
#define GL_DEBUG_SOURCE_APPLICATION 0x824A
#define GL_DEBUG_SOURCE_OTHER 0x824B
#define GL_DEBUG_TYPE_ERROR 0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR 0x824E
#define GL_DEBUG_TYPE_PORTABILITY 0x824F
#define GL_DEBUG_TYPE_PERFORMANCE 0x8250
#define GL_DEBUG_TYPE_OTHER 0x8251
#define GL_DEBUG_TYPE_MARKER 0x8268
#define GL_DEBUG_TYPE_PUSH_GROUP 0x8269
#define GL_DEBUG_TYPE_POP_GROUP 0x826A
#define GL_DEBUG_SEVERITY_NOTIFICATION 0x826B
#define GL_DEBUG_SEVERITY_HIGH 0x9146
#define GL_DEBUG_SEVERITY_MEDIUM 0x9147
#define GL_DEBUG_SEVERITY_LOW 0x9148
#define GL_TEXTURE_BASE_LEVEL 0x813C
#define GL_TEXTURE_MAX_LEVEL 0x813D

	
	class RendererGL : public RendererBase
	{
	public:
		RendererGL();
		bool CreateDevice(Window& window);
		void DestroyDevice();
		void Swap();
		void HandleWindowResize(const uint32_t width, const uint32_t height);
		void FillOutRendererInfo();
		bool CreateTexture(Texture2D& texture);
		bool LoadTexture(const std::string& fileName, Texture2D &texture);
		void UnLoadTexture(Texture2D& texture);
		bool LoadShader(const std::string vertex, const std::string fragment, Shader& shader);
		bool LoadShader(const std::string vertex, const std::string fragment, const std::string geometry, Shader& shader);
		bool LoadTextShader(const std::string shaderText, const std::string vertexEntryPoint, const std::string fragmentEntryPoint, Shader& shader) { return false; }
		void UnLoadShader(Shader& shader);
		void Clear(const uint32_t bufferFlags, const Color color);
	protected:
		void _ReadExtensions();

	private:
		std::string _validateShader(const uint32_t shader, const char* file); 
		std::string _validateProgram(const uint32_t program);


		typedef HDC _glDeviceContext_t;
		typedef HGLRC _glRenderContext_t;

		_glDeviceContext_t _glDeviceContext = NULL;
		_glRenderContext_t _glRenderContext = NULL;
		typedef BOOL(WINAPI* _PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
		_PFNWGLCHOOSEPIXELFORMATARBPROC _wglChoosePixelFormatARB = nullptr;

		typedef HGLRC(WINAPI* _PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hshareContext, const int* attribList);
		_PFNWGLCREATECONTEXTATTRIBSARBPROC _wglCreateContextAttribsARB = nullptr;

		typedef const GLubyte* (WINAPI* _PFNGLGETSTRINGIPROC) (GLenum name, GLuint index);
		_PFNGLGETSTRINGIPROC _glGetStringi = nullptr;

		typedef int (WINAPI* _PFNWGLGETSWAPINTERVALEXTPROC) ();
		_PFNWGLGETSWAPINTERVALEXTPROC _wglGetSwapInterval = nullptr;

		typedef BOOL(WINAPI* _PFNWGLSWAPINTERVALEXTPROC) (int interval);
		_PFNWGLSWAPINTERVALEXTPROC _wglSwapInterval = nullptr;

		typedef void (WINAPI* _PFNGLGETINTERNALFORMATIVPROC) (GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint* params);
		_PFNGLGETINTERNALFORMATIVPROC _glGetInternalformativ = nullptr;

		typedef void (WINAPI* _PFNGLGENERATEMIPMAPPROC) (GLenum target);
		_PFNGLGENERATEMIPMAPPROC _glGenerateMipmap = nullptr;

		typedef void (WINAPI* _PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) (GLenum target, GLenum attachment, GLenum pname, GLint* params);
		_PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC _glGetFramebufferAttachmentParameteriv = nullptr;

		typedef GLuint(WINAPI* _PFNGLCREATESHADERPROC) (GLenum type);
		_PFNGLCREATESHADERPROC _glCreateShader = nullptr;

		typedef void (WINAPI* _PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const char* const* string, const GLint* length);
		_PFNGLSHADERSOURCEPROC _glShaderSource = nullptr;

		typedef void (WINAPI* _PFNGLCOMPILESHADERARBPROC) (uint32_t shaderObj);
		_PFNGLCOMPILESHADERARBPROC _glCompileShader = nullptr;

		typedef void (WINAPI* _PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei bufSize, GLsizei* length, char* infoLog);
		_PFNGLGETSHADERINFOLOGPROC _glGetShaderInfoLog = nullptr;

		typedef GLuint(WINAPI* _PFNGLCREATEPROGRAMPROC) (void);
		_PFNGLCREATEPROGRAMPROC _glCreateProgram = nullptr;

		typedef void (WINAPI* _PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
		_PFNGLATTACHSHADERPROC _glAttachShader = nullptr;

		typedef void (WINAPI* _PFNGLLINKPROGRAMPROC) (GLuint program);
		_PFNGLLINKPROGRAMPROC _glLinkProgram = nullptr;

		typedef void (WINAPI* _PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei bufSize, GLsizei* length, char* infoLog);
		_PFNGLGETPROGRAMINFOLOGPROC _glGetProgramInfoLog = nullptr;

		typedef void (WINAPI* _PFNGLGETPROGRAMIVPROC) (GLuint program, GLenum pname, GLint* param);
		_PFNGLGETPROGRAMIVPROC _glGetProgramiv = nullptr;

		typedef void (WINAPI* _PFNGLVALIDATEPROGRAMPROC) (GLuint program);
		_PFNGLVALIDATEPROGRAMPROC _glValidateProgram = nullptr;

		typedef void (WINAPI* _PFNGLDELETEPROGRAMPROC) (GLuint program);
		_PFNGLDELETEPROGRAMPROC _glDeleteProgram = nullptr;

		typedef void (WINAPI* _PFNGLDELETESHADERPROC) (GLuint shader);
		_PFNGLDELETESHADERPROC _glDeleteShader = nullptr;

		typedef void (WINAPI* _PFNGLDETACHSHADERPROC) (GLuint program, GLuint shader);
		_PFNGLDETACHSHADERPROC _glDetachShader = nullptr;

		typedef void (WINAPI* _GLDEBUGPROCAMD)(GLenum source, GLuint id, GLenum category, GLenum severity, GLsizei length, const char* message, void* userParam);
		
		typedef void (WINAPI* _PFNGLDEBUGMESSAGECALLBACKAMDPROC) (_GLDEBUGPROCAMD callback, void* userParam);
		_PFNGLDEBUGMESSAGECALLBACKAMDPROC _glDebugMessageCallback = nullptr;

		typedef void (WINAPI* _PFNGLDEBUGMESSAGECONTROLPROC) (GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled);
		_PFNGLDEBUGMESSAGECONTROLPROC _glDebugMessageControl = nullptr;

		static void _openglCallbackFunction(GLenum source, GLuint id, GLenum category, GLenum severity, GLsizei length, const char* message, void* userParam);

	};

	void RendererGL::_openglCallbackFunction(GLenum source, GLuint id, GLenum category, GLenum severity, GLsizei length, const char* message, void* userParam)
	{
		// Some debug messages are just annoying informational messages
		switch (id)
		{
		case 131169: // The driver allocated storage for renderbuffer
		case 131185: // glBufferData
		case 131184: // VBO buffer data
		case 131218: // NVIDIA recompile msg
			std::cout << "____USELESS----------\n";
			return;
		}

		printf("Message: %s\n", message);
		printf("Source: ");

		switch (source)
		{
		case GL_DEBUG_SOURCE_API:
			printf("API");
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			printf("Window System");
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			printf("Shader Compiler");
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			printf("Third Party");
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			printf("Application");
			break;
		case GL_DEBUG_SOURCE_OTHER:
			printf("Other");
			break;
		}

		printf("\n");
		printf("Type: ");

		switch (category)
		{
		case GL_DEBUG_TYPE_ERROR:
			printf("Error");
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			printf("Deprecated Behaviour");
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			printf("Undefined Behaviour");
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			printf("Portability");
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			printf("Performance");
			break;
		case GL_DEBUG_TYPE_MARKER:
			printf("Marker");
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			printf("Push Group");
			break;
		case GL_DEBUG_TYPE_POP_GROUP:
			printf("Pop Group");
			break;
		case GL_DEBUG_TYPE_OTHER:
			printf("Other");
			break;
		}

		printf("\n");
		printf("ID: %d\n", id);
		printf("Severity: ");

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			printf("High");
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			printf("Medium");
			break;
		case GL_DEBUG_SEVERITY_LOW:
			printf("Low");
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			printf("Notification");
			break;
		}
		printf("\n\n");
		return;
	}

	inline RendererGL::RendererGL()
	{
		_glDeviceContext_t glTempDevice = NULL;
		_glRenderContext_t glTempRender = NULL;
		game::Window tempWindow;
		Attributes tempAttrib;
		PIXELFORMATDESCRIPTOR pixelFormatDescriptor =
		{
			sizeof(PIXELFORMATDESCRIPTOR), 1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			PFD_MAIN_PLANE, 0, 0, 0, 0
		};

		// Create a temporary invisible window
		tempAttrib.WindowVisible = false;
		tempWindow.SetAttributes(tempAttrib);
		if (!tempWindow.CreateTheWindow())
		{
			std::cout << "Temporary window creation failed.\n";
			return;
		}

		// Get the device context
		glTempDevice = GetDC(tempWindow.GetHandle());

		// Choose a generic pixel format
		uint32_t tempPixelFormat = 0;
		tempPixelFormat = ChoosePixelFormat(glTempDevice, &pixelFormatDescriptor);
		if (!tempPixelFormat)
		{
			std::cout << "Temporary Window ChoosePixelFormat Failed.\n";
			return;
		}

		// Set the generic pixel format
		if (!SetPixelFormat(glTempDevice, tempPixelFormat, &pixelFormatDescriptor))
		{
			std::cout << "Temporary Window SetPixelFormat Failed.\n";
		}

		// Create the OpenGL rendering context
		glTempRender = wglCreateContext(glTempDevice);
		if (!glTempRender)
		{
			std::cout << "Temporary Window wglCreateContext Failed.\n";
			return;
		}

		// Use new rendering context
		wglMakeCurrent(glTempDevice, glTempRender);


		// Load necessary extensions for actual OpenGl render context
		_wglChoosePixelFormatARB = (_PFNWGLCHOOSEPIXELFORMATARBPROC)(wglGetProcAddress("wglChoosePixelFormatARB"));
		if (_wglChoosePixelFormatARB == nullptr)
		{
			std::cout << "wglChoosePixelFormatARM failed to load.\n";
			return;
		}

		_wglCreateContextAttribsARB = (_PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
		if (_wglCreateContextAttribsARB == nullptr)
		{
			std::cout << "wglCreateContextAttribsARB failed to load.\n";
			return;
		}

		// Need to know max multisamples the card can do before we create the real window
		glGetIntegerv(GL_MAX_SAMPLES, &systemInfo.gpuInfo.maxMultisamples);
		if (systemInfo.gpuInfo.maxMultisamples == 32)
		{
			// Nvidia reports 32 samples, but doesn't work for frame buffer
			systemInfo.gpuInfo.maxMultisamples = 16;
		}

		// Clean up OpenGL stuff
		wglMakeCurrent(NULL, NULL);
		if (glTempRender) wglDeleteContext(glTempRender);

		// Clean up temporary window stuff
		PostMessage(tempWindow.GetHandle(), WM_DESTROY, 0, 0);
	}

	inline bool RendererGL::CreateDevice(Window& window)
	{

		const int glContextAttributes[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB,  _attributes.ContextMajor,
			WGL_CONTEXT_MINOR_VERSION_ARB,  _attributes.ContextMinor,
			WGL_CONTEXT_FLAGS_ARB,
			_attributes.DebugMode ? WGL_CONTEXT_DEBUG_BIT_ARB : 0,
			WGL_CONTEXT_PROFILE_MASK_ARB,
			_attributes.GLBackwardsCompatible ? WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB : WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};
		int32_t pixelFormatsChosen[1] = { 0 };
		uint32_t numberOfPixelFormatsChosen = 0;

		int32_t colorBits = _attributes.RedSize + _attributes.BlueSize + _attributes.GreenSize + _attributes.AlphaSize;

		// Make sure we don't go above limits
		if (_attributes.MultiSamples > systemInfo.gpuInfo.maxMultisamples)
		{
			_attributes.MultiSamples = systemInfo.gpuInfo.maxMultisamples;
		}
		int32_t glPixelAttributeList[] =
		{
			WGL_SUPPORT_OPENGL_ARB, 1,
			WGL_DRAW_TO_WINDOW_ARB, 1,
			WGL_COLOR_BITS_ARB, colorBits,
			WGL_RED_BITS_ARB, _attributes.RedSize,
			WGL_GREEN_BITS_ARB, _attributes.GreenSize,
			WGL_BLUE_BITS_ARB, _attributes.BlueSize,
			WGL_ALPHA_BITS_ARB, _attributes.AlphaSize,
			WGL_DEPTH_BITS_ARB, _attributes.DepthSize,
			WGL_STENCIL_BITS_ARB, _attributes.StencilSize,
			WGL_DOUBLE_BUFFER_ARB, _attributes.DoubleBuffer,
			WGL_STEREO_ARB, 0,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
			WGL_SWAP_METHOD_ARB, WGL_SWAP_EXCHANGE_ARB,
			WGL_SAMPLE_BUFFERS_ARB, _attributes.MultiSamples <= 1 ? 0 : 1,
			WGL_SAMPLES_ARB, _attributes.MultiSamples,
			0
		};

		float_t pixelAttribFloatList[] = { 0, 0 };
		PIXELFORMATDESCRIPTOR pixelFormatDescriptor = { 0 };

		// Get the window handle as the device we are using
		_glDeviceContext = GetDC(window.GetHandle());


		// Choose a suitable pixel format for what attributes we want for real window
		_wglChoosePixelFormatARB(_glDeviceContext, glPixelAttributeList, pixelAttribFloatList, 1, &pixelFormatsChosen[0], &numberOfPixelFormatsChosen);
		if (!numberOfPixelFormatsChosen)
		{
			lastError = { GameErrors::GameOpenGLSpecific, "No compatible pixel formats found." };
			return false;
		}

		// Fills out some of the pixelformatedescriptor
		if (!DescribePixelFormat(_glDeviceContext, pixelFormatsChosen[0], sizeof(PIXELFORMATDESCRIPTOR), &pixelFormatDescriptor))
		{
			lastError = { GameErrors::GameWindowsSpecific, "DescribePixelFormat failed. Window Error " + std::to_string(GetLastError()) };
			return false;
		}

		// Set our pixel format that we got from the temporary window
		if (!SetPixelFormat(_glDeviceContext, pixelFormatsChosen[0], &pixelFormatDescriptor))
		{
			lastError = { GameErrors::GameWindowsSpecific, "SetPixelFormat failed. Windows Error " + std::to_string(GetLastError()) };
			return false;
		}

		// Create the rendering context
		_glRenderContext = _wglCreateContextAttribsARB(_glDeviceContext, 0, glContextAttributes);
		if (_glRenderContext == nullptr)
		{
			lastError = { GameErrors::GameOpenGLSpecific, "wglCreateContextAttribsARB failed" };
			return false;
		}

		// Use the rendering context we just created
		wglMakeCurrent(_glDeviceContext, _glRenderContext);


		// Load wglSwapInterval
		_wglSwapInterval = (_PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
		if (_wglSwapInterval == NULL)
		{
			lastError = { GameErrors::GameOpenGLSpecific, "Loading SwapInterval Failed" };
			return false;
		}

		// Load wglGetSwapInterval
		_wglGetSwapInterval = (_PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");
		if (_wglGetSwapInterval == nullptr)
		{
			lastError = { GameErrors::GameOpenGLSpecific, "Loading GetSwapInterval Failed" };
			return false;
		}

		// Load glGetStringi
		_glGetStringi = (_PFNGLGETSTRINGIPROC)wglGetProcAddress("glGetStringi");
		if (_glGetStringi == nullptr)
		{
			lastError = { GameErrors::GameOpenGLSpecific, "Extension glGetStringi not available." };
			return false;
		}

		// Load glGetInternalformativ
		_glGetInternalformativ = (_PFNGLGETINTERNALFORMATIVPROC)wglGetProcAddress("glGetInternalformativ");
		if (_glGetInternalformativ == nullptr)
		{
			lastError = { GameErrors::GameOpenGLSpecific, "Extension glGetInternalformativ not available." };
			return false;
		}

		_glGenerateMipmap = (_PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
		if (_glGetInternalformativ == nullptr)
		{
			lastError = { GameErrors::GameOpenGLSpecific, "Extension glGenerateMipmap not available." };
			return false;
		}

		_glGetFramebufferAttachmentParameteriv = (_PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)wglGetProcAddress("glGetFramebufferAttachmentParameteriv");
		if (_glGetFramebufferAttachmentParameteriv == nullptr)
		{
			lastError = { GameErrors::GameOpenGLSpecific, "Extension glGetFramebufferAttachmentParameteriv not available " };
			return false;
		}

		_glCreateShader = (_PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
		if (_glCreateShader == nullptr)
		{
			lastError = { GameErrors::GameOpenGLSpecific, "Extension glCreateShader not available." };
			return false;
		}

		_glShaderSource = (_PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
		if (_glShaderSource == nullptr)
		{
			lastError = { GameErrors::GameOpenGLSpecific, "Extension glShaderSource not available." };
			return false;
		}

		_glCompileShader = (_PFNGLCOMPILESHADERARBPROC)wglGetProcAddress("glCompileShader");
		if (_glCompileShader == nullptr)
		{
			lastError = { GameErrors::GameOpenGLSpecific, "Extension glCompileShader not available." };
			return false;
		}

		_glGetShaderInfoLog = (_PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
		if (_glGetShaderInfoLog == nullptr)
		{
			lastError = { GameErrors::GameOpenGLSpecific, "Extension glGetShaderInfoLog not available." };
			return false;
		}

		_glCreateProgram = (_PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
		if (_glCreateProgram == nullptr)
		{
			lastError = { GameErrors::GameOpenGLSpecific, "Extension glCreateProgram not available." };
			return false;
		}

		_glAttachShader = (_PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
		if (_glAttachShader == nullptr)
		{
			lastError = { GameErrors::GameOpenGLSpecific, "Extension glAttachShader not available." };
			return false;
		}

		_glLinkProgram = (_PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
		if (_glLinkProgram == nullptr)
		{
			lastError = { GameErrors::GameOpenGLSpecific, "Extension glLinkProgram not available." };
			return false;
		}

		_glGetProgramInfoLog = (_PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
		if (_glGetProgramInfoLog == nullptr)
		{
			lastError = { GameErrors::GameOpenGLSpecific, "Extension glGetProgramInfo not available." };
			return false;
		}

		_glGetProgramiv = (_PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
		if (_glGetProgramiv == nullptr)
		{
			lastError = { GameErrors::GameOpenGLSpecific, "Extension glGetProgramiv not available." };
			return false;
		}

		_glValidateProgram = (_PFNGLVALIDATEPROGRAMPROC)wglGetProcAddress("glValidateProgram");
		if (_glValidateProgram == nullptr)
		{
			lastError = { GameErrors::GameOpenGLSpecific, "Extension glValidateprogram not available." };
			return false;
		}

		_glDeleteProgram = (_PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
		if (_glDeleteProgram == nullptr)
		{
			lastError = { GameErrors::GameOpenGLSpecific, "Extension glDeleteProgram not available." };
			return false;
		}

		_glDeleteShader = (_PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
		if (_glDeleteShader == nullptr)
		{
			lastError = { GameErrors::GameOpenGLSpecific, "Extension glDeleteShader not available." };
			return false;
		}

		_glDetachShader = (_PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
		if (_glDetachShader == nullptr)
		{
			lastError = { GameErrors::GameOpenGLSpecific, "Extension glDetachShader not available." };
			return false;
		}

		_glDebugMessageCallback = (_PFNGLDEBUGMESSAGECALLBACKAMDPROC)wglGetProcAddress("glDebugMessageCallback");
		if (_glDebugMessageCallback == nullptr)
		{
			lastError = { GameErrors::GameOpenGLSpecific, "Extension glDebugMessageCallback not available." };
			return false;
		}

		_glDebugMessageControl = (_PFNGLDEBUGMESSAGECONTROLPROC)wglGetProcAddress("glDebugMessageControl");
		if (_glDebugMessageControl == nullptr)
		{
			lastError = { GameErrors::GameOpenGLSpecific, "Extension glDebugMessgeControl not available" };
			return false;
		}

		// Set vertical sync
		if (_attributes.VsyncOn)
			_wglSwapInterval(1);
		else
			_wglSwapInterval(0);

		// If multisampling is desired enable it
		if (_attributes.MultiSamples > 1)
		{
			glEnable(GL_MULTISAMPLE);
		}
		else
		{
			glDisable(GL_MULTISAMPLE);
		}

		if (_attributes.DebugMode)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

			_glDebugMessageCallback(_openglCallbackFunction, nullptr);
			_glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}

		// Read all available extensions
		_ReadExtensions();

		// Initialize COM
		HRESULT hr = CoInitialize(NULL);


		return true;
	}
	
	inline void RendererGL::_ReadExtensions()
	{
		int32_t numberOfExtensions = 0;
		std::string extensionName;

		// Get the number of OpenGL extensions available
		glGetIntegerv(GL_NUM_EXTENSIONS, &numberOfExtensions);
		_logger->WriteQuiet("Listing " + std::to_string(numberOfExtensions) + " OpenGL Extensions Available.");

		// Write all extensions out to the log file, not stdout 
		// and store in a vector
		for (int32_t extensionNumber = 0; extensionNumber < numberOfExtensions; extensionNumber++)
		{
			extensionName = (char*)_glGetStringi(GL_EXTENSIONS, extensionNumber);
			_extensionsAvailable.emplace_back(extensionName);
			_logger->WriteQuiet(extensionName);
		}
	}

	inline void RendererGL::DestroyDevice()
	{
		// Shut down COM stuff
		CoUninitialize();
		// Clean up OpenGL stuff
		wglMakeCurrent(NULL, NULL);
		if (_glRenderContext) wglDeleteContext(_glRenderContext);
		_glRenderContext = NULL;
	}

	inline void RendererGL::Swap()
	{
		SwapBuffers(_glDeviceContext);
	};

	inline void RendererGL::HandleWindowResize(const uint32_t width, const uint32_t height)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, width, height);
	}

	inline void RendererGL::FillOutRendererInfo()
	{
		std::stringstream sStream;
		std::string tempValue;

		// Lambda to log easier
		auto LOG = [&](std::stringstream& stream)
		{
			_logger->Write(stream.str());
			stream.str("");
		};
		
		// Log the OpenGL rendering device
		systemInfo.gpuInfo.renderer = (char*)glGetString(GL_RENDERER);
		sStream << "OpenGL renderer : " << systemInfo.gpuInfo.renderer;
		LOG(sStream);

		// Log the version of OpenGL context
		systemInfo.gpuInfo.version = (char*)glGetString(GL_VERSION);
		sStream << "OpenGL version : " << systemInfo.gpuInfo.version;
		LOG(sStream);

		// Extract numerical major and minor version numbers
		tempValue = (char*)glGetString(GL_VERSION);
		systemInfo.gpuInfo.versionMajor = std::strtol(tempValue.c_str(), NULL, 10);
		tempValue.erase(0, 2);
		systemInfo.gpuInfo.versionMinor = std::strtol(tempValue.c_str(), NULL, 10);
		tempValue = "";

		// Log the vender of the renderer
		systemInfo.gpuInfo.vendor = (char*)glGetString(GL_VENDOR);
		sStream << "Vendor : " << systemInfo.gpuInfo.vendor;
		LOG(sStream);



		// Log the max shader language version
		systemInfo.gpuInfo.maxShaderLanguageVersion = (char*)_glGetStringi(GL_SHADING_LANGUAGE_VERSION, 0);
		sStream << "Max supported GLSL version : " << systemInfo.gpuInfo.maxShaderLanguageVersion;
		LOG(sStream);

		// Log video memory
		if (systemInfo.gpuInfo.vendor.find("NVIDIA") != std::string::npos)
		{
#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049
			glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &systemInfo.gpuInfo.totalMemory);
			glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &systemInfo.gpuInfo.freeMemory);
			systemInfo.gpuInfo.totalMemory /= 1024;
			systemInfo.gpuInfo.freeMemory /= 1024;
			sStream << "GPU total memory : " << systemInfo.gpuInfo.totalMemory << "MB";
			LOG(sStream);
			sStream << "GPU available memory : " << systemInfo.gpuInfo.freeMemory << "MB";
			LOG(sStream);
#undef GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX
#undef GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 
		}
		else
		{
			sStream << "GPU total memory : Can not retrieve video memory on AMD.";
			LOG(sStream);
			sStream << "GPU available memory : Can not retrieve video memory on AMD.";
			LOG(sStream);
		}

		// Log internal pixel format
		_glGetInternalformativ(GL_TEXTURE_2D, GL_RGBA8, GL_TEXTURE_IMAGE_FORMAT, 1, &systemInfo.gpuInfo.internalPixelFormat);
		// Convert to hexidecimal
		sStream << std::hex << systemInfo.gpuInfo.internalPixelFormat;
		std::string hex(sStream.str());
		sStream.str("");
		systemInfo.gpuInfo.internalPixelFormat = std::strtol(hex.c_str(), NULL, 16);
		sStream << "Internal pixel format : " << systemInfo.gpuInfo.internalPixelFormat;
		std::string pixelFormat = (systemInfo.gpuInfo.internalPixelFormat == 0x1908) ? " (RGBA)" : " (Unknown)";
		sStream << pixelFormat;
		LOG(sStream);

		// Log internal pixel type
		_glGetInternalformativ(GL_TEXTURE_2D, GL_RGBA8, GL_TEXTURE_IMAGE_TYPE, 1, &systemInfo.gpuInfo.internalPixelType);
		// Convert to hex
		sStream << std::hex << systemInfo.gpuInfo.internalPixelType;
		hex = sStream.str();
		sStream.str("");
		systemInfo.gpuInfo.internalPixelType = std::strtol(hex.c_str(), NULL, 16);
		sStream << "Internal pixel type : " << systemInfo.gpuInfo.internalPixelType;
		std::string pixelType;
		if (systemInfo.gpuInfo.internalPixelType == 0x8367)
			pixelType = " (GL_UNSIGNED_INT_8_8_8_8_REV)";
		else if (systemInfo.gpuInfo.internalPixelType == 0x1401)
			pixelType = " (GL_UNSIGNED_BYTE)";
		else
			pixelType = " (Unknown)";
		sStream << pixelType;
		LOG(sStream);
		// The difference is endianness, both pixel formats are RGBA

		// Log multisampling
		if (_attributes.MultiSamples > 1)
		{
			systemInfo.gpuInfo.multisampleSamples = _attributes.MultiSamples;
			sStream << "Multisampling samples : " << std::dec << systemInfo.gpuInfo.multisampleSamples;
			sStream << " (" << systemInfo.gpuInfo.maxMultisamples << " max)";
			LOG(sStream);
		}
		else
		{
			sStream << "Multisampling samples : disabled";
			LOG(sStream);
		}


		// Log front buffer
		int32_t r = 0, g = 0, b = 0, a = 0;
		_glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_FRONT_LEFT, GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE, &r);
		_glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_FRONT_LEFT, GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE, &g);
		_glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_FRONT_LEFT, GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE, &b);
		_glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_FRONT_LEFT, GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE, &a);
		// hex to dec
		systemInfo.gpuInfo.frontBufferColorSize = r + b + g + a;
		sStream << std::dec << systemInfo.gpuInfo.frontBufferColorSize;
		hex = sStream.str();
		sStream.str("");
		systemInfo.gpuInfo.frontBufferColorSize = std::strtol(hex.c_str(), NULL, 10);
		sStream << "Front buffer : " << systemInfo.gpuInfo.frontBufferColorSize << " bits";
		LOG(sStream);

		// Log back buffer
		_glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_BACK_LEFT, GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE, &r);
		_glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_BACK_LEFT, GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE, &g);
		_glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_BACK_LEFT, GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE, &b);
		_glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_BACK_LEFT, GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE, &a);
		systemInfo.gpuInfo.backBufferColorSize = r + b + g + a;
		// hex to dec
		sStream << std::dec << systemInfo.gpuInfo.backBufferColorSize;
		hex = sStream.str();
		sStream.str("");
		systemInfo.gpuInfo.backBufferColorSize = std::strtol(hex.c_str(), NULL, 10);
		sStream << "Back buffer : " << systemInfo.gpuInfo.backBufferColorSize << " bits";
		LOG(sStream);

		// Log depth buffer
		_glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH, GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &systemInfo.gpuInfo.depthBufferSize);
		sStream << "Depth buffer : " << systemInfo.gpuInfo.depthBufferSize << " bits";
		LOG(sStream);

		// Get the max anisotropy
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &systemInfo.gpuInfo.maxAnisotropy);
		// Convert hex to dec
		sStream << std::dec << systemInfo.gpuInfo.maxAnisotropy;
		hex = sStream.str();
		sStream.str("");
		systemInfo.gpuInfo.maxAnisotropy = std::strtol(hex.c_str(), NULL, 10);
		sStream << "Max anisotropy : " << systemInfo.gpuInfo.maxAnisotropy << "x";
		LOG(sStream);

	}
	
	inline void RendererGL::Clear(const uint32_t bufferFlags, const Color color)
	{
		uint32_t flags = 0;
		if (bufferFlags | GAME_FRAME_BUFFER_BIT)
		{
			glClearColor(color.rf, color.gf, color.bf, color.af);
			flags = GL_COLOR_BUFFER_BIT;
		}
		if (bufferFlags | GAME_DEPTH_STENCIL_BUFFER_BIT)
		{
			flags |= GL_DEPTH_BUFFER_BIT;
		}
		glClear(flags);
	}

	inline bool RendererGL::CreateTexture(Texture2D& texture)
	{
		texture.oneOverWidth = 1.0f / (float_t)texture.width;
		texture.oneOverHeight = 1.0f / (float_t)texture.height;
		texture.isCopy = false;

		glGenTextures(1, &texture.bind);
		glBindTexture(GL_TEXTURE_2D, texture.bind);
		//GL_NEAREST is point filtering
		//GL_LINEAR will give you bilinear filtering.GL_LINEAR_MIPMAP_LINEAR should be trilinear.
		//GL_NEAREST_MIPMAP_NEAREST: takes the nearest mipmap to match the pixel sizeand uses nearest neighbor interpolation for texture sampling.
		//GL_LINEAR_MIPMAP_NEAREST : takes the nearest mipmap leveland samples that level using linear interpolation.
		//GL_NEAREST_MIPMAP_LINEAR : linearly interpolates between the two mipmaps that most closely match the size of a pixeland samples the interpolated level via nearest neighbor interpolation.
		//GL_LINEAR_MIPMAP_LINEAR : linearly interpolates between the two closest mipmapsand samples the interpolated level via linear interpolation.
		if (texture.filterType == TextureFilterType::Point)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);// LINEAR_MIPMAP_LINEAR); // min
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);// GL_LINEAR); //max
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		}
		else if (texture.filterType == TextureFilterType::Bilinear)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);// LINEAR_MIPMAP_LINEAR); // min
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);// GL_LINEAR); //max
		}
		else if (texture.filterType == TextureFilterType::Trilinear)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Anisotropy
		if (texture.anisotropyLevel > systemInfo.gpuInfo.maxAnisotropy)
		{
			texture.anisotropyLevel = systemInfo.gpuInfo.maxAnisotropy;
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, texture.anisotropyLevel);
		// When adding gfx options, just div by 2 down to 2x; so 16, 8, 4, 2, 0


		// Uses internal pixel type for textures
		if (texture.componentsPerPixel == 4)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, systemInfo.gpuInfo.internalPixelType, 0);
		}
		else if (texture.componentsPerPixel == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.width, texture.height, 0, GL_RGBA, systemInfo.gpuInfo.internalPixelType, 0);
		}

		if (texture.isMipMapped)
			_glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
		return true;
	}

	inline bool RendererGL::LoadTexture(const std::string& fileName, Texture2D &texture)
	{
		void* data = nullptr;
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t componentsPerPixel = 0;
		ImageLoader imageLoader;

		// Read data
		data = imageLoader.Load(fileName.c_str(), width, height, componentsPerPixel);
		if (data == nullptr)
		{
			lastError = { GameErrors::GameContent, "Failed to load texture : " + fileName };
			return false;
		}
		texture.width = width;
		texture.height = height;
		texture.oneOverWidth = 1.0f / (float_t)width;
		texture.oneOverHeight = 1.0f / (float_t)height;
		texture.isCopy = false;
		texture.name = fileName;

		glGenTextures(1, &texture.bind);
		glBindTexture(GL_TEXTURE_2D, texture.bind);
		// GL_Nearest is point filtering
		//GL_LINEAR will give you bilinear filtering.GL_LINEAR_MIPMAP_LINEAR should be trilinear.
		//GL_NEAREST_MIPMAP_NEAREST: takes the nearest mipmap to match the pixel sizeand uses nearest neighbor interpolation for texture sampling.
		//GL_LINEAR_MIPMAP_NEAREST : takes the nearest mipmap leveland samples that level using linear interpolation.
		//GL_NEAREST_MIPMAP_LINEAR : linearly interpolates between the two mipmaps that most closely match the size of a pixeland samples the interpolated level via nearest neighbor interpolation.
		//GL_LINEAR_MIPMAP_LINEAR : linearly interpolates between the two closest mipmapsand samples the interpolated level via linear interpolation.
		if (texture.filterType == TextureFilterType::Point)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		else if (texture.filterType == TextureFilterType::Bilinear)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else if (texture.filterType == TextureFilterType::Trilinear)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Anisotropy
		if (texture.anisotropyLevel > systemInfo.gpuInfo.maxAnisotropy)
		{
			texture.anisotropyLevel = systemInfo.gpuInfo.maxAnisotropy;
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, texture.anisotropyLevel);
		// When adding gfx options, just div by 2 down to 2x; so 16, 8, 4, 2, 0


		// Uses internal pixel type for textures
		if (componentsPerPixel == 4)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, systemInfo.gpuInfo.internalPixelType, data);
		}
		else if (componentsPerPixel == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, systemInfo.gpuInfo.internalPixelType, data);
		}

		if (texture.isMipMapped)
		{
			_glGenerateMipmap(GL_TEXTURE_2D);
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		return true;
	}

	inline void RendererGL::UnLoadTexture(Texture2D& texture)
	{
		glDeleteTextures(1, &texture.bind);
		texture.bind = 0;
		texture.width = 0;
		texture.height = 0;
		texture.oneOverWidth = 0.0f;
		texture.oneOverHeight = 0.0f;
		texture.componentsPerPixel = 0;
		texture.isCopy = false;
		texture.name = "NULL";
		// Attributes of texture filtering
		texture.isMipMapped = true;
		texture.filterType = TextureFilterType::Trilinear;
		texture.anisotropyLevel = 1;
	}

	inline std::string RendererGL::_validateShader(const uint32_t shader, const char* file) {
		const uint32_t BUFFER_SIZE = 512;
		char buffer[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		int32_t length = 0;
		std::stringstream err;

		_glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);
		if (length > 0) {
			err << "Shader " << shader << " (" << (file ? file : "") << ") compile: " << buffer << std::endl;
		}
		return err.str();
	}

	inline std::string RendererGL::_validateProgram(const uint32_t program)
	{
		const unsigned int BUFFER_SIZE = 512;
		char buffer[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		GLsizei length = 0;
		std::stringstream err;

		memset(buffer, 0, BUFFER_SIZE);
		_glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);
		if (length > 0)
			err << "Program " << program << " link: " << buffer << std::endl;

		_glValidateProgram(program);
		GLint status;
		_glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
		if (status == GL_FALSE)
			err << "Error validating shader " << program << std::endl;
		return err.str();
	}

	inline bool RendererGL::LoadShader(const std::string vertex, const std::string fragment, Shader &shader)
	{
		std::ifstream file;
		std::string vertexFile;
		std::string fragmentFile;

		// Load vertex shader
		file.open(vertex,std::iostream::_Nocreate);
		if (file.is_open())
		{
			do
			{
				vertexFile += file.get();
			} while (!file.eof());
			file.close();
			vertexFile.pop_back();
		}
		else
		{
			lastError = { GameErrors::GameRenderer, "Could not open " + vertex + "." };
			return false;
		}

		// Load fragment shader
		file.open(fragment, std::iostream::_Nocreate);
		if (file.is_open())
		{
			do
			{
				fragmentFile += file.get();
			} while (!file.eof());
			file.close();
			fragmentFile.pop_back();
		}
		else
		{
			lastError = { GameErrors::GameRenderer, "Could not open " + fragment + "." };
			return false;
		}
		
		// Create the shaders
		shader.vertexId = _glCreateShader(GL_VERTEX_SHADER);
		shader.fragmentId = _glCreateShader(GL_FRAGMENT_SHADER);
		const char* vs = (const char*)vertexFile.c_str();
		const char* fs = (const char*)fragmentFile.c_str();
		_glShaderSource(shader.vertexId, 1, &vs, 0);
		_glShaderSource(shader.fragmentId, 1, &fs, 0);

		// Compile the vertex shader
		_glCompileShader(shader.vertexId);
		std::string output;
		output = _validateShader(shader.vertexId, vertex.c_str());
		if (output.length() > 0)
		{
			lastError = { GameErrors::GameOpenGLSpecific, output };
			return false;
		}
		
		// Compile the fragment shader
		_glCompileShader(shader.fragmentId);
		output = _validateShader(shader.fragmentId, fragment.c_str());
		if (output.length() > 0)
		{
			lastError = { GameErrors::GameOpenGLSpecific, output };
			return false;
		}

		// Link the vertex and fragment shader
		shader.shaderId = _glCreateProgram();
		_glAttachShader(shader.shaderId, shader.fragmentId);
		_glAttachShader(shader.shaderId, shader.vertexId);
		_glLinkProgram(shader.shaderId);
		output = _validateProgram(shader.shaderId);
		if (output.length() > 0)
		{
			lastError = { GameErrors::GameOpenGLSpecific, output };
			return false;
		}

		return true;
	}

	inline bool RendererGL::LoadShader(const std::string vertex, const std::string fragment, const std::string geometry, Shader& shader)
	{
		lastError = { GameErrors::GameOpenGLSpecific, "Geometry Shaders not supported in OpenGL, for now." };
		return false;
	}

	inline void RendererGL::UnLoadShader(Shader& shader)
	{
		if (shader.shaderId)
		{
			_glDetachShader(shader.shaderId, shader.fragmentId);
			_glDetachShader(shader.shaderId, shader.vertexId);

			_glDeleteShader(shader.fragmentId);
			_glDeleteShader(shader.vertexId);
			_glDeleteProgram(shader.shaderId);
		}

		shader.shaderId = 0;
		shader.fragmentId = 0;
		shader.vertexId = 0;
	}

// Undefine what we have done, if someone uses an extension loader 
#undef GL_TEXTURE_BASE_LEVEL 
#undef GL_TEXTURE_MAX_LEVEL 
#undef GL_DEBUG_SEVERITY_HIGH
#undef GL_DEBUG_SEVERITY_MEDIUM
#undef GL_DEBUG_SEVERITY_LOW
#undef GL_DEBUG_SOURCE_API 
#undef GL_DEBUG_SOURCE_WINDOW_SYSTEM 
#undef GL_DEBUG_SOURCE_SHADER_COMPILER 
#undef GL_DEBUG_SOURCE_THIRD_PARTY 
#undef GL_DEBUG_SOURCE_APPLICATION 
#undef GL_DEBUG_SOURCE_OTHER 
#undef GL_DEBUG_TYPE_ERROR 
#undef GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 
#undef GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR 
#undef GL_DEBUG_TYPE_PORTABILITY 
#undef GL_DEBUG_TYPE_PERFORMANCE 
#undef GL_DEBUG_TYPE_OTHER 
#undef GL_DEBUG_TYPE_MARKER 
#undef GL_DEBUG_TYPE_PUSH_GROUP 
#undef GL_DEBUG_TYPE_POP_GROUP 
#undef GL_DEBUG_SEVERITY_NOTIFICATION 
#undef GL_DEBUG_SEVERITY_HIGH_ARB 
#undef GL_DEBUG_SEVERITY_MEDIUM_ARB 
#undef GL_DEBUG_SEVERITY_LOW_ARB 

#undef GLAPIENTRY
#undef GL_DEBUG_OUTPUT_SYNCHRONOUS
#undef GL_DEBUG_OUTPUT 
#undef GL_VALIDATE_STATUS 
#undef GL_FRAGMENT_SHADER 
#undef GL_VERTEX_SHADER 
#undef GL_MULTISAMPLE
#undef WGL_SAMPLE_BUFFERS_ARB 
#undef WGL_SAMPLES_ARB
#undef GL_MAX_SAMPLES
#undef GL_FRAMEBUFFER 
#undef GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE
#undef GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE 
#undef GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE 
#undef GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE 
#undef GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE 
#undef GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE 
#undef GL_CLAMP_TO_EDGE
#undef GL_TEXTURE_MAX_ANISOTROPY_EXT
#undef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
#undef GL_UNSIGNED_INT_8_8_8_8_REV
#undef GL_UNSIGNED_BYTE
#undef GL_NUM_SHADING_LANGUAGE_VERSIONS 
#undef GL_SHADING_LANGUAGE_VERSION
#undef GL_TEXTURE_IMAGE_FORMAT
#undef GL_TEXTURE_IMAGE_TYPE 
#undef WGL_CONTEXT_DEBUG_BIT_ARB
#undef WGL_CONTEXT_MAJOR_VERSION_ARB 
#undef WGL_CONTEXT_MINOR_VERSION_ARB 
#undef WGL_CONTEXT_FLAGS_ARB 
#undef WGL_CONTEXT_CORE_PROFILE_BIT_ARB 
#undef WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 
#undef WGL_CONTEXT_PROFILE_MASK_ARB 
#undef WGL_DRAW_TO_WINDOW_ARB 
#undef WGL_ACCELERATION_ARB 
#undef WGL_SWAP_METHOD_ARB 
#undef WGL_SUPPORT_OPENGL_ARB 
#undef WGL_DOUBLE_BUFFER_ARB 
#undef WGL_STEREO_ARB 
#undef WGL_PIXEL_TYPE_ARB 
#undef WGL_COLOR_BITS_ARB 
#undef WGL_RED_BITS_ARB 
#undef WGL_GREEN_BITS_ARB 
#undef WGL_BLUE_BITS_ARB 
#undef WGL_ALPHA_BITS_ARB 
#undef WGL_STENCIL_BITS_ARB 
#undef WGL_FULL_ACCELERATION_ARB 
#undef WGL_SWAP_EXCHANGE_ARB 
#undef WGL_SWAP_COPY_ARB
#undef WGL_TYPE_RGBA_ARB 
#undef GL_NUM_EXTENSIONS 

}
