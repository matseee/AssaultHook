#include "draw.h"

void opengl::Draw::Line(geometry::Line line, const GLubyte color[3]) {
	glColor3ub(color[0], color[1], color[2]);
	glBegin(GL_LINES);
	glVertex2f(line.a.x, line.a.y);
	glVertex2f(line.b.x, line.b.y);
	glEnd();
}

void opengl::Draw::FilledRect(geometry::Rect rect, const GLubyte color[3]) {
	glColor3ub(color[0], color[1], color[2]);
	glBegin(GL_QUADS);
	glVertex2f(rect.x, rect.y);
	glVertex2f(rect.x + rect.width, rect.y);
	glVertex2f(rect.x + rect.width, rect.y + rect.height);
	glVertex2f(rect.x, rect.y + rect.height);
	glEnd();
}

void opengl::Draw::Outline(geometry::Rect rect, float lineWidth, const GLubyte color[3]) {
	glLineWidth(lineWidth);
	glBegin(GL_LINE_STRIP);
	glColor3ub(color[0], color[1], color[2]);
	glVertex2f(rect.x - 0.5f, rect.y - 0.5f);
	glVertex2f(rect.x + rect.width + 0.5f, rect.y - 0.5f);
	glVertex2f(rect.x + rect.width + 0.5f, rect.y + rect.height + 0.5f);
	glVertex2f(rect.x - 0.5f, rect.y + rect.height + 0.5f);
	glVertex2f(rect.x - 0.5f, rect.y - 0.5f);
	glEnd();
}
