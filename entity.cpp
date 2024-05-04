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

bool GameEntity::CheckCollision(shared_ptr<GameEntity> one, shared_ptr<GameEntity> two) { // AABB - AABB collision
	bool collisionX = one->x + one->sizeX >= two->x && two->x + two->sizeX >= one->x;
	bool collisionY = one->y + one->sizeY >= two->y && two->y + two->sizeY >= one->y;
	return collisionX && collisionY;
}

// -----------
// RECTANGLE
// -----------

void Rectangle::draw(){
	glPushMatrix();

	glTranslatef(x, y, z);
	if (textureController.hasTexture()){
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureController.getTexture().id);
	
		glBegin(GL_POLYGON);
		{
			glTexCoord2f(0.0, 0.0);
			glVertex3f(-0.5, -0.5, 0);
			glTexCoord2f(1.0, 0.0);
			glVertex3f(0.5 + sizeX, -0.5, 0);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(0.5 + sizeX, 0.5 + sizeY, 0);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(-0.5, 0.5 + sizeY, 0);
		}
		glEnd();

		glDisable(GL_TEXTURE_2D);		
	}
	else {
		glBegin(GL_POLYGON);
		{
			setColor(color);
			glVertex3f(-0.5, -0.5, 0);
			glVertex3f(0.5 + sizeX, -0.5, 0);
			glVertex3f(0.5 + sizeX, 0.5 + sizeY, 0);
			glVertex3f(-0.5, 0.5 + sizeY, 0);
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

shared_ptr<GameEntity> createPlayer(GLfloat x, GLfloat y){
  vector<string> playerSprites({
    "sprite/nyan0.png",
    "sprite/nyan1.png",
    "sprite/nyan2.png",
    "sprite/nyan3.png",
  });

	return std::make_shared<Rectangle>(x, y, 1, 1, playerSprites, 1);
}

shared_ptr<GameEntity> createCollectible(GLfloat x, GLfloat y){
  return std::make_shared<Rectangle>(x, y, .2, .2, vector<string>({"sprite/cookie.png"}), 1, Color::RED, 0, false);
}
