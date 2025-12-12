#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <random>

// File-scope: store the player's fixed Y height so we can force horizontal-only motion
static float s_playerFixedY = 0.5f;

Game::Game()
    : spawnTimer(0.0f), playerDead(false)
{
    // seed RNG with high-resolution clock
    rng.seed((uint32_t)std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

void Game::Reset()
{
    falling.clear();
    spawnTimer = 0.0f;

    // Player stands at 0.5 height
    player.groundY = 0.5f;
    player.pos = glm::vec3(0.0f, player.groundY, 0.0f);

    player.color = glm::vec3(1.0f, 0.8f, 0.1f);
    playerDead = false;
}

static float randf(std::mt19937 &rng, float a, float b)
{
    std::uniform_real_distribution<float> d(a, b);
    return d(rng);
}

void Game::SpawnObject()
{
    Falling f;
    // spawn X,Z in range [-4,4]
    f.pos.x = randf(rng, -4.0f, 4.0f);
    // spawn high above camera so it falls into view
    f.pos.y = randf(rng, 4.0f, 7.0f);
    f.pos.z = randf(rng, -4.0f, 4.0f);

    // give some random horizontal velocity so it can fall at an angle
    float horizontalSpeed = randf(rng, -1.5f, 1.5f);
    float horizontalSpeedZ = randf(rng, -1.0f, 1.0f);
    float downwardSpeed = randf(rng, 1.2f, 3.0f);
    f.vel = glm::vec3(horizontalSpeed, -downwardSpeed, horizontalSpeedZ);

    // random bright-ish color (pick in HSV-like range but here simple)
    float r = randf(rng, 0.6f, 1.0f);
    float g = randf(rng, 0.1f, 0.6f);
    float b = randf(rng, 0.1f, 0.9f);
    f.color = glm::vec3(r, g, b);

    f.alive = true;
    f.rot = randf(rng, 0.0f, 6.2831853f); // random start angle 0..2pi

    // random axis, not zero
    glm::vec3 ax(randf(rng, -1.0f, 1.0f), randf(rng, -1.0f, 1.0f), randf(rng, -1.0f, 1.0f));
    if (glm::length(ax) < 0.001f)
        ax = glm::vec3(0.0f, 1.0f, 0.0f);
    f.rotAxis = glm::normalize(ax);

    f.rotSpeed = randf(rng, 1.0f, 6.0f); // radians/sec

    falling.push_back(f);
}

void Game::Update(float dt, const bool keys[1024], const glm::vec3 &cameraFront, const glm::vec3 &cameraUp)
{
    if (playerDead)
        return;

    const float floorTop = -0.5f;
    player.Update(dt, keys, cameraFront, cameraUp);

    const float floorHalf = 12.0f * 0.5f; // = 6.0f

    // 玩家半径（0.6 缩放的 cube）
    const float playerHalf = 0.3f;

    // 限制 X、Z 位置保持在地面内部
    player.pos.x = glm::clamp(player.pos.x, -floorHalf + playerHalf, floorHalf - playerHalf);
    player.pos.z = glm::clamp(player.pos.z, -floorHalf + playerHalf, floorHalf - playerHalf);

    // 强制 Y 不动（保持水平移动）
    player.pos.y = player.groundY;

    // ---------- Spawn and falling updates ----------
    spawnTimer -= dt;
    if (spawnTimer <= 0.0f)
    {
        spawnTimer = randf(rng, 0.4f, 0.9f); // slightly varying spawn interval
        SpawnObject();
    }

    for (auto &o : falling)
    {
        if (!o.alive)
            continue;
        // gravity
        o.vel += glm::vec3(0.0f, -9.8f * dt * 0.2f, 0.0f);
        o.pos += o.vel * dt;

        // update rotation
        o.rot += o.rotSpeed * dt;

        // simple AABB collision with player (player y is fixed, so this still works)
        glm::vec3 amin = o.pos - glm::vec3(0.25f);
        glm::vec3 amax = o.pos + glm::vec3(0.25f);
        glm::vec3 bmin = player.pos - glm::vec3(0.3f);
        glm::vec3 bmax = player.pos + glm::vec3(0.3f);
        bool hit = (amin.x <= bmax.x && amax.x >= bmin.x) &&
                   (amin.y <= bmax.y && amax.y >= bmin.y) &&
                   (amin.z <= bmax.z && amax.z >= bmin.z);
        if (hit)
            playerDead = true;

        // landing on floor: compute floor top and falling half-size
        const float fallHalf = 0.25f; // falling cube scaled by 0.5 => half-size = 0.25

        if (o.pos.y - fallHalf <= floorTop)
        {
            // place it on surface and stop vertical movement
            o.pos.y = floorTop + fallHalf;
            o.vel.y = 0.0f;
            // optionally remove or keep - here we mark as not alive to remove
            o.alive = false;
        }

        if (o.pos.y < -6.0f)
            o.alive = false;
    }

    // remove dead
    falling.erase(std::remove_if(falling.begin(), falling.end(),
                                 [](const Falling &f)
                                 { return !f.alive; }),
                  falling.end());
}

void Game::Render(unsigned int shader3D)
{
    glUseProgram(shader3D);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.55f, 0.0f));
    model = glm::scale(model, glm::vec3(12.0f, 0.1f, 12.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader3D, "uModel"), 1, GL_FALSE, &model[0][0]);
    glUniform3f(glGetUniformLocation(shader3D, "uColor"), 0.5f, 0.5f, 0.4f);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::mat4(1.0f);
    model = glm::translate(model, player.pos);
    model = glm::scale(model, glm::vec3(0.6f));
    glUniformMatrix4fv(glGetUniformLocation(shader3D, "uModel"), 1, GL_FALSE, &model[0][0]);
    glUniform3f(glGetUniformLocation(shader3D, "uColor"), player.color.r, player.color.g, player.color.b);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // render falling blocks (with rotation)
    for (auto &o : falling)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, o.pos);
        model = glm::rotate(model, o.rot, o.rotAxis);
        model = glm::scale(model, glm::vec3(0.5f));
        glUniformMatrix4fv(glGetUniformLocation(shader3D, "uModel"), 1, GL_FALSE, &model[0][0]);
        glUniform3f(glGetUniformLocation(shader3D, "uColor"), o.color.r, o.color.g, o.color.b);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}
