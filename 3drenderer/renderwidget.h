#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QOpenGLWidget>
#include <QImage>
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

    void importOBJFromPath(char* path);
    void importDiffuseTexture(QImage img);
    void importBumpMap(QImage img);

    void resetCamera();

    void setWireframeOverwrite(bool value);
    void setEdgesVisible(bool value);
    void setFlatFaces(bool value);
    void setDiffuseTextureActive(bool value);
    void setBumMapActive(bool value);
    void setDiffuseColor(float r, float g, float b);
    void setShininess(float s);

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

    void createBuffers( unsigned int* VAO,
                        unsigned int* VBO,
                        unsigned int* EBO);

    void loadBuffers( unsigned int VAO,
                      unsigned int VBO,
                      unsigned int EBO,
                      std::vector<glm::vec3>& positions,
                      std::vector<glm::vec3>& normals,
                      std::vector<glm::vec3>& tangents,
                      std::vector<glm::vec3>& bitangents,
                      std::vector<glm::vec2>& UVs,
                      std::vector<unsigned int>& indices);

    void createTexture(unsigned int* textureID);

    void loadTexture(unsigned int textureID, QImage img);

    void reloadMesh();

    void computeTangentBasis( std::vector<glm::vec3> & positions,
                            std::vector<glm::vec3> & normals,
                            std::vector<glm::vec2> & UVs,
                            std::vector<unsigned int> & indices,
                            std::vector<glm::vec3>& tangents,
                            std::vector<glm::vec3>& bitangents);

    QOpenGLShaderProgram* program;

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    unsigned int countElements;

    unsigned int DIFFUSE_TEXTURE_2D;
    unsigned int BUMP_TEXTURE_2D;

    Camera* camera;
    glm::mat4x4 model;
    glm::mat4x4 view;
    glm::mat4x4 proj;

    glm::vec2 lastArcballScreenCoordinates;
    bool isArcballMovementActive;
    glm::vec2 lastPanScreenCoordinates;
    bool isPanMovementActive;

    bool isWireframeOverwrite;
    bool isEdgesVisible;
    bool isFlatFaces;
    bool isDiffuseTextureActive;
    bool isBumpMapActive;

    glm::vec3 diffuseColor;
    float materialShininess;

    Mesh* mesh;
};

#endif // RENDERWIDGET_H
