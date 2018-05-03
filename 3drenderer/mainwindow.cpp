#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QImage>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
  this->ui->setupUi(this);

  this->connect(this->ui->importOBJButton,
                SIGNAL(clicked(bool)),
                this,
                SLOT(onImportOBJClick(bool)));

  this->connect(this->ui->importDiffuseTextureButton,
                SIGNAL(clicked(bool)),
                this,
                SLOT(onImportDiffuseTextureClick(bool)));

  this->connect(this->ui->importBumpMapButton,
                SIGNAL(clicked(bool)),
                this,
                SLOT(onImportBumpMapClick(bool)));

  this->connect(this->ui->cameraResetButton,
                SIGNAL(clicked(bool)),
                this,
                SLOT(onCameraResetClick(bool)));

  // Checkboxes

  this->connect(this->ui->checkboxWireframeOverwrite,
                SIGNAL(clicked(bool)),
                this,
                SLOT(onSetWireframeOverwrite(bool)));

  this->connect(this->ui->checkboxFlatFaces,
                SIGNAL(clicked(bool)),
                this,
                SLOT(onSetFlatFaces(bool)));

  this->connect(this->ui->checkboxVisibleEdges,
                SIGNAL(clicked(bool)),
                this,
                SLOT(onSetEdgesVisible(bool)));

  this->connect(this->ui->checkboxDiffuseTexture,
                SIGNAL(clicked(bool)),
                this,
                SLOT(onSetDiffuseTextureActive(bool)));

  this->connect(this->ui->checkboxBumpMap,
                SIGNAL(clicked(bool)),
                this,
                SLOT(onSetBumMapActive(bool)));

  // Spin Boxes

  this->connect(this->ui->spinBoxR,
                SIGNAL(valueChanged(double)),
                this,
                SLOT(onChangeDiffuseColorR(double)));

  this->connect(this->ui->spinBoxG,
                SIGNAL(valueChanged(double)),
                this,
                SLOT(onChangeDiffuseColorG(double)));

  this->connect(this->ui->spinBoxB,
                SIGNAL(valueChanged(double)),
                this,
                SLOT(onChangeDiffuseColorB(double)));

  this->connect(this->ui->spinBoxShininess,
                SIGNAL(valueChanged(double)),
                this,
                SLOT(onChangeShininess(double)));

  this->sendDiffuseColorToOpenGL();
  this->ui->openGLWidget->setShininess(this->ui->spinBoxShininess->value());
}

void MainWindow::onImportOBJClick(bool isClicked)
{
  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("Open OBJ File"),
                                                  "",
                                                  tr("OBJ Files (*.obj);;All Files (*)"));
  QByteArray array = fileName.toLocal8Bit();
  char* buffer = array.data();
  this->ui->openGLWidget->importOBJFromPath(buffer);
}

void MainWindow::onImportDiffuseTextureClick(bool isClicked)
{
  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("Open Diffuse Texture File"),
                                                  "",
                                                  tr("All Files (*)"));
  QByteArray array = fileName.toLocal8Bit();
  char* buffer = array.data();

  QPixmap labelPixmap(fileName);
  labelPixmap = labelPixmap.scaled( this->ui->diffuseTextureLabel->width(),
                                    this->ui->diffuseTextureLabel->height(),
                                    Qt::KeepAspectRatio,
                                    Qt::FastTransformation);
  QImage img(fileName);

  this->ui->diffuseTextureLabel->setPixmap(labelPixmap);
  this->ui->openGLWidget->importDiffuseTexture(img);
}


void MainWindow::onImportBumpMapClick(bool isClicked)
{
  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("Open Bump Map Texture File"),
                                                  "",
                                                  tr("All Files (*)"));
  QByteArray array = fileName.toLocal8Bit();
  char* buffer = array.data();

  QPixmap labelPixmap(fileName);
  labelPixmap = labelPixmap.scaled( this->ui->bumpMapTextureLabel->width(),
                                    this->ui->bumpMapTextureLabel->height(),
                                    Qt::KeepAspectRatio,
                                    Qt::FastTransformation);
  QImage img(fileName);

  this->ui->bumpMapTextureLabel->setPixmap(labelPixmap);
  this->ui->openGLWidget->importBumpMap(img);
}

void MainWindow::onCameraResetClick(bool isClicked)
{
  this->ui->openGLWidget->resetCamera();
}

void MainWindow::onSetWireframeOverwrite(bool value)
{
  this->ui->openGLWidget->setWireframeOverwrite(value);
}

void MainWindow::onSetEdgesVisible(bool value)
{
  this->ui->openGLWidget->setEdgesVisible(value);
}

void MainWindow::onSetFlatFaces(bool value)
{
  this->ui->openGLWidget->setFlatFaces(value);
}

void MainWindow::onSetDiffuseTextureActive(bool value)
{
  this->ui->openGLWidget->setDiffuseTextureActive(value);
}

void MainWindow::onSetBumMapActive(bool value)
{
  this->ui->openGLWidget->setBumMapActive(value);
}

void MainWindow::onChangeDiffuseColorR(double r)
{
  this->sendDiffuseColorToOpenGL();
}

void MainWindow::onChangeDiffuseColorG(double g)
{
  this->sendDiffuseColorToOpenGL();
}

void MainWindow::onChangeDiffuseColorB(double b)
{
  this->sendDiffuseColorToOpenGL();
}

void MainWindow::onChangeShininess(double s)
{
  this->ui->openGLWidget->setShininess(s);
}

void MainWindow::sendDiffuseColorToOpenGL()
{
  float r = this->ui->spinBoxR->value();
  float g = this->ui->spinBoxG->value();
  float b = this->ui->spinBoxB->value();
  this->ui->openGLWidget->setDiffuseColor(r, g, b);
}

MainWindow::~MainWindow()
{
    delete ui;
}
