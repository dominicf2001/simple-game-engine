#pragma once

#include "utility.hpp"
#include <GL/gl.h>
#include <functional>
#include <vector>
#include <string>
#include <initializer_list>

using std::vector; using::string;

class Panel;
class Button {  
public:
  friend class Panel;
  Button(std::function<void(Button&)> onClick, string text="") : 
      x(0), y(0), sizeX(20), sizeY(20), 
      clicked(false), onClick(onClick), panel(nullptr),
      text(text) {
  };
  void click();
  void draw();
  void move(GLfloat x, GLfloat y) { this->x = x; this->y = y; };
  bool checkCollision(GLfloat x, GLfloat y);

  string text;
  Panel* panel;
  GLfloat x, y;
  GLfloat sizeX, sizeY;
  bool clicked;
private:
  std::function<void(Button&)> onClick;
};

class Panel {
public:
  friend class Button;
  Panel(GLfloat thickness, Color color, Orientation orientation, std::initializer_list<Button> buttons);
  void draw();
  int findCollision(GLfloat x, GLfloat y);

  vector<Button> buttons;
  const GLfloat thickness;
  const Color color;
  const Orientation orientation;
};
