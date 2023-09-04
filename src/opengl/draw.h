#pragma once
#pragma comment(lib, "OpenGL32.lib")
#include <Windows.h>
#include <stdio.h>
#include <gl\GL.h>

#include "../geometry/geometry.h"

namespace opengl {
	namespace Draw {
		void Line(geometry::Line line, const GLubyte color[3]);
		void FilledRect(geometry::Rect rect, const GLubyte color[3]);
		void Outline(geometry::Rect rect, float lineWidth, const GLubyte color[3]);
	}
}
