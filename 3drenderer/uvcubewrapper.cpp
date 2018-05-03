#include "uvcubewrapper.h"

#include "cmath"

UVCubeWrapper::UVCubeWrapper()
{
  this->maxX = 1.0f;
  this->maxY = 1.0f;
  this->maxZ = 1.0f;
}

glm::vec2 UVCubeWrapper::uv(glm::vec3 pos)
{
  float M = glm::max(glm::abs(pos.x), glm::max(glm::abs(pos.y), glm::abs(pos.z)));
  float u;
  float v;
  if (glm::abs(M - glm::abs(pos.x)) < 0.0001)
  {
    u = pos.y / this->maxY;
    v = pos.z / this->maxZ;
  }
  else if (glm::abs(M - glm::abs(pos.y)) < 0.0001)
  {
    u = pos.x / this->maxX;
    v = pos.z / this->maxZ;
  }
  else
  {
    u = pos.x / this->maxX;
    v = pos.y / this->maxY;
  }
  return glm::vec2(u / 2.0f + 0.5f, v / 2.0f + 0.5f);
}

void UVCubeWrapper::runBoundingBox(std::vector<glm::vec3>& positions)
{
  this->maxX = -1.0;
  this->maxY = -1.0;
  this->maxZ = -1.0;
  for(int i=0; i<positions.size(); i++)
  {
    this->maxX = glm::max(glm::abs(positions[i].x), this->maxX);
    this->maxY = glm::max(glm::abs(positions[i].y), this->maxY);
    this->maxZ = glm::max(glm::abs(positions[i].z), this->maxZ);
  }
}
