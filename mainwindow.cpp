#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStyle>
#include <QTime>
#include <QMultimedia>
#include <QMediaMetaData>
#include <QMediaTimeInterval>
#include <QMediaTimeRange>
#include <QMessageBox>

#include <QFile>
#include <QTextStream>
/*
#include <QStyledItemDelegate>
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
};*/


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
    m_playlist_model->setHorizontalHeaderLabels(QStringList() << "Audio track" << "File path" << "Duration");
    this->ui->tableViewPlaylist->hideColumn(1);
    int c_duration_width = 64;
    this->ui->tableViewPlaylist->setColumnWidth(2, c_duration_width);
    this->ui->tableViewPlaylist->setColumnWidth(0, this->ui->tableViewPlaylist->width() - c_duration_width * 1.2);
    //this->ui->tableViewPlaylist->horizontalHeader()->setStretchLastSection(true);
    this->ui->tableViewPlaylist->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->ui->tableViewPlaylist->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewPlaylist->setContextMenuPolicy(Qt::CustomContextMenu);

    m_playlist=new QMediaPlaylist(m_player);
    m_player->setPlaylist(m_playlist);

    connect(this->ui->pushButtonPrev, &QPushButton::clicked, this->m_playlist, &QMediaPlaylist::previous);
    connect(this->ui->pushButtonNext, &QPushButton::clicked, this->m_playlist, &QMediaPlaylist::next);
    connect(this->m_playlist, &QMediaPlaylist::currentIndexChanged, this->ui->tableViewPlaylist, &QTableView::selectRow);
    connect(this->ui->tableViewPlaylist, &QTableView::doubleClicked, [this](const QModelIndex &index){
            m_playlist->setCurrentIndex(index.row()); m_player->play();
    });
    connect(m_playlist, &QMediaPlaylist::currentIndexChanged, [this](int index){
        ui->labelFile->setText(m_playlist_model->data(m_playlist_model->index(index, 0)).toString());

        QVariant bitrateVariant = m_player->metaData(QMediaMetaData::AudioBitRate);
        QVariant sampleRateVariant = m_player->metaData(QMediaMetaData::SampleRate);

        if (bitrateVariant.isValid() && sampleRateVariant.isValid()) {

            ui->label_Bitrate->setText("kbps: " + QString::number(bitrateVariant.toFloat() / 1000));
            ui->label_sampleRate->setText("kHz: " + QString::number(sampleRateVariant.toFloat() / 1000));
        } else {
            ui->label_Bitrate->setText("kbps: N/A");
            ui->label_sampleRate->setText("kHz: N/A");
        }
    });

    QString filename = DEFAULT_PLAYLIST_LOCATION + "playlist.m3u";
    loadPlaylist(filename);
    //ui->tableViewPlaylist->setItemDelegate(new PlaylistDelegate);

    //                     popup init:
    popMenu = new QMenu(ui->tableViewPlaylist);
    //QAction *actionUpdateInfo = new QAction();
    QAction *actionDelInfo = new QAction();
    QAction *actionClearList = new QAction();
    //actionUpdateInfo -> setText (QString ("Изменить"));
    actionDelInfo -> setText (QString ("Удалить"));
    actionClearList -> setText(QString("Очистить список"));
    //popMenu->addAction(actionUpdateInfo);
    popMenu->addAction(actionDelInfo);
    popMenu->addAction(actionClearList);
    connect(ui->tableViewPlaylist, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotContextMenu(QPoint)));
    connect(actionDelInfo, &QAction::triggered, this, &MainWindow::onDeleteButtonClicked);
    connect(actionClearList, &QAction::triggered, this, &MainWindow::onDeleteAllButtonClicked);
}

MainWindow::~MainWindow()
{
    QString filename = DEFAULT_PLAYLIST_LOCATION + "playlist.m3u";
    //savePlaylist(filename);
    delete m_playlist_model;
    delete m_playlist;
    delete m_player;
    delete ui;
}

void MainWindow::setPlaybackMode()
{
    QMediaPlaylist::PlaybackMode mode = QMediaPlaylist::PlaybackMode::CurrentItemOnce;
        if(this->ui->checkBoxLoop->isChecked())mode = QMediaPlaylist::PlaybackMode::Loop;
        if(this->ui->checkBoxShuffle->isChecked())mode = QMediaPlaylist::PlaybackMode::Random;
        this->m_playlist->setPlaybackMode(mode);
}

void MainWindow::savePlaylist(QString filename)
{
    QString format = filename.split('.').last();
    m_playlist->save(QUrl::fromLocalFile(filename), format.toStdString().c_str()); // .toStdString().c_str() нужен для того чтобы строка была типа const

    QFile file(DEFAULT_PLAYLIST_LOCATION + "Index.txt");
        if (file.open(QIODevice::ReadWrite)) {
            QTextStream stream(&file);
            stream << m_playlist->currentIndex() << endl;
        }
}

void MainWindow::loadFileToPlaylist(QString filesPath)
{
    filesPath = filesPath.remove('\n');
    // ///////////
    //QMediaPlayer* player = new QMediaPlayer();
    //   QString duration;// = QTime::fromMSecsSinceStartOfDay(player.duration()).toString("hh:mm:ss");
    //    connect(player, &QMediaPlayer::durationChanged,
    //            [player, &duration](quint64 dur)
    //   {
    //       duration = QTime::fromMSecsSinceStartOfDay(dur).toString("hh:mm:ss");
    //    });
    //player->setMedia(QUrl(filesPath));
    // ///////////
    //QMediaPlayer* player = new QMediaPlayer;
    //QMessageBox mb(QMessageBox::Icon::Information, QString("Adding"), filesPath, QMessageBox::Ok, this);
    //mb.show();

   // QVariant data = player->metaData(QMediaMetaData::Duration);
   // QDateTime duration(data.toDateTime());

    m_playlist->addMedia(QUrl(filesPath));
    QList<QStandardItem*> items;
    items.append(new QStandardItem(QDir(filesPath).dirName()));
    items.append(new QStandardItem(filesPath));
    m_playlist_model->appendRow(items);

    //delete player;
}

void MainWindow::loadPlaylist(QString filename)
{
    //QString format = filename.split('.').last();
    //m_playlist->load(QUrl::fromLocalFile(filename), format.toStdString().c_str());
    //for(int i=0;i<m_playlist->mediaCount();i++){
    //    QString url = m_playlist->media(i).canonicalUrl().url();
    //    QList<QStandardItem*> items;
    //    items.append(new QStandardItem(QDir(url).dirName()));
    //    m_playlist_model->appendRow(items);
    //}
    QVector<QString> lines = loadPlaylistToArray(filename);
    for(int i=0; i<lines.size(); i++) loadFileToPlaylist(lines[i]);


    QFile file(DEFAULT_PLAYLIST_LOCATION + "Index.txt");
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream stream(&file);
             ui->tableViewPlaylist->selectRow(stream.readLine().toInt());
        }
}

QVector<QString> MainWindow::loadPlaylistToArray(QString filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QList<QString> lines;
    while(!file.atEnd())
    {
        QByteArray line = file.readLine();
        lines.append(line);
    }
    return lines.toVector();
}

void MainWindow::slotContextMenu(QPoint pos)
{
    QModelIndex index = ui->tableViewPlaylist->indexAt(pos);
    if (index.isValid()) {
              popMenu->exec(QCursor::pos()); // Позиция, где появляется меню, - это текущая позиция мыши
    }
}

void MainWindow::onDeleteButtonClicked()
{
    QModelIndexList selectedIndexes = ui->tableViewPlaylist->selectionModel()->selectedIndexes();

    for(const QModelIndex& Index : selectedIndexes){
        m_playlist_model->removeRow(Index.row());
        m_playlist->removeMedia(Index.row());

        if (Index.row() == m_playlist->currentIndex()) ui->labelFile->setText("File:");
    }
}

void MainWindow::onDeleteAllButtonClicked()
{
    m_playlist_model->removeRows(0, m_playlist_model->rowCount());
    m_playlist->clear(); ui->labelFile->setText("File:");
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

    QStringList files = QFileDialog::getOpenFileNames
                (
                    this,
                    "Open file",
                    "D:\\Users\\Clayman\\Music\\Sergo",
                    "Audio files (*.mp3 *.flac);; MP-3 (*.mp3);; Flac (*.flac);; Playlists (*.m3u *.CUE)"
                );
        for(QString filesPath:files)
        {
    //		QList<QStandardItem*> items;
    //		items.append(new QStandardItem(QDir(filesPath).dirName()));
    //		items.append(new QStandardItem(filesPath));
    //		m_playlist_model->appendRow(items);
    //		m_playlist->addMedia(QUrl(filesPath));
            this->loadFileToPlaylist(filesPath);
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
        m_player->setMuted(checked); ui->pushButtonMute->setIcon(style()->standardIcon(checked?QStyle::SP_MediaVolumeMuted:QStyle::SP_MediaVolume));
        ui->labelVolume->setText(QString("Volume: ").append(QString::number(checked?0:m_player->volume())));
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    this->setPlaybackMode();
}


void MainWindow::on_checkBoxShuffle_stateChanged(int arg1)
{
    this->setPlaybackMode();
}

