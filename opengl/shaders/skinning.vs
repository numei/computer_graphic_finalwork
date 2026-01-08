#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in ivec4 aBoneIDs;
layout(location = 3) in vec4 aWeights;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform mat3 uNormalMat;

// bone transforms (set from CPU)
const int MAX_BONES = 100;
uniform mat4 uBones[MAX_BONES];

out vec3 vNormal;
out vec3 vWorldPos;

void main()
{
    mat4 skinMat = mat4(0.0);
    skinMat += uBones[aBoneIDs.x] * aWeights.x;
    skinMat += uBones[aBoneIDs.y] * aWeights.y;
    skinMat += uBones[aBoneIDs.z] * aWeights.z;
    skinMat += uBones[aBoneIDs.w] * aWeights.w;

    vec4 skinnedPos = skinMat * vec4(aPos, 1.0);
    vec4 worldPos = uModel * skinnedPos;
    vWorldPos = worldPos.xyz;

    // normal (approx): apply upper-left 3x3 of skin matrix then normal matrix
    vec3 skinnedNormal = mat3(skinMat) * aNormal;
    vNormal = normalize(uNormalMat * skinnedNormal);

    gl_Position = uProj * uView * worldPos;
}
