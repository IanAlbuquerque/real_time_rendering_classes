#version 460 core

// light-fragment-shader

//uniform sampler2D diffuseTextureSampler;
//uniform sampler2D bumpMapSampler;

uniform bool isWireframeOverwrite;
uniform bool isEdgesVisible;

//uniform vec3 lightsColors[32];

in vec2 TexCoords;

uniform sampler2D positionTextureSampler;
uniform sampler2D normalTextureSampler;
uniform sampler2D triangleCoordinatesTextureSampler;
uniform sampler2D textureCoordinatesTextureSampler;
uniform sampler2D materialAmbientTextureSampler;
uniform sampler2D materialDiffuseTextureSampler;
uniform sampler2D materialSpecularTextureSampler;

out vec3 finalColor;

void main()
{
    vec3 position = texture(positionTextureSampler, TexCoords).rgb;
    vec3 triangleCoordinate = texture(triangleCoordinatesTextureSampler, TexCoords).rgb;
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


    vec3 lightPositionVSpace = vec3(0.0, 0.0, 0.0);

    vec3 materialAmbient = texture(materialAmbientTextureSampler, TexCoords).rgb;
    vec3 materialDiffuse = texture(materialDiffuseTextureSampler, TexCoords).rgb;
    vec3 materialSpecular = texture(materialSpecularTextureSampler, TexCoords).rgb;

    vec3 N = texture(normalTextureSampler, TexCoords).rgb;
    vec3 L = normalize((lightPositionVSpace - position));

    float incidence = dot(L, N);

    // AMBIENT
    vec3 ambient = vec3(0.1, 0.1, 0.1) * materialAmbient;

    // DIFUSE
    vec3 diffuse = incidence * materialDiffuse;

    // SPECULAR
    vec3 V = normalize((vec3(0.0, 0.0, 0.0) - position));
    vec3 H = normalize(L + V);
    float specualarFactor = pow(max(dot(N,H),0.0), 32.0);
    vec3 specular = specualarFactor * materialSpecular;

    finalColor = ambient + diffuse + specular;
}
