#include "gui.hpp"
#include "utility.hpp"
#include <GL/gl.h>

void Button::draw() {        
    glScalef(sizeX, sizeY, 1);
    setColor(Color::RED);
    glBegin(GL_QUADS);
    {
        glVertex2f(-0.5, -0.5);
        glVertex2f(0.5, -0.5);
        glVertex2f(0.5, 0.5);
        glVertex2f(-0.5, 0.5);
    }
    glEnd();
}

void Button::click() {
    onClick(*this);
}

bool Button::checkCollision(GLfloat mouseX, GLfloat mouseY) {
		bool insideX = (mouseX >= x && mouseX <= x + sizeX);
		bool insideY = (mouseY >= y && mouseY <= y + sizeY);
		return insideX && insideY;
}

Panel::Panel(GLfloat thickness, Color color, Orientation orientation, std::initializer_list<Button> buttons) :
    thickness(thickness), color(color), orientation(orientation), buttons(buttons) {

    for (auto button : buttons) {
        button.panel = this;
    }
};

void Panel::draw() {
    glPushMatrix();
	  glViewport(0, WIN_H - TOP_PANEL_H, WIN_W, TOP_PANEL_H);

    GLfloat x(0), y(0), sizeX(0), sizeY(0);
    switch (orientation) {
        case Orientation::BOTTOM:
            x = 0;
            y = 0;
            sizeX = 20;
            sizeY = 20;
            break;
        case Orientation::TOP:
            x = 0;
            y = 0;
            sizeX = 20;
            sizeY = 20;
            break;
        case Orientation::LEFT:
            x = 0;
            y = 0;
            sizeX = 20;
            sizeY = 20;
            break;
        case Orientation::RIGHT:
            x = 0;
            y = 0;
            sizeX = 20;
            sizeY = 20;
            break;
    }

    glPushMatrix();
    // Draw the panel
    glScalef(sizeX, sizeY, 1);
	  glTranslatef(x, y, 1);
    setColor(color);
    glBegin(GL_POLYGON);
    {
        glVertex3f(-0.5, -0.5, 0);
        glVertex3f(0.5, -0.5, 0);
        glVertex3f(0.5, 0.5, 0);
        glVertex3f(-0.5, 0.5, 0);
    }
    glEnd();
    glPopMatrix();

    for (auto button : buttons){
        button.sizeX = .005;
        button.sizeY = .1;
        button.move(x, y);
        button.draw();
    }

    glPopMatrix();
}

int Panel::findCollision(GLfloat x, GLfloat y) {
    for (int i = 0; i < buttons.size(); ++i) {
        if (buttons[i].checkCollision(x, y)) {
            return i;
        }
    }
    return -1;
}
