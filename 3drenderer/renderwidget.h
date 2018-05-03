#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <QMatrix4x4>

#include <vector>

#include "glm/glm.hpp"
#include "camera.h"
#include "mesh.h"

class RenderWidget
        : public QOpenGLWidget
        , protected QOpenGLExtraFunctions
{
public:
    RenderWidget(QWidget* parent);
    virtual ~RenderWidget();

private:
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);

    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);

    void createCube();
    void createVBO();

    QOpenGLShaderProgram* program;

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    std::vector< glm::vec3 > vertices;
    std::vector< glm::vec3 > normals;
    std::vector< unsigned int > indices;

    Camera* camera;
    glm::mat4x4 model;
    glm::mat4x4 view;
    glm::mat4x4 proj;

    glm::vec2 lastArcballScreenCoordinates;
    bool isArcballMovementActive;
    glm::vec2 lastPanScreenCoordinates;
    bool isPanMovementActive;

    bool hasWireframe;
    bool hasOcclusion;

    Mesh* mesh;
};

#endif // RENDERWIDGET_H
