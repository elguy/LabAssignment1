#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

private:
    void openBitmap(QString filePath);
    void createQImage(BITMAPINFOHEADER *bitmapInfoHeader, std::vector<RGBQUAD *> BITMAPCOLORTABLE, std::vector<unsigned char> BITMAPPIXELINDICES);
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
