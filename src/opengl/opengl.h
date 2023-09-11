#pragma once
#include "draw.h"
#include "text.h"

namespace opengl {
    void SetupOrtho();
    void RestoreGL();

    namespace Color {
        const GLubyte BLACK[3] = { 0, 0, 0 };
        const GLubyte WHITE[3] = { 255, 255, 255 };
        const GLubyte GRAY[3] = { 128, 128, 128 };
        const GLubyte GRAYLIGHT[3] = { 211, 211, 211 };

        const GLubyte RED[3] = { 64, 0, 0 };
        const GLubyte REDLIGHT[3] = { 255, 0, 0 };
        const GLubyte GREEN[3] = { 0, 128, 0 };
        const GLubyte GREENLIGHT[3] = { 0, 255, 0 };
        const GLubyte BLUE[3] = { 0, 0, 255 };
        const GLubyte ORANGE[3] = { 255, 165, 0 };
    };
};
