#include <Qt>
#include <QFileDialog>
#include <QMessageBox>

#include <fstream>
#include <iostream>
#include "bitmap.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnOpenBitmap_clicked()
{
     auto bmpFilePath = QFileDialog::getOpenFileName(this, tr("Open Bitmap Image"), "", tr("Bitmap Images (*.bmp)"));

     openBitmap(bmpFilePath);
}

void MainWindow::openBitmap(QString filePath) {
    std::ifstream bmpFile(filePath.toStdString(), std::ifstream::binary);

    if (bmpFile) {
        bmpFile.seekg(0, std::ifstream::end);
        int fileLength = bmpFile.tellg();
        bmpFile.seekg(0, std::ifstream::beg);

        BITMAPFILEHEADER *bitmapFileHeader = readBitmapFileHeader(bmpFile);

        if (!verifyBitmapFileHeader(bitmapFileHeader, fileLength)) {
            bmpFile.close();
            return;
        }

        BITMAPINFOHEADER *bitmapInfoHeader = readBitmapInfoHeader(bmpFile);

        if (!verifyBitmapInfoHeader(bitmapInfoHeader)) {
            bmpFile.close();
            return;
        }

        int colorTableEntries = getColorTableEntries(bitmapInfoHeader);

        std::vector<RGBQUAD *> BITMAPCOLORTABLE = readBitmapColorTable(bmpFile, colorTableEntries);

        std::vector<unsigned char> BITMAPPIXELINDICES = readBitmapPixelIndices(bmpFile, bitmapInfoHeader);

        createQImage(bitmapInfoHeader, BITMAPCOLORTABLE, BITMAPPIXELINDICES);
    } else {

    }
}

void MainWindow::createQImage(BITMAPINFOHEADER *bitmapInfoHeader, std::vector<RGBQUAD *> BITMAPCOLORTABLE, std::vector<unsigned char> BITMAPPIXELINDICES) {
    QImage qImg(bitmapInfoHeader->biWidth, bitmapInfoHeader->biHeight, QImage::Format_RGB32);

    int index = 0;
    for(int i = bitmapInfoHeader->biHeight - 1; i >= 0; i--) {
        for (int j = 0; j < bitmapInfoHeader->biWidth; j++) {

            unsigned char pixelIndex = BITMAPPIXELINDICES[index];

            RGBQUAD* rgb = BITMAPCOLORTABLE[pixelIndex];


            qImg.setPixel(j, i, QColor(rgb->rgbRed, rgb->rgbGreen, rgb->rgbBlue).rgba());
            index++;
        }
    }

    auto pixImg = QPixmap::fromImage(qImg);

    QGraphicsScene *scene = new QGraphicsScene;
    scene->addPixmap(pixImg);
    ui->graphicsView->setScene(scene);
}
