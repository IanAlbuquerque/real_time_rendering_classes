#include "mesh.h"

#include "glm/glm.hpp"
#include <QDebug>

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"

Vertex::Vertex(float x, float y, float z)
{
  this->position = glm::vec3(x, y, z);
  this->halfedge = NULL;
}

Face::Face()
{
  this->halfedge = NULL;
}

Halfedge::Halfedge()
{
  this->vertex = NULL;
  this->face = NULL;
  this->next = NULL;
  this->prev = NULL;
  this->opposite = NULL;
}

Mesh::Mesh()
{

}

void Mesh::avgSmoothing()
{
  std::vector<glm::vec3> newVerticesPositions;
  for(int i=0; i<this->vertices.size(); i++)
  {
    std::vector<glm::vec3> adjVerticesPositions;
    Halfedge* startingHalfedge = this->vertices[i]->halfedge;
    adjVerticesPositions.push_back(startingHalfedge->vertex->position);
    Halfedge* currentHalfedge = startingHalfedge->prev->opposite;
    do
    {
      adjVerticesPositions.push_back(currentHalfedge->vertex->position);
      currentHalfedge = currentHalfedge->prev->opposite;
    }
    while(currentHalfedge != startingHalfedge);

    glm::vec3 avgPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    for(int j=0; j<adjVerticesPositions.size(); j++)
    {
      avgPosition += adjVerticesPositions[j];
    }
    avgPosition /= (float) adjVerticesPositions.size();

    newVerticesPositions.push_back(avgPosition);
  }
  for(int i=0; i<this->vertices.size(); i++)
  {
    this->vertices[i]->position += 0.5f * (newVerticesPositions[i] - this->vertices[i]->position);
  }
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
    // `err` may contain warning message.
    qDebug() << err.c_str();
  }

  if (!ret)
  {
    exit(1);
  }

  for(size_t v=0; v < attrib.vertices.size()/3; v++)
  {
    // qDebug() << v << attrib.vertices.size();
    float vx = attrib.vertices[3*v+0];
    float vy = attrib.vertices[3*v+1];
    float vz = attrib.vertices[3*v+2];
    this->vertices.push_back(new Vertex(vx, vy, vz));
  }

  int totalNumVertices = this->vertices.size();
  // Halfedge** edgesSet = new Halfedge*[totalNumVertices * totalNumVertices];
  std::map<int, Halfedge*> edgesSet;

  for (size_t s = 0; s < shapes.size(); s++) {
    // Loop over faces(polygon)

//    for(int vi=0; vi<totalNumVertices; vi++)
//    {
//      for(int vj=0; vj<totalNumVertices; vj++)
//      {
//        edgesSet[totalNumVertices * vi + vj] = NULL;
//      }
//    }
    edgesSet.clear();

    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      this->faces.push_back(new Face());
      int fv = shapes[s].mesh.num_face_vertices[f];
      int halfEdgeIndex = this->halfedges.size();
      for (size_t v = 0; v < fv; v++) {
        Halfedge* newHalfEdge = new Halfedge();
        this->halfedges.push_back(newHalfEdge);
        int vertex_idx = shapes[s].mesh.indices[index_offset + v].vertex_index;
        this->vertices[vertex_idx]->halfedge = newHalfEdge;
        newHalfEdge->face = this->faces[this->faces.size() - 1];
      }
      for (size_t v = 1; v < fv; v++) {
        int vertex_idx = shapes[s].mesh.indices[index_offset + v].vertex_index;
        this->halfedges[halfEdgeIndex + v - 1]->vertex = this->vertices[vertex_idx];
        int prev_vertex_idx = shapes[s].mesh.indices[index_offset + v - 1].vertex_index;
        edgesSet[totalNumVertices * prev_vertex_idx + vertex_idx] = this->halfedges[halfEdgeIndex + v - 1];
        try
        {
          this->halfedges[halfEdgeIndex + v - 1]->opposite = edgesSet.at(totalNumVertices * vertex_idx + prev_vertex_idx);
          edgesSet.at(totalNumVertices * vertex_idx + prev_vertex_idx)->opposite = this->halfedges[halfEdgeIndex + v - 1];
        } catch(const std::exception& e) {}
      }
      int vertex_idx = shapes[s].mesh.indices[index_offset + 0].vertex_index;
      this->halfedges[halfEdgeIndex + fv - 1]->vertex = this->vertices[vertex_idx];
      int prev_vertex_idx = shapes[s].mesh.indices[halfEdgeIndex + fv - 1].vertex_index;
      edgesSet[totalNumVertices * prev_vertex_idx + vertex_idx] = this->halfedges[halfEdgeIndex + fv - 1];
      try
      {
        this->halfedges[halfEdgeIndex + fv - 1]->opposite = edgesSet.at(totalNumVertices * vertex_idx + prev_vertex_idx);
        edgesSet.at(totalNumVertices * vertex_idx + prev_vertex_idx)->opposite = this->halfedges[halfEdgeIndex + fv - 1];
      } catch(const std::exception& e) {}

      for (size_t v = 1; v < fv - 1; v++) {
        this->halfedges[halfEdgeIndex + v]->prev = this->halfedges[halfEdgeIndex + v - 1];
        this->halfedges[halfEdgeIndex + v]->next = this->halfedges[halfEdgeIndex + v + 1];
      }
      this->halfedges[halfEdgeIndex]->prev = this->halfedges[halfEdgeIndex + fv - 1];
      this->halfedges[halfEdgeIndex]->next = this->halfedges[halfEdgeIndex + 1];
      this->halfedges[halfEdgeIndex + fv - 1]->prev = this->halfedges[halfEdgeIndex + fv - 2];
      this->halfedges[halfEdgeIndex + fv - 1]->next = this->halfedges[halfEdgeIndex];

      this->faces[this->faces.size() - 1]->halfedge = this->halfedges[halfEdgeIndex];
      index_offset += fv;
    }
  }
}

void Mesh::loadPyramid()
{
  this->vertices.push_back(new Vertex(-1.0f, -1.0f, -1.0f));
  this->vertices.push_back(new Vertex(1.0f, -1.0f, -1.0f));
  this->vertices.push_back(new Vertex(1.0f, -1.0f, 1.0f));
  this->vertices.push_back(new Vertex(-1.0f, -1.0f, 1.0f));
  this->vertices.push_back(new Vertex(0.0f, 1.0f, 0.0f));
  for(int i=0; i<5; i++)
    this->faces.push_back(new Face());
  for(int i=0; i<16; i++)
    this->halfedges.push_back(new Halfedge());

  this->vertices[0]->halfedge = this->halfedges[0];
  this->vertices[1]->halfedge = this->halfedges[1];
  this->vertices[2]->halfedge = this->halfedges[2];
  this->vertices[3]->halfedge = this->halfedges[3];
  this->vertices[4]->halfedge = this->halfedges[6];

  this->faces[0]->halfedge = this->halfedges[0];
  this->faces[1]->halfedge = this->halfedges[4];
  this->faces[2]->halfedge = this->halfedges[7];
  this->faces[3]->halfedge = this->halfedges[10];
  this->faces[4]->halfedge = this->halfedges[13];

  this->halfedges[0]->vertex = this->vertices[0];
  this->halfedges[1]->vertex = this->vertices[1];
  this->halfedges[2]->vertex = this->vertices[2];
  this->halfedges[3]->vertex = this->vertices[3];
  this->halfedges[4]->vertex = this->vertices[2];
  this->halfedges[5]->vertex = this->vertices[1];
  this->halfedges[6]->vertex = this->vertices[4];
  this->halfedges[7]->vertex = this->vertices[1];
  this->halfedges[8]->vertex = this->vertices[0];
  this->halfedges[9]->vertex = this->vertices[4];
  this->halfedges[10]->vertex = this->vertices[0];
  this->halfedges[11]->vertex = this->vertices[3];
  this->halfedges[12]->vertex = this->vertices[4];
  this->halfedges[13]->vertex = this->vertices[3];
  this->halfedges[14]->vertex = this->vertices[2];
  this->halfedges[15]->vertex = this->vertices[4];

  this->halfedges[0]->face = this->faces[0];
  this->halfedges[1]->face = this->faces[0];
  this->halfedges[2]->face = this->faces[0];
  this->halfedges[3]->face = this->faces[0];
  this->halfedges[4]->face = this->faces[1];
  this->halfedges[5]->face = this->faces[1];
  this->halfedges[6]->face = this->faces[1];
  this->halfedges[7]->face = this->faces[2];
  this->halfedges[8]->face = this->faces[2];
  this->halfedges[9]->face = this->faces[2];
  this->halfedges[10]->face = this->faces[3];
  this->halfedges[11]->face = this->faces[3];
  this->halfedges[12]->face = this->faces[3];
  this->halfedges[13]->face = this->faces[4];
  this->halfedges[14]->face = this->faces[4];
  this->halfedges[15]->face = this->faces[4];

  this->halfedges[0]->next = this->halfedges[1];
  this->halfedges[1]->next = this->halfedges[2];
  this->halfedges[2]->next = this->halfedges[3];
  this->halfedges[3]->next = this->halfedges[0];
  this->halfedges[4]->next = this->halfedges[5];
  this->halfedges[5]->next = this->halfedges[6];
  this->halfedges[6]->next = this->halfedges[4];
  this->halfedges[7]->next = this->halfedges[8];
  this->halfedges[8]->next = this->halfedges[9];
  this->halfedges[9]->next = this->halfedges[7];
  this->halfedges[10]->next = this->halfedges[11];
  this->halfedges[11]->next = this->halfedges[12];
  this->halfedges[12]->next = this->halfedges[10];
  this->halfedges[13]->next = this->halfedges[14];
  this->halfedges[14]->next = this->halfedges[15];
  this->halfedges[15]->next = this->halfedges[13];

  this->halfedges[0]->prev = this->halfedges[3];
  this->halfedges[1]->prev = this->halfedges[0];
  this->halfedges[2]->prev = this->halfedges[1];
  this->halfedges[3]->prev = this->halfedges[2];
  this->halfedges[4]->prev = this->halfedges[6];
  this->halfedges[5]->prev = this->halfedges[4];
  this->halfedges[6]->prev = this->halfedges[5];
  this->halfedges[7]->prev = this->halfedges[9];
  this->halfedges[8]->prev = this->halfedges[7];
  this->halfedges[9]->prev = this->halfedges[8];
  this->halfedges[10]->prev = this->halfedges[12];
  this->halfedges[11]->prev = this->halfedges[10];
  this->halfedges[12]->prev = this->halfedges[11];
  this->halfedges[13]->prev = this->halfedges[15];
  this->halfedges[14]->prev = this->halfedges[13];
  this->halfedges[15]->prev = this->halfedges[14];

  this->halfedges[0]->opposite = this->halfedges[7];
  this->halfedges[1]->opposite = this->halfedges[4];
  this->halfedges[2]->opposite = this->halfedges[13];
  this->halfedges[3]->opposite = this->halfedges[10];
  this->halfedges[4]->opposite = this->halfedges[1];
  this->halfedges[5]->opposite = this->halfedges[9];
  this->halfedges[6]->opposite = this->halfedges[14];
  this->halfedges[7]->opposite = this->halfedges[0];
  this->halfedges[8]->opposite = this->halfedges[12];
  this->halfedges[9]->opposite = this->halfedges[5];
  this->halfedges[10]->opposite = this->halfedges[3];
  this->halfedges[11]->opposite = this->halfedges[15];
  this->halfedges[12]->opposite = this->halfedges[8];
  this->halfedges[13]->opposite = this->halfedges[2];
  this->halfedges[14]->opposite = this->halfedges[6];
  this->halfedges[15]->opposite = this->halfedges[11];
}

void Mesh::getTriangles(std::vector<glm::vec3>* vertices, std::vector<glm::vec3>* normals, std::vector<unsigned int>* indices)
{
  for(int i=0; i<this->faces.size(); i++)
  {
    Halfedge* he = this->faces[i]->halfedge;

    Vertex* v1 = he->vertex;
    Vertex* v2 = he->next->vertex;
    Vertex* v3 = he->next->next->vertex;
    glm::vec3 normal = glm::normalize(glm::cross(v2->position - v1->position, v3->position - v1->position));

    Halfedge* he_i = he;
    int numVertices = 0;
    int vertexInitialIndex = vertices->size();
    do
    {
      vertices->push_back(he_i->vertex->position);
      normals->push_back(normal);
      he_i = he_i->next;
      numVertices++;
    }
    while(he_i != he);

    for(int j=0; j<numVertices - 2; j++)
    {
      indices->push_back(vertexInitialIndex);
      indices->push_back(vertexInitialIndex+j+1);
      indices->push_back(vertexInitialIndex+j+2);
    }
  }
}
