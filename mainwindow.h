#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setPlaybackMode();
    void savePlaylist(QString filename);
    void loadFileToPlaylist(QString filePath);
    void loadPlaylist(QString filename);
    QVector<QString> loadPlaylistToArray(QString filename);
private slots:
      // Функция отклика контекстного меню
    void slotContextMenu(QPoint pos);

    void onDeleteButtonClicked();
    void onDeleteAllButtonClicked();

    void on_duration_changed(quint64 duration);

    void on_position_changed(quint64 position);

    void on_pushButtonOpen_clicked();

    void on_horizontaSliderlVolume_valueChanged(int value);

    void on_pushButtonPlay_clicked();

    void on_pushButtonPause_clicked();

    void on_horizontalSliderProgress_sliderMoved(int position);

    void on_pushButtonMute_clicked(bool checked);

    void on_checkBox_stateChanged(int arg1);

    void on_checkBoxShuffle_stateChanged(int arg1);

private:
    Ui::MainWindow *ui; // pointer, потому что все должно находиться в динам. памяти
    QMenu* popMenu;
    QMediaPlayer* m_player;
    QMediaPlaylist* m_playlist;
    QStandardItemModel* m_playlist_model;

    const QString DEFAULT_PLAYLIST_LOCATION = "C:\\Users\\79141\\Qt sourse\\repos\\MediaPlayer\\";
};
#endif // MAINWINDOW_H
