#version 410 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};


in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 color;

uniform vec3 viewPos;
uniform Light light;
uniform Material material;

void main()
{
    vec3 lightDir = normalize(light.position - FragPos);

    
    // Ambient light
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    
    // Diffuse light
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    
    // Specular Light
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    
    // Spotlight with Soft Edges
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);
    diffuse *= intensity;
    specular *= intensity;
    
    // Attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1.0f /
    (light.constant + light.linear * distance + light.quadratic * ( distance*distance ) );
    
    //        ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    vec3 result = (ambient + diffuse + specular);
    color = vec4(result, 1.0f);
    
}
