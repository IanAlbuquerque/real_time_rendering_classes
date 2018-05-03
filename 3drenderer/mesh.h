#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include "glm/glm.hpp"
#include <vector>
#include <string>

struct Vertex
{
  glm::vec3 position;
  // outgoing halfedge
  int halfedge;
};

struct Face
{
  // one of its halfedges
  int halfedge;
};

struct Halfedge
{
  // vertex it points
  int vertex;
  int face;
  int next;
  int prev;
  int opposite;
};

class Mesh
{
private:
  std::vector<Vertex> vertices;
  std::vector<Face> faces;
  std::vector<Halfedge> halfedges;

  Vertex newV(glm::vec3 pos);
  Face newF();
  Halfedge newH();

  int  hV(int v);
  int  hF(int f);
  int  vH(int h);
  int  fH(int h);
  int  nH(int h);
  int  pH(int h);
  int  oH(int h);

  std::vector<int> getVertexFacesIndexes(int v);
  glm::vec3 getFaceNormal(int f);
  glm::vec3 getVertexNormal(int v);
public:
  Mesh();
  void getTriangles(std::vector<glm::vec3>* vertices, std::vector<glm::vec3>* normals, std::vector<unsigned int>* indices, bool isFlatFaces);
  void loadObj(std::string inputFilePath);
};

#endif // TRIANGLEMESH_H
