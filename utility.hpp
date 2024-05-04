#pragma once

#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <vector>
#include <string>

#define MAIN_PANEL_W 600 // Width of the main panel
#define SIDE_PANEL_W 200 // Width of each side panel
#define MIDDLE_PANEL_H 600 // Height of middle panels
#define BOTTOM_PANEL_H 200 // Height of the bottom panel
#define TOP_PANEL_H 50 // Height of top panel
#define WIN_W 1000 // Adjusted window width
#define WIN_H 850 // Adjusted window height

using std::vector, std::string;

enum class Color { RED, GREEN, BLUE, CYAN, MAGENTA, COOL_GRAY, LIGHT_BLUE };

enum class Orientation { TOP, BOTTOM, LEFT, RIGHT };

struct DisplayState {
    bool axis = true;
    bool clear = false;
    bool isPaused = true;
    bool gameStarted = false;
};

class Camera {
public:
    void lookAt(GLfloat, GLfloat);
    GLfloat x = 0;
    GLfloat y = 0;
};

class FrameState {
public:
  static FrameState& instance(){
      static FrameState instance;
      return instance;
  }

  void nextFrame(){ frame = (frame + 1) % maxFrame; };
  int getFrame(){ return frame; };

  FrameState(FrameState const&) = delete;
  void operator=(FrameState const&) = delete;
private:
    const int maxFrame = 5;
    int frame = 0;
    FrameState() {};
};

struct Texture {
  GLuint id = 0;
  string fileName = "";
};

class TextureController {
public:
  TextureController(vector<string>& textureFileNames);
  Texture getTexture();
  void addTexture(string textureFileName) { textures.push_back(Texture{.fileName = textureFileName}); };
  bool hasTexture(){ return !textures.empty(); };
private:
    bool texturesLoaded = false;  
    int currentTextureIndex = 0;
    int lastFrame = 0;
    vector<Texture> textures;
    void ensureTexturesLoaded();
};

void loadTextures(vector<Texture>&);

void setColor(Color color);

void drawAxis(GLfloat, GLfloat);

void drawText(float, float, const string, Color); 

