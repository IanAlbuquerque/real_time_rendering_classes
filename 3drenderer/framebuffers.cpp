#include "framebuffers.h"
#include <QOpenGLFunctions>

FrameBuffers::FrameBuffers(QOpenGLContext* context)
  : QOpenGLFunctions(context)
{
  this->initializeOpenGLFunctions();
}

FrameBuffers::~FrameBuffers()
{

}


bool FrameBuffers::init(unsigned int width, unsigned int height)
{
    // Create the FBO
    this->glGenFramebuffers(1, &(this->fbo));
    this->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->fbo);

    // Create the gbuffer textures
    this->glGenTextures(FrameBuffers::FRAMEBUFFER_NUM_TEXTURES, this->textures);
    this->glGenTextures(1, &this->depthTexture);

    for (unsigned int i = 0 ; i < FrameBuffers::FRAMEBUFFER_NUM_TEXTURES ; i++)
    {
       this->glBindTexture(GL_TEXTURE_2D, this->textures[i]);
       this->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
       this->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, this->textures[i], 0);
    }

    // depth
    this->glBindTexture(GL_TEXTURE_2D, this->depthTexture);
    this->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    this->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthTexture, 0);

    // restore default FBO
    this->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    return true;
}

void FrameBuffers::bindForWriting()
{
    this->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->fbo);
}

void FrameBuffers::bindForReading()
{
    this->glBindFramebuffer(GL_READ_FRAMEBUFFER, this->fbo);
}
