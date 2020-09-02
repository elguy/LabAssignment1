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

void MainWindow::on_btnOpenOverlay_clicked()
{

}

void MainWindow::on_btnSaveBitmap_clicked()
{

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

        std::vector<QColor> bitmapColorTable = readBitmapColorTable(bmpFile, colorTableEntries);

        std::vector<unsigned char> bitmapPixelIndices = readBitmapPixelIndices(bmpFile, bitmapInfoHeader);

        createQImage(bitmapInfoHeader, bitmapColorTable, bitmapPixelIndices);
    } else {

    }
}

void MainWindow::createQImage(BITMAPINFOHEADER *bitmapInfoHeader, std::vector<QColor> bitmapColorTable, std::vector<unsigned char> bitmapPixelIndices) {
    QImage qImg(bitmapInfoHeader->biWidth, bitmapInfoHeader->biHeight, QImage::Format_RGB32);

    int index = 0;
    for(int i = bitmapInfoHeader->biHeight - 1; i >= 0; i--) {
        for (int j = 0; j < bitmapInfoHeader->biWidth; j++) {

            unsigned char pixelIndex = bitmapPixelIndices[index];

            QColor rgb = bitmapColorTable[pixelIndex];


            qImg.setPixel(j, i, rgb.rgba());
            index++;
        }
    }

    auto pixImg = QPixmap::fromImage(qImg);

    QGraphicsScene *scene = new QGraphicsScene;
    scene->addPixmap(pixImg);
    ui->gfxBitmap->setScene(scene);
}
