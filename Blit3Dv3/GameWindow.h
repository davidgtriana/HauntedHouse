#pragma once
#include"dago/Dago.h"
#include"SimulationScene.h"


class GameWindow : public dago::GameAdapter {
public:
    dago::SceneManager* sceneManager = new dago::SceneManager();

    GameWindow() {}
    ~GameWindow() {
        if (sceneManager) {
            delete sceneManager;
            sceneManager = NULL;
        }
    }
    void init() {
        // beginning of the game
        sceneManager->set(new SimulationScene());
    }
    
    void update(double deltaTime) { 
        sceneManager->update(deltaTime);
    }
    void draw() { 
        sceneManager->draw();
    }
    void doInput(int key, int scancode, int action, int mods) { 
        sceneManager->doInput(key, scancode, action, mods);
    }
    void doCursor(double x, double y) { 
        sceneManager->doCursor(x, y);
    }
    void doMouseButton(int button, int action, int mods) { 
        sceneManager->doMouseButton(button, action, mods);
    }
};
