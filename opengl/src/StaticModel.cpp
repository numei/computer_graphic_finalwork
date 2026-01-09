// src/StaticModel.cpp
#include "StaticModel.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include <cstring>

// stb_image single-file loader
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static glm::vec3 aiVec3ToGlm(const aiVector3D &v) { return glm::vec3(v.x, v.y, v.z); }
static glm::vec2 aiVec2ToGlm(const aiVector3D &v) { return glm::vec2(v.x, v.y); }

StaticModel::StaticModel() {}
StaticModel::~StaticModel() { Cleanup(); }

void StaticModel::Cleanup()
{
    for (auto &m : meshes)
    {
        if (m.ebo)
            glDeleteBuffers(1, &m.ebo);
        if (m.vbo)
            glDeleteBuffers(1, &m.vbo);
        if (m.vao)
            glDeleteVertexArrays(1, &m.vao);
        if (m.diffuseTex)
            glDeleteTextures(1, &m.diffuseTex);
    }
    meshes.clear();
}

GLuint StaticModel::LoadTextureFromFile(const std::string &filename, bool &outHasAlpha)
{
    outHasAlpha = false;
    int w, h, n;
    stbi_uc *data = stbi_load(filename.c_str(), &w, &h, &n, 4); // force 4 channels (RGBA)
    if (!data)
    {
        std::cerr << "stb_image failed to load: " << filename << "\n";
        return 0;
    }
    // if original channels < 4, n may be < 4; but we forced load to 4 -> check alpha content
    outHasAlpha = false;
    for (int i = 0; i < w * h; ++i)
    {
        if (data[i * 4 + 3] < 250)
        {
            outHasAlpha = true;
            break;
        } // loose test
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // wrap repeat default
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}

bool StaticModel::LoadFromFile(const std::string &path)
{
    Cleanup();

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path,
                                             aiProcess_Triangulate |
                                                 aiProcess_GenSmoothNormals |
                                                 aiProcess_FlipUVs |
                                                 aiProcess_CalcTangentSpace |
                                                 aiProcess_JoinIdenticalVertices |
                                                 aiProcess_OptimizeMeshes |
                                                 aiProcess_PreTransformVertices // bake node transforms into vertices -> simpler
    );

    if (!scene || !scene->HasMeshes())
    {
        std::cerr << "StaticModel: failed to load " << path << " (" << importer.GetErrorString() << ")\n";
        return false;
    }

    // directory for relative texture paths
    size_t p = path.find_last_of("/\\");
    directory = (p == std::string::npos) ? "." : path.substr(0, p);

    // For each mesh, collect vertex/index data and material
    meshes.resize(scene->mNumMeshes);

    for (unsigned int m = 0; m < scene->mNumMeshes; ++m)
    {
        aiMesh *mesh = scene->mMeshes[m];
        std::vector<SimpleVertex> verts;
        std::vector<unsigned int> inds;
        verts.resize(mesh->mNumVertices);
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            verts[i].pos = aiVec3ToGlm(mesh->mVertices[i]);
            verts[i].normal = mesh->HasNormals() ? aiVec3ToGlm(mesh->mNormals[i]) : glm::vec3(0, 1, 0);
            if (mesh->mTextureCoords[0])
            {
                // std::cerr << "Debug: mesh->mTextureCoords[0] exists." << std::endl;
                verts[i].uv = aiVec2ToGlm(mesh->mTextureCoords[0][i]);
            }

            else
                verts[i].uv = glm::vec2(0.0f, 0.0f);
        }
        for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
        {
            const aiFace &face = mesh->mFaces[f];
            if (face.mNumIndices != 3)
                continue;
            inds.push_back(face.mIndices[0]);
            inds.push_back(face.mIndices[1]);
            inds.push_back(face.mIndices[2]);
        }

        // create GL buffers
        MeshRenderData &dst = meshes[m];
        dst.indexCount = static_cast<GLsizei>(inds.size());

        glGenVertexArrays(1, &dst.vao);
        glGenBuffers(1, &dst.vbo);
        glGenBuffers(1, &dst.ebo);

        glBindVertexArray(dst.vao);
        glBindBuffer(GL_ARRAY_BUFFER, dst.vbo);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(SimpleVertex), verts.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dst.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(unsigned int), inds.data(), GL_STATIC_DRAW);

        // attribs: location 0 = pos, 1 = normal, 2 = uv
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (void *)offsetof(SimpleVertex, pos));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (void *)offsetof(SimpleVertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (void *)offsetof(SimpleVertex, uv));

        glBindVertexArray(0);

        // material handling
        dst.hasDiffuse = false;
        dst.hasAlpha = false;
        dst.isHair = false;
        dst.diffuseTex = 0;
        dst.diffuseColor = glm::vec3(1.0f);

        if (scene->mNumMaterials > 0 && mesh->mMaterialIndex < scene->mNumMaterials)
        {
            aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];

            // diffuse color (fallback)
            aiColor3D col(1.0f, 1.0f, 1.0f);
            if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, col))
            {
                dst.diffuseColor = glm::vec3(col.r, col.g, col.b);
            }
            // diffuse texture
            if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
            {
                aiString texPath;
                mat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath);
                std::string texFile = texPath.C_Str();
                std::string full = texFile;
                // if relative path -> make absolute relative to model directory
                if (texFile.find_first_of("/\\") == std::string::npos)
                    full = directory + "/" + texFile;
                dst.diffuseTex = LoadTextureFromFile(full, dst.hasAlpha);

                if (dst.diffuseTex)
                {
                    std::cout << "StaticModel: loaded diffuse texture " << full << "\n";
                    dst.hasDiffuse = true;
                }
                else
                {
                    std::cerr << "StaticModel: failed to load diffuse texture " << full << "\n";
                }
            }

            // opacity or transparency detection
            float opacity = 1.0f;
            if (AI_SUCCESS == aiGetMaterialFloat(mat, AI_MATKEY_OPACITY, &opacity))
            {
                if (opacity < 0.999f)
                    dst.hasAlpha = true;
            }

            // heuristic: if material name or texture filename contains "hair" or "fur", mark as hair
            aiString matName;
            if (AI_SUCCESS == mat->Get(AI_MATKEY_NAME, matName))
            {
                std::string name = matName.C_Str();
                for (auto &c : name)
                    c = tolower(c);
                if (name.find("hair") != std::string::npos || name.find("fur") != std::string::npos)
                {
                    dst.isHair = true;
                    dst.alphaCutoff = 0.4f;
                }
            }
            if (!dst.isHair && dst.hasDiffuse)
            {
                // also check texture filename
                std::string t = "";
                if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
                {
                    aiString tpath;
                    mat->GetTexture(aiTextureType_DIFFUSE, 0, &tpath);
                    t = tpath.C_Str();
                    for (auto &c : t)
                        c = tolower(c);
                    if (t.find("hair") != std::string::npos || t.find("fur") != std::string::npos)
                    {
                        dst.isHair = true;
                        dst.alphaCutoff = 0.4f;
                    }
                }
            }
        }
    }

    bboxInitialized = false;
    ComputeBBoxRecursive(scene->mRootNode, scene, glm::mat4(1.0f));
    // std::cout << "StaticModel: loaded meshes=" << meshes.size() << " from " << path << std::endl;
    return true;
}

void StaticModel::Draw(GLuint shaderProgram) const
{
    // we assume shaderProgram is already in use, and uniforms uHasDiffuse, uHasAlpha, uUseAlphaTest,
    // uAlphaCutoff, uMatDiffuse and sampler2D uDiffuseMap exist.
    GLint locHasDiffuse = glGetUniformLocation(shaderProgram, "uHasDiffuse");
    GLint locHasAlpha = glGetUniformLocation(shaderProgram, "uHasAlpha");
    GLint locUseAlphaTest = glGetUniformLocation(shaderProgram, "uUseAlphaTest");
    GLint locAlphaCutoff = glGetUniformLocation(shaderProgram, "uAlphaCutoff");
    GLint locMatDiffuse = glGetUniformLocation(shaderProgram, "uMatDiffuse");
    GLint locDiffuseMap = glGetUniformLocation(shaderProgram, "uDiffuseMap");

    for (const auto &m : meshes)
    {
        // set diffuse color
        if (locMatDiffuse >= 0)
            glUniform3f(locMatDiffuse, m.diffuseColor.r, m.diffuseColor.g, m.diffuseColor.b);

        // texture binding
        if (m.hasDiffuse && m.diffuseTex)
        {
            if (locHasDiffuse >= 0)
                glUniform1i(locHasDiffuse, 1);
            if (locDiffuseMap >= 0)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, m.diffuseTex);
                glUniform1i(locDiffuseMap, 0);
            }
        }
        else
        {
            if (locHasDiffuse >= 0)
                glUniform1i(locHasDiffuse, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // alpha/hair handling
        if (locHasAlpha >= 0)
            glUniform1i(locHasAlpha, m.hasAlpha ? 1 : 0);
        if (locUseAlphaTest >= 0)
            glUniform1i(locUseAlphaTest, (m.hasAlpha || m.isHair) ? 1 : 0);
        if (locAlphaCutoff >= 0)
            glUniform1f(locAlphaCutoff, m.alphaCutoff);

        // blending for hair: enable blending if isHair
        if (m.isHair)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            // also disable depth write optionally to reduce artifacts:
            glDepthMask(GL_FALSE);
        }

        // draw mesh
        glBindVertexArray(m.vao);
        glDrawElements(GL_TRIANGLES, m.indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // restore state
        if (m.isHair)
        {
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
        }
    }

    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

static glm::mat4 aiMatToGlm(const aiMatrix4x4 &m)
{
    return glm::mat4(
        m.a1, m.b1, m.c1, m.d1,
        m.a2, m.b2, m.c2, m.d2,
        m.a3, m.b3, m.c3, m.d3,
        m.a4, m.b4, m.c4, m.d4);
}

void StaticModel::ComputeBBoxRecursive(
    aiNode *node,
    const aiScene *scene,
    const glm::mat4 &parentTransform)
{
    glm::mat4 nodeTransform = parentTransform * aiMatToGlm(node->mTransformation);

    // 遍历该 node 挂载的 mesh
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
        {
            glm::vec3 p = aiVec3ToGlm(mesh->mVertices[v]);
            glm::vec4 worldP = nodeTransform * glm::vec4(p, 1.0f);
            glm::vec3 wp(worldP);

            if (!bboxInitialized)
            {
                bboxMin = bboxMax = wp;
                bboxInitialized = true;
            }
            else
            {
                bboxMin = glm::min(bboxMin, wp);
                bboxMax = glm::max(bboxMax, wp);
            }
        }
    }

    // 递归子节点
    for (unsigned int c = 0; c < node->mNumChildren; ++c)
    {
        ComputeBBoxRecursive(node->mChildren[c], scene, nodeTransform);
    }
    // std::cout << "bboxMin = "
    //           << bboxMin.x << ", "
    //           << bboxMin.y << ", "
    //           << bboxMin.z << std::endl;

    // std::cout << "bboxMax = "
    //           << bboxMax.x << ", "
    //           << bboxMax.y << ", "
    //           << bboxMax.z << std::endl;
}
