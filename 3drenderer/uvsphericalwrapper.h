#ifndef UVSPHERICALWRAPPER_H
#define UVSPHERICALWRAPPER_H

#include "uvwrapper.h"

class UVSphericalWrapper: public UVWrapper
{
public:
  UVSphericalWrapper();
  glm::vec2 uv(glm::vec3 pos);
};

#endif // UVSPHERICALWRAPPER_H
