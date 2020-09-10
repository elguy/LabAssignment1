#ifndef ADJUSTMENTSWINDOW_H
#define ADJUSTMENTSWINDOW_H

#include "headers/bitmap.h"
#include <QMainWindow>
#include <QGraphicsView>

namespace Ui {
class AdjustmentsWindow;
}

class AdjustmentsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdjustmentsWindow(QWidget *parent = nullptr);
    ~AdjustmentsWindow();

private slots:
    void on_sldrBrightness_sliderMoved(int position);

    void on_sldrContrast_sliderMoved(int position);

    void on_btnOpenBitmap_clicked();

    void on_btnSaveBitmap_clicked();

    void on_sldrBrightness_sliderReleased();

    void on_sldrContrast_sliderReleased();

private:
    Ui::AdjustmentsWindow *ui;
    BITMAP *bmpBitmap = nullptr;
    void showErrorMessage(QString errorMessage);
    bool openFileDialog(QString &filePath);
    void setGraphicsView(BITMAP &bitmap, QGraphicsView *qGraphicsView);
    bool saveFileDialog(QString &filePath);
};

#endif // ADJUSTMENTSWINDOW_H
