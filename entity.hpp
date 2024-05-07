#pragma once

#include <GL/gl.h>
#include <GL/glut.h>
#include <memory>
#include <vector>
#include "./utility.hpp"
#include <string>

using std::vector, std::shared_ptr, std::string;

class GameEntity {
public:
	GLfloat x, y, sizeX, sizeY;
  GLfloat speed;
	GLfloat mass;
  Color   color;
	bool    isSolid;
	bool    gravity;

  GameEntity(GLfloat x=0, GLfloat y=0, GLfloat sizeX=0, GLfloat sizeY=0, const vector<string>& textures = {}, GLfloat speed=0, Color color=Color::BLUE, GLfloat mass=0, bool isSolid=true, bool gravity=true) 
    : x(x), y(y), sizeX(sizeX), sizeY(sizeY), speed(speed), mass(mass), color(color), isSolid(isSolid), gravity(gravity), textureController(textures) {};

  virtual void draw() = 0;
  void move(GLfloat, GLfloat);
  void initTextures(vector<string>);

  static bool CheckCollision(shared_ptr<GameEntity>, shared_ptr<GameEntity>);
protected:
  TextureController textureController;
};

class Rectangle : public GameEntity {
public:
  using GameEntity::GameEntity;
  void draw() override;
};

class EntityContainer {
public:
  void drawAll();
  void add(shared_ptr<GameEntity>);
  void remove(int);
  int  findCollision(shared_ptr<GameEntity>);

  vector<shared_ptr<GameEntity>> entities;
};

shared_ptr<GameEntity> createPlayer(GLfloat x, GLfloat y, GLfloat sizeX=50, GLfloat sizeY=50);
shared_ptr<GameEntity> createCollectible(GLfloat x, GLfloat y, GLfloat sizeX=30, GLfloat sizeY=30);
