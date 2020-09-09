#ifndef OVERLAYWINDOW_H
#define OVERLAYWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include "bitmap.h"

QT_BEGIN_NAMESPACE
namespace Ui { class OverlayWindow; }
QT_END_NAMESPACE

class OverlayWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit OverlayWindow(QWidget *parent = nullptr);
    ~OverlayWindow();

private slots:
    void on_btnOpenBitmap_clicked();

    void on_btnOpenOverlay_clicked();

    void on_btnSaveBitmap_clicked();

    void on_btnGenerateOverlaid_clicked();

private:
    bool openFileDialog(QString &filePath);
    bool saveFileDialog(QString &filepath);
    //bool openBitmap(QString filePath, BITMAP &bitmap);
    void setGraphicsView(BITMAP &bitmap, QGraphicsView *qGraphicsView);
    void generateOverlaidBitmap();
    void showErrorMessage(QString errorMessage);
    void enableGenerateButton();
    void enableSaveButton();
    BITMAP *bmpOriginal = nullptr;
    BITMAP *bmpOverlay = nullptr;
    BITMAP *bmpOverlaid = nullptr;
    Ui::OverlayWindow *ui;
};
#endif // OVERLAYWINDOW_H
