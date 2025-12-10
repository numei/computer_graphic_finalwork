#ifndef PLAYER_HPP
#define PLAYER_HPP
#include <glm/glm.hpp>

class Player
{
public:
    glm::vec3 pos;
    glm::vec3 color;
    Player();
    void Update(float dt, const bool keys[1024], const glm::vec3 &cameraFront, const glm::vec3 &cameraUp);
};
#endif
