#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStyle>
#include <QTime>
/*#include <QStyledItemDelegate>
#include <QPainter>
#include <QModelIndex>
#include <QFontMetrics>
#include <QTableView>

class PlaylistDelegate : public QStyledItemDelegate {
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QStyledItemDelegate::paint(painter, option, index);

        if (option.state & QStyle::State_Selected) {
            QRect rect = option.rect;
            QFontMetrics metrics(option.font);
            QString text = index.data().toString();
            int textWidth = metrics.width(text);
            int textHeight = metrics.height();
            painter->setPen(Qt::blue);
            painter->drawLine(rect.left(), rect.top() + textHeight + 2, rect.left() + textWidth, rect.top() + textHeight + 2);
        }
    }
}; */


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->pushButtonMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    ui->pushButtonOpen->setIcon(style()->standardIcon(QStyle::SP_DriveDVDIcon));
    ui->pushButtonPrev->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui->pushButtonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->pushButtonStop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui->pushButtonPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    ui->pushButtonNext->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    ui->pushButtonMute->setCheckable(1); ////////////////////////////////////////////

    //                     Player Init:
    m_player = new QMediaPlayer(this);
    m_player->setVolume(70);
    ui->labelVolume->setText(QString("Volume: ").append(QString::number(m_player->volume())));
    ui->horizontaSliderlVolume->setValue(m_player->volume());

    //                     Connect:
    connect(m_player, &QMediaPlayer::durationChanged, this, &MainWindow::on_duration_changed);
    connect(m_player, &QMediaPlayer::positionChanged, this, &MainWindow::on_position_changed);

    //                     PlayList Init:
    m_playlist_model = new QStandardItemModel(this);
    this->ui->tableViewPlaylist->setModel(m_playlist_model);
    m_playlist_model->setHorizontalHeaderLabels(QStringList() << "Audio track" << "File path");
    this->ui->tableViewPlaylist->hideColumn(1);
    this->ui->tableViewPlaylist->horizontalHeader()->setStretchLastSection(true);
    this->ui->tableViewPlaylist->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_playlist=new QMediaPlaylist(m_player);
    m_player->setPlaylist(m_playlist);

    connect(this->ui->pushButtonPrev, &QPushButton::clicked, this->m_playlist, &QMediaPlaylist::previous);
    connect(this->ui->pushButtonNext, &QPushButton::clicked, this->m_playlist, &QMediaPlaylist::next);
    connect(this->ui->tableViewPlaylist, &QTableView::doubleClicked, [this](const QModelIndex &index){
            m_playlist->setCurrentIndex(index.row()); m_player->play();
    });
    connect(m_playlist, &QMediaPlaylist::currentIndexChanged, [this](int index){
            ui->labelFile->setText(m_playlist_model->data(m_playlist_model->index(index, 0)).toString());
    });

    //ui->tableViewPlaylist->setItemDelegate(new PlaylistDelegate);
}

MainWindow::~MainWindow()
{
    delete m_playlist_model;
    delete m_playlist;
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
    //QString file = QFileDialog::getOpenFileName (
    //            this,
    //            "Open file",
    //            NULL,
    //            "Audio files (*.mp3 *.flac);; MP-3 (*.mp3);; Flac (*.flac)");
    //this->ui->labelFile->setText(file);
    //this->setWindowTitle("Media Player - " + file.split('/').last());
    //this->m_player->setMedia(QUrl::fromLocalFile(file));
    //this->ui->labelDuration->setText(QString("Duration: ").append(QString::number(m_player->duration())));

QStringList files = QFileDialog::getOpenFileNames(
            this,
            "Open file",
            "C:\\Users\\79141\\Downloads",
            "Audio files (*.mp3 *.flac);; MP_3 (*.mp3);; Flac (*.flac)");
for(QString filesPath:files){
    QList<QStandardItem*> items;
    items.append(new QStandardItem(QDir(filesPath).dirName()));
    items.append(new QStandardItem(filesPath));
    m_playlist_model->appendRow(items);
    m_playlist->addMedia(QUrl(filesPath));
}
}


void MainWindow::on_horizontaSliderlVolume_valueChanged(int value)
{
    m_player->setVolume(value);
    ui->labelVolume->setText(QString("Volume: ").append(QString::number(value)));
    ui->pushButtonMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume)); m_player->setMuted(0);
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


void MainWindow::on_pushButtonMute_clicked(bool checked)
{
    if(checked){
        ui->pushButtonMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        ui->labelVolume->setText(QString("Volume: 0")); m_player->setMuted(1); ui->pushButtonMute->setChecked(1);
    }
    else{
        ui->labelVolume->setText(QString("Volume: ").append(QString::number(m_player->volume())));
        m_player->setMuted(0); ui->pushButtonMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    }
}
