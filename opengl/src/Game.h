#ifndef GAME_HPP
#define GAME_HPP
#include <vector>
#include <glm/glm.hpp>
#include "Player.h"

struct Falling
{
    glm::vec3 pos;
    glm::vec3 vel;
    glm::vec3 color;
    bool alive;
};

class Game
{
public:
    Player player;
    std::vector<Falling> falling;
    float spawnTimer;
    bool playerDead;
    Game();
    void Reset();
    void Update(float dt, const bool keys[1024], const glm::vec3 &cameraFront, const glm::vec3 &cameraUp);
    void Render(unsigned int shader3D);
};
#endif
