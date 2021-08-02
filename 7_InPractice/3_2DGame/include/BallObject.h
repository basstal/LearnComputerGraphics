#ifndef _BALLOBJECT_H_
#define _BALLOBJECT_H_

#include <GameObject.h>

class BallObject : public GameObject
{
public:
    // ball state	
    float     Radius;
    bool      Stuck;

    glm::vec2 velocity = glm::vec2(500.0f, 500.0f);


    BallObject();
    BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, std::shared_ptr<Texture2D> sprite);

    glm::vec2 Move(float dt, unsigned int window_width);
    void      Reset(glm::vec2 position, glm::vec2 velocity);
};

#endif