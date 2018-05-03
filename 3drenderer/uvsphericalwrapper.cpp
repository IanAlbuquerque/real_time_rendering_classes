#include "uvwrapper.h"
#include "uvsphericalwrapper.h"

#include "cmath"

UVSphericalWrapper::UVSphericalWrapper()
{

}


glm::vec2 UVSphericalWrapper::uv(glm::vec3 pos)
{
  glm::vec3 normal = glm::normalize(pos);
  float u = 0.5 + (atan2(normal.z, normal.x) / (2.0 * M_PI));
  float v = 0.5 - (asin(normal.y) / M_PI);
  return glm::vec2(-u, v);
}
