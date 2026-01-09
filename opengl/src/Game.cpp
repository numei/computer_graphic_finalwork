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

glm::vec3 sunDir = glm::normalize(glm::vec3(-0.4f, -1.0f, -0.2f));

glm::mat4 lightView = glm::lookAt(
    -sunDir * 10.0f,
    glm::vec3(0.0f),
    glm::vec3(0, 1, 0));

glm::mat4 lightProj = glm::ortho(
    -10.0f, 10.0f,
    -10.0f, 10.0f,
    1.0f, 30.0f);

glm::mat4 lightVP = lightProj * lightView;

Game::Game()
    : spawnTimer(0.0f), playerDead(false)
{
    // seed RNG with high-resolution clock
    rng.seed((uint32_t)std::chrono::high_resolution_clock::now().time_since_epoch().count());
}
void Game::InitShadowMap()
{
    // ===== Shadow map framebuffer =====
    glGenFramebuffers(1, &depthFBO);

    // depth texture
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_SIZE, SHADOW_SIZE, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // attach
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

    // Player radius (0.6 scaled cube)
    const float playerHalf = 0.3f;

    // Clamp X, Z positions to stay within floor bounds
    player.pos.x = glm::clamp(player.pos.x, -floorHalf + playerHalf, floorHalf - playerHalf);
    player.pos.z = glm::clamp(player.pos.z, -floorHalf + playerHalf, floorHalf - playerHalf);

    // Force Y to stay fixed (horizontal movement only)
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
void Game::Render(unsigned int shader3D, const glm::vec3 &cameraPos)
{

    // --------- 1) Compute light (sun) matrices ----------
    // directional sun direction (from world toward sun): choose as needed
    glm::vec3 sunDir = glm::normalize(glm::vec3(-0.4f, -1.0f, -0.2f));

    // position the "sun camera" at -sunDir * dist looking at origin (or scene center)
    float lightDist = 12.0f;
    glm::mat4 lightView = glm::lookAt(-sunDir * lightDist, glm::vec3(0.0f), glm::vec3(0, 1, 0));

    // orthographic projection for directional light
    float orthoSize = 8.0f;
    glm::mat4 lightProj = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, 1.0f, 40.0f);

    glm::mat4 lightVP = lightProj * lightView;

    // preserve current viewport so we can restore it after shadow pass
    GLint prevViewport[4];
    glGetIntegerv(GL_VIEWPORT, prevViewport);

    // --------- 2) Shadow pass: render depth from light's POV ----------
    if (depthFBO != 0 && shadowShader != 0)
    {
        glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
        glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        // optional: bias via polygon offset to reduce peter-panning
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 1.0f);

        glUseProgram(shadowShader);
        GLint locLightVP = glGetUniformLocation(shadowShader, "uLightVP");
        if (locLightVP >= 0)
            glUniformMatrix4fv(locLightVP, 1, GL_FALSE, &lightVP[0][0]);

        // Render scene geometry for depth (player, floor, falling)
        // Player model
        {

            float scaleY = playerModel.modelScale.y;
            const float floorTop = -0.5f;
            player.pos.y = floorTop - playerModel.bboxMin.y * scaleY;
            glm::vec3 bboxMin = playerModel.bboxMin; // must be set in StaticModel during load
            float scale = playerModel.modelScale.x;  // assume uniform scale
            float yOffset = -bboxMin.y * scale;      // positive: bring feet to y=0
            glm::mat4 modelMat = glm::mat4(1.0f);
            modelMat = glm::translate(modelMat, glm::vec3(player.pos.x, player.pos.y + yOffset, player.pos.z));
            modelMat = glm::rotate(modelMat, glm::radians(180.0f), glm::vec3(0, 1, 0));
            modelMat = glm::scale(modelMat, playerModel.modelScale);

            GLint locModel = glGetUniformLocation(shadowShader, "uModel");
            if (locModel >= 0)
                glUniformMatrix4fv(locModel, 1, GL_FALSE, &modelMat[0][0]);

            // Draw model; StaticModel::Draw should bind its VAO and issue glDrawElements.
            // It may set uniforms if they exist, but shadowShader ignores color/texture.
            playerModel.Draw(shadowShader);
        }

        // Ensure cube VAO bound for floor/cubes
        if (cubeVAO != 0)
            glBindVertexArray(cubeVAO);

        // floor
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, -0.55f, 0.0f));
            model = glm::scale(model, glm::vec3(12.0f, 0.1f, 12.0f));
            GLint locModel = glGetUniformLocation(shadowShader, "uModel");
            if (locModel >= 0)
                glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);

            // Draw cube geometry (positions at location 0)
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // falling cubes
        for (auto &o : falling)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, o.pos);
            model = glm::rotate(model, o.rot, o.rotAxis);
            model = glm::scale(model, glm::vec3(0.5f));
            GLint locModel = glGetUniformLocation(shadowShader, "uModel");
            if (locModel >= 0)
                glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // restore state
        glBindVertexArray(0);
        glUseProgram(0);
        glDisable(GL_POLYGON_OFFSET_FILL);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // restore viewport
        glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
    }
    else
    {
        // If shadow resources not initialized, ensure viewport is correct (safe-guard)
        glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
    }

    // --------- 3) Normal render pass (camera) ----------
    glUseProgram(shader3D);

    // upload directional light, camera and lightVP for shadow sampling
    GLint locViewPos = glGetUniformLocation(shader3D, "uViewPos");
    if (locViewPos >= 0)
        glUniform3fv(locViewPos, 1, &cameraPos[0]);

    GLint locLightDir = glGetUniformLocation(shader3D, "uLightDir");
    if (locLightDir >= 0)
        glUniform3fv(locLightDir, 1, &sunDir[0]);

    GLint locLightColor = glGetUniformLocation(shader3D, "uLightColor");
    if (locLightColor >= 0)
        glUniform3f(locLightColor, 1.0f, 0.98f, 0.9f);

    GLint locLightIntensity = glGetUniformLocation(shader3D, "uLightIntensity");
    if (locLightIntensity >= 0)
        glUniform1f(locLightIntensity, 1.2f);

    // supply lightVP and shadow map to shader
    GLint locLightVP_main = glGetUniformLocation(shader3D, "uLightVP");
    if (locLightVP_main >= 0)
        glUniformMatrix4fv(locLightVP_main, 1, GL_FALSE, &lightVP[0][0]);

    // bind depth map to texture unit 3 for shadow lookup
    if (depthMap != 0)
    {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        GLint locShadow = glGetUniformLocation(shader3D, "uShadowMap");
        if (locShadow >= 0)
            glUniform1i(locShadow, 3);
    }

    // helper to set uModel + uNormalMat for shader3D
    auto setModelAndNormal = [&](const glm::mat4 &modelMat)
    {
        GLint locModel = glGetUniformLocation(shader3D, "uModel");
        if (locModel >= 0)
            glUniformMatrix4fv(locModel, 1, GL_FALSE, &modelMat[0][0]);

        glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(modelMat)));
        GLint locNormal = glGetUniformLocation(shader3D, "uNormalMat");
        if (locNormal >= 0)
            glUniformMatrix3fv(locNormal, 1, GL_FALSE, &normalMat[0][0]);
    };

    // ---------- draw player (with textures) ----------
    {
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, player.pos);
        modelMat = glm::rotate(modelMat, glm::radians(180.0f), glm::vec3(0, 1, 0));
        modelMat = glm::scale(modelMat, playerModel.modelScale);

        setModelAndNormal(modelMat);

        GLint locHasDiffuse = glGetUniformLocation(shader3D, "uHasDiffuse");
        GLint locUseAlphaTest = glGetUniformLocation(shader3D, "uUseAlphaTest");
        GLint locAlphaCutoff = glGetUniformLocation(shader3D, "uAlphaCutoff");
        GLint locMatDiffuse = glGetUniformLocation(shader3D, "uMatDiffuse");
        GLint locDiffuseMap = glGetUniformLocation(shader3D, "uDiffuseMap");

        if (locHasDiffuse >= 0)
            glUniform1i(locHasDiffuse, 1);
        if (locUseAlphaTest >= 0)
            glUniform1i(locUseAlphaTest, 1);
        if (locAlphaCutoff >= 0)
            glUniform1f(locAlphaCutoff, 0.3f);
        if (locMatDiffuse >= 0)
            glUniform3f(locMatDiffuse, 1.0f, 1.0f, 1.0f);
        if (locDiffuseMap >= 0)
            glUniform1i(locDiffuseMap, 0);

        // ensure texture unit 0 is active and playerModel binds there
        glActiveTexture(GL_TEXTURE0);
        playerModel.Draw(shader3D);

        // unbind
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // ---------- draw floor + falling cubes ----------
    if (cubeVAO != 0)
        glBindVertexArray(cubeVAO);

    GLint locHasDiffuse_main = glGetUniformLocation(shader3D, "uHasDiffuse");
    GLint locUseAlphaTest_main = glGetUniformLocation(shader3D, "uUseAlphaTest");
    GLint locMatDiffuse_main = glGetUniformLocation(shader3D, "uMatDiffuse");

    if (locHasDiffuse_main >= 0)
        glUniform1i(locHasDiffuse_main, 0);
    if (locUseAlphaTest_main >= 0)
        glUniform1i(locUseAlphaTest_main, 0);

    // give cube a constant normal if it has none
    glDisableVertexAttribArray(1);
    glVertexAttrib3f(1, 0.0f, 1.0f, 0.0f);

    // floor
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.55f, 0.0f));
        model = glm::scale(model, glm::vec3(12.0f, 0.1f, 12.0f));
        setModelAndNormal(model);

        if (locMatDiffuse_main >= 0)
            glUniform3f(locMatDiffuse_main, 0.5f, 0.5f, 0.4f);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // falling cubes
    for (auto &o : falling)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, o.pos);
        model = glm::rotate(model, o.rot, o.rotAxis);
        model = glm::scale(model, glm::vec3(0.5f));
        setModelAndNormal(model);

        if (locMatDiffuse_main >= 0)
            glUniform3f(locMatDiffuse_main, o.color.r, o.color.g, o.color.b);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindVertexArray(0);

    // re-enable attrib 1 if other code expects it as per-vertex
    glEnableVertexAttribArray(1);
}

void Game::RenderSceneGeometry(unsigned int shader)
{
    // player
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, player.pos);
    model = glm::rotate(model, glm::radians(180.f), {0, 1, 0});
    model = glm::scale(model, playerModel.modelScale);

    glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"),
                       1, GL_FALSE, &model[0][0]);
    playerModel.Draw(shader);

    // floor
    model = glm::mat4(1.0f);
    model = glm::translate(model, {0, -0.55f, 0});
    model = glm::scale(model, {12, 0.1f, 12});

    glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"),
                       1, GL_FALSE, &model[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // falling cubes
    for (auto &o : falling)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, o.pos);
        model = glm::rotate(model, o.rot, o.rotAxis);
        model = glm::scale(model, glm::vec3(0.5f));

        glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"),
                           1, GL_FALSE, &model[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}
