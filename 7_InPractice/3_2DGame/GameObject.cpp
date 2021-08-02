#include <GameObject.h>


GameObject::GameObject(glm::vec2 inPos, glm::vec2 inSize, std::shared_ptr<Texture2D> inTex, glm::vec3 inColor)
{
    pos = inPos;
    size = inSize;
    texture = inTex;
    color = inColor;
}
