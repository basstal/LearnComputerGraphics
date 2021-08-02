#include <GameLevel.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>


GameLevel::GameLevel()
{
    ResourceManager::LoadTexture("Assets/breakout/block.png", true, "block");
    ResourceManager::LoadTexture("Assets/breakout/block_solid.png", true, "block_solid");
    RowsCount = 0;
    ColsCount = 0;
}

void GameLevel::Draw(SpriteRenderer &renderer)
{
    for(const GameObject & Brick : Bricks)
    {
        if (!Brick.bIsPlaceholder && !Brick.bDestroyed)
        {
            renderer.DrawSprite(Brick.texture, Brick.pos, Brick.size, 0.0f, Brick.color);
        }
    }
}

void GameLevel::Reload(unsigned int levelWidth, unsigned int levelHeight)
{
    float unit_width = levelWidth / (float)ColsCount;
    float unit_height = levelHeight / (float)RowsCount;
    unsigned int CurRows = 0, CurCols = 0;
    for (GameObject & Brick : Bricks)
    {
        if (CurCols >= ColsCount)
        {
            CurRows ++;
            CurCols = 0;
        }
        if (!Brick.bIsPlaceholder)
        {
            glm::vec2 pos(unit_width * CurCols, unit_height * CurRows);
            glm::vec2 size(unit_width, unit_height);
            Brick.size = size;
            Brick.pos = pos;
        }
        CurCols++;
    }
}

void GameLevel::Load(const char *file, unsigned int levelWidth, unsigned int levelHeight)
{
    Bricks.clear();

    unsigned int tileCode;
    std::string line;
    std::ifstream fstream(file);
    std::vector<std::vector<unsigned int>> tileData;
    if (fstream)
    {
        while(std::getline(fstream, line))
        {
            std::istringstream sstream(line);
            std::vector<unsigned int> rows;
            while(sstream >> tileCode)
            {
                rows.push_back(tileCode);
            }
            tileData.push_back(rows);
        }
        RowsCount = tileData.size();
        if (!tileData.empty())
        {
            ColsCount = tileData[0].size();
            init(tileData, levelWidth, levelHeight);
        }
    }

}


void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, 
              unsigned int levelWidth, unsigned int levelHeight)
{
    unsigned int height = tileData.size();
    unsigned int width = tileData[0].size();
    float unit_width = levelWidth / (float)width;
    float unit_height = levelHeight / (float)height;

    // 基于tileData初始化tile位置等数据
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            unsigned int tileCode = tileData[y][x];
            glm::vec2 pos(unit_width * x, unit_height * y);
            glm::vec2 size(unit_width, unit_height);
            if (tileCode == 1)  // solid
            {
                GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
                obj.IsSolid = true;
                Bricks.push_back(obj);
            }
            else if (tileCode > 1)
            {
                glm::vec3 color = glm::vec3(1.0f);
                switch(tileCode)
                {
                    case 2:
                        color = glm::vec3(0.2f, 0.6f, 1.0f);
                        break;
                    case 3:
                        color = glm::vec3(0.0f, 0.7f, 1.0f);
                        break;
                    case 4:
                        color = glm::vec3(0.8f, 0.8f, 0.4f);
                        break;
                    case 5:
                        color = glm::vec3(1.0f, 0.5f, 0.0f);
                        break;
                    default:
                        break;
                }
                Bricks.push_back(GameObject(pos, size, ResourceManager::GetTexture("block"), color));
            }
            else
            {
                Bricks.push_back(GameObject());
            }
        }
    }
}