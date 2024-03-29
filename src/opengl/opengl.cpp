#include "opengl.h"

void opengl::SetupOrtho() {
    GLint viewport[4];
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, viewport[2], viewport[3]);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, viewport[2], viewport[3], 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
}

void opengl::RestoreGL() {
    glPopMatrix();
    glPopAttrib();
}
