#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "musicpage.h"

#include <QDir>
#include <QUrl>
#include <QIcon>
#include <QFile>
#include <QLabel>
#include <QDebug>
#include <QSlider>
#include <qsettings.h>
#include <QPushButton>
#include <QMediaContent>
#include <QMediaPlayer>
#include <QMediaPlaylist>

#include <ctime>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":resources/icon/app/icon.ico"));
    this->setWindowTitle("music-player");
    this->setMenuBar(nullptr);
    this->removeToolBar(ui->mainToolBar);
    this->setStatusBar(nullptr);
    this->resize(720, 480);

    // 初始化窗口
    initWindow();

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::initWindow()
{
    srand(0);

    // 初始化音乐播放器
    initMusicPlay();

    // 初始化配置
    initConfig();

    // 初始化 ui
    initUi();


}

// 初始化音乐播放器
void MainWindow::initMusicPlay()
{
    // 初始化播放器，本地播放列表
    m_musicPlayer = new QMediaPlayer(this);
    m_musicPlayer->setVolume(40);
    m_localMusicPlayList = new QMediaPlaylist(m_musicPlayer);
    m_musicPlayer->setPlaylist(m_localMusicPlayList);
    connect(m_musicPlayer, &QMediaPlayer::currentMediaChanged, this, &MainWindow::musicChanged);
    connect(m_musicPlayer, &QMediaPlayer::positionChanged, this, &MainWindow::musicPositionChanged);

}

// 配置初始化
void MainWindow::initConfig()
{
    // 读取配置文件
    readConfig();

    // 读取本地音乐
    QString musicPath = m_config->value("path/music_path").toString();
    readLocalMusic(musicPath);

}

// 读取配置
void MainWindow::readConfig()
{
    if (m_config == nullptr)
    {
        QString configPath = ":/config/config.ini";
        if (!QFile::exists(configPath))
        {
            // todo
            // 弹出一个错误窗口，显示缺失配置文件，确定并退出
            qDebug() << tr("配置文件不存在！");
            return;
        }
        else
        {
            m_config = new QSettings(configPath, QSettings::IniFormat, this);
            m_config->setIniCodec("utf-8");
        }
    }

    // 读取音乐路径
    QString musicPath = m_config->value("path/music_path").toString();
    if (musicPath == "")
    {
        m_config->setValue("path/music_path", "E:/music");
        musicPath = m_config->value("path/music_path").toString();
        // todo
        // 没有写入默认配置问题
    }
}

// 读取音乐目录的所有音乐文件名
void MainWindow::readLocalMusic(QString musicPath)
{
    // 查找本地音乐文件
    QDir musicDir(musicPath);
    QStringList nameFileters;
    nameFileters << "*.mp3";
    qDebug() << tr("读取本地音乐：");
    for (const QFileInfo& fileInfo : musicDir.entryInfoList(nameFileters, QDir::Files | QDir::Readable, QDir::Name))
    {
        if (fileInfo.isFile())
        {
            m_localMusicPlayList->addMedia(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
            qDebug() << fileInfo.absoluteFilePath();
        }
    }

}


template<typename T>
void showValue(const T& value)
{
    qDebug() << "value: " << value;
}




// 初始化 ui
void MainWindow::initUi()
{
    // 音量条
    m_volumeBar = new QSlider(Qt::Horizontal, this);
    m_volumeBar->setMinimum(0);
    m_volumeBar->setMaximum(100);
    m_volumeBar->setValue(40);
    m_volumeBar->resize(100, 20);
    m_volumeBar->move(560, 430);
    m_volumeBar->show();
    connect(m_volumeBar, &QSlider::valueChanged, m_musicPlayer, &QMediaPlayer::setVolume);


    // 进度条
    m_progressBar = new QSlider(Qt::Horizontal, this);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(1000);
    m_progressBar->resize(720, 20);
    m_progressBar->move(0, 400);
    m_progressBar->show();
    connect(m_progressBar, &QSlider::sliderMoved, this, &MainWindow::setMusicPosition);


    // 当前播放音乐显示
    m_playMusicName = new QLabel(this);
    m_playMusicName->resize(180, 20);
    m_playMusicName->move(20, 420);
    m_playMusicName->show();


    // 进度条 播放时间/总时间 显示
    m_musicDuration = new QLabel(this);
    m_musicDuration->resize(80, 20);
    m_musicDuration->move(60, 440);
    m_musicDuration->show();


    // todo
    // 音乐列表(QListWidget)
    // 本地音乐
    m_buttonLocalMusic = new QPushButton(this);
    m_buttonLocalMusic->setText(tr("本地音乐"));
    m_buttonLocalMusic->move(40, 100);
    m_buttonLocalMusic->show();
    // 我喜欢
    m_buttonMyLikeMusic = new QPushButton(this);
    m_buttonMyLikeMusic->setText(tr("我喜欢"));
    m_buttonMyLikeMusic->move(40, 160);
    m_buttonMyLikeMusic->show();


    // todo
    // 设置界面


    // todo
    // 背景图


    //////////////////////////////////////////////////
    // 按钮
    // 设置上一曲，下一曲按钮
    m_buttonPrevMusic = new QPushButton(this);
    m_buttonPrevMusic->setText(tr("上一曲"));
    m_buttonPrevMusic->resize(60, 20);
    m_buttonPrevMusic->move(220, 430);
    m_buttonPrevMusic->show();
    m_buttonNextMusic = new QPushButton(this);
    m_buttonNextMusic->setText(tr("下一曲"));
    m_buttonNextMusic->resize(60, 20);
    m_buttonNextMusic->move(440, 430);
    m_buttonNextMusic->show();
    connect(m_buttonPrevMusic, &QPushButton::clicked, this, &MainWindow::playPrevMusic);
    connect(m_buttonNextMusic, &QPushButton::clicked, this, &MainWindow::playNextMusic);

    // 暂停按钮
    m_buttonPauseMusic = new QPushButton(this);
    m_buttonPauseMusic->setText(tr("暂停/开始"));
    m_buttonPauseMusic->resize(80, 20);
    m_buttonPauseMusic->move(320, 430);
    m_buttonPauseMusic->show();
    connect(m_buttonPauseMusic, &QPushButton::clicked, this, &MainWindow::pauseMusic);




}

// 使用音乐路径播放音乐
void MainWindow::playMusic(QString musicPath)
{
    m_musicPlayer->setMedia(QUrl::fromLocalFile(musicPath));
    m_musicPlayer->play();
//    QString musicName = musicPath.split("/").back();
//    m_playMusicName->setText(musicName);
//    qDebug() << tr("开始播放音乐: %1").arg(musicPath);
}

// 播放播放列表中下标为 index 的音乐
void MainWindow::playMusic(int index)
{
    QMediaPlaylist* currPlayList = getCurrPlayList();
    if (index < 0 || index >= currPlayList->mediaCount())
    {
        qDebug() << tr("音乐播放下标 index: %1 非法").arg(index);
        return;
    }

    currPlayList->setCurrentIndex(index);
    m_musicPlayer->play();
//    // 显示正在播放的音乐名
//    QString musicName = getCurrPlayMusicName();
//    m_playMusicName->setText(musicName);
//    qDebug() << tr("开始播放音乐: %1").arg(musicName);
}

// 暂停/开始
void MainWindow::pauseMusic()
{
    const auto state = m_musicPlayer->state();
    if (state == QMediaPlayer::PlayingState)
    {
        m_musicPlayer->pause();
    }
    else if (state == QMediaPlayer::PausedState)
    {
        m_musicPlayer->play();
    }
}

// 下一曲
void MainWindow::playNextMusic()
{
    QMediaPlaylist* currPlayList = getCurrPlayList();
    const int n = currPlayList->mediaCount();
    if (n == 0) return;

    int index = (currPlayList->currentIndex() + 1) % n;
    playMusic(index);
}

// 上一曲
void MainWindow::playPrevMusic()
{
    QMediaPlaylist* currPlayList = getCurrPlayList();
    const int n = currPlayList->mediaCount();
    if (n == 0) return;

    int index = (currPlayList->currentIndex() - 1) % n;
    playMusic(index);
}

// 随机播放
void MainWindow::playRandomMusic()
{
    QMediaPlaylist* currPlayList = getCurrPlayList();
    const int n = currPlayList->mediaCount();
    if (n == 0) return;

    int index = rand() % n;
    playMusic(index);
}

// 获取当前播放列表
QMediaPlaylist* MainWindow::getCurrPlayList() const
{
    return m_musicPlayer->playlist();
}

// 获取当前播放的音乐全路径
QString MainWindow::getCurrPlayMusicPath() const
{
    const auto state = m_musicPlayer->state();
    if (state != QMediaPlayer::StoppedState)
    {
        QMediaPlaylist* currPlayList = getCurrPlayList();
        return currPlayList->currentMedia().canonicalUrl().fileName();
    }

    return "";
}

// 获取当前播放的音乐名
QString MainWindow::getCurrPlayMusicName() const
{
    QString musicPath = getCurrPlayMusicPath();
    QString tempMusicName = musicPath.split("/").back();
    return tempMusicName.split(".").front();
}

// 播放的音乐切换
void MainWindow::musicChanged(const QMediaContent &media)
{
    QString tempMusicName = media.canonicalUrl().fileName().split("/").back();
    QString musicName = tempMusicName.split(".").front();
    m_playMusicName->setText(musicName);
    qDebug() << tr("开始播放音乐: %1").arg(musicName);
}

// 音乐进度变化
void MainWindow::musicPositionChanged(qint64 value)
{
    static const auto millisecondToString = [](qint64 num) -> QString {
        const int second = num / 1000;
        const int minute = second / 60;
        return QString("%1:%2").arg(minute, 2, 10, QChar('0')).arg(second % 60, 2, 10, QChar('0'));
    };


    // 当前音乐总时长，单位毫秒
    const qint64 total = m_musicPlayer->duration();
    if (total == 0) return;

    // 修改音乐时长
    QString currDura = millisecondToString(value);
    QString totalDura = millisecondToString(total);
    m_musicDuration->setText(QString("%1/%2").arg(currDura).arg(totalDura));


    // 手动拖动进度条退出
    if (m_progressBar->isSliderDown()) return;
    // 修改音乐进度条
    const int totalProg = m_progressBar->maximum();
    m_progressBar->setValue(static_cast<double>(value) / total * totalProg);
}

// 设置音乐进度
void MainWindow::setMusicPosition(int value)
{
    const int totalProg = m_progressBar->maximum();
    const int totalDura = m_musicPlayer->duration();
    if (totalDura == 0) return;

    m_musicPlayer->setPosition(static_cast<double>(value) / totalProg * totalDura);
    // todo
    // 拖动进度条时暂停播放，拖动结束后恢复播放

}
