#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;  
in vec3 FragPos;  


uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;


uniform int hasTexture;
uniform sampler2D texture_diffuse1;

void main()
{    

    float ambientStrength = 0.05;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * objectColor;

    if (hasTexture == 1) {
        FragColor = texture(texture_diffuse1, TexCoords) * vec4(result, 1.0);
    } else {
        FragColor = vec4(0.0,0.0,1.0,1.0) * vec4(result, 1.0) ;
    }
}