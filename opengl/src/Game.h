#ifndef GAME_HPP
#define GAME_HPP
#include <vector>
#include <random>
#include <glm/glm.hpp>
#include "Player.h"

struct Falling
{
    glm::vec3 pos;
    glm::vec3 vel;
    glm::vec3 color;
    bool alive;
    float rot;         // current rotation angle (radians)
    glm::vec3 rotAxis; // rotation axis
    float rotSpeed;    // radians per second
};

class Game
{
public:
    Player player;
    std::vector<Falling> falling;
    float spawnTimer;
    bool playerDead;

    std::mt19937 rng;

    Game();
    void Reset();
    void Update(float dt, const bool keys[1024], const glm::vec3 &cameraFront, const glm::vec3 &cameraUp);
    void Render(unsigned int shader3D);

private:
    void SpawnObject();
};
#endif
