#ifndef UVWRAPPER_H
#define UVWRAPPER_H

#include "glm/glm.hpp"

class UVWrapper
{
public:
  UVWrapper();
  virtual glm::vec2 uv(glm::vec3 pos) = 0;
};

#endif // UVWRAPPER_H
