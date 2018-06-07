#ifndef FRAMEBUFFERS_H
#define FRAMEBUFFERS_H


#include <QOpenGLFunctions>

class FrameBuffers
    : public QOpenGLFunctions
{
public:
  enum FRAMEBUFFER_TEXTURE_TYPE {
      FRAMEBUFFER_TEXTURE_TYPE_POSITION,
      FRAMEBUFFER_TEXTURE_TYPE_NORMAL,
      FRAMEBUFFER_TEXTURE_TYPE_TRIANGLE_COORDINATES,
      FRAMEBUFFER_TEXTURE_TYPE_TEXTURE_COORDINATES,
      FRAMEBUFFER_TEXTURE_TYPE_MATERIAL_AMBIENT,
      FRAMEBUFFER_TEXTURE_TYPE_MATERIAL_DIFFUSE,
      FRAMEBUFFER_TEXTURE_TYPE_MATERIAL_SPECULAR,
      FRAMEBUFFER_NUM_TEXTURES
  };
  FrameBuffers(QOpenGLContext* context);
  ~FrameBuffers();
  bool init(unsigned int width, unsigned int height);
  void bindForWriting();
  void bindForReading();

private:
  GLuint fbo;
  GLuint textures[FRAMEBUFFER_NUM_TEXTURES];
  GLuint depthTexture;
};

#endif // FRAMEBUFFERS_H
