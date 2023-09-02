#pragma once
#pragma comment(lib, "OpenGL32.lib")
#include <Windows.h>
#include <stdio.h>
#include <gl\GL.h>

#include "../geometry/geometry.h"

namespace opengl {
	namespace Text {
		class Font {
		public:
			bool bBuilt = false;
			unsigned int base;
			HDC hdc = nullptr;
			int height;
			int width;

			void Build(int height);
			void Print(float x, float y, const unsigned char color[3], const char* format, ...);

			Vector3 CenterText(float x, float y, float width, float height, float textWidth, float textHeight);
			float CenterText(float x, float width, float textWidth);
			float EndText(float x, float width, float textWidth);
		};
	}
}
