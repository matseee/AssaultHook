#pragma once
#pragma comment(lib, "OpenGL32.lib")
#include <Windows.h>
#include <stdio.h>
#include <gl\GL.h>

#include "geomatry.h"

namespace rgb {
	const GLubyte black[3] = { 0,0,0 };
	const GLubyte white[3] = { 255,255,255 };
	const GLubyte lightgray[3] = { 192,192,192 };

	const GLubyte red[3] = { 255,0,0 };
	const GLubyte green[3] = { 0,255,0 };
	const GLubyte blue[3] = { 0, 0, 255 };
}

namespace openGLDraw {
	void SetupOrtho();
	void RestoreGL();

	void DrawLine(Vector2 a, Vector2 b, const GLubyte color[3]);
	void DrawFilledRect(float x, float y, float width, float height, const GLubyte color[3]);
	void DrawOutline(float x, float y, float width, float height, float lineWidth, const GLubyte color[3]);
}

