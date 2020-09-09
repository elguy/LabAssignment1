#ifndef ADJUSTMENTSWINDOW_H
#define ADJUSTMENTSWINDOW_H

#include <QMainWindow>

namespace Ui {
class AdjustmentsWindow;
}

class AdjustmentsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdjustmentsWindow(QWidget *parent = nullptr);
    ~AdjustmentsWindow();

private:
    Ui::AdjustmentsWindow *ui;
};

#endif // ADJUSTMENTSWINDOW_H
