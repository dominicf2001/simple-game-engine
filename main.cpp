#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <FreeImage.h>
#include "./utility.hpp"
#include "./entity.hpp"
#include <iostream>
#include <memory>
#include <string>
#include "./include/IrrKlang/irrKlang.h"
#include "examples/ButtonExample/include/GL/freeglut_std.h"
#include "gui.hpp"

using namespace irrklang;
using std::string, std::make_shared;

ISoundEngine* soundEngine;

// state
// ----
bool gameWon = false;
bool gameLost = false;

int timeLeft = 100;

int jumpTimer = 0;
bool playerIsJumping = false;

DisplayState displayState;

Camera camera;
float cameraX = 0, cameraY = 0;

int score = 0;

bool gameStarted = false;
// ----


// main entities
// ----
shared_ptr<GameEntity> player;
shared_ptr<GameEntity> ground;
shared_ptr<GameEntity> end;
// ----

// gui
// ----
Panel topPanel(100, Color::COOL_GRAY, Orientation::TOP, {
    Button([](Button& btn){
        std::cout << "Button 1 pressed\n";
    }), 
    Button([](Button& btn){
        std::cout << "Button 2 pressed\n";
    })
});
Panel bottomPanel(115, Color::RED, Orientation::BOTTOM, {});
Panel leftPanel(200, Color::BLUE, Orientation::LEFT, {});
Panel rightPanel(200, Color::GREEN, Orientation::RIGHT, {});
// ----

EntityContainer obstacles;
EntityContainer collectibles;

void tryToCollect(){
    int collectibleCollideIndex = collectibles.findCollision(player);
    if (collectibleCollideIndex != -1) {
        ++score;
        collectibles.remove(collectibleCollideIndex);
        if (!soundEngine->play2D("./audio/crunch.mp3", false, false, true)) {
            std::cerr << "Failed to play sound: \n";
            exit(1);
        }
    }
}

void applyGravity() {    
    player->y -= 1;    
    if (obstacles.findCollision(player) != -1) {
        player->y += 1;
    }
}

void tryToWin(){
    if (GameEntity::CheckCollision(end, player)) {
        gameWon = true;
        displayState.isPaused = true;
        if (!soundEngine->play2D("./audio/win.mp3", false, false, true)) {
            std::cerr << "Failed to play sound: \n";
            exit(1);
        }
    }
}

namespace keys {
    void special(int key, int x, int y) {
        if (displayState.isPaused){
            return;
        }

        switch (key){
            case GLUT_KEY_LEFT: {        
                player->x -= player->speed;
                                
                if (obstacles.findCollision(player) != -1) {
                    player->x += player->speed;
                }               
                break;
            }
            case GLUT_KEY_RIGHT: {
                player->x += player->speed;

                if (obstacles.findCollision(player) != -1) {
                    player->x -= player->speed;
                }
                break;
            }
        }
        glutPostRedisplay();
    }

    void mouse(int button, int state, int x, int y) {
        if (state == GLUT_DOWN){            
            switch (button){
            case GLUT_LEFT_BUTTON:
                {
                    const int btnIndex = topPanel.findCollision(x, y);
                    std::cout << "btn index: " << btnIndex << '\n';
                    if (btnIndex != -1){
                        topPanel.buttons[btnIndex].click();
                    }
                }
                break;
            }
        }
    }

    void normal(unsigned char key, int x, int y) {
        switch (key){
        case 'a':
            displayState.axis = !displayState.axis;
            break;
        case 'c':
            displayState.clear = true;
            break;
        case 'g':
            if (!gameStarted){
                gameStarted = true;
                displayState.isPaused = false;
            }
            break;
        case 'm':
            displayState.clear = false;
            break;
        case 'p':
            soundEngine->setAllSoundsPaused(!displayState.isPaused);
            displayState.isPaused = !displayState.isPaused;
            break;
        case 32: // space
            if (!soundEngine->play2D("./audio/jump.mp3", false, false, true)) {
                std::cerr << "Failed to play sound: \n";
                exit(1);
            }
            player->y -= 1;    
            if (obstacles.findCollision(player) != -1){
                jumpTimer = 5;
                playerIsJumping = true;
            }
            player->y += 1;
            break;
        case 27: // esc
            exit(1);
            break;
        }
        glutPostRedisplay();
    }
}

void timer(int v) {
    if (displayState.isPaused){
	      glutTimerFunc(100, timer, v);
        return;
    }
    
    FrameState::instance().nextFrame();

    if (playerIsJumping){
        jumpTimer -= 1;
        player->y += 1;

        if (jumpTimer <= 0 || obstacles.findCollision(player) != -1) {
            jumpTimer = 0;
            playerIsJumping = false;
        }
    }

    tryToCollect();
    tryToWin();

    --timeLeft;
    if (player->y < -12 || timeLeft <= 0){
        gameLost = true;
        displayState.isPaused = true;
        if (!soundEngine->play2D("./audio/death.mp3", false, false, true)) {
            std::cerr << "Failed to play sound: \n";
            exit(1);
        }
    }

    // camera.lookAt(player->x, player->y);
	  glutPostRedisplay();
	  glutTimerFunc(100, timer, v); // Creates a frame delay that is counted in miliseconds
}

void display() {    
    // draw main panel
    glPushMatrix();
	  glViewport(SIDE_PANEL_W, BOTTOM_PANEL_H, MAIN_PANEL_W, MIDDLE_PANEL_H);
	  glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // glMatrixMode(GL_MODELVIEW);
    // glLoadIdentity();
    // gluLookAt(cameraX, cameraY, 0.1,
    // 	cameraX, cameraY, 0.0f, 
    // 	0.0f, 1.0f, 0.0f);
    
    // draw sprite
    player->draw();

    // clear screen
    if (displayState.clear){
        glFlush();
        return;
    }

    // draw axis
    if (displayState.axis) {
         drawAxis(camera.x, camera.y);   
    }

    if (!playerIsJumping && !displayState.isPaused) {
        applyGravity();
    }

    // draw obstacles
    obstacles.drawAll();

    // draw end
    end->draw();

    // draw score
    drawText(camera.x + 5, camera.y + 8, "Score: " + std::to_string(score), Color::GREEN);

    // draw time left
    drawText(camera.x - 8, camera.y + 8, "Time left: " + std::to_string(timeLeft), Color::GREEN);

    // draw collectibles
    collectibles.drawAll();

    if (!gameStarted){
        drawText(camera.x, camera.y + 3, "Press g to start", Color::GREEN);
    }

    if (gameLost){        
        drawText(camera.x, camera.y + 3, "YOU DIED", Color::RED);
    }
    
    if (gameWon){
        drawText(camera.x, camera.y + 3, "YOU WIN!", Color::LIGHT_BLUE);
    }

    if (displayState.isPaused && gameStarted && !gameWon && !gameLost){
        drawText(camera.x, camera.y, "Paused", Color::GREEN);
    }

	  glPopMatrix();
    // bottomPanel.draw();
    // leftPanel.draw();
    // rightPanel.draw();
    topPanel.draw();

	  glutSwapBuffers();
    glFlush();
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); // RGB mode with double buffering
    
    // init window
    glutInitWindowPosition(WIN_W, WIN_H);
    glutCreateWindow("Dominic Ferrando 811075176");    
    
	  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // clear the window screen and change the background color
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	  glOrtho(-8, 8, -8, 8, -10, 10);

    // init sound
    soundEngine = createIrrKlangDevice();
    ISoundSource* soundSource = soundEngine->addSoundSourceFromFile("./audio/ocean-man.mp3");
    soundSource->setDefaultVolume(0.4f);
    
    if (!soundEngine->play2D(soundSource, true, false, true)) {
         std::cerr << "Failed to play sound: \n";
         exit(1);
    }

    // register key events
    glutKeyboardFunc(keys::normal);
    glutMouseFunc(keys::mouse);
    glutSpecialFunc(keys::special);

    // create player
    player = createPlayer();

    // create ground
    ground = make_shared<Rectangle>(-8, -8, 30, 1, vector<string>({"sprite/ground.png"}));
    obstacles.add(ground);

    // create platforms
    obstacles.add(make_shared<Rectangle>(-1, -4, 3, 1, vector<string>({"sprite/platform.png"})));
    obstacles.add(make_shared<Rectangle>(7, -3, 5, 1, vector<string>({"sprite/platform.png"})));

    // create end
    end = make_shared<Rectangle>(20, -6, 1, 1, vector<string>({"sprite/flag.png"}));
    
    // create collectibles
    collectibles.add(createCollectible(8, 2));
    collectibles.add(createCollectible(-6, -5));

	  glutTimerFunc(0, timer, 0);
    glutDisplayFunc(display);
    glutMainLoop();
}

