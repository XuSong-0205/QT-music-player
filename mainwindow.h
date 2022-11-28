#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>


namespace Ui {
class MainWindow;
}


class QLabel;
class QWidget;
class QSlider;
class QSettings;
class QPushButton;
class QListWidget;
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
    void playPrevMusic();
    void playNextMusic();

    QMediaPlaylist* getCurrPlayList() const;    // 获取当前播放列表
    QString getCurrPlayMusicPath() const;       // 获取当前播放的音乐全路径
    QString getCurrPlayMusicName() const;       // 获取当前播放的音乐名

    void changeShowWidget(QWidget* widget);     // 切换当前显示的界面
    void setBackground(QString backgroundPath); // 设置背景图片
    void setWidgetTransparent(QWidget* widget, double level = 0.0 /*0.0 - 1.0*/); // 设置窗口透明

    QString setPlayMode(int mode) const;        // 设置播放模式


private:
    // 读取键值为 key 对应类型的值，如果不存在该值则使用默认值创建
    int getIntConfigValue(QString key, int defaultValue = 0);
    bool getBoolConfigValue(QString key, bool defaultValue = false);
    double getDoubleConfigValue(QString key, double defaultValue = 0);
    QString getStringConfigValue(QString key, QString defaultValue = "");



private slots:
    void musicChanged(const QMediaContent& media);  // 播放的音乐切换
    void musicPositionChanged(qint64 value);        // 音乐进度变化
    void setMusicPosition(int value);               // 设置音乐进度


private:
    Ui::MainWindow *ui;

    QSettings* m_config = nullptr;                  // 配置文件
    QMediaPlayer* m_musicPlayer = nullptr;          // 音乐播放器
    QMediaPlaylist* m_localMusicPlayList = nullptr; // 播放列表

    QSlider* m_volumeBar = nullptr;                 // 音乐音量条
    QSlider* m_progressBar = nullptr;               // 音乐进度条
    QLabel* m_playMusicName = nullptr;              // 正在播放的音乐名
    QLabel* m_musicDuration = nullptr;              // 音乐播放时长/总时长

    // todo
    // 新写一个类，按钮，界面，功能
    QPushButton* m_buttonLocalMusic = nullptr;      // 本地音乐
    QListWidget* m_listWidgetLocal = nullptr;       // 本地音乐列表
    //==//
    QPushButton* m_buttonMyLike = nullptr;          // 我喜欢
    //==//
    QPushButton* m_buttonSetup = nullptr;           // 设置按钮
    QWidget* m_widgetSetup = nullptr;               // 设置界面
    // 1. 设置音乐路径
    QPushButton* m_buttonChoicePath = nullptr;      // 选择音乐路径
    QLabel* m_labelMusicPath = nullptr;             // 音乐路径标签
    // 2. 设置背景图片
    QPushButton* m_buttonChoiceBackground = nullptr;// 选择背景图片
    QLabel* m_labelBackgroundPath = nullptr;        // 背景图片标签
    // 3. 设置窗口透明度
    QLabel* m_labelTransparent = nullptr;           // 窗口透明度标签
    QLabel* m_labelTransparentValue = nullptr;      // 窗口透明度值
    QSlider* m_sliderTransparent = nullptr;         // 窗口透明度滚动条
    //
    QWidget* m_widgetCurrShow = nullptr;            // 当前显示的界面




    QPushButton* m_buttonMusicPath = nullptr;       // 音乐路径按钮
    QPushButton* m_buttonPrevMusic = nullptr;       // 上一曲按钮
    QPushButton* m_buttonNextMusic = nullptr;       // 下一曲按钮
    QPushButton* m_buttonPauseMusic = nullptr;      // 暂停播放
    QPushButton* m_buttonPlayMode = nullptr;        // 播放模式
};

#endif // MAINWINDOW_H
