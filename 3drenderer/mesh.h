#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include "glm/glm.hpp"
#include <vector>
#include <string>

class Vertex;
class Face;
class Halfedge;
class Mesh;

class Vertex
{
friend class Mesh;
private:
  glm::vec3 position;
  Halfedge *halfedge;
public:
  Vertex(float x, float y, float z);
};

class Face
{
friend class Mesh;
private:
  Halfedge *halfedge;
public:
  Face();
};

class Halfedge
{
friend class Mesh;
private:
  Vertex *vertex;
  Face *face;
  Halfedge *next;
  Halfedge *prev;
  Halfedge *opposite;
public:
  Halfedge();
};

class Mesh
{
private:
   std::vector<Vertex*> vertices;
   std::vector<Face*> faces;
   std::vector<Halfedge*> halfedges;
public:
  Mesh();
  void loadPyramid();
  void getTriangles(std::vector<glm::vec3>* vertices, std::vector<glm::vec3>* normals, std::vector<unsigned int>* indices);
  void loadObj(std::string inputFilePath);
  void avgSmoothing();
};

#endif // TRIANGLEMESH_H
