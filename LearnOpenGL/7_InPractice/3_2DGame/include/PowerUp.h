#ifndef _POWERUP_H_
#define _POWERUP_H_

#include <GameObject.h>
#include <Texture.h>


class PowerUp : public GameObject 
{
public:
    // powerup state
    std::string Type;
    float       Duration;	
    bool        Activated;
    glm::vec2   Velocity;
    // constructor
    PowerUp(std::string type, glm::vec3 color, float duration, 
            glm::vec2 position, std::shared_ptr<Texture2D> texture) 
        : GameObject(position, glm::vec2(60.0f, 20.0f), texture, color), 
          Type(type), Duration(duration), Activated(false), Velocity(glm::vec2(0.0f, 150.0f))
    { }
};  

#endif