#include "entity.hpp"
#include "utility.hpp"
#include <FreeImage.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <memory>
#include <vector>
#include <string>

using std::shared_ptr, std::string;

// -----------
// ENTITY
// -----------
void GameEntity::move(GLfloat x, GLfloat y) {
	this->x = x;	
	this->y = y;
}

bool GameEntity::CheckCollision(shared_ptr<GameEntity> one, shared_ptr<GameEntity> two) { 
		GLfloat xOffsetMult = .95;
		GLfloat yOffsetMult = .8;
	
    GLfloat oneLeft = one->x - (one->sizeX * xOffsetMult);
    GLfloat oneRight = one->x + (one->sizeX * xOffsetMult);
    GLfloat oneTop = one->y + (one->sizeY * yOffsetMult);
    GLfloat oneBottom = one->y - (one->sizeY * yOffsetMult);

    GLfloat twoLeft = two->x - (two->sizeX * xOffsetMult);
    GLfloat twoRight = two->x + (two->sizeX * xOffsetMult);
    GLfloat twoTop = two->y + (two->sizeY * yOffsetMult);
    GLfloat twoBottom = two->y - (two->sizeY * yOffsetMult);

    bool collisionX = (oneLeft <= twoRight) && (oneRight >= twoLeft);
    bool collisionY = (oneBottom <= twoTop) && (oneTop >= twoBottom);
    
    return collisionX && collisionY;
}

// -----------
// RECTANGLE
// -----------

void Rectangle::draw() {
	glPushMatrix();

	glTranslatef(x, y, 0);
  glScalef(sizeX, sizeY, 1);
	if (textureController.hasTexture()){
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureController.getTexture().id);
	
		glBegin(GL_POLYGON);
		{
			glTexCoord2f(0.0, 0.0);
			glVertex2f(-1.0, -1.0);
			glTexCoord2f(1.0, 0.0);
			glVertex2f(1.0, -1.0);
			glTexCoord2f(1.0, 1.0);
			glVertex2f(1.0, 1.0);
			glTexCoord2f(0.0, 1.0);
			glVertex2f(-1.0, 1.0);
		}
		glEnd();

		glDisable(GL_TEXTURE_2D);		
	}
	else {
		setColor(color);
		glBegin(GL_POLYGON);
		{
			glVertex2f(-1.0, -1.0);
			glVertex2f(1.0, -1.0);
			glVertex2f(1.0, 1.0);
			glVertex2f(-1.0, 1.0);
		}
		glEnd();
	}
	glPopMatrix();
}

// -----------
// ENTITY CONTAINER
// -----------

void EntityContainer::add(shared_ptr<GameEntity> entity) {
		entities.push_back(entity);
}

void EntityContainer::remove(int i) {
	if (i >= 0 && i < entities.size()){
			entities.erase(entities.begin() + i);
	}
}

void EntityContainer::drawAll() {
	for (auto entity : entities){
		entity->draw();
	}
}

int EntityContainer::findCollision(shared_ptr<GameEntity> checkedEntity) {
	for (int i = 0; i < entities.size(); ++i){
		if (GameEntity::CheckCollision(entities[i], checkedEntity)){
			return i;
		}
	}
	return -1;
}

// -----------
// PARTICULARS
// -----------

shared_ptr<GameEntity> createPlayer(GLfloat x, GLfloat y, GLfloat sizeX, GLfloat sizeY){
  vector<string> playerSprites({
    "sprite/nyan0.png",
    "sprite/nyan1.png",
    "sprite/nyan2.png",
    "sprite/nyan3.png",
  });

	return std::make_shared<Rectangle>(x, y, sizeX, sizeY, playerSprites, 50);
}

shared_ptr<GameEntity> createCollectible(GLfloat x, GLfloat y, GLfloat sizeX, GLfloat sizeY){
  return std::make_shared<Rectangle>(x, y, sizeX, sizeY, vector<string>({"sprite/cookie.png"}), 1, Color::RED, 0, false);
}
