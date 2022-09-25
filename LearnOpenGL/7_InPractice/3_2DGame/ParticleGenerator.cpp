#include <ParticleGenerator.h>

#include <BallObject.h>
#include <ResourceManager.h>

using namespace std;

ParticleGenerator::ParticleGenerator(std::shared_ptr<Shader> inShader, std::shared_ptr<Texture2D> inTex, unsigned int in_nr_particles)
{
    shader = inShader;
    texture = inTex;
    nr_particles = in_nr_particles;
    // init Particles
    for (unsigned int i = 0; i < nr_particles; ++i)
    {
        Particles.push_back(Particle());
    }
    glGenVertexArrays(1, &particleVAO);
    glBindVertexArray(particleVAO);
    unsigned int VBO;
    float vertices[] = { 
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 
    
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}



unsigned int ParticleGenerator::FirstUnusedParticle()
{
    for (unsigned int i = lastUsedParticle; i < nr_particles; ++i)
    {
        if (Particles[i].Life <= 0.0f)
        {
            lastUsedParticle = i;
            return i;
        }
    }
    for (unsigned int i = 0; i < lastUsedParticle; ++i)
    {
        if (Particles[i].Life <= 0.0f)
        {
            lastUsedParticle = i;
            return i;
        }
    }
    // override first particle if all others are alive
    lastUsedParticle = 0;
    return 0;
}

void ParticleGenerator::RespawnParticle(Particle & particle, GameObject * object, glm::vec2 offset)
{
    float random = ((rand() % 100) - 50) / 10.0f;
    float rColor = 0.7 + ((rand() % 100) / 100.0f);
    particle.Position = object->pos + random + offset;
    particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.Life = 1.5f;
    if (BallObject * ball = dynamic_cast<BallObject *>(object))
    {
        particle.Velocity = ball->velocity * 0.1f;
    }
    else
    {
        particle.Velocity = glm::vec2(500.0f) * 0.1f;
    }
    
}

void ParticleGenerator::Update(float dt, GameObject * Obj, unsigned int nr_new_particles, glm::vec2 offset)
{
    for (unsigned int i = 0 ; i < nr_new_particles; ++i)
    {
        int unusedParticle = FirstUnusedParticle();
        RespawnParticle(Particles[unusedParticle], Obj, offset);
    }
    // update all particles
    for (unsigned int i = 0; i < nr_particles; ++i)
    {
        Particle &p = Particles[i];
        p.Life -= dt;
        if (p.Life > 0.0f)
        {
            p.Position -= p.Velocity * dt;
            p.Color.a -= dt * 1.5f;
        }

    }
}

void ParticleGenerator::Draw(glm::mat4& projection)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    shader->use();
    shader->setMat4("projection", projection);
    for (Particle & particle : Particles)
    {
        if (particle.Life > 0.0f)
        {
            shader->setVec2("offset", particle.Position);
            shader->setVec4("color", particle.Color);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture->TextureID);
            glBindVertexArray(particleVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}