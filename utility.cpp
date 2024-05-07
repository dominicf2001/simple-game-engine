#include "./utility.hpp"
#include <FreeImage.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cstdio>
#include <iostream>
#include <stdexcept>

// -----------
// CAMERA
// -----------

void Camera::lookAt(GLfloat x, GLfloat y, bool jump){     
    GLfloat newX, newY = 0; 
    if (jump){
        newX = x;
        newY = y;
    }
    else {
        // smooth the camera movement
        newX = this->x + (x - this->x) * .08f;    
        newY = this->y + (y - this->y) * .08f;
    }

    gluLookAt(newX, newY, 0.1, 
        newX, newY, 0, 
        0, 1, 0);

    this->x = newX;
    this->y = newY;
}

// -----------
// MISC
// -----------

void setColor(Color color){
    switch (color){
    case Color::RED:
        glColor3f(0.8, 0.2, 0.2);
        break;
    case Color::GREEN:
        glColor3f(0.2, 0.8, 0.2);
        break;
    case Color::BLUE:
        glColor3f(0.2, 0.2, 0.8);
        break;
    case Color::CYAN:
        glColor3f(0, 0.8, 0.8);
        break;
    case Color::MAGENTA:
        glColor3f(0.8, 0, 0.8);
        break;
    case Color::COOL_GRAY:
        glColor3f(0.5, 0.6, 0.7);
        break;
    case Color::LIGHT_BLUE:
        glColor3f(0.4, 0.6, 0.8);
        break;
    }
}

void drawText(float x, float y, string string, Color color){  
    setColor(color);
    glRasterPos2f(x, y);
    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (unsigned char*)string.c_str());
}
 
void drawAxis(GLfloat x, GLfloat y){
	  glPushMatrix();
	  glTranslatef(x, y, 0);
    const GLfloat axisVal = 1;

    // y axis
    glBegin(GL_LINES);
    {
        setColor(Color::GREEN);
        glVertex3f(-axisVal, -axisVal, 0);
        glVertex3f(-axisVal, axisVal, 0);   
    }
    glEnd();

    glBegin(GL_POLYGON);
    {
        setColor(Color::GREEN);
        glVertex3f(-axisVal - .3, axisVal, 0);
        glVertex3f(-axisVal + .3, axisVal, 0);
        glVertex3f(-axisVal, axisVal + .4, 0);
    }
    glEnd();

    // x axis
    glBegin(GL_LINES);
    {
        setColor(Color::RED);
        glVertex3f(-axisVal, -axisVal, 0);
        glVertex3f(axisVal, -axisVal, 0); 
    }
    glEnd();

    glBegin(GL_POLYGON);
    {
        setColor(Color::RED);
        glVertex3f(axisVal, -axisVal + .3, 0);
        glVertex3f(axisVal, -axisVal - .3, 0);
        glVertex3f(axisVal + .4, -axisVal, 0);
    }
    glEnd();

	  glPopMatrix();
}

// -----------
// TEXTURE CONTROLLER
// -----------

TextureController::TextureController(const vector<string>& textureFileNames){
  for (auto textureFileName : textureFileNames){
    addTexture(textureFileName);
  } 
}

void TextureController::ensureTexturesLoaded() {
  if (hasTexture() && !texturesLoaded){
	  	loadTextures(textures);
			texturesLoaded = true;
		}
}

Texture TextureController::getTexture() {
    ensureTexturesLoaded();
    if (!hasTexture()){
      throw std::runtime_error("No textures loaded.");
    }

    const int currentFrame = FrameState::instance().getFrame();
    // std::cout << "last frame: " << lastFrame << '\n'; 
    // std::cout << "current frame: " << currentFrame << '\n'; 
    // std::cout << "current texture index: " << currentTextureIndex << '\n'; 
    if (lastFrame != currentFrame){
      lastFrame = currentFrame;
      currentTextureIndex = (currentTextureIndex + 1) % textures.size(); 
    }
    return textures[currentTextureIndex]; 
}

void loadTextures(vector<Texture>& textures) {
  std::cout << "Loading textures...\n";
  GLuint ids[textures.size()];
	glGenTextures(textures.size(), ids); // Get the texture object IDs.

  for (int i = 0; i < textures.size(); ++i) {
    textures[i].id = ids[i]; 
		FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(textures[i].fileName.c_str());
		if (format == FIF_UNKNOWN) {
			printf("Unknown file type for texture image file %s\n", textures[i].fileName.c_str());
			continue;
		}

		FIBITMAP* bitmap = FreeImage_Load(format, textures[i].fileName.c_str(), 0); // Read image from file.
		if (!bitmap) {
			printf("Failed to load image %s\n", textures[i].fileName.c_str());
			continue;
		}

		FIBITMAP* bitmap32 = FreeImage_ConvertTo32Bits(bitmap); // Convert to 32-bit (with alpha channel)
		FreeImage_Unload(bitmap);
		if (bitmap32) {
      std::cout << "Textures[i].id: " << textures[i].id;
 			glBindTexture(GL_TEXTURE_2D, textures[i].id); // Bind texture
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FreeImage_GetWidth(bitmap32), FreeImage_GetHeight(bitmap32), 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(bitmap32));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			FreeImage_Unload(bitmap32);
		}
    std::cout << "Loaded image " << textures[i].fileName << '\n';
  }
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  std::cout << "Finished loading textures...\n";
}
