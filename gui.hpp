#pragma once

#include "utility.hpp"
#include <GL/gl.h>
#include <vector>
#include <initializer_list>

using std::vector;

class Panel;
class Button {  
public:
  friend class Panel;
  Button(void (*onClick)(Button& thisBtn)) : 
      x(0), y(0), sizeX(.005), sizeY(.005), clicked(false), onClick(onClick), panel(nullptr) {
  };
  void click();
  void draw();
  void move(GLfloat x, GLfloat y) { this->x = x; this->y = y; };
  bool checkCollision(GLfloat x, GLfloat y);

  Panel* panel;
  GLfloat x, y;
  GLfloat sizeX, sizeY;
  bool clicked;
private:
  void (*onClick)(Button& thisBtn);
};

class Panel {
public:
  friend class Button;
  Panel(GLfloat thickness, Color color, Orientation orientation, std::initializer_list<Button> buttons);
  void draw();
  int findCollision(GLfloat x, GLfloat y);

  vector<Button> buttons;
private:
  GLfloat thickness;
  Color color;
  Orientation orientation;
};
