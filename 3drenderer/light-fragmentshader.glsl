#version 460 core

uniform bool isWireframeOverwrite;
uniform bool isEdgesVisible;
uniform bool isFlatFaces;
uniform bool isDiffuseTextureActive;
uniform bool isBumpMapActive;

uniform vec3 diffuseColor;
uniform float materialShininess;

uniform sampler2D diffuseTextureSampler;
uniform sampler2D bumpMapSampler;

in vec3 fragmentPositionVSpace;
in vec3 fragmentNormalVSpace;
in vec3 fragmentTangentVSpace;
in vec3 fragmentBitangentVSpace;
in vec3 fragmentTriangleCoordinate;
in vec2 fragmentUV;

layout (location = 0) out vec3 WorldPosOut;
layout (location = 1) out vec3 NormalOut;
layout (location = 2) out vec3 TriangleCoordinatesOut;
layout (location = 3) out vec3 TexCoordOut;
layout (location = 4) out vec3 MaterialAmbientOut;
layout (location = 5) out vec3 MaterialDiffuseOut;
layout (location = 6) out vec3 MaterialSpecularOut;


void main()
{
    vec3 lightPositionVSpace = vec3(0.0, 0.0, 0.0);

    vec3 materialAmbient = diffuseColor;
    vec3 materialDiffuse = diffuseColor;
    vec3 materialSpecular = vec3(1.0, 1.0, 1.0);

    if(isDiffuseTextureActive)
    {
      materialAmbient = texture(diffuseTextureSampler, fragmentUV).bgr;
      materialDiffuse = texture(diffuseTextureSampler, fragmentUV).bgr;
    }

    vec3 N = normalize(fragmentNormalVSpace);

    if(isBumpMapActive)
    {
      vec3 bump = ((texture(bumpMapSampler, fragmentUV).bgr * 2.0) - 1.0);
      N = bump.r * fragmentTangentVSpace +
          bump.g * fragmentBitangentVSpace +
          bump.b * fragmentNormalVSpace;
    }

    WorldPosOut = fragmentPositionVSpace;
    NormalOut = N;
    TriangleCoordinatesOut = fragmentTriangleCoordinate;
    TexCoordOut = fragmentUV;
    MaterialAmbientOut = materialAmbient;
    MaterialDiffuseOut = materialDiffuse;
    MaterialSpecularOut = materialSpecular;
}
