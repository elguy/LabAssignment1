#include "headers/adjustmentswindow.h"
#include "ui_adjustmentswindow.h"
#include <QFileDialog>
#include <QMessageBox>

AdjustmentsWindow::AdjustmentsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AdjustmentsWindow)
{
    adjustedColorTable.reserve(256);
    ui->setupUi(this);
}

AdjustmentsWindow::~AdjustmentsWindow()
{
    delete ui;
}

void AdjustmentsWindow::on_sldrBrightness_sliderMoved(int position)
{
    QString brightnessValue = QString::number(position);

    if (position >= 0) {
        brightnessValue = "+" + brightnessValue;
    }

    ui->lblBrightnessValue->setText(brightnessValue);

    adjustBrigtness(bmpBitmap, adjustedColorTable, ui->sldrBrightness->sliderPosition());
    setGraphicsView(*bmpBitmap, adjustedColorTable, ui->gfxBitmap);
}

void AdjustmentsWindow::on_sldrContrast_sliderMoved(int position)
{
    QString contrastValue = QString::number(position);

    if (position >= 0) {
        contrastValue = "+" + contrastValue;
    }

    ui->lblContrastValue->setText(contrastValue);

    //TODO: implement code to change contrast

}

void AdjustmentsWindow::on_btnOpenBitmap_clicked()
{
    QString bmpFilePath;
    QString errorMessage;

    if (openFileDialog(bmpFilePath)) {
        bmpBitmap = new BITMAP;

        if (openBitmap(bmpFilePath, *bmpBitmap, errorMessage)) {
           copy(bmpBitmap->bitmapColorTable.begin(), bmpBitmap->bitmapColorTable.end(), std::back_inserter(adjustedColorTable));
           setGraphicsView(*bmpBitmap, adjustedColorTable, ui->gfxBitmap);
           enableControls();
       } else {
            showErrorMessage(errorMessage);
        }
    }
}

void AdjustmentsWindow::on_btnSaveBitmap_clicked()
{
    QString bmpFilePath;

    if (saveFileDialog(bmpFilePath)) {
        bmpBitmap->bitmapColorTable = adjustedColorTable;
        saveBitmap(bmpBitmap, bmpFilePath);
    }
}

void AdjustmentsWindow::enableControls() {
    ui->btnSaveBitmap->setEnabled(true);
    ui->sldrBrightness->setEnabled(true);
    ui->sldrContrast->setEnabled(true);
}

void AdjustmentsWindow::showErrorMessage(QString errorMessage) {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Icon::Critical);
    msgBox.setWindowTitle("Error!");
    msgBox.setText(errorMessage);
    msgBox.exec();
}

bool AdjustmentsWindow::openFileDialog(QString &filePath) {
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

bool AdjustmentsWindow::saveFileDialog(QString &filePath) {
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

void AdjustmentsWindow::setGraphicsView(BITMAP &bitmap,  std::vector<QColor> adjustedColorTable, QGraphicsView *qGraphicsView) {
    QImage qImg(bitmap.bitmapInfoHeader->biWidth, bitmap.bitmapInfoHeader->biHeight, QImage::Format_RGB32);

    int index = 0;
    for(int i = bitmap.bitmapInfoHeader->biHeight - 1; i >= 0; i--) {
        for (int j = 0; j < bitmap.bitmapInfoHeader->biWidth; j++) {

            unsigned char pixelIndex = bitmap.bitmapPixelIndices[index];

            QColor rgb = adjustedColorTable[pixelIndex];


            qImg.setPixel(j, i, rgb.rgba());
            index++;
        }
    }

    auto pixImg = QPixmap::fromImage(qImg);

    QGraphicsScene *scene = new QGraphicsScene;
    scene->addPixmap(pixImg);
    qGraphicsView->setScene(scene);
}
