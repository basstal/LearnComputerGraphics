#include <BallObject.h>


glm::vec2 BallObject::Move(float dt, unsigned int window_width)
{
    if (!Stuck)
    {
        pos += velocity * dt;
        if (pos.x <= 0.0f)
        {
            velocity.x = - velocity.x;
            pos.x = 0.0f;
        }
        else if (pos.x + size.x >= window_width)
        {
            velocity.x = -velocity.x;
            pos.x = window_width - size.x;
        }
        if (pos.y <= 0.0f)
        {
            velocity.y = -velocity.y;
            pos.y = 0.0f;
        }
    }
    return pos;
}


void      BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{

}

BallObject::BallObject()
{
    
}

BallObject::BallObject(glm::vec2 inPos, float inRadius, glm::vec2 inVelocity, std::shared_ptr<Texture2D> inSprite)
{
    Stuck = true;
    bDestroyed = false;
    pos = inPos;
    Radius = inRadius;
    velocity = inVelocity;
    texture = inSprite;
    size = glm::vec2(inRadius + 3.0f, inRadius + 3.0f);
    color = glm::vec3(1.0f);
}