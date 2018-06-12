#version 460 core

// light-fragment-shader

uniform bool isPositionBufferVisible;
uniform bool isNormalBufferVisible;
uniform bool isTriangleCoordsBufferVisible;
uniform bool isTextureCoordsBufferVisible;
uniform bool isMaterialAmbientBufferVisible;
uniform bool isMaterialDiffuseBufferVisible;
uniform bool isMaterialSpecularBufferVisible;

uniform bool isWireframeOverwrite;
uniform bool isEdgesVisible;

in vec2 TexCoords;

uniform sampler2D positionTextureSampler;
uniform sampler2D normalTextureSampler;
uniform sampler2D triangleCoordinatesTextureSampler;
uniform sampler2D textureCoordinatesTextureSampler;
uniform sampler2D materialAmbientTextureSampler;
uniform sampler2D materialDiffuseTextureSampler;
uniform sampler2D materialSpecularTextureSampler;

vec3 positionBufferValue;
vec3 normalBufferValue;
vec3 triangleCoordinatesBufferValue;
vec3 textureCoordinatesBufferValue;
vec3 materialAmbientBufferValue;
vec3 materialDiffuseBufferValue;
vec3 materialSpecularBufferValue;

out vec3 finalColor;

uniform int time;
uniform mat4 mv;

const float PI = 3.1415926535897932384626433832795;

void main()
{
    // Retrieve data from textures
    positionBufferValue = texture(positionTextureSampler, TexCoords).rgb;
    normalBufferValue = texture(normalTextureSampler, TexCoords).rgb;
    triangleCoordinatesBufferValue = texture(triangleCoordinatesTextureSampler, TexCoords).rgb;
    textureCoordinatesBufferValue = texture(textureCoordinatesTextureSampler, TexCoords).rgb;
    materialAmbientBufferValue = texture(materialAmbientTextureSampler, TexCoords).rgb;
    materialDiffuseBufferValue = texture(materialDiffuseTextureSampler, TexCoords).rgb;
    materialSpecularBufferValue = texture(materialSpecularTextureSampler, TexCoords).rgb;

    // Discards background fragments
    if(triangleCoordinatesBufferValue.x < 0.001 && triangleCoordinatesBufferValue.y < 0.001 && triangleCoordinatesBufferValue.z < 0.001)
    {
        discard;
        return;
    }

    // Options for showing buffers
    if(isPositionBufferVisible) {
        finalColor = positionBufferValue;
        finalColor += vec3(1.0, 1.0, 1.0);
        finalColor /= 2.0;
        return;
    } else if(isNormalBufferVisible) {
        finalColor = normalBufferValue;
        finalColor += vec3(1.0, 1.0, 1.0);
        finalColor /= 2.0;
        return;
    } else if(isTriangleCoordsBufferVisible) {
        finalColor = triangleCoordinatesBufferValue;
        return;
    } else if(isTextureCoordsBufferVisible) {
        finalColor = textureCoordinatesBufferValue;
        return;
    } else if(isMaterialAmbientBufferVisible) {
        finalColor = materialAmbientBufferValue;
        return;
    } else if(isMaterialDiffuseBufferVisible) {
        finalColor = materialDiffuseBufferValue;
        return;
    } else if(isMaterialSpecularBufferVisible) {
        finalColor = materialSpecularBufferValue;
        return;
    }

    vec3 position = positionBufferValue;
    vec3 triangleCoordinate = triangleCoordinatesBufferValue;

    // Options for showing wireframe
    if((isWireframeOverwrite || isEdgesVisible) &&
       (triangleCoordinate.x < 0.01 || triangleCoordinate.y < 0.01 || triangleCoordinate.z < 0.01))
    {
        finalColor = vec3(1, 1, 1);
        return;
    }
    if(isWireframeOverwrite)
    {
        discard;
        return;
    }

    // Regular shading
    vec3 materialAmbient = materialAmbientBufferValue;
    vec3 materialDiffuse = materialDiffuseBufferValue;
    vec3 materialSpecular = materialSpecularBufferValue;

    // Variables to be used
    vec3 lightPosition;
    vec3 lightColor;
    vec3 N;
    vec3 L;
    float incidence;
    vec3 diffuse;
    vec3 V;
    vec3 H;
    float specularFactor;
    vec3 specular;

    // AMBIENT
    vec3 ambient = vec3(0.1, 0.1, 0.1) * materialAmbient;
    finalColor = ambient;

    int numLights = 20;
    float numLightsFloat = numLights;
    float timeFloat = time / 1000.0 * 0.8;
    for(int i=0; i<numLights; i++)
    {
        // Generate Lights
        lightPosition = vec3( 10.0 * sin(timeFloat * 0.7 + i % 5 + i * 2.0 * PI / numLightsFloat),
                              0.0,
                              10.0 * cos(timeFloat * 0.3 + i % 5 + i * 2.0 * PI / numLightsFloat));

        lightColor = vec3(i%3==0?1.0:0.0,
                          i%3==1?1.0:0.0,
                          i%3==2?1.0:0.0);
        lightColor *= 0.2;

        N = normalBufferValue;
        L = normalize((lightPosition - position));

        incidence = dot(L, N);

        // DIFUSE
        diffuse = max(incidence * materialDiffuse, 0);

        // SPECULAR
        V = normalize((vec3(0.0, 0.0, 0.0) - position));
        H = normalize(L + V);
        specularFactor = pow(max(dot(N,H),0.0), 64.0);
        specular = max(specularFactor * materialSpecular, 0);

        finalColor += lightColor * (diffuse + specular);
    }

}
