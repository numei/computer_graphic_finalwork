#ifndef PLAYER_HPP
#define PLAYER_HPP
#include <glm/glm.hpp>

class Player
{
public:
    glm::vec3 pos;
    glm::vec3 color;

    float moveSpeed = 5.0f;
    float groundY = 0.5f; // 默认玩家所在高度

public:
    Player() {}
    void Update(float dt, const bool keys[1024],
                const glm::vec3 &cameraFront,
                const glm::vec3 &cameraUp);
};
#endif
