#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>


namespace Ui {
class MainWindow;
}


class QLabel;
class QSlider;
class QSettings;
class QPushButton;
class QMediaContent;
class QMediaPlayer;
class QMediaPlaylist;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    // 初始化整个界面
    void initWindow();

    // 初始化音乐播放器
    void initMusicPlay();

    // 配置函数
    void initConfig();
    void readConfig();
    void readLocalMusic(QString musicPath);

    // ui 界面
    void initUi();

    // 功能函数
    void playMusic(QString musicPath);
    void playMusic(int index);
    void pauseMusic();
    void playNextMusic();
    void playPrevMusic();
    void playRandomMusic();

    QMediaPlaylist* getCurrPlayList() const;    // 获取当前播放列表
    QString getCurrPlayMusicPath() const;       // 获取当前播放的音乐全路径
    QString getCurrPlayMusicName() const;       // 获取当前播放的音乐名


private slots:
    void musicChanged(const QMediaContent& media);  // 播放的音乐切换
    void musicPositionChanged(qint64 value);        // 音乐进度变化
    void setMusicPosition(int value);               // 设置音乐进度



private:
    Ui::MainWindow *ui;

    QMediaPlayer* m_musicPlayer = nullptr;          // 音乐播放器
    QMediaPlaylist* m_localMusicPlayList = nullptr; // 播放列表

    QSlider* m_volumeBar = nullptr;                 // 音乐音量条
    QSlider* m_progressBar = nullptr;               // 音乐进度条
    QLabel* m_playMusicName = nullptr;              // 正在播放的音乐名
    QLabel* m_musicDuration = nullptr;              // 音乐播放时长/总时长

    // todo
    // 新写一个类，按钮，界面，功能
    QPushButton* m_buttonLocalMusic = nullptr;      // 本地音乐
    QPushButton* m_buttonMyLikeMusic = nullptr;     // 我喜欢



    QPushButton* m_buttonMusicPath = nullptr;       // 音乐路径按钮
    QPushButton* m_buttonPrevMusic = nullptr;       // 上一曲按钮
    QPushButton* m_buttonNextMusic = nullptr;       // 下一曲按钮
    QPushButton* m_buttonPauseMusic = nullptr;      // 暂停播放

    QSettings* m_config = nullptr;                  // 配置文件

};

#endif // MAINWINDOW_H
