#include "renderwidget.h"

#include <QImage>
#include <QGLWidget>
#include <QMouseEvent>
#include <QOpenGLTexture>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"
#include "mesh.h"

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
  this->isBumMapActive = false;

  this->diffuseColor = glm::vec3(0.0f, 0.0f, 0.0f);
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
  this->program->setUniformValue("isBumMapActive", this->isBumMapActive);

  this->program->setUniformValue("diffuseColor", QVector3D( this->diffuseColor[0],
                                                            this->diffuseColor[1],
                                                            this->diffuseColor[2]));

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
  std::vector<unsigned int> indices;

  this->loadBuffers(*VAO,
                    *VBO,
                    *EBO,
                    positions,
                    normals,
                    indices);
}
                    
void RenderWidget::loadBuffers( unsigned int VAO,
                  unsigned int VBO,
                  unsigned int EBO,
                  std::vector<glm::vec3>& positions,
                  std::vector<glm::vec3>& normals,
                  std::vector<unsigned int>& indices)
{
  // Data structure to be stored in the VBO
  struct Vertex
  {
      glm::vec3 pos;
      glm::vec3 normal;
  };

  // Intercalating Data in the VBO
  std::vector<Vertex> vboDataArray;

  vboDataArray.reserve(positions.size());
  for (unsigned int i = 0; i < positions.size(); i++)
  {
    vboDataArray.push_back({
      positions[i],
      normals[i]
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
  this->isBumMapActive = value;
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
  std::vector<unsigned int> indices;

  this->mesh->getTriangles(&positions, &normals, &indices, this->isFlatFaces);
  this->countElements = indices.size();
  this->loadBuffers(  this->VAO,
                      this->VBO,
                      this->EBO,
                      positions,
                      normals,
                      indices);
  this->update();  
}

void RenderWidget::resetCamera()
{
  delete this->camera;
  this->camera = new Camera(
    glm::vec3(0.0f, 0.0f, -2.0f), // eye
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
