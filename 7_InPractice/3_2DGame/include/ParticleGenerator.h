#ifndef _PARTICLEGENERATOR_H_
#define _PARTICLEGENERATOR_H_
#include <memory>

#include <Shader.h>
#include <Texture.h>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GameObject.h>


struct Particle {
    glm::vec2 Position, Velocity;
    glm::vec4 Color;
    float     Life;
  
    Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};

class ParticleGenerator
{
protected:
    unsigned int particleVAO;
public:
    std::vector<Particle> Particles;

    std::shared_ptr<Shader> shader;
    std::shared_ptr<Texture2D> texture;
    unsigned int lastUsedParticle = 0;
    unsigned int nr_particles;

public:
    ParticleGenerator(std::shared_ptr<Shader> shader, std::shared_ptr<Texture2D> texture, unsigned int nr_particles);
    void Update(float dt, GameObject * Obj, unsigned int nr_new_particles, glm::vec2 offset);
    void Draw(glm::mat4& projection);

protected:
    unsigned int FirstUnusedParticle();
    void RespawnParticle(Particle & particle, GameObject * object, glm::vec2 offset);
};
#endif