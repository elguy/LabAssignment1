#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include "bitmap.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnOpenBitmap_clicked();

    void on_btnOpenOverlay_clicked();

    void on_btnSaveBitmap_clicked();

private:
    bool openFileDialog(QString &filePath);
    bool saveFileDialog();
    bool openBitmap(QString filePath, BITMAP &bitmap);
    void setGraphicsView(BITMAP &bitmap, QGraphicsView *qGraphicsView);
    void generateOverlaidBitmap();
    void showErrorMessage(QString errorMessage);
    BITMAP *bmpOriginal;
    BITMAP *bmpOverlay;
    BITMAP *bmpOverlaid;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
