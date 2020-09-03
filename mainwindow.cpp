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
     QString bmpFilePath;

     if (openFileDialog(bmpFilePath)) {
         bmpOriginal = new BITMAP;
         openBitmap(bmpFilePath, *bmpOriginal);

         setGraphicsView(*bmpOriginal, ui->gfxBitmap);

         ui->tabWidget->setCurrentIndex(0);
     }
}

void MainWindow::on_btnOpenOverlay_clicked()
{
    QString bmpFilePath;

    if (openFileDialog(bmpFilePath)) {
        bmpOverlay = new BITMAP;
        openBitmap(bmpFilePath, *bmpOverlay);

        setGraphicsView(*bmpOverlay, ui->gfxOverlay);

        ui->tabWidget->setCurrentIndex(1);

        generateOverlaidBitmap();
    }
}

void MainWindow::on_btnSaveBitmap_clicked()
{

}

bool MainWindow::openFileDialog(QString &filePath) {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Bitmap Images (*.bmp)"));

    if (dialog.exec() && dialog.selectedFiles().length() > 0) {
        filePath = dialog.selectedFiles()[0];
        return true;
    }

    return false;
}

bool MainWindow::saveFileDialog() {
    return true;
}


bool MainWindow::openBitmap(QString filePath, BITMAP &bitmap) {
    std::ifstream bmpFile(filePath.toStdString(), std::ifstream::binary);

    if (bmpFile) {
        bmpFile.seekg(0, std::ifstream::end);
        int fileLength = bmpFile.tellg();
        bmpFile.seekg(0, std::ifstream::beg);

        BITMAPFILEHEADER *bitmapFileHeader = readBitmapFileHeader(bmpFile);

        if (!verifyBitmapFileHeader(bitmapFileHeader, fileLength)) {
            bmpFile.close();
            return false;
        }

        BITMAPINFOHEADER *bitmapInfoHeader = readBitmapInfoHeader(bmpFile);

        if (!verifyBitmapInfoHeader(bitmapInfoHeader)) {
            bmpFile.close();
            return false;
        }

        int colorTableEntries = getColorTableEntries(bitmapInfoHeader);

        std::vector<QColor> bitmapColorTable = readBitmapColorTable(bmpFile, colorTableEntries);

        std::vector<unsigned char> bitmapPixelIndices = readBitmapPixelIndices(bmpFile, bitmapFileHeader, bitmapInfoHeader);

        bitmap.bitmapFileHeader = bitmapFileHeader;
        bitmap.bitmapInfoHeader = bitmapInfoHeader;
        bitmap.bitmapColorTable = bitmapColorTable;
        bitmap.bitmapPixelIndices = bitmapPixelIndices;

        return true;
    } else {
        return false;
    }
}

void MainWindow::setGraphicsView(BITMAP &bitmap, QGraphicsView *qGraphicsView) {
    QImage qImg(bitmap.bitmapInfoHeader->biWidth, bitmap.bitmapInfoHeader->biHeight, QImage::Format_RGB32);

    int index = 0;
    for(int i = bitmap.bitmapInfoHeader->biHeight - 1; i >= 0; i--) {
        for (int j = 0; j < bitmap.bitmapInfoHeader->biWidth; j++) {

            unsigned char pixelIndex = bitmap.bitmapPixelIndices[index];

            QColor rgb = bitmap.bitmapColorTable[pixelIndex];


            qImg.setPixel(j, i, rgb.rgba());
            index++;
        }
    }

    auto pixImg = QPixmap::fromImage(qImg);

    QGraphicsScene *scene = new QGraphicsScene;
    scene->addPixmap(pixImg);
    qGraphicsView->setScene(scene);
}

void MainWindow::generateOverlaidBitmap() {
    //check if bitmap height x width are the same
    //check if overlay contains entries in it's color table not present in the original's

    compareBitmapDimensions(bmpOriginal, bmpOverlay);
}
