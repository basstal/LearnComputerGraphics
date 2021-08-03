#ifndef _GAME_H_
#define _GAME_H_

#include <SpriteRenderer.h>
#include <ResourceManager.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <GameLevel.h>
#include <GameObject.h>
#include <BallObject.h>
#include <tuple>
#include <ParticleGenerator.h>
#include <PostProcessor.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <PowerUp.h>
#include <irrKlang.h>
#include <TextRenderer.h>

enum GameState
{
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

typedef std::tuple<bool, Direction, glm::vec2> Collision;
class Game
{
public:
    PostProcessor * Processor;
    ParticleGenerator * Particle;
    // game state
    std::vector<GameLevel>    Levels;
    std::vector<std::string> LevelFiles;
    unsigned int CurrentLevel;
    GameObject * Player;
    GameState    State;	
    float       PlayerVelocity = 500.0f;
    glm::vec2   PlayerSize = glm::vec2(100.0f, 20.0f);
    glm::vec2   BallVelocity = glm::vec2(100.0f, -350.0f);
    float       BallRadius = 25.f;
    BallObject  * Ball;
    bool         Keys[1024] = {false};
    bool         KeysProcessed[1024] = {false};
    float       ShakeTime;
    int         Probability = 10;
    bool        bLevelChangedDown = false, bLevelChangedUp = false;
    unsigned int Lives;

    std::vector<PowerUp> PowerUps;
    irrklang::ISoundEngine *SoundEngine;
    TextRenderer * Text;
    std::string bleepSoundPath, solidSoundPath, powerupSoundPath, bleep1SoundPath;
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
    void FramebufferCallback(GLFWwindow *window, int previousWidth, int previousHeight);
    void ReloadLevels();
    // **
    // @param[in] key The [keyboard key](@ref keys) that was pressed or released.
    // @param[in] scancode The system-specific scancode of the key.
    // @param[in] action `GLFW_PRESS`, `GLFW_RELEASE` or `GLFW_REPEAT`.  Future
    // releases may add more actions.
    // @param[in] mods Bit field describing which [modifier keys](@ref mods) were
    // held down.
    // **
    void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);

    void DoCollisions();
    void ResetLevel();
    void ResetPlayer();
    void RenderImGui(GLFWwindow *window);

    void SpawnPowerUps(GameObject &block);
    void UpdatePowerUps(float dt);
    void ActivatePowerUp(PowerUp &powerUp);

protected:
    Direction VectorDirection(glm::vec2 target);
    Collision CheckCollision(BallObject &Ball, GameObject &Obj);
    bool CheckCollision(GameObject &lhs, GameObject &rhs);

    SpriteRenderer *Renderer;
};

#endif