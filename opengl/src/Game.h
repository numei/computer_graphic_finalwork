#ifndef GAME_HPP
#define GAME_HPP
#include <vector>
#include <random>
#include <iostream>
#include <glm/glm.hpp>
#include "Player.h"
#include "StaticModel.h"
#include "Shader.h"

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
    // ===== Shadow mapping =====
    unsigned int depthFBO = 0;
    unsigned int depthMap = 0;

    static constexpr unsigned int SHADOW_SIZE = 2048;

    // shadow shader program id
    unsigned int shadowShader = 0;

    // render helpers
    void RenderSceneGeometry(unsigned int shader);

    Game();
    void InitShadowMap();
    void Reset();
    void Update(float dt, const bool keys[1024], const glm::vec3 &cameraFront, const glm::vec3 &cameraUp);
    void Render(unsigned int shader3D, const glm::vec3 &cameraPos);
    void SetCubeVAO(unsigned int vao) { cubeVAO = vao; }

    StaticModel playerModel;

    void LoadPlayerModel(const std::string &path)
    {
        if (!playerModel.LoadFromFile(path))
        {
            std::cerr << "Failed to load player model: " << path << std::endl;
        }
        else
        {

            // 可选：设置默认缩放来匹配原来 cube 大小
            playerModel.modelScale = glm::vec3(0.6f);
        }
    }

private:
    unsigned int cubeVAO = 0;
    void SpawnObject();
};
#endif
