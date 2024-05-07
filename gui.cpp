#include "gui.hpp"
#include "utility.hpp"
#include <GL/gl.h>
#include <iostream>

void Button::draw() {        
    glPushMatrix();
	  glTranslatef(x, y, 0);
    glScalef(sizeX, sizeY, 1);
    setColor(Color::RED);
    glBegin(GL_POLYGON);
    {
			glVertex2f(-1.0, -1.0);
			glVertex2f(1.0, -1.0);
			glVertex2f(1.0, 1.0);
			glVertex2f(-1.0, 1.0);
    }
    glEnd();
    drawText(-.65, -.5, text, Color::LIGHT_BLUE);
    glPopMatrix();
}

void Button::click() {
    onClick(*this);
}

bool Button::checkCollision(GLfloat mouseX, GLfloat mouseY) {
    // std::cout << "MouseX: " << mouseX << '\n';
    // std::cout << "MouseY: " << mouseY << '\n';

    // std::cout << "ButtonX: " << this->x << '\n';
    // std::cout << "ButtonY: " << this->y << '\n';
    
		GLfloat xOffsetMult = 1;
		GLfloat yOffsetMult = 1;
	
    GLfloat left = x - (sizeX * xOffsetMult);
    GLfloat right = x + (sizeX * xOffsetMult);
    GLfloat top = y + (sizeY * yOffsetMult);
    GLfloat bottom = y - (sizeY * yOffsetMult);

    bool collisionX = (left <= mouseX) && (right >= mouseX);
    bool collisionY = (bottom <= mouseY) && (top >= mouseY);
    
    return collisionX && collisionY;
}

Panel::Panel(GLfloat thickness, Color color, Orientation orientation, std::initializer_list<Button> buttons) :
    thickness(thickness), color(color), orientation(orientation), buttons(buttons) {

    for (auto button : buttons) {
        button.panel = this;
    }
};

void Panel::draw() {
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    GLfloat x(0), y(0), sizeX(0), sizeY(0);
    switch (orientation) {
        case Orientation::BOTTOM:
            x = 0;
            y = 0;
            sizeX = 20;
            sizeY = 20;
            break;
        case Orientation::TOP:
            x = WIN_W / 2;
            y = WIN_H - thickness;
            sizeX = WIN_W;
            sizeY = thickness;
            break;
        case Orientation::LEFT:
            x = thickness;
            y = 0;
            sizeX = thickness;
            sizeY = WIN_H;
            break;
        case Orientation::RIGHT:
            x = 0;
            y = 0;
            sizeX = 20;
            sizeY = 20;
            break;
    }

    // Draw the panel
    glPushMatrix();
	  glTranslatef(x, y, 0);
    glScalef(sizeX, sizeY, 1);
    setColor(color);
    glBegin(GL_POLYGON);
    {
			glVertex2f(-1.0, -1.0);
			glVertex2f(1.0, -1.0);
			glVertex2f(1.0, 1.0);
			glVertex2f(-1.0, 1.0);
    }
    glEnd();
    glPopMatrix();

    int margin = 0;
    if (orientation == Orientation::TOP || orientation == Orientation::BOTTOM){
        for (auto& button : buttons){
            button.sizeX = 40;
            button.sizeY = 20;
            button.move((x - WIN_W / 2.5) + margin, y);

            button.draw();

            margin += WIN_W / 7.5;
        }
    }
    else {
        for (auto& button : buttons){
            button.sizeX = 90;
            button.sizeY = 20;
            button.move(x, ((WIN_H / 1.3) - y - margin));

            button.draw();

            margin += 100;
        }
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
