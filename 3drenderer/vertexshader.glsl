#version 460 core

layout( location = 0 ) in vec3 vertexPositionMSpace;
layout( location = 1 ) in vec3 vertexNormalMSpace;

uniform mat4 mvp;
uniform mat4 mv;
uniform mat4 mv_ti;

out vec3 vertexPositionVSpace;
out vec3 vertexNormalVSpace;

void main()
{
    gl_Position = mvp * vec4(vertexPositionMSpace, 1.0);

    vertexPositionVSpace = ( mv * vec4(vertexPositionMSpace,1.0) ).xyz;
    vertexNormalVSpace = ( mv * vec4(vertexNormalMSpace,0.0) ).xyz;
}
