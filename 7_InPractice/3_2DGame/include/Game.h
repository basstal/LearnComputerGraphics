#ifndef _GAME_H_
#define _GAME_H_

#include <SpriteRenderer.h>
#include <ResourceManager.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <GameLevel.h>

enum GameState
{
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game
{
public:
    // game state
    std::vector<GameLevel>    Levels;
    std::vector<std::string> LevelFiles;
    unsigned int CurrentLevel;
    GameState    State;	
    bool         Keys[1024];
    // unsigned int Width, Height;
    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render(GLFWwindow * window);
    void FramebufferCallback(GLFWwindow *window);
    void LoadLevels();
protected:
    SpriteRenderer *Renderer;
};

#endif