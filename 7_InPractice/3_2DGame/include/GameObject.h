#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <memory>

#include <Texture.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ostream>
#include <SpriteRenderer.h>


class GameObject
{
public:
    glm::vec2 pos;
    glm::vec2 size;
    glm::vec3 color;
    std::shared_ptr<Texture2D> texture;
    bool IsSolid;
    bool bIsPlaceholder;
    bool bDestroyed;

    GameObject();
    GameObject(glm::vec2 inPos, glm::vec2 inSize, std::shared_ptr<Texture2D> inTex, glm::vec3 inColor = glm::vec3(1.0f));
    virtual void Draw(SpriteRenderer & Renderer);
};


std::ostream& operator << (std::ostream & os, const GameObject & rhs);

#endif