#version 460 core

uniform bool isWireframeOverwrite;
uniform bool isEdgesVisible;
uniform bool isFlatFaces;
uniform bool isDiffuseTextureActive;
uniform bool isBumMapActive;
uniform vec3 diffuseColor;

in vec3 fragmentPositionVSpace;
in vec3 fragmentNormalVSpace;
in vec3 fragmentTriangleCoordinate;

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
    vec3 materialDiffuse = diffuseColor;
    vec3 materialSpecular = vec3(1.0, 1.0, 1.0);

    float materialShininess = 24.0;

    vec3 ambient = vec3(0.1, 0.1, 0.1);
    vec3 diffuse = vec3(0.0, 0.0, 0.0);
    vec3 specular = vec3(0.0, 0.0, 0.0);

    vec3 N = normalize(fragmentNormalVSpace);
    vec3 L = normalize(lightPositionVSpace - fragmentPositionVSpace);

    float iDif = dot(L,N);

    if( iDif > 0 )
    {
        diffuse = iDif * materialDiffuse;

        vec3 V = normalize(-fragmentPositionVSpace);
        vec3 H = normalize(L + V);

        float iSpec = pow(max(dot(N,H),0.0), materialShininess);
        specular = iSpec * materialSpecular;
    }
    else
    {
        discard;
        return;
    }

    finalColor = ambient + diffuse + specular;
}
