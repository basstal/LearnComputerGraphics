#include <Game.h>



#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Utils.h>

using namespace std;


extern int WIDTH, HEIGHT;

Game::Game(unsigned int width, unsigned int height)
{
    LevelFiles = std::vector<std::string>{
        "Assets/breakout/level0.txt",
        "Assets/breakout/level1.txt",
        "Assets/breakout/level2.txt",
        "Assets/breakout/level3.txt",
    };
    CurrentLevel = 0;
    State = GameState::GAME_ACTIVE;
}

Game::~Game()
{

}

void Game::Init()
{
    std::shared_ptr<Shader> shader = ResourceManager::LoadShader("Shaders/7_3/sprite.vert", "Shaders/7_3/sprite.frag", nullptr, string("SpriteShader"));
    shader->use();
    

    Renderer = new SpriteRenderer(shader);
    ResourceManager::LoadTexture("Assets/awesomeface.png", true, "face");
    ResourceManager::LoadTexture("Assets/breakout/background.jpg", true, "background");

    LoadLevels();
}

void Game::LoadLevels()
{
    string path;
    for (std::string LevelFile : LevelFiles)
    {
        GameLevel Level = GameLevel();
        getProjectFilePath(LevelFile.c_str(), path);
        Level.Load(path.c_str(), WIDTH, HEIGHT / 2);
        Levels.push_back(Level);
    }
    CurrentLevel = 0;
}

void Game::ProcessInput(float dt)
{

}
void Game::Update(float dt)
{
    
}
void Game::Render(GLFWwindow * window)
{
    // Renderer->DrawSprite(ResourceManager::GetTexture("face"), glm::vec2(200.0f, 200.0f), glm::vec2(300.0f, 400.0f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    if (State == GAME_ACTIVE)
    {
        glm::mat4 projection = glm::ortho(0.0f, (float)WIDTH, (float)HEIGHT, 0.0f, -1.0f, 1.0f);
        Renderer->shader->setMat4("projection", projection);
        Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0, 0), glm::vec2(WIDTH, HEIGHT));
        Levels[CurrentLevel].Draw(*Renderer);
    }
}

void Game::FramebufferCallback(GLFWwindow *window)
{
    LoadLevels();
}
