#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStyle>
#include <QTime>

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

    connect(m_player, &QMediaPlayer::durationChanged, this, &MainWindow::on_duration_changed);
    connect(m_player, &QMediaPlayer::positionChanged, this, &MainWindow::on_position_changed);
}

MainWindow::~MainWindow()
{
    delete m_player;
    delete ui;
}

void MainWindow::on_duration_changed(quint64 duration)
{
    this->ui->horizontalSliderProgress->setMaximum(duration);
    QTime q_time = QTime::fromMSecsSinceStartOfDay(duration);
    ui->labelDuration->setText(QString("Duration: ").append(q_time.toString("hh:mm:ss")));
}

void MainWindow::on_position_changed(quint64 position)
{
    QTime q_time = QTime::fromMSecsSinceStartOfDay(position);
    this->ui->labelProgress->setText(QString(q_time.toString("hh:mm:ss")));
    this->ui->horizontalSliderProgress->setSliderPosition(position);
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
    //this->ui->labelDuration->setText(QString("Duration: ").append(QString::number(m_player->duration())));
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


void MainWindow::on_horizontalSliderProgress_sliderMoved(int position)
{
    this->m_player->setPosition(position);
}

