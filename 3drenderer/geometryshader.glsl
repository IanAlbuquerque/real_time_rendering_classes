#version 150 compatibility

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 vertexPositionVSpace[];
in vec3 vertexNormalVSpace[];

out vec3 fragmentPositionVSpace;
out vec3 fragmentNormalVSpace;
out vec3 fragmentTriangleCoordinate;

void main()
{

    gl_Position = gl_in[0].gl_Position;
    fragmentTriangleCoordinate = vec3(1, 0, 0);
    fragmentPositionVSpace = vertexPositionVSpace[0];
    fragmentNormalVSpace = vertexNormalVSpace[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    fragmentTriangleCoordinate = vec3(0, 1, 0);
    fragmentPositionVSpace = vertexPositionVSpace[1];
    fragmentNormalVSpace = vertexNormalVSpace[1];
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    fragmentTriangleCoordinate = vec3(0, 0, 1);
    fragmentPositionVSpace = vertexPositionVSpace[2];
    fragmentNormalVSpace = vertexNormalVSpace[2];
    EmitVertex();

    EndPrimitive();
}
