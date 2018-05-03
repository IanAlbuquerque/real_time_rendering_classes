#ifndef UVCUBEWRAPPER_H
#define UVCUBEWRAPPER_H

#include "uvwrapper.h"
#include <vector>

class UVCubeWrapper: public UVWrapper
{
private:
  float maxX;
  float maxY;
  float maxZ;
public:
  UVCubeWrapper();
  glm::vec2 uv(glm::vec3 pos);
  void runBoundingBox(std::vector<glm::vec3>& positions);
};

#endif // UVCUBEWRAPPER_H
