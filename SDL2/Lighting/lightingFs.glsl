#version 410 core

in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
    // Diffuse light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;
    
    // Ambient light
    float ambientStrength = 0.3f;
    vec3 ambient = ambientStrength * lightColor;
    
    vec3 result = (ambient + diffuse) * objectColor;
    color = vec4(result, 1.0f);
}
