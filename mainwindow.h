#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>


namespace Ui {
class MainWindow;
}


class QSettings;
class QPushButton;
class QMediaPlayer;
class QMediaPlaylist;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void initWindow();
    void readConfig();
    void readLocalMusic(QString musicPath);


private:
    void playMusic(QString musicPath);
    void playMusic(int index);
    void pauseMusic();
    void playNextMusic();
    void playPrevMusic();
    void playRandomMusic();



private:
    Ui::MainWindow *ui;

    QMediaPlayer* m_musicPlayer = nullptr;      // 音乐播放器
    QMediaPlaylist* m_musicPlayList = nullptr;  // 播放列表


    QPushButton* m_buttonMusicPath = nullptr;   // 音乐路径按钮
    QPushButton* m_buttonPrevMusic = nullptr;   // 上一曲按钮
    QPushButton* m_buttonNextMusic = nullptr;   // 下一曲按钮
    QPushButton* m_buttonPauseMusic = nullptr;  // 暂停播放

    QSettings* m_config = nullptr;              // 配置文件
private:
    std::vector<QString> m_musicNames;          // 本地音乐名

};

#endif // MAINWINDOW_H
