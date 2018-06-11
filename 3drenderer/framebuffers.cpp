#include "framebuffers.h"
#include <QOpenGLFunctions_3_3_Core>
#include <QDebug>

FrameBuffers::FrameBuffers()
  : QOpenGLFunctions_3_3_Core()
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
    this->glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);

    // Create the gbuffer textures
    this->glGenTextures(FrameBuffers::FRAMEBUFFER_NUM_TEXTURES, &(this->textures[0]));
//    this->glGenTextures(1, &this->depthTexture);

    for (unsigned int i = 0 ; i < FrameBuffers::FRAMEBUFFER_NUM_TEXTURES ; i++)
    {
       this->glBindTexture(GL_TEXTURE_2D, this->textures[i]);
       this->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
       this->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        this->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
       this->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, this->textures[i], 0);
    }

    // depth
//    this->glBindTexture(GL_TEXTURE_2D, this->depthTexture);
//    this->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//    this->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    this->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    this->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthTexture, 0);

    unsigned int attachments[7] = { GL_COLOR_ATTACHMENT0,
                                    GL_COLOR_ATTACHMENT1,
                                    GL_COLOR_ATTACHMENT2,
                                    GL_COLOR_ATTACHMENT3,
                                    GL_COLOR_ATTACHMENT4,
                                    GL_COLOR_ATTACHMENT5,
                                    GL_COLOR_ATTACHMENT6 };
    this->glDrawBuffers(7, attachments);

    this->glGenRenderbuffers(1, &(this->depthTexture));
    this->glBindRenderbuffer(GL_RENDERBUFFER, this->depthTexture);
    this->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    this->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depthTexture);
    // finally check if framebuffer is complete;
    if (this->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "Frame buffer not complete!";

    this->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // restore default FBO
//    this->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    return true;
}

void FrameBuffers::bindForWriting()
{
//     this->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->fbo);
    this->glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
}

void FrameBuffers::bindForReading()
{
//     this->glBindFramebuffer(GL_READ_FRAMEBUFFER, this->fbo);
    this->glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffers::setReadBuffer(FRAMEBUFFER_TEXTURE_TYPE textureType)
{
//    this->glReadBuffer(GL_COLOR_ATTACHMENT0 + textureType);
    this->glBindTexture(GL_TEXTURE_2D, this->textures[textureType]);
}
