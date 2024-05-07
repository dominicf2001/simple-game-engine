#include <GL/freeglut_std.h>
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

int timeLeft = 10000;

int jumpTimer = 0;
bool playerIsJumping = false;

DisplayState displayState;

Camera camera;

int score = 0;

bool gameStarted = false;

bool filePanelOpen = false;
bool helpPanelOpen = false;
// ----


// main entities
// ----
shared_ptr<GameEntity> player;
shared_ptr<GameEntity> ground;
shared_ptr<GameEntity> end;
// ----

// gui
// ----
Panel topPanel(75, Color::COOL_GRAY, Orientation::TOP, {
    Button([](Button& btn){
        std::cout << "File button pressed\n";
        filePanelOpen = !filePanelOpen;
        glutPostRedisplay();
    }, "File"), 
    Button([](Button& btn){
        std::cout << "Help button pressed\n";
        helpPanelOpen = !helpPanelOpen;
        glutPostRedisplay();
    }, "Help")
});
Panel bottomPanel(115, Color::RED, Orientation::BOTTOM, {});
Panel leftPanel(115, Color::BLUE, Orientation::LEFT, {});
Panel rightPanel(200, Color::GREEN, Orientation::RIGHT, {});

// file button
Button quitButton([](Button& btn){
    std::cout << "Quit button pressed\n";
    exit(0);
}, "Quit");

// help buttons
Button controlsButton([](Button& btn){
    std::cout << "Right and Left arrows to move.\nSpace to jump\ng to exit/enter editor mode\n";
}, "Controls");

Button aboutButton([](Button& btn){
    std::cout << "This is a simple game engine\n";
}, "About");

Button contactButton([](Button& btn){
    std::cout << "Boognish\n";
}, "Contact");

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
    player->y -= 20;    
    if (obstacles.findCollision(player) != -1) {
        player->y += 20;
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
        if (gameStarted && displayState.isPaused){
            return;
        }

        switch (key){
            case GLUT_KEY_LEFT: {       
                if (gameStarted){
                    player->x -= player->speed;
                                
                    if (obstacles.findCollision(player) != -1) {
                        player->x += player->speed;
                    }               
                }
                else {
                    camera.x -= 50;
                }
                break;
            }
            case GLUT_KEY_RIGHT: {
                if (gameStarted){
                    player->x += player->speed;

                    if (obstacles.findCollision(player) != -1) {
                        player->x -= player->speed;
                    }
                }
                else {
                    camera.x += 50;
                }
                break;
            }
            case GLUT_KEY_UP: {
                if (!gameStarted){
                   camera.y += 50; 
                }
                break;
            }
            case GLUT_KEY_DOWN: {
                if (!gameStarted){
                   camera.y -= 50; 
                }
                break;
            }
        }
        glutPostRedisplay();
    }

    void mouse(int button, int state, int x, int y) {
        GLfloat worldX = x;
        GLfloat worldY = WIN_H - y;
        
        if (state == GLUT_DOWN){            
            switch (button){
            case GLUT_LEFT_BUTTON:
                {
                    int btnIndex = -1;

                    btnIndex = topPanel.findCollision(worldX, worldY);
                    if (btnIndex != -1){
                        topPanel.buttons[btnIndex].click();
                    }

                    btnIndex = leftPanel.findCollision(worldX, worldY);
                    if (btnIndex != -1){
                        leftPanel.buttons[btnIndex].click();
                    }
                    else {
                        if (filePanelOpen && quitButton.checkCollision(worldX, worldY)){
                            quitButton.click();
                        };
                        if (helpPanelOpen && controlsButton.checkCollision(worldX, worldY)){
                            controlsButton.click();
                        };
                        if (helpPanelOpen && aboutButton.checkCollision(worldX, worldY)){
                            aboutButton.click();
                        };
                        if (helpPanelOpen && contactButton.checkCollision(worldX, worldY)){
                            contactButton.click();
                        };
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
            if (gameStarted){
                gameStarted = false;
                soundEngine->setAllSoundsPaused(!displayState.isPaused);
                displayState.isPaused = true;
            }
            else {
                gameStarted = true;
                soundEngine->setAllSoundsPaused(!displayState.isPaused);
                displayState.isPaused = false;
            }
            break;
        case 'm':
            displayState.clear = false;
            break;
        case 'p':
            if (gameStarted){
                soundEngine->setAllSoundsPaused(!displayState.isPaused);
                displayState.isPaused = !displayState.isPaused;
            }
            break;
        case 32: // space
            if (!soundEngine->play2D("./audio/jump.mp3", false, false, true)) {
                std::cerr << "Failed to play sound: \n";
                exit(1);
            }
            player->y -= 20;    
            if (obstacles.findCollision(player) != -1){
                jumpTimer = 6;
                playerIsJumping = true;
            }
            player->y += 20;
            break;
        case 27: // esc
            exit(0);
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
        player->y += 40;

        if (jumpTimer <= 0 || obstacles.findCollision(player) != -1) {
            player->y -= 40;
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

	  glutPostRedisplay();
	  glutTimerFunc(100, timer, v); // Creates a frame delay that is counted in miliseconds
}

void display() {
    // draw main panel
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
	  glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    if (gameStarted){
        camera.lookAt(player->x - (WIN_W / 2), player->y - (WIN_H / 2));
    }
    else {
        camera.lookAt(camera.x, camera.y, true);
    }
    
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
    if (gameStarted){
        drawText(camera.x + (leftPanel.thickness * 3), camera.y + (WIN_H - (topPanel.thickness * 3)), "Score: " + std::to_string(score), Color::GREEN);

        // draw time left
        drawText(camera.x + (leftPanel.thickness * 3) + 200, camera.y + (WIN_H - (topPanel.thickness * 3)), "Time left: " + std::to_string(timeLeft), Color::GREEN);        
    }

    // draw collectibles
    collectibles.drawAll();

    if (!gameStarted){
        drawText(camera.x + (WIN_W / 2) - 50, camera.y + (WIN_H / 2) - 50, "Press g to start", Color::GREEN);
    }

    if (gameLost){        
        drawText(camera.x + (WIN_W / 2) - 50, camera.y + (WIN_H / 2) - 50, "YOU DIED", Color::RED);
    }
    
    if (gameWon){
        drawText(camera.x + (WIN_W / 2) - 50, camera.y + (WIN_H / 2) - 50, "YOU WIN!", Color::LIGHT_BLUE);
    }

    if (displayState.isPaused && gameStarted && !gameWon && !gameLost){
        drawText(camera.x + (WIN_W / 2) - 50, camera.y + (WIN_H / 2) - 50, "Paused", Color::GREEN);
    }

    glPopMatrix();

    // draw panels
    // bottomPanel.draw();
    // rightPanel.draw();
    leftPanel.draw();
    topPanel.draw();

    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // draw dropdowns
    if (filePanelOpen){
        glPushMatrix();
    	  glTranslatef(125, 1050, 0);
        glScalef(100, 25, 1);
        setColor(Color::GREEN);
        glBegin(GL_POLYGON);
        {
    			glVertex2f(-1.0, -1.0);
    			glVertex2f(1.0, -1.0);
    			glVertex2f(1.0, 1.0);
    			glVertex2f(-1.0, 1.0);
        }
        glEnd();
        glPopMatrix();

        quitButton.sizeX = 70;
        quitButton.sizeY = 20;
        quitButton.move(125, 1050);
        quitButton.draw();
    }
    if (helpPanelOpen){
        glPushMatrix();
    	  glTranslatef(250, 1000, 0);
        glScalef(100, 80, 1);
        setColor(Color::GREEN);
        glBegin(GL_POLYGON);
        {
    			glVertex2f(-1.0, -1.0);
    			glVertex2f(1.0, -1.0);
    			glVertex2f(1.0, 1.0);
    			glVertex2f(-1.0, 1.0);
        }
        glEnd();
        glPopMatrix();

        controlsButton.sizeX = 70;
        controlsButton.sizeY = 20;
        controlsButton.move(235, 1050);
        controlsButton.draw();

        aboutButton.sizeX = 70;
        aboutButton.sizeY = 20;
        aboutButton.move(235, 1000);
        aboutButton.draw();

        contactButton.sizeX = 70;
        contactButton.sizeY = 20;
        contactButton.move(235, 950);
        contactButton.draw();
    }
    glPopMatrix();

	  glutSwapBuffers();
    glFlush();
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); // RGB mode with double buffering
    
    // init window
    glutCreateWindow("Dominic Ferrando 811075176");    
    
	  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, WIN_W, 0.0f, WIN_H, 0.0f, 1.0f);

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
    player = createPlayer(500, 800, 50, 50);

    // create ground
    ground = make_shared<Rectangle>(600, 200, 500, 50, vector<string>({"sprite/ground.png"}));
    obstacles.add(ground);

    // create platforms
    obstacles.add(make_shared<Rectangle>(550, 400, 100, 50, vector<string>{"sprite/platform.png"}));
    // obstacles.add(make_shared<Rectangle>(7, -3, 5, 1, vector<string>{"sprite/platform.png"}));

    // create end
    end = make_shared<Rectangle>(1100, 350, 30, 60, vector<string>({"sprite/flag.png"}));
    
    // create collectibles
    collectibles.add(createCollectible(300, 350));
    collectibles.add(createCollectible(550, 540));

    // init camera
    camera.x = player->x - (WIN_W / 2);
    camera.y = player->y - (WIN_H / 2);

    for (int i = 0; i < obstacles.entities.size(); ++i){
        leftPanel.buttons.push_back(Button([i](Button& btn){
            std::cout << "Selected obstacle " << std::to_string(i) << '\n';
        }, "Obstacle " + std::to_string(i)));
    }
    
	  glutTimerFunc(0, timer, 0);
    glutDisplayFunc(display);
    glutMainLoop();
}

