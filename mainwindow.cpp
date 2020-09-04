#include <Qt>
#include <QFileDialog>
#include <QMessageBox>

#include <fstream>
#include <iostream>
#include <unordered_set>
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

         if (openBitmap(bmpFilePath, *bmpOriginal)) {
            setGraphicsView(*bmpOriginal, ui->gfxBitmap);

            ui->tabWidget->setCurrentIndex(0);
            enableGenerateButton();
        }
     }
}

void MainWindow::on_btnOpenOverlay_clicked()
{
    QString bmpFilePath;

    if (openFileDialog(bmpFilePath)) {
        bmpOverlay = new BITMAP;

        if (openBitmap(bmpFilePath, *bmpOverlay)) {
            setGraphicsView(*bmpOverlay, ui->gfxOverlay);

            ui->tabWidget->setCurrentIndex(1);
            enableGenerateButton();
        }
    }
}

void MainWindow::on_btnGenerateOverlaid_clicked()
{
    if (bmpOriginal != nullptr && bmpOverlay != nullptr) {
        generateOverlaidBitmap();
    } else {
        //this shouldn't happen but catch it anyway
        showErrorMessage("Either an original or an overlay bitmap has not yet been opened.");
    }
}

void MainWindow::on_btnSaveBitmap_clicked()
{
    QString bmpFilePath;

    if (saveFileDialog(bmpFilePath)) {
        saveBitmap(&bmpOverlaid, bmpFilePath);
    }
}

bool MainWindow::openFileDialog(QString &filePath) {
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Bitmap Images (*.bmp)"));

    if (dialog.exec() && dialog.selectedFiles().length() > 0) {
        filePath = dialog.selectedFiles()[0];
        return true;
    }

    return false;
}

bool MainWindow::saveFileDialog(QString &filePath) {
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("Bitmap Images (*.bmp)"));

    if (dialog.exec() && dialog.selectedFiles().length() > 0) {
        filePath = dialog.selectedFiles()[0];
        return true;
    }

    return false;
}

void MainWindow::showErrorMessage(QString errorMessage) {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Icon::Critical);
    msgBox.setWindowTitle("Error!");
    msgBox.setText(errorMessage);
    msgBox.exec();
}

void MainWindow::enableGenerateButton() {
    if (bmpOriginal != nullptr && bmpOverlay != nullptr) {
        ui->btnGenerateOverlaid->setEnabled(true);
    }
}


bool MainWindow::openBitmap(QString filePath, BITMAP &bitmap) {
    std::ifstream bmpFile(filePath.toStdString(), std::ifstream::binary);

    if (bmpFile) {
        bmpFile.seekg(0, std::ifstream::end);
        int fileLength = bmpFile.tellg();
        bmpFile.seekg(0, std::ifstream::beg);

        BITMAPFILEHEADER *bitmapFileHeader = readBitmapFileHeader(bmpFile);
        QString errorMessage;

        if (!verifyBitmapFileHeader(bitmapFileHeader, fileLength, errorMessage)) {
            bmpFile.close();
            showErrorMessage(errorMessage);
            return false;
        }

        BITMAPINFOHEADER *bitmapInfoHeader = readBitmapInfoHeader(bmpFile);

        if (!verifyBitmapInfoHeader(bitmapInfoHeader, errorMessage)) {
            bmpFile.close();
            showErrorMessage(errorMessage);
            return false;
        }

        int colorTableEntries = getColorTableEntries(bitmapInfoHeader);

        std::vector<QColor> bitmapColorTable = readBitmapColorTable(bmpFile, bitmapInfoHeader, colorTableEntries);

        std::unordered_set<unsigned char> colorSet;

        std::vector<unsigned char> bitmapPixelIndices = readBitmapPixelIndices(bmpFile, bitmapFileHeader, bitmapInfoHeader, colorSet);

        bmpFile.close();

        bitmap.bitmapFileHeader = bitmapFileHeader;
        bitmap.bitmapInfoHeader = bitmapInfoHeader;
        bitmap.bitmapColorTable = bitmapColorTable;
        bitmap.bitmapPixelIndices = bitmapPixelIndices;
        bitmap.colorSet = colorSet;

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

    QString errorMessage;
    if (!compareBitmapDimensions(bmpOriginal, bmpOverlay, errorMessage)) {
        showErrorMessage(errorMessage);
        return;
    }

    if (!compareColorTables(bmpOriginal, bmpOverlay, errorMessage)) {
        showErrorMessage(errorMessage);
        return;
    }

    bmpOverlaid = *bmpOriginal;
    for(unsigned int i = 0; i < bmpOriginal->bitmapPixelIndices.size(); i++)
    {
        QColor rgb = bmpOverlay->bitmapColorTable[bmpOverlay->bitmapPixelIndices[i]];
        if(!(rgb.red() == 255 && rgb.green() == 255 && rgb.blue() == 255))
        {
            bmpOverlaid.bitmapPixelIndices[i] = bmpOverlay->bitmapPixelIndices[i];
        }
    }
    //*(MainWindow::bmpOverlaid) = newBMP;
    //bmpOverlaid = newBMP;
    ui->btnSaveBitmap->setEnabled(true);
    ui->tabWidget->setCurrentIndex(2);
    setGraphicsView(bmpOverlaid, ui->gfxOverlaid);

}
