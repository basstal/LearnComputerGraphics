#include <GameObject.h>


GameObject::GameObject()
{
    bIsPlaceholder = true;
    bDestroyed = false;
    IsSolid = false;
}

GameObject::GameObject(glm::vec2 inPos, glm::vec2 inSize, std::shared_ptr<Texture2D> inTex, glm::vec3 inColor)
{
    pos = inPos;
    size = inSize;
    texture = inTex;
    color = inColor;
    IsSolid = false;
    bDestroyed = false;
    bIsPlaceholder = false;
}

void GameObject::Draw(SpriteRenderer & Renderer)
{
    Renderer.DrawSprite(texture, pos, size, 0.0f, color);
}


std::ostream& operator << (std::ostream & os, const GameObject & rhs)
{
    os << "pos:" << rhs.pos.x <<", " << rhs.pos.y << "\tsize:" << rhs.size.x << ", " << rhs.size.y;
    return os;
}
