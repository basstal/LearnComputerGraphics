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
    ShakeTime = 0.0f;
}

Game::~Game()
{

}

void Game::Init()
{
    std::shared_ptr<Shader> shader = ResourceManager::LoadShader("Shaders/7_3/Sprite.vert", "Shaders/7_3/Sprite.frag", nullptr, string("SpriteShader"));
    shader->use();
    std::shared_ptr<Shader> particleShader = ResourceManager::LoadShader("Shaders/7_3/Particle.vert", "Shaders/7_3/Particle.frag", nullptr, string("Particle"));
    particleShader->use();
    std::shared_ptr<Shader> postprocessingShader = ResourceManager::LoadShader("Shaders/7_3/PostProcessing.vert", "Shaders/7_3/PostProcessing.frag", nullptr, string("PostProcessing"));
    postprocessingShader->use();

    Renderer = new SpriteRenderer(shader);
    ResourceManager::LoadTexture("Assets/awesomeface.png", true, "face");
    ResourceManager::LoadTexture("Assets/breakout/background.jpg", true, "background");
    ResourceManager::LoadTexture("Assets/breakout/paddle.png", true, "paddle");
    ResourceManager::LoadTexture("Assets/breakout/particle.png", true, "particle");
    ResourceManager::LoadTexture("Assets/breakout/powerup_speed.png", true, "powerup_speed");
    ResourceManager::LoadTexture("Assets/breakout/powerup_sticky.png", true, "powerup_sticky");
    ResourceManager::LoadTexture("Assets/breakout/powerup_passthrough.png", true, "powerup_passthrough");
    ResourceManager::LoadTexture("Assets/breakout/powerup_increase.png", true, "powerup_increase");
    ResourceManager::LoadTexture("Assets/breakout/powerup_confuse.png", true, "powerup_confuse");
    ResourceManager::LoadTexture("Assets/breakout/powerup_chaos.png", true, "powerup_chaos");

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

    std::shared_ptr<Texture2D> particleTex = ResourceManager::GetTexture(string("particle"));
    Particle = new ParticleGenerator(particleShader, particleTex, 500);

    Processor = new PostProcessor(postprocessingShader);
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
    Particle->Update(dt, dynamic_cast<GameObject *>(Ball), 2, glm::vec2(Ball->Radius / 2.0f));
    Ball->Move(dt, WIDTH);
    DoCollisions();
    if (Ball->pos.y >= HEIGHT)
    {
        ResetLevel();
        ResetPlayer();
    }
    if (ShakeTime > 0.0f)
    {
        ShakeTime -= dt;
        if (ShakeTime <= 0.0f)
        {
            Processor->bShake = false;
        }
    }
    UpdatePowerUps(dt);
}
void Game::Render(GLFWwindow * window)
{
    if (State == GAME_ACTIVE)
    {
        Processor->PreDraw();
        Renderer->shader->use();
        glm::mat4 projection = glm::ortho(0.0f, (float)WIDTH, (float)HEIGHT, 0.0f, -1.0f, 1.0f);
        Renderer->shader->setMat4("projection", projection);
        Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0, 0), glm::vec2(WIDTH, HEIGHT));
        Levels[CurrentLevel].Draw(*Renderer);
        Player->Draw(*Renderer);
        for (PowerUp &powerUp : PowerUps)
        {
            if (!powerUp.bDestroyed)
            {
                powerUp.Draw(*Renderer);
            }
        }
        Particle->Draw(projection);
        Ball->Draw(*Renderer);

        Processor->Draw();
    }
}

void Game::RenderImGui(GLFWwindow *window)
{
    ImGui::Begin("Editor");
    {
        ImGui::Separator();
        
        ImGui::Checkbox("PostProcessing Chaos", &Processor->bChaos);
        ImGui::Checkbox("PostProcessing Confuse", &Processor->bConfuse);
        ImGui::Checkbox("PostProcessing Shake", &Processor->bShake);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
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
    Processor->ReloadTex();
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
                    SpawnPowerUps(Brick);
                }
                else
                {
                    ShakeTime = 0.05f;
                    Processor->bShake = true;
                }
                
                // ** 如果遇到非Solid的砖块并且在passthrough的状态，则不改变速度方向
                if (!(Ball->PassThrough && !Brick.IsSolid))
                {
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
    }
    Collision collision = CheckCollision(*Ball, *Player);
    if (!Ball->Stuck && std::get<0>(collision))
    {
        Ball->Stuck = Ball->Sticky;
        float centerBoard = Player->pos.x + Player->size.x / 2.0f;
        float distance = (Ball->pos.x + Ball->Radius) - centerBoard;
        float percentage = distance / (Player->size.x / 2.0f);

        float strength = 2.0f;
        glm::vec2 oldVelocity = Ball->velocity;
        Ball->velocity.x = BallVelocity.x * percentage * strength;
        Ball->velocity.y = -1.0f * std::abs(Ball->velocity.y);
        Ball->velocity = glm::normalize(Ball->velocity) * glm::length(oldVelocity);
    }
    for (PowerUp & powerUp : PowerUps)
    {
        if (!powerUp.bDestroyed)
        {
            if (powerUp.pos.y >= HEIGHT)
            {
                powerUp.bDestroyed = true;
            }
            if (CheckCollision(*Player, powerUp))
            {
                ActivatePowerUp(powerUp);
                powerUp.bDestroyed = true;
                powerUp.Activated = true;
            }
        }
    }
}

bool Game::CheckCollision(GameObject &lhs, GameObject &rhs)
{
    glm::vec2 lhsRightBottom = lhs.pos + lhs.size;
    glm::vec2 rhsRightBottom = rhs.pos + rhs.size;

    // bool collisionX = lhsRightBottom.x >= rhs.pos.x && rhsRightBottom.x >= lhs.pos.x;
    // bool collisionY = lhsRightBottom.y >= rhs.pos.y && rhsRightBottom.y >= lhs.pos.y;
    // return collisionX && collisionY;
    if (lhs.pos.x < rhs.pos.x && rhs.pos.x < lhsRightBottom.x &&
        lhs.pos.y < rhs.pos.y && lhsRightBottom.y > rhs.pos.y)
    {
        return true;
    }
    if (lhs.pos.x < rhsRightBottom.x && rhsRightBottom.x < lhsRightBottom.x &&
        lhs.pos.y < rhsRightBottom.y && lhsRightBottom.y > rhsRightBottom.y)
    {
        return true;
    }
    return false;
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
    Ball->velocity = BallVelocity;
    Ball->pos = Player->pos + glm::vec2(PlayerSize.x / 2.0f - BallRadius, - BallRadius * 2.0f);
    Ball->PassThrough = false;
    Ball->Sticky = false;
    PowerUps.clear();
    Processor->bChaos = false;
    Processor->bConfuse = false;
    Player->color = glm::vec3(1.0f);
    Player->size = PlayerSize;
}

void Game::ActivatePowerUp(PowerUp &powerUp)
{
    if (powerUp.Type == "speed")
    {
        Ball->velocity *= 1.2;
    }
    else if (powerUp.Type == "sticky")
    {
        Ball->Sticky = true;
        Player->color = glm::vec3(1.0f, 0.5f, 1.0f);
    }
    else if (powerUp.Type == "pass-through")
    {
        Ball->PassThrough = true;
        Ball->color = glm::vec3(1.0f, 0.5f, 0.5f);
    }
    else if (powerUp.Type == "pad-size-increase")
    {
        Player->size.x += 50;
    }
    else if (powerUp.Type == "confuse")
    {
        if (!Processor->bChaos)
        {
            Processor->bConfuse = true; // only activate if chaos wasn't already active
        }
    }
    else if (powerUp.Type == "chaos")
    {
        if (!Processor->bConfuse)
        {
            Processor->bChaos = true;
        }
    }
}

void Game::SpawnPowerUps(GameObject &block)
{
    unsigned int random = rand() % Probability;
    switch(random)
    {
        case 0:
            PowerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.pos, ResourceManager::GetTexture("powerup_speed")));
            break;
        case 1:
            PowerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.pos, ResourceManager::GetTexture("powerup_sticky")));
            break;
        case 2:
            PowerUps.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.pos, ResourceManager::GetTexture("powerup_passthrough")));
            break;
        case 3:
            PowerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.pos, ResourceManager::GetTexture("powerup_increase")));
            break;
        case 4:
            PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.pos, ResourceManager::GetTexture("powerup_confuse")));
            break;
        case 5:
            PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.pos, ResourceManager::GetTexture("powerup_chaos")));
            break;
    }
}

bool isOtherPowerUpActive(std::vector<PowerUp> & PowerUps, std::string tag)
{
    for (PowerUp & powerUp : PowerUps)
    {
        if (powerUp.Activated && powerUp.Type == tag)
        {
            return true;
        }
    }
    return false;
}
void Game::UpdatePowerUps(float dt)
{
    for (PowerUp &powerUp : PowerUps)
    {
        powerUp.pos += powerUp.Velocity * dt;
        if (powerUp.Activated)
        {
            powerUp.Duration -= dt;
            if (powerUp.Duration <= 0.0f)
            {
                powerUp.Activated = false;
                // deactivate effects
                if (powerUp.Type == "sticky")
                {
                    if (!isOtherPowerUpActive(PowerUps, "sticky"))
                    {	// only reset if no other PowerUp of type sticky is active
                        Ball->Sticky = false;
                        Player->color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.Type == "pass-through")
                {
                    if (!isOtherPowerUpActive(PowerUps, "pass-through"))
                    {	// only reset if no other PowerUp of type pass-through is active
                        Ball->PassThrough = false;
                        Ball->color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.Type == "confuse")
                {
                    if (!isOtherPowerUpActive(PowerUps, "confuse"))
                    {	// only reset if no other PowerUp of type confuse is active
                        Processor->bConfuse = false;
                    }
                }
                else if (powerUp.Type == "chaos")
                {
                    if (!isOtherPowerUpActive(PowerUps, "chaos"))
                    {	// only reset if no other PowerUp of type chaos is active
                        Processor->bChaos = false;
                    }
                }
            }
        }
    }
    PowerUps.erase(std::remove_if(PowerUps.begin(), PowerUps.end(), [](const PowerUp &powerUp) { return powerUp.bDestroyed && !powerUp.Activated;}), PowerUps.end());
}
