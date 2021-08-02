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
    ResourceManager::LoadTexture("Assets/breakout/paddle.png", true, "paddle");

    // init level
    string path;
    for (std::string LevelFile : LevelFiles)
    {
        GameLevel Level = GameLevel();
        getProjectFilePath(LevelFile.c_str(), path);
        Level.Load(path.c_str(), WIDTH, HEIGHT / 2);
        Levels.push_back(Level);
    }
    CurrentLevel = 0;

    // init player paddle
    glm::vec2 playerPos = glm::vec2(
        WIDTH / 2.0f - PlayerSize.x / 2.0f,
        HEIGHT - PlayerSize.y
    );
    Player = new GameObject(playerPos, PlayerSize, ResourceManager::GetTexture("paddle"));

    // init player ball
    glm::vec2 ballPos = playerPos + glm::vec2(PlayerSize.x / 2.0f - BallRadius, - BallRadius * 2.0f);
    Ball = new BallObject(ballPos, BallRadius, BallVelocity, ResourceManager::GetTexture("face"));

}

void Game::ReloadLevels()
{
    for (GameLevel &Level : Levels)
    {
        Level.Reload(WIDTH, HEIGHT / 2);
    }
    
}

void Game::ProcessInput(float dt)
{
    if (State == GAME_ACTIVE)
    {
        float velocity = PlayerVelocity * dt;
        if (Keys[GLFW_KEY_A])
        {
            if (Player->pos.x >= 0.0f)
            {
                Player->pos.x -= velocity;
                if (Ball->Stuck)
                {
                    Ball->pos.x -= velocity;
                }
            }
        }
        if (Keys[GLFW_KEY_D])
        {
            if (Player->pos.x <= WIDTH - Player->size.x)
            {
                Player->pos.x += velocity;
                if (Ball->Stuck)
                {
                    Ball->pos.x += velocity;
                }
            }
        }
        if (Keys[GLFW_KEY_SPACE])
        {
            Ball->Stuck = false;
        }
    }
}
void Game::Update(float dt)
{
    Ball->Move(dt, WIDTH);
    DoCollisions();
    if (Ball->pos.y >= HEIGHT)
    {
        ResetLevel();
        ResetPlayer();
    }
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
        Player->Draw(*Renderer);
        Ball->Draw(*Renderer);
    }
}

void Game::FramebufferCallback(GLFWwindow *window, int previousWidth, int previousHeight)
{
    ReloadLevels();
    float playerWidthRatio = Player->pos.x / (float)previousWidth;
    int currentPosWidth = (int)(playerWidthRatio * WIDTH);
    Player->pos.x = currentPosWidth;
    Player->pos.y = HEIGHT - PlayerSize.y;
    Ball->pos = Player->pos + glm::vec2(PlayerSize.x / 2.0f - BallRadius, - BallRadius * 2.0f);
}

void Game::KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    Keys[key] = action == GLFW_PRESS || action == GLFW_REPEAT;
}

void Game::DoCollisions()
{
    for (GameObject  &Brick : Levels[CurrentLevel].Bricks)
    {
        if (!Brick.bDestroyed)
        {
            Collision collision = CheckCollision(*Ball, Brick);
            if (std::get<0>(collision))
            {
                if (!Brick.IsSolid)
                {
                    Brick.bDestroyed = true;
                }
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (dir == LEFT || dir == RIGHT)
                {
                    Ball->velocity.x = - Ball->velocity.x;
                    float penetration = Ball->Radius - std::abs(diff_vector.x);
                    if (dir == LEFT)
                    {
                        Ball->pos.x += penetration;
                    }
                    else
                    {
                        Ball->pos.x -= penetration;
                    }
                }
                else
                {
                    Ball->velocity.y = - Ball->velocity.y;
                    float penetration = Ball->Radius - std::abs(diff_vector.y);
                    if (dir == UP)
                    {
                        Ball->pos.y -= penetration;
                    }
                    else
                    {
                        Ball->pos.y += penetration;
                    }
                    
                }
            }
        }
    }
    Collision collision = CheckCollision(*Ball, *Player);
    if (!Ball->Stuck && std::get<0>(collision))
    {
        float centerBoard = Player->pos.x + Player->size.x / 2.0f;
        float distance = (Ball->pos.x + Ball->Radius) - centerBoard;
        float percentage = distance / (Player->size.x / 2.0f);

        float strength = 2.0f;
        glm::vec2 oldVelocity = Ball->velocity;
        Ball->velocity.x = BallVelocity.x * percentage * strength;
        Ball->velocity.y = -1.0f * std::abs(Ball->velocity.y);
        Ball->velocity = glm::normalize(Ball->velocity) * glm::length(oldVelocity);
    }
}

Collision Game::CheckCollision(BallObject &Ball, GameObject &Obj)
{
    // get center point circle first 
    glm::vec2 center(Ball.pos + Ball.Radius);
    // calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents(Obj.size.x / 2.0f, Obj.size.y / 2.0f);
    glm::vec2 aabb_center(
        Obj.pos.x + aabb_half_extents.x, 
        Obj.pos.y + aabb_half_extents.y
    );
    // get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // add clamped value to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;
    // retrieve vector between center circle and closest point AABB and check if length <= radius
    difference = closest - center;
    if (glm::length(difference) <= Ball.Radius)
    {

        return std::make_tuple(true, VectorDirection(difference), difference);
    }
    else
    {
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
    }
}

Direction Game::VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f), // up
        glm::vec2(1.0f, 0.0f), // right
        glm::vec2(0.0f, -1.0f), // down
        glm::vec2(-1.0f, 0.0f), // left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4 ;++i)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

void Game::ResetLevel()
{
    for(GameObject & Brick : Levels[CurrentLevel].Bricks)
    {
        Brick.bDestroyed = false;
    }
}
void Game::ResetPlayer()
{
    Ball->Stuck = true;
    Ball->pos = Player->pos + glm::vec2(PlayerSize.x / 2.0f - BallRadius, - BallRadius * 2.0f);
}