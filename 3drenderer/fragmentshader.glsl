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

out vec3 finalColor;

void main()
{
    if((isWireframeOverwrite || isEdgesVisible) && (fragmentTriangleCoordinate.x < 0.01 || fragmentTriangleCoordinate.y < 0.01 || fragmentTriangleCoordinate.z < 0.01))
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

    vec3 materialAmbient = diffuseColor;
    vec3 materialDiffuse = diffuseColor;
    vec3 materialSpecular = vec3(1.0, 1.0, 1.0);

    if(isDiffuseTextureActive)
    {
      materialAmbient = texture(diffuseTextureSampler, fragmentUV).bgr;
      materialDiffuse = texture(diffuseTextureSampler, fragmentUV).bgr;
    }

    vec3 N = normalize(fragmentNormalVSpace);
    vec3 realN = N;
    vec3 L = normalize((lightPositionVSpace - fragmentPositionVSpace));

    if(isBumpMapActive)
    {
      vec3 bump = ((texture(bumpMapSampler, fragmentUV).bgr * 2.0) - 1.0);
      N = bump.r * fragmentTangentVSpace +
          bump.g * fragmentBitangentVSpace +
          bump.b * fragmentNormalVSpace;
    }

    float realIncidence = dot(L, realN);
    float incidence = dot(L, N);

    if (realIncidence < 0)
    {
      discard;
      return;
    }

    // AMBIENT
    vec3 ambient = vec3(0.1, 0.1, 0.1) * materialAmbient;

    // DIFUSE
    vec3 diffuse = incidence * materialDiffuse;

    // SPECULAR
    vec3 V = normalize((vec3(0.0, 0.0, 0.0) - fragmentPositionVSpace));
    vec3 H = normalize(L + V);
    float specualarFactor = pow(max(dot(N,H),0.0), materialShininess);
    vec3 specular = specualarFactor * materialSpecular;

    finalColor = ambient + diffuse + specular;
}
