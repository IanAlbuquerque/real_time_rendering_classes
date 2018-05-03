#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

class Camera
{
private:
  // Basic Properties
  glm::vec3 eye;
  glm::vec3 at;
  glm::vec3 up;
  float fovy;
  float zNear;
  float zFar;
  float width;
  float height;

  glm::quat q;
  glm::vec3 arcballScreenCoordsToUnitSphere(glm::vec2 screenCoordinates);
public:
  Camera( glm::vec3 eye,
          glm::vec3 at,
          glm::vec3 up,
          float fovy,
          float zNear,
          float zFar,
          float width,
          float height);

  // Basic Updates
  void updateWH(float width, float height);

  // Matrices Retrieval
  glm::mat4x4 getViewMatrix();
  glm::mat4x4 getProjectionMatrix();

  void zoomBy(float delta);
  void cameraPan(glm::vec2 delta);

  void arcballMoveScreenCoordinates(glm::vec2 m1, glm::vec2 m2);

};

#endif // CAMERA_H
