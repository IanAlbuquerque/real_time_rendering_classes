#include "renderwidget.h"

#include <QImage>
#include <QGLWidget>
#include <QMouseEvent>
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
  this->setFocusPolicy(Qt::TabFocus);
  this->isArcballMovementActive = false;
  this->isPanMovementActive = false;

  this->camera = new Camera(
    glm::vec3(0.0f, 0.0f, -2.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    30.0f,
    0.001f,
    10.0f,
    this->width(),
    this->height()
    );

  this->hasWireframe = false;
  this->hasOcclusion = true;
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

  this->createCube();
  this->createVBO();
}


void RenderWidget::paintGL()
{
  this->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  this->glBindVertexArray(VAO);

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

  this->program->setUniformValue("hasWireframe", this->hasWireframe);
  this->program->setUniformValue("hasOcclusion", this->hasOcclusion);

  this->glDrawElements(GL_TRIANGLES, (GLsizei) indices.size(), GL_UNSIGNED_INT, 0);
//  this->glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
}


void RenderWidget::resizeGL(int width, int height)
{
  this->glViewport(0, 0, width, height);
  this->camera->updateWH(width, height);
}


void RenderWidget::keyPressEvent(QKeyEvent *event)
{
  struct vertex
  {
      glm::vec3 pos;
      glm::vec3 normal;
  };

  std::vector<vertex> vbo;
  switch(event->key())
  {
    case Qt::Key_P:
      this->vertices.clear();
      this->normals.clear();
      this->indices.clear();
      this->mesh->avgSmoothing();
      this->mesh->getTriangles(&(this->vertices), &(this->normals), &(this->indices));

      vbo.reserve(this->vertices.size());
      for (unsigned int i = 0; i < vertices.size(); i++)
      {
        vbo.push_back({vertices[i], normals[i]});
      }

      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferData(GL_ARRAY_BUFFER, vbo.size() * sizeof(vertex), &vbo[0], GL_STATIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
      this->update();
      break;
    case Qt::Key_1:
      this->hasWireframe = !this->hasWireframe;
      this->update();
      break;
    case Qt::Key_2:
      this->hasOcclusion = !this->hasOcclusion;
      this->update();
      break;
    default:
      break;
  }
}

void RenderWidget::keyReleaseEvent(QKeyEvent *event)
{
}

void RenderWidget::mousePressEvent(QMouseEvent *event)
{
  int buttons = event->buttons();
  if(buttons & Qt::RightButton)
  {
    this->isArcballMovementActive = true;
    this->lastArcballScreenCoordinates = glm::vec2(event->x(), event->y());
  }
  if(buttons & Qt::MidButton)
  {
    this->isPanMovementActive = true;
    this->lastPanScreenCoordinates = glm::vec2(event->x(), event->y());
  }
}


void RenderWidget::mouseReleaseEvent(QMouseEvent *event)
{
  int buttons = ~event->buttons();
  if(buttons & Qt::RightButton)
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
  if(this->isArcballMovementActive)
  {
    glm::vec2 currentArcballScreenCoordinates = glm::vec2(event->x(), event->y());
    this->camera->arcballMoveScreenCoordinates(this->lastArcballScreenCoordinates, currentArcballScreenCoordinates);
    this->lastArcballScreenCoordinates = currentArcballScreenCoordinates;
    this->update();
  }
  if(this->isPanMovementActive)
  {
    glm::vec2 currentPanScreenCoordinates = glm::vec2(event->x(), event->y());
    this->camera->cameraPan(currentPanScreenCoordinates - this->lastPanScreenCoordinates);
    this->lastPanScreenCoordinates = currentPanScreenCoordinates;
    this->update();
  }
}


void RenderWidget::wheelEvent(QWheelEvent *event)
{
//  this->camera->increaseZoomBy(0.005 * event->delta());
  this->camera->zoomBy(event->delta());
  this->update();
}


void RenderWidget::createCube()
{
  this->mesh = new Mesh();
  this->mesh->loadObj("C:\\Users\\Ian Albuquerque\\Desktop\\mesh_processing\\3drenderer\\models\\cow_2904.obj");
  this->mesh->getTriangles(&(this->vertices), &(this->normals), &(this->indices));
}

void RenderWidget::createVBO()
{
  struct vertex
  {
      glm::vec3 pos;
      glm::vec3 normal;
  };

  std::vector<vertex> vbo;
  vbo.reserve(this->vertices.size());
  for (unsigned int i = 0; i < vertices.size(); i++)
  {
    vbo.push_back({vertices[i], normals[i]});
  }

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vbo.size() * sizeof(vertex), &vbo[0], GL_STATIC_DRAW);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

  glBindBuffer( GL_ARRAY_BUFFER, VBO );

  // vertexPos
  glEnableVertexAttribArray( 0 );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0 );

  // vertexNormal
  glEnableVertexAttribArray( 1 );
  glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)sizeof(glm::vec3) );

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}

