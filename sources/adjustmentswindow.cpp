#include "headers/adjustmentswindow.h"
#include "ui_adjustmentswindow.h"

AdjustmentsWindow::AdjustmentsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AdjustmentsWindow)
{
    ui->setupUi(this);
}

AdjustmentsWindow::~AdjustmentsWindow()
{
    delete ui;
}
