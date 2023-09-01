#pragma once
#include "draw.h"
#include "text.h"

namespace opengl {
	void SetupOrtho();
	void RestoreGL();

	namespace Color {
		const GLubyte BLACK[3] = { 0,0,0 };
		const GLubyte WHITE[3] = { 255,255,255 };
		const GLubyte LIGHTGRAY[3] = { 192,192,192 };

		const GLubyte RED[3] = { 255,0,0 };
		const GLubyte GREEN[3] = { 0,255,0 };
		const GLubyte BLUE[3] = { 0, 0, 255 };
		const GLubyte ORANGE[3] = { 255, 165, 0 };
	};
};
