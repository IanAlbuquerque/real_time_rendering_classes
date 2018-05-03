#include "renderwidget.h"

#include <QDebug>

#include <QImage>
#include <QGLWidget>
#include <QMouseEvent>
#include <QOpenGLTexture>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"
#include "mesh.h"
#include "uvsphericalwrapper.h"
#include "uvcubewrapper.h"

#include <cmath>
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

RenderWidget::RenderWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , program(nullptr)
{
  this->setFocusPolicy(Qt::StrongFocus);
  this->isArcballMovementActive = false;
  this->isPanMovementActive = false;

  this->resetCamera();

  this->isWireframeOverwrite = false;
  this->isEdgesVisible = false;
  this->isFlatFaces = false;
  this->isDiffuseTextureActive = false;
  this->isBumpMapActive = false;

  this->diffuseColor = glm::vec3(0.0f, 0.0f, 0.0f);
  this->materialShininess = 24.0f;
  this->isSphericalMapping = true;
}


RenderWidget::~RenderWidget()
{
  delete this->program;
  delete this->camera;
  delete this->mesh;

  this->glDeleteVertexArrays(1, &VAO);
  this->glDeleteBuffers(1, &VBO);
  this->glDeleteBuffers(1, &EBO);
}


void RenderWidget::initializeGL()
{
  this->initializeOpenGLFunctions();

  this->glEnable(GL_DEPTH_TEST);

  this->glClearColor(0, 0, 0, 1);
  this->glViewport(0, 0, width(), height());

  this->program = new QOpenGLShaderProgram();
  this->program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertexshader.glsl");
  this->program->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/shaders/geometryshader.glsl");
  this->program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragmentshader.glsl");
  this->program->link();

  this->createBuffers(&(this->VAO),
                      &(this->VBO),
                      &(this->EBO));
  this->countElements = 0;        
  this->createTexture(&(this->DIFFUSE_TEXTURE_2D));
  this->createTexture(&(this->BUMP_TEXTURE_2D));              
}


void RenderWidget::paintGL()
{
  this->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  this->glBindVertexArray(this->VAO);
  this->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);

  this->program->bind();

  this->view = this->camera->getViewMatrix();
  this->proj = this->camera->getProjectionMatrix();
  this->model = glm::mat4();
  //  this->model = glm::scale(this->model, glm::vec3(10.0f, 10.0f, 10.0f));

  QMatrix4x4 m(glm::value_ptr(glm::transpose(this->model)));
  QMatrix4x4 v(glm::value_ptr(glm::transpose(this->view)));
  QMatrix4x4 p(glm::value_ptr(glm::transpose(this->proj)));

  QMatrix4x4 mv = v * m;
  QMatrix4x4 mvp = p * mv;
  this->program->setUniformValue("mv", mv);
  this->program->setUniformValue("mv_ti", mv.inverted().transposed());
  this->program->setUniformValue("mvp", mvp);

  this->program->setUniformValue("isWireframeOverwrite", this->isWireframeOverwrite);
  this->program->setUniformValue("isEdgesVisible", this->isEdgesVisible);
  this->program->setUniformValue("isFlatFaces", this->isFlatFaces);
  this->program->setUniformValue("isDiffuseTextureActive", this->isDiffuseTextureActive);
  this->program->setUniformValue("isBumpMapActive", this->isBumpMapActive);

  
  this->glActiveTexture(GL_TEXTURE0);
  this->glBindTexture(GL_TEXTURE_2D, this->DIFFUSE_TEXTURE_2D);
  this->program->setUniformValue("diffuseTextureSampler", 0);

  this->glActiveTexture(GL_TEXTURE1);
  this->glBindTexture(GL_TEXTURE_2D, this->BUMP_TEXTURE_2D);
  this->program->setUniformValue("bumpMapSampler", 1);

  this->program->setUniformValue("diffuseColor", QVector3D( this->diffuseColor[0],
                                                            this->diffuseColor[1],
                                                            this->diffuseColor[2]));

  this->program->setUniformValue("materialShininess", this->materialShininess);

  this->glDrawElements( GL_TRIANGLES,
                        (GLsizei) this->countElements,
                        GL_UNSIGNED_INT,
                        (void *) 0);
}


void RenderWidget::resizeGL(int width, int height)
{
  this->glViewport(0, 0, width, height);
  this->camera->updateWH(width, height);
}


void RenderWidget::keyPressEvent(QKeyEvent *event)
{
  switch(event->key())
  {
    default:
      break;
  }
}

void RenderWidget::keyReleaseEvent(QKeyEvent *event)
{
}

void RenderWidget::mousePressEvent(QMouseEvent *event)
{
  glm::vec2 clickPosition = glm::vec2(event->x(), -1.0f * event->y() + this->height());
  int buttons = event->buttons();
  if(buttons & Qt::LeftButton)
  {
    this->isArcballMovementActive = true;
    this->lastArcballScreenCoordinates = clickPosition;
  }
  if(buttons & Qt::MidButton)
  {
    this->isPanMovementActive = true;
    this->lastPanScreenCoordinates = clickPosition;
  }
}

void RenderWidget::mouseReleaseEvent(QMouseEvent *event)
{
  int buttons = ~event->buttons();
  if(buttons & Qt::LeftButton)
  {
    this->isArcballMovementActive = false;
  }
  if(buttons & Qt::MidButton)
  {
    this->isPanMovementActive = false;
  }
}

void RenderWidget::mouseMoveEvent(QMouseEvent *event)
{
  glm::vec2 clickPosition = glm::vec2(event->x(), -1.0f * event->y() + this->height());
  if(this->isArcballMovementActive)
  {
    glm::vec2 currentArcballScreenCoordinates = clickPosition;
    this->camera->arcballMoveScreenCoordinates(this->lastArcballScreenCoordinates, currentArcballScreenCoordinates);
    this->lastArcballScreenCoordinates = currentArcballScreenCoordinates;
    this->update();
  }
  if(this->isPanMovementActive)
  {
    glm::vec2 currentPanScreenCoordinates = clickPosition;
    this->camera->cameraPan(currentPanScreenCoordinates - this->lastPanScreenCoordinates);
    this->lastPanScreenCoordinates = currentPanScreenCoordinates;
    this->update();
  }
}

void RenderWidget::wheelEvent(QWheelEvent *event)
{
  this->camera->zoomBy(event->delta());
  this->update();
}

void RenderWidget::importOBJFromPath(char* path)
{
  this->mesh = new Mesh();
  this->mesh->loadObj(path);
  this->reloadMesh();
}

void RenderWidget::importDiffuseTexture(QImage img)
{
  this->loadTexture(this->DIFFUSE_TEXTURE_2D, img);
}

void RenderWidget::importBumpMap(QImage img)
{
  this->loadTexture(this->BUMP_TEXTURE_2D, img);
}

void RenderWidget::createTexture(unsigned int* textureID)
{
  this->glGenTextures(1, textureID);
}

void RenderWidget::loadTexture(unsigned int textureID, QImage img)
{
  this->glBindTexture(GL_TEXTURE_2D, textureID);

  this->glTexImage2D( GL_TEXTURE_2D,
                      0,
                      GL_RGBA,
                      img.width(),
                      img.height(),
                      0,
                      GL_RGBA,
                      GL_UNSIGNED_BYTE,
                      img.bits());

  this->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  this->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  this->glGenerateMipmap(GL_TEXTURE_2D);
}

void RenderWidget::createBuffers( unsigned int* VAO,
                                  unsigned int* VBO,
                                  unsigned int* EBO)
{
  this->glGenVertexArrays(1, (GLuint*) VAO);
  this->glBindVertexArray((GLuint) VAO);
  this->glGenBuffers(1, (GLuint*) VBO);
  this->glGenBuffers(1, (GLuint*) EBO);
  
  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec3> tangents;
  std::vector<glm::vec3> bitangents;
  std::vector<glm::vec2> UVs;
  std::vector<unsigned int> indices;

  this->loadBuffers(*VAO,
                    *VBO,
                    *EBO,
                    positions,
                    normals,
                    tangents,
                    bitangents,
                    UVs,
                    indices);
}
                    
void RenderWidget::loadBuffers( unsigned int VAO,
                  unsigned int VBO,
                  unsigned int EBO,
                  std::vector<glm::vec3>& positions,
                  std::vector<glm::vec3>& normals,
                  std::vector<glm::vec3>& tangents,
                  std::vector<glm::vec3>& bitangents,
                  std::vector<glm::vec2>& UVs,
                  std::vector<unsigned int>& indices)
{
  // Data structure to be stored in the VBO
  struct Vertex
  {
      glm::vec3 pos;
      glm::vec3 normal;
      glm::vec3 tangent;
      glm::vec3 bitangent;
      glm::vec2 uv;
  };

  // Intercalating Data in the VBO
  std::vector<Vertex> vboDataArray;

  vboDataArray.reserve(positions.size());
  for (unsigned int i = 0; i < positions.size(); i++)
  {
    vboDataArray.push_back({
      positions[i],
      normals[i],
      tangents[i],
      bitangents[i],
      UVs[i]
    });
  }

  // Binds the Current VAO
  this->glBindVertexArray(VAO);

  // Memory Allocation
  this->glBindBuffer(GL_ARRAY_BUFFER, VBO);
  this->glBufferData( GL_ARRAY_BUFFER,
                      vboDataArray.size() * sizeof(Vertex),
                      &vboDataArray[0],
                      GL_DYNAMIC_DRAW);

  this->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO);
  this->glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                      indices.size() * sizeof(unsigned int),
                      &indices[0],
                      GL_DYNAMIC_DRAW);

  // Position Attribute
  this->glEnableVertexAttribArray( 0 );
  this->glVertexAttribPointer(  0,                            // index
                                3,                            // size
                                GL_FLOAT,                     // type
                                GL_FALSE,                     // normalized
                                sizeof(Vertex),               // stride
                                (void*) 0 );                  // pointer

  // Normal Attribute
  this->glEnableVertexAttribArray( 1 );
  this->glVertexAttribPointer(  1,                            // index
                                3,                            // size
                                GL_FLOAT,                     // type
                                GL_FALSE,                     // normalized
                                sizeof(Vertex),               // stride
                                (void*) sizeof(glm::vec3) );  // pointer

  // Tangent Attribute
  this->glEnableVertexAttribArray( 2 );
  this->glVertexAttribPointer(  2,                            // index
                                3,                            // size
                                GL_FLOAT,                     // type
                                GL_FALSE,                     // normalized
                                sizeof(Vertex),               // stride
                                (void*) (2 * sizeof(glm::vec3)) );  // pointer

  // Bitangent Attribute
  this->glEnableVertexAttribArray( 3 );
  this->glVertexAttribPointer(  3,                            // index
                                3,                            // size
                                GL_FLOAT,                     // type
                                GL_FALSE,                     // normalized
                                sizeof(Vertex),               // stride
                                (void*) (3 * sizeof(glm::vec3)) );  // pointer

  // UV Attribute
  this->glEnableVertexAttribArray( 4 );
  this->glVertexAttribPointer(  4,                                  // index
                                2,                                  // size
                                GL_FLOAT,                           // type
                                GL_FALSE,                           // normalized
                                sizeof(Vertex),                     // stride
                                (void*) (4 * sizeof(glm::vec3)) );  // pointer
}

void RenderWidget::setWireframeOverwrite(bool value)
{
  this->isWireframeOverwrite = value;
  this->update();  
}

void RenderWidget::setEdgesVisible(bool value)
{
  this->isEdgesVisible = value;
  this->update();  
}

void RenderWidget::setFlatFaces(bool value)
{
  this->isFlatFaces = value;
  this->reloadMesh();
}

void RenderWidget::setDiffuseTextureActive(bool value)
{
  this->isDiffuseTextureActive = value;
  this->update();  
}

void RenderWidget::setBumMapActive(bool value)
{
  this->isBumpMapActive = value;
  this->update();  
}

void RenderWidget::setDiffuseColor(float r, float g, float b)
{
  this->diffuseColor = glm::vec3(r, g, b);
  this->update();  
}

void RenderWidget::reloadMesh()
{
  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec3> tangents;
  std::vector<glm::vec3> bitangents;
  std::vector<glm::vec2> UVs;
  std::vector<unsigned int> indices;

  this->mesh->getTriangles(&positions, &normals, &indices, this->isFlatFaces);
  this->countElements = indices.size();

  UVSphericalWrapper uvSphericalWrapper;
  UVCubeWrapper uvCubeWrapper;
  uvCubeWrapper.runBoundingBox(positions);
  for(int i=0; i<positions.size(); i++)
  {
    glm::vec2 uv = this->isSphericalMapping ? uvSphericalWrapper.uv(positions[i]) : uvCubeWrapper.uv(positions[i]);
    UVs.push_back(uv);
  }

  this->computeTangentBasis(  positions,
                              normals,
                              UVs,
                              indices,
                              tangents,
                              bitangents);

  this->loadBuffers(  this->VAO,
                      this->VBO,
                      this->EBO,
                      positions,
                      normals,
                      tangents,
                      bitangents,
                      UVs,
                      indices);
  this->update();  
}

void RenderWidget::resetCamera()
{
  delete this->camera;
  this->camera = new Camera(
    glm::vec3(0.0f, 0.0f, 2.0f), // eye
    glm::vec3(0.0f, 0.0f, 0.0f),  // at
    glm::vec3(0.0f, 1.0f, 0.0f),  // up
    30.0f,                        // fovy
    0.001f,                       // zNear
    10.0f,                        // zFar
    this->width(),                // width
    this->height()                // height
    );
  this->update();
}

void RenderWidget::setShininess(float s)
{
  this->materialShininess = s;
  this->update();
}

void RenderWidget::setSphericalMapping(bool v)
{
   this->isSphericalMapping = v;
   this->reloadMesh();
}

void RenderWidget::computeTangentBasis( std::vector<glm::vec3> & positions,
                          std::vector<glm::vec3> & normals,
                          std::vector<glm::vec2> & UVs,
                          std::vector<unsigned int> & indices,
                          std::vector<glm::vec3>& tangents,
                          std::vector<glm::vec3>& bitangents){

  // For averaging out
  std::vector<int> numFacesInThatVertex(positions.size());

  tangents.resize(positions.size());
  bitangents.resize(positions.size());

  for(int i=0; i < indices.size(); i++)
  {
    numFacesInThatVertex[indices[i]]++;
  }

  for (int i=0; i < indices.size(); i+=3)
  {
    // Shortcuts for positions indexes
    int vi0 = indices[i+0];
    int vi1 = indices[i+1];
    int vi2 = indices[i+2];

    // Shortcuts for positions
    glm::vec3 & v0 = positions[vi0];
    glm::vec3 & v1 = positions[vi1];
    glm::vec3 & v2 = positions[vi2];

    // Shortcuts for UVs
    glm::vec2 & uv0 = UVs[vi0];
    glm::vec2 & uv1 = UVs[vi1];
    glm::vec2 & uv2 = UVs[vi2];

    // Edges of the triangle : position delta
    glm::vec3 deltaPos1 = v1-v0;
    glm::vec3 deltaPos2 = v2-v0;

    // UV delta
    glm::vec2 deltaUV1 = uv1-uv0;
    glm::vec2 deltaUV2 = uv2-uv0;

    float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
    glm::vec3 tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r;
    glm::vec3 bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r;

    tangent = glm::normalize(tangent);
    bitangent = glm::normalize(bitangent);
    // bitangent = glm::cross(glm::normalize(glm::cross(v1 - v0, v2 - v0)), tangent);

    // Set the same tangent for all three positions of the triangle.
    // They will be merged later, in vboindexer.cpp
    tangents[vi0] += (tangent / numFacesInThatVertex[vi0]);
    tangents[vi1] += (tangent / numFacesInThatVertex[vi1]);
    tangents[vi2] += (tangent / numFacesInThatVertex[vi2]);

    // Same thing for bitangents
    bitangents[vi0] += (bitangent / numFacesInThatVertex[vi0]);
    bitangents[vi1] += (bitangent / numFacesInThatVertex[vi1]);
    bitangents[vi2] += (bitangent / numFacesInThatVertex[vi2]);
  }
}
