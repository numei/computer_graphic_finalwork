#version 330 core
in vec3 vNormal;
in vec3 vWorldPos;
out vec4 FragColor;

uniform vec3 uViewPos;

// material
uniform vec3 uMatAmbient;
uniform vec3 uMatDiffuse;
uniform vec3 uMatSpecular;
uniform float uMatShininess;

// light
uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform float uLightIntensity;

void main()
{
    vec3 N = normalize(vNormal);
    vec3 L = normalize(uLightPos - vWorldPos);
    vec3 V = normalize(uViewPos - vWorldPos);
    vec3 R = reflect(-L, N);

    vec3 ambient = 0.1 * uMatAmbient * uLightColor * uLightIntensity;
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * uMatDiffuse * uLightColor * uLightIntensity;
    float spec = pow(max(dot(R,V),0.0), uMatShininess);
    vec3 specular = spec * uMatSpecular * uLightColor * uLightIntensity;

    vec3 color = ambient + diffuse + specular;
    FragColor = vec4(color, 1.0);
}
