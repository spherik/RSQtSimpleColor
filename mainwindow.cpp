#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onNewImage()));
    // Create a PXCSenseManager instance
    sm = PXCSenseManager::CreateInstance();

    // Select the color stream
    sm->EnableStream(PXCCapture::STREAM_TYPE_COLOR,640,480);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onNewImage()
{
    if (sm->AcquireFrame(true)<PXC_STATUS_NO_ERROR)
        return;

    // retrieve the sample
    PXCCapture::Sample *sample=sm->QuerySample();

    //image is a PXCImage instance
    PXCImage::ImageData colorData, depthData;
    sample->color->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB32, &colorData);
    int height = sample->color->QueryInfo().height;
    int width= sample->color->QueryInfo().width;

    // image planes are in data.planes[0-3] with pitch data.pitches[0-3]
    QImage colorImage(colorData.planes[0],width, height,QImage::Format_RGB32);

    sample->color->ReleaseAccess(&colorData);

    ui->colorImage->setPixmap( QPixmap::fromImage(colorImage).scaled(width,height,Qt::KeepAspectRatio));

    // go fetching the next sample
    sm->ReleaseFrame();
}

void MainWindow::onStart()
{
    sm->Init();
    timer->start(1000/24);
    this->ui->startButton->setEnabled(false);
    this->ui->stopButton->setEnabled(true);
}

void MainWindow::onStop()
{
    timer->stop();
    sm->Release();
    this->ui->startButton->setEnabled(true);
    this->ui->stopButton->setEnabled(false);

}
