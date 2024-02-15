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

private slots:
    void on_duration_changed(quint64 duration);

    void on_position_changed(quint64 position);

    void on_pushButtonOpen_clicked();

    void on_horizontaSliderlVolume_valueChanged(int value);

    void on_pushButtonPlay_clicked();

    void on_pushButtonPause_clicked();

    void on_horizontalSliderProgress_sliderMoved(int position);

    void on_pushButtonMute_clicked(bool checked);

private:
    Ui::MainWindow *ui; // pointer, потому что все должно находиться в динам. памяти
    QMediaPlayer* m_player;
    QMediaPlaylist* m_playlist;
    QStandardItemModel* m_playlist_model;
};
#endif // MAINWINDOW_H
