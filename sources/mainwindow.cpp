#include "headers/adjustmentswindow.h"
#include "headers/mainwindow.h"
#include "headers/overlaywindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnOverlays_clicked()
{
    OverlayWindow *ow = new OverlayWindow;
    ow->show();
}

void MainWindow::on_btnAdjustments_clicked()
{
    AdjustmentsWindow *aw = new AdjustmentsWindow;
    aw->show();
}
