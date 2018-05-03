#version 460 core

layout( location = 0 ) in vec3 vertexPositionMSpace;
layout( location = 1 ) in vec3 vertexNormalMSpace;
layout( location = 2 ) in vec3 vertexTangentMSpace;
layout( location = 3 ) in vec3 vertexBitangentMSpace;
layout( location = 4 ) in vec2 vertexTextureCoord;

uniform mat4 mvp;
uniform mat4 mv;
uniform mat4 mv_ti;

out vec3 vertexPositionVSpace;
out vec3 vertexNormalVSpace;
out vec3 vertexTangentVSpace;
out vec3 vertexBitangentVSpace;
out vec2 vertexTextureVSpace;

void main()
{
  gl_Position = mvp * vec4(vertexPositionMSpace, 1.0);

  vertexPositionVSpace = ( mv * vec4(vertexPositionMSpace, 1.0) ).xyz;

  vertexNormalVSpace = ( mv_ti * vec4(vertexNormalMSpace, 0.0) ).xyz;
  vertexTangentVSpace = ( mv_ti * vec4(vertexTangentMSpace, 0.0) ).xyz;
  vertexBitangentVSpace = ( mv_ti * vec4(vertexBitangentMSpace, 0.0) ).xyz;

  vertexTextureVSpace = vertexTextureCoord;
}
