#ifndef _GAMELEVEL_H_
#define _GAMELEVEL_H_

#include <vector>
#include <GameObject.h>
#include <SpriteRenderer.h>
#include <ResourceManager.h>

class GameLevel
{
public:
    // level state
    std::vector<GameObject> Bricks;
    // constructor
    GameLevel();
    // loads level from file
    void Load(const char *file, unsigned int levelWidth, unsigned int levelHeight);
    void Reload(unsigned int levelWidth, unsigned int levelHeight);
    // render level
    void Draw(SpriteRenderer &renderer);
    // check if the level is completed (all non-solid tiles are destroyed)
    bool IsCompleted();

    unsigned int ColsCount, RowsCount;
private:
    // initialize level from tile data
    void init(std::vector<std::vector<unsigned int>> tileData, 
              unsigned int levelWidth, unsigned int levelHeight);
};

#endif