#version 400 core
out vec4 FragColor;

// the direction is always from the position to the center of the scene
// struct sDirectionalLight {
//     vec3 position;
//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
// };

// struct sPointLight {
//     vec3 position;
//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;

//     float quadratic;
//     float constant;
//     float linear;
// };

// struct sMaterial {
//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
//     float shininess;
//     float opacity;
// };

// struct sState {
//     bool use_shadows;
//     bool use_texture_diffuse;
//     bool use_texture_normal;
//     bool use_texture_specular;
//     bool use_texture_emissive;
// };

/* input variables */
// in vec3 FragPos;
in vec3 Normal;
flat in int Id;
// in vec2 TexCoords;
// in vec4 FragPosLightSpace;

#define MAX_POINT_LIGHTS 8

/* uniforms */
// uniform sampler2D shadowMap;
// uniform sampler2D texture_diffuse1;
// uniform sampler2D texture_normal1; // not used anymore
// uniform sampler2D texture_specular1;
// uniform sampler2D texture_emissive1;

uniform vec3 cameraPos;
// uniform sMaterial material;
// uniform sDirectionalLight directionalLight;
// uniform sPointLight pointLights[MAX_POINT_LIGHTS];
// uniform int nPointLights;
// uniform sState state;

/* global variables */
// vec3    gDiffuse;
// vec3    gSpecular;
// vec3    gEmissive;
// vec3    gNormal;

/* prototypes */
// vec3    computeDirectionalLight( sDirectionalLight light, vec3 normal, vec3 viewDir, vec4 fragPosLightSpace );
// vec3    computePointLight( sPointLight light, vec3 normal, vec3 fragPos,vec3 viewDir );
// float   computeShadows( vec4 fragPosLightSpace );
// void    handleStates( void );


void main() {
    // vec3 fragPos = vec3(model * vec4(gl_Position, 1.0));
    // handleStates();
    // vec3 viewDir = normalize(cameraPos - FragPos);

    // vec3 result = computeDirectionalLight(directionalLight, gNormal, viewDir, FragPosLightSpace);
    // for (int i = 0; i < nPointLights && i < MAX_POINT_LIGHTS; ++i)
    //     result += computePointLight(pointLights[i], gNormal, FragPos, viewDir);

    // FragColor = vec4(result, 1.0f);
    // FragColor.w = material.opacity;

    FragColor = vec4(Normal * 0.5 + 0.5, 1.0);
}

// void    handleStates( void ) {
//     gDiffuse  = (state.use_texture_diffuse  ? texture(texture_diffuse1,  TexCoords).rgb : material.diffuse);
//     gSpecular = (state.use_texture_specular ? texture(texture_specular1, TexCoords).rgb : material.specular);
//     gEmissive = (state.use_texture_emissive ? texture(texture_emissive1, TexCoords).rgb : material.ambient);
//     gNormal = normalize(Normal);
// }

// vec3 computeDirectionalLight( sDirectionalLight light, vec3 normal, vec3 viewDir, vec4 fragPosLightSpace ) {
//     vec3 lightDir = normalize(light.position);
//     /* diffuse */
//     float diff = max(dot(normal, lightDir), 0.0);
//     /* specular */
//     vec3 halfwayDir = normalize(lightDir + viewDir);
//     float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

//     /* compute terms */
//     vec3 ambient  = light.ambient  * gDiffuse;
//     vec3 diffuse  = light.diffuse  * diff * gDiffuse;
//     vec3 specular = light.specular * spec * gSpecular;

//     if (state.use_shadows) {
//         float shadow  = computeShadows(fragPosLightSpace);
//         return (gEmissive + ambient + (1.0 - shadow) * (diffuse + specular));
//     }
//     return (gEmissive + ambient + (diffuse + specular));
// }

// vec3 computePointLight( sPointLight light, vec3 normal, vec3 fragPos, vec3 viewDir ) {
//     vec3 lightDir = normalize(light.position - fragPos);
//     /* diffuse */
//     float diff = max(dot(normal, lightDir), 0.0);
//     /* specular */
//     vec3 reflectDir = reflect(-lightDir, normal);
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//     /* attenuation */
//     float dist = length(light.position - fragPos);
//     float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
//     /* compute terms */
//     vec3 ambient  = light.ambient  * gDiffuse * attenuation;
//     vec3 diffuse  = light.diffuse  * diff * gDiffuse * attenuation;
//     vec3 specular = light.specular * spec * gSpecular * attenuation;

//     return (gEmissive + ambient + diffuse + specular);
// }

// float computeShadows( vec4 fragPosLightSpace ) {
//     vec3 projCoords = (fragPosLightSpace.xyz / fragPosLightSpace.w) * 0.5 + 0.5;
//     float currentDepth = projCoords.z;
//     float bias = 0.0025;
//     /* PCF */
//     float shadow = 0.0;
//     vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
//     for (int x = -1; x <= 1; ++x) {
//         for (int y = -1; y <= 1; ++y) {
//             float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
//             shadow += (currentDepth - bias > pcfDepth ? 1.0 : 0.0);
//         }
//     }
//     return (projCoords.z > 1.0 ? 0.0 : shadow / 9.0);
// }
