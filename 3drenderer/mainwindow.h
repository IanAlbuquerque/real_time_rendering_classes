#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    void sendDiffuseColorToOpenGL();

public slots:
    void onImportOBJClick(bool isClicked);
    void onImportDiffuseTextureClick(bool isClicked);
    void onImportBumpMapClick(bool isClicked);

    void onCameraResetClick(bool isClicked);

    void onSetWireframeOverwrite(bool value);
    void onSetEdgesVisible(bool value);
    void onSetFlatFaces(bool value);
    void onSetDiffuseTextureActive(bool value);
    void onSetBumMapActive(bool value);

    void onChangeDiffuseColorR(double r);
    void onChangeDiffuseColorG(double g);
    void onChangeDiffuseColorB(double b);

    void onChangeShininess(double s);

    void onClickSphericalMapping(bool value);
    void onClickCubeMapping(bool value);

    void onClickNoneBuffer(bool value);
    void onClickPositionBuffer(bool value);
    void onClickNormalBuffer(bool value);
    void onClickTriangleCoordsBuffer(bool value);
    void onClickTextureCoordsBuffer(bool value);
    void onClickMaterialAmbientBuffer(bool value);
    void onClickMaterialDiffuseBuffer(bool value);
    void onClickMaterialSpecularBuffer(bool value);
};

#endif // MAINWINDOW_H
