#pragma once
#pragma comment(lib, "OpenGL32.lib")
#include <Windows.h>
#include <stdio.h>
#include <gl\GL.h>

#include "../geometry/geometry.h"

namespace opengl {
	namespace Draw {
		void Line(Vector2 a, Vector2 b, const GLubyte color[3]);
		void FilledRect(float x, float y, float width, float height, const GLubyte color[3]);
		void Outline(float x, float y, float width, float height, float lineWidth, const GLubyte color[3]);
	}
}
