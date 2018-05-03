#include "mesh.h"

#include "glm/glm.hpp"
#include <QDebug>

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"

Mesh::Mesh()
{

}

int Mesh::hV(int v)
{
  return this->vertices[v].halfedge;
}

int Mesh::hF(int f)
{
  return this->faces[f].halfedge;
}

int Mesh::vH(int h)
{
  return this->halfedges[h].vertex;
}

int Mesh::fH(int h)
{
  return this->halfedges[h].face;
}

int Mesh::nH(int h)
{
  return this->halfedges[h].next;
}

int Mesh::pH(int h)
{
  return this->halfedges[h].prev;
}

int Mesh::oH(int h)
{
  return this->halfedges[h].opposite;
}

Vertex Mesh::newV(glm::vec3 pos)
{
  return { pos, -1 };
}

Face Mesh::newF()
{
  return { -1 };
}

Halfedge Mesh::newH()
{
  return { -1, -1, -1, -1, -1 };
}

void Mesh::loadObj(std::string inputFilePath)
{
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string err;
  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputFilePath.c_str());

  if (!err.empty())
  {
    qDebug() << err.c_str();
  }

  if (!ret)
  {
    exit(1);
  }

  for(size_t v=0; v < attrib.vertices.size()/3; v++)
  {
    float vx = attrib.vertices[3*v+0];
    float vy = attrib.vertices[3*v+1];
    float vz = attrib.vertices[3*v+2];
    this->vertices.push_back(this->newV(glm::vec3(vx, vy, vz)));
  }

  int totalNumVertices = this->vertices.size();

  // map from int to halfedge
  std::map<int, int> edgesSet;

  for (size_t s = 0; s < shapes.size(); s++) {
    edgesSet.clear();

    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      this->faces.push_back(this->newF());
      int fv = shapes[s].mesh.num_face_vertices[f];
      int halfEdgeIndex = this->halfedges.size();
      for (size_t v = 0; v < fv; v++) {
        int newHalfEdgeIndex = this->halfedges.size();
        this->halfedges.push_back(this->newH());
        int vertex_idx = shapes[s].mesh.indices[index_offset + v].vertex_index;
        this->vertices[vertex_idx].halfedge = newHalfEdgeIndex;
        this->halfedges[newHalfEdgeIndex].face = this->faces.size() - 1;
      }
      for (size_t v = 1; v < fv; v++) {
        int vertex_idx = shapes[s].mesh.indices[index_offset + v].vertex_index;
        this->halfedges[halfEdgeIndex + v - 1].vertex = vertex_idx;
        int prev_vertex_idx = shapes[s].mesh.indices[index_offset + v - 1].vertex_index;
        edgesSet[totalNumVertices * prev_vertex_idx + vertex_idx] = halfEdgeIndex + v - 1;
        try
        {
          this->halfedges[halfEdgeIndex + v - 1].opposite = edgesSet.at(totalNumVertices * vertex_idx + prev_vertex_idx);
          this->halfedges[edgesSet.at(totalNumVertices * vertex_idx + prev_vertex_idx)].opposite = halfEdgeIndex + v - 1;
        } catch(const std::exception& e) {}
      }
      int vertex_idx = shapes[s].mesh.indices[index_offset + 0].vertex_index;
      this->halfedges[halfEdgeIndex + fv - 1].vertex = vertex_idx;
      int prev_vertex_idx = shapes[s].mesh.indices[halfEdgeIndex + fv - 1].vertex_index;
      edgesSet[totalNumVertices * prev_vertex_idx + vertex_idx] = halfEdgeIndex + fv - 1;
      try
      {
        this->halfedges[halfEdgeIndex + fv - 1].opposite = edgesSet.at(totalNumVertices * vertex_idx + prev_vertex_idx);
        this->halfedges[edgesSet.at(totalNumVertices * vertex_idx + prev_vertex_idx)].opposite = halfEdgeIndex + fv - 1;
      } catch(const std::exception& e) {}

      for (size_t v = 1; v < fv - 1; v++) {
        this->halfedges[halfEdgeIndex + v].prev = halfEdgeIndex + v - 1;
        this->halfedges[halfEdgeIndex + v].next = halfEdgeIndex + v + 1;
      }
      this->halfedges[halfEdgeIndex].prev = halfEdgeIndex + fv - 1;
      this->halfedges[halfEdgeIndex].next = halfEdgeIndex + 1;
      this->halfedges[halfEdgeIndex + fv - 1].prev = halfEdgeIndex + fv - 2;
      this->halfedges[halfEdgeIndex + fv - 1].next = halfEdgeIndex;

      this->faces[this->faces.size() - 1].halfedge = halfEdgeIndex;
      index_offset += fv;
    }
  }
}

void Mesh::getTriangles(  std::vector<glm::vec3>* vertices,
                          std::vector<glm::vec3>* normals,
                          std::vector<unsigned int>* indices,
                          bool isFlatFaces)
{
  if (isFlatFaces)
  {
    for(int f=0; f<this->faces.size(); f++)
    {
      glm::vec3 normal = this->getFaceNormal(f);

      int h = hF(f);
      int numVertices = 0;
      int vertexInitialIndex = vertices->size();
      do
      {
        vertices->push_back(this->vertices[vH(h)].position);
        normals->push_back(normal);
        h = nH(h);
        numVertices++;
      }
      while(h != hF(f));

      for(int j=0; j<numVertices - 2; j++)
      {
        indices->push_back(vertexInitialIndex);
        indices->push_back(vertexInitialIndex+j+1);
        indices->push_back(vertexInitialIndex+j+2);
      }
    }
  }
  else
  {
    for(int v=0; v < this->vertices.size(); v++)
    {
      vertices->push_back(this->vertices[v].position);
      normals->push_back(this->getVertexNormal(v));
    }
    for(int f=0; f < this->faces.size(); f++)
    {
      indices->push_back(vH(hF(f)));
      indices->push_back(vH(nH(hF(f))));
      indices->push_back(vH(nH(nH(hF(f)))));
    }
  }

}

glm::vec3 Mesh::getFaceNormal(int f)
{
  Vertex v1 = this->vertices[vH(hF(f))];
  Vertex v2 = this->vertices[vH(nH(hF(f)))];
  Vertex v3 = this->vertices[vH(nH(nH(hF(f))))];
  return glm::normalize(glm::cross(v2.position - v1.position, v3.position - v1.position));
}

glm::vec3 Mesh::getVertexNormal(int v)
{
  glm::vec3 normal;
  std::vector<int> faces = this->getVertexFacesIndexes(v);
  for(int i=0; i < faces.size(); i++)
  {
    normal += this->getFaceNormal(faces[i]) / ((float) faces.size());
  }
  return normal;
}

std::vector<int> Mesh::getVertexFacesIndexes(int v)
{
  std::vector<int> output;
  int hi = hV(v);
  do
  {
    output.push_back(fH(hi));
    hi = oH(pH(hi));
  }
  while(hi != hV(v));
  return output;
}
