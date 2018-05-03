#include "camera.h"
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <QDebug>

Camera::Camera( glm::vec3 eye,
                glm::vec3 at,
                glm::vec3 up,
                float fovy,
                float zNear,
                float zFar,
                float width,
                float height)
{
  this->eye = eye;
  this->at = at;
  this->up = up;
  this->fovy = fovy;
  this->zNear = zNear;
  this->zFar = zFar;
  this->width = width;
  this->height = height;

  this->q = glm::quat(0.0f, 1.0f, 0.0f, 0.0f);
}

void Camera::updateWH(float width, float height)
{
  this->width = width;
  this->height = height;
}

glm::mat4x4 Camera::getViewMatrix()
{
//   return glm::lookAt(this->eye, this->at, this->up) * glm::toMat4(this->q);
   return glm::lookAt(this->eye, this->at, this->up);
}

glm::mat4x4 Camera::getProjectionMatrix()
{
  return glm::perspective( this->fovy,
                           this->width / this->height,
                           this->zNear,
                           this->zFar);
}

void Camera::zoomBy(float delta)
{
  this->eye += 0.1f * (this->at - this->eye) * delta/abs(delta);
}

void Camera::cameraPan(glm::vec2 delta)
{
  float screenToWorldRatio = glm::abs(glm::tan(this->fovy / 2.0f) * this->zNear * 2.0f / (float) this->height);
  delta *= screenToWorldRatio * 500.0f;
  glm::vec3 rightUnit = glm::normalize(glm::cross(this->at - this->eye, this->up));
  glm::vec3 upUnit = glm::normalize(glm::cross(rightUnit, this->at - this->eye));
  this->eye += rightUnit * delta.x;
  this->eye += upUnit * delta.y;
  this->at += rightUnit * delta.x;
  this->at += upUnit * delta.y;
}

glm::vec3 Camera::arcballScreenCoordsToUnitSphere(glm::vec2 screenCoordinates)
{
  float screenRatio = glm::min(this->width, this->height) / 2.0f;

  screenCoordinates.x = screenCoordinates.x - this->width/ 2.0f;
  screenCoordinates.y = screenCoordinates.y - this->height/ 2.0f;
  screenCoordinates /= screenRatio;

  float r = screenCoordinates.x * screenCoordinates.x + screenCoordinates.y * screenCoordinates.y;

  glm::vec3 arcballPoint;

  if (r > 1.0f)
  {
    float s = 1.0f / glm::sqrt(r);
    arcballPoint.x = s * screenCoordinates.x;
    arcballPoint.y = s * screenCoordinates.y;
    arcballPoint.z = 0.0;
  }
  else
  {
    arcballPoint.x = screenCoordinates.x;
    arcballPoint.y = screenCoordinates.y;
    arcballPoint.z = glm::sqrt(1.0f - r);
  }

  return arcballPoint;
}

void Camera::arcballMoveScreenCoordinates(glm::vec2 m1, glm::vec2 m2)
{
  glm::vec3 p1 = this->arcballScreenCoordsToUnitSphere(m1);
  glm::vec3 p2 = this->arcballScreenCoordsToUnitSphere(m2);

  glm::vec3 cross = glm::cross(p1, p2);
  glm::quat q = glm::quat(glm::dot(p1, p2), cross.x, cross.y, cross.z);
  glm::mat4 R = glm::toMat4(q);

  glm::mat4 F = glm::translate(glm::lookAt(this->eye, this->at, this->up), this->eye - this->at);
  glm::mat4 Finv = glm::inverse(F);

  glm::vec4 eye = glm::vec4(this->eye, 1.0f);
  glm::vec4 upPoint = glm::vec4(this->eye + this->up, 1.0f);

  this->eye = glm::vec3(Finv*R*F*eye);
  this->up = glm::vec3(Finv*R*F*upPoint) - this->eye;
}
