#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->pushButtonOpen->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    ui->pushButtonPrev->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui->pushButtonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->pushButtonStop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui->pushButtonPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    ui->pushButtonNext->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));

    m_player = new QMediaPlayer(this);
    m_player->setVolume(70);
    ui->labelVolume->setText(QString("Volume: ").append(QString::number(m_player->volume())));
    ui->horizontaSliderlVolume->setValue(m_player->volume());

    ui->horizontalSliderProgress->setValue(m_player->position() / (m_player->duration()/1000));
}

MainWindow::~MainWindow()
{
    delete m_player;
    delete ui;
}


void MainWindow::on_pushButtonOpen_clicked()
{
    QString file = QFileDialog::getOpenFileName (
                this,
                "Open file",
                NULL,
                "Audio files (*.mp3 *.flac);; MP-3 (*.mp3);; Flac (*.flac)");
    this->ui->labelFile->setText(file);
    this->setWindowTitle("Media Player - " + file.split('/').last());
    this->m_player->setMedia(QUrl::fromLocalFile(file));
    this->ui->labelDuration->setText(QString("Duration: ").append(QString::number(m_player->duration())));
}


void MainWindow::on_horizontaSliderlVolume_valueChanged(int value)
{
    m_player->setVolume(value);
    ui->labelVolume->setText(QString("Volume: ").append(QString::number(value)));
}


void MainWindow::on_pushButtonPlay_clicked()
{
    m_player->play();
}

void MainWindow::on_pushButtonPause_clicked()
{
    m_player->pause();
}

