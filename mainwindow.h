#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QFileDialog>
#include "cipher.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void updateModImageView();
    ~MainWindow();

public slots:
    void onOpenDialogClick();
    void onSaveDialogClick();
    void onMessageChanged();
    void onBitsUsedCountChanged(int value);
    void onDecodeBtnClicked();

private:
    Ui::MainWindow *ui;
    bool isEncrypting;
    Cipher* currentSession;
    bool isLabelRed;

    void controlMaxIndicator();
};

#endif // MAINWINDOW_H
