#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Player.h"

Player::Player() : pos(0.0f, 0.5f, 0.0f), color(0.1f, 0.6f, 0.4f) {}

void Player::Update(float dt, const bool keys[1024], const glm::vec3 &cameraFront, const glm::vec3 &cameraUp)
{
    float speed = 3.0f * dt;
    if (keys[GLFW_KEY_W])
        pos += speed * cameraFront;
    if (keys[GLFW_KEY_S])
        pos -= speed * cameraFront;
    if (keys[GLFW_KEY_A])
        pos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
    if (keys[GLFW_KEY_D])
        pos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
}
