#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    currentSession = NULL;
    isLabelRed = false;

    connect(ui->openBtn,       SIGNAL(clicked()),         this, SLOT(onOpenDialogClick()));
    connect(ui->message,       SIGNAL(textChanged()),     this, SLOT(onMessageChanged()));
    connect(ui->bitsUsedCount, SIGNAL(valueChanged(int)), this, SLOT(onBitsUsedCountChanged(int)));
    connect(ui->saveBtn,       SIGNAL(clicked()),         this, SLOT(onSaveDialogClick()));
    connect(ui->decodeBtn,     SIGNAL(clicked()),         this, SLOT(onDecodeBtnClicked()));
    connect(ui->zoomSlider,    SIGNAL(valueChanged(int)), this, SLOT(onZoomChanged(int)));
    connect(ui->noShameChckBx, SIGNAL(pressed()),         this, SLOT(onPressChckBx()));
    connect(ui->noShameChckBx, SIGNAL(released()),        this, SLOT(onReleaseChckBx()));
    connect(ui->aboutBtn,      SIGNAL(clicked()),         this, SLOT(onAboutBtnClicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onOpenDialogClick()
{
    const char* action;

    if (this->isEncrypting) {
        action = "Open image to be encoded...";
    }
    else {
        action = "Open image to be decoded...";
    }

    QString filename = QFileDialog::getOpenFileName(this, tr(action), "/", tr("Image Files (*.png *.jpg *.bmp)"));

    if (filename == NULL) {
        return;
    }
    this->setWindowTitle("HiddenCat: " + filename);

    QImage* workingImage = new QImage();
    workingImage->load(filename);

    QGraphicsScene* scene = new QGraphicsScene;
    scene->addPixmap(QPixmap::fromImage(*workingImage));
    ui->modImageView->setScene(scene);

    if (currentSession != NULL) {
        delete currentSession;
    }

    currentSession = new Cipher(*workingImage, ui->bitsUsedCount->value());
    currentSession->setMessage(ui->message->toPlainText());
    this->onBitsUsedCountChanged(ui->bitsUsedCount->value());
}

void MainWindow::onSaveDialogClick() {
    if (currentSession == NULL) {
        return;
    }

    QString filename = QFileDialog::getSaveFileName(this, tr("Save encoded image..."), "/home/mde/Pictures/untitled.png", tr("Images (*.png *.bmp *.jpg)"));
    currentSession->encodeImage()->save(filename);
}

void MainWindow::onBitsUsedCountChanged(int value)
{
    if (value == 0) {
        ui->maxCharsCounter->setText("Maximum: 0 ");
        return;
    }
    if (currentSession == NULL) {
        ui->maxCharsCounter->setText("Maximum: - ");
        return;
    }

    int maxLen = currentSession->setNewBitsUsedPerPixel(value);
    ui->maxCharsCounter->setText(QString("Maximum: %1 ").arg(maxLen));
    controlMaxIndicator();

    updateModImageView();
}

void MainWindow::onMessageChanged()
{
    const QString& message = ui->message->toPlainText();
    ui->charCounter->setText(QString("Message Length: %1").arg(message.length()));

    if (currentSession == NULL) {
        return;
    }
    currentSession->setMessage(message);
    controlMaxIndicator();

    updateModImageView();
}

void MainWindow::onDecodeBtnClicked() {
    if (currentSession == NULL) {
        return;
    }

    ui->message->setPlainText(currentSession->decodeImage());
}

void MainWindow::updateModImageView() {
    ui->modImageView->scene()->clear();
    ui->modImageView->scene()->addPixmap(QPixmap::fromImage(*(currentSession->encodeImage())));
}

void MainWindow::controlMaxIndicator() {

    const bool messageTooLong = currentSession->isMessageTooLong();

    if (!isLabelRed && messageTooLong) {
        ui->charCounter->setStyleSheet("QLabel { color: red; }");
        isLabelRed = true;
    }
    else if (isLabelRed && !messageTooLong) {
        ui->charCounter->setStyleSheet("");
        isLabelRed = false;
    }
}

void MainWindow::onZoomChanged(int factor) {
    QMatrix matrix;
    double rfactor = factor / 100.0;
    matrix.scale(rfactor, rfactor);
    ui->modImageView->setMatrix(matrix);

    ui->zoomLbl->setText(QString("%1\%").arg(factor));
}

void MainWindow::onReleaseChckBx() {
    ui->noShameChckBx->setChecked(false);
    if (currentSession != NULL) {
        updateModImageView();
    }
}

void MainWindow::onPressChckBx() {
    ui->noShameChckBx->setChecked(true);

    if (currentSession != NULL) {
        ui->modImageView->scene()->clear();
        ui->modImageView->scene()->addPixmap(QPixmap::fromImage(*(currentSession->noShameImage())));
    }
}

void MainWindow::onAboutBtnClicked() {
    QMessageBox msgBox;
    msgBox.setText("Version 0.2\n"
                   "Written in Qt 5.3 and C++\n\n"
                   "Special Thanks to Janie Liu,\n"
                   "whose extraordinary attempts to hide information\n"
                   "ultimately became the fodder for this application.");
    msgBox.setWindowTitle("About HiddenCat");
    msgBox.exec();
}
