#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <cstdlib>
#include <algorithm>
#include <iostream>

Game::Game() : spawnTimer(0.0f), playerDead(false) {}

void Game::Reset()
{
    falling.clear();
    spawnTimer = 0.0f;
    player.pos = glm::vec3(0.0f, 0.5f, 0.0f);
    playerDead = false;
}

void Game::Update(float dt, const bool keys[1024], const glm::vec3 &cameraFront, const glm::vec3 &cameraUp)
{
    if (playerDead)
        return;
    player.Update(dt, keys, cameraFront, cameraUp);
    spawnTimer -= dt;
    if (spawnTimer <= 0.0f)
    {
        spawnTimer = 0.6f;
        Falling f;
        f.pos = glm::vec3((rand() % 600) / 100.0f - 3.0f, 5.0f, (rand() % 600) / 100.0f - 3.0f);
        f.vel = glm::vec3(0.0f, -(1.0f + (rand() % 200) / 100.0f), 0.0f);
        f.color = glm::vec3((rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f);
        f.alive = true;
        falling.push_back(f);
    }
    for (auto &o : falling)
    {
        if (!o.alive)
            continue;
        o.vel += glm::vec3(0.0f, -9.8f * dt * 0.2f, 0.0f);
        o.pos += o.vel * dt;
        // simple AABB collision
        glm::vec3 amin = o.pos - glm::vec3(0.25f);
        glm::vec3 amax = o.pos + glm::vec3(0.25f);
        glm::vec3 bmin = player.pos - glm::vec3(0.3f);
        glm::vec3 bmax = player.pos + glm::vec3(0.3f);
        bool hit = (amin.x <= bmax.x && amax.x >= bmin.x) && (amin.y <= bmax.y && amax.y >= bmin.y) && (amin.z <= bmax.z && amax.z >= bmin.z);
        if (hit)
            playerDead = true;
        if (o.pos.y < -5.0f)
            o.alive = false;
    }
    falling.erase(std::remove_if(falling.begin(), falling.end(), [](const Falling &f)
                                 { return !f.alive; }),
                  falling.end());
}

void Game::Render(unsigned int shader3D)
{
    glUseProgram(shader3D);

    // ====== FLOOR (深灰蓝) ======
    glm::mat4 model;
    // model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(0.0f, -0.55f, 0.0f));
    // model = glm::scale(model, glm::vec3(12.0f, 0.1f, 12.0f));
    // glUniformMatrix4fv(glGetUniformLocation(shader3D, "uModel"), 1, GL_FALSE, &model[0][0]);
    // glUniform3f(glGetUniformLocation(shader3D, "vColor"), 0.1f, 0.1f, 0.25f);
    // glDrawArrays(GL_TRIANGLES, 0, 36);

    // ====== PLAYER (亮黄色) ======
    model = glm::mat4(1.0f);
    model = glm::translate(model, player.pos);
    model = glm::scale(model, glm::vec3(0.6f));
    glUniformMatrix4fv(glGetUniformLocation(shader3D, "uModel"), 1, GL_FALSE, &model[0][0]);

    // 强制黄色
    glUniform3f(glGetUniformLocation(shader3D, "vColor"), 1.0f, 0.9f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // todo: add random angle to throw objects
    // ====== FALLING (亮红色) ======
    for (auto &o : falling)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, o.pos);
        model = glm::scale(model, glm::vec3(0.5f));
        glUniformMatrix4fv(glGetUniformLocation(shader3D, "uModel"), 1, GL_FALSE, &model[0][0]);
        // 强制掉落物红色
        glUniform3f(glGetUniformLocation(shader3D, "vColor"), 1.0f, 0.2f, 0.2f);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}
