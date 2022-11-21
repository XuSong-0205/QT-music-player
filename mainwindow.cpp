#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QIcon>
#include <qsettings.h>
#include <QFile>
#include <QDir>
#include <QUrl>
#include <QDebug>
#include <QPushButton>
#include <QMediaPlayer>
#include <QMediaPlaylist>

#include <ctime>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":resources/ANIME.ico"));
    this->setWindowTitle("happy music");
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
    // 设置音乐路径按钮


    // 设置上一曲，下一曲按钮
    m_buttonPrevMusic = new QPushButton(this);
    m_buttonPrevMusic->setText(tr("上一曲"));
    m_buttonPrevMusic->move(120, 400);
    m_buttonPrevMusic->show();
    m_buttonNextMusic = new QPushButton(this);
    m_buttonNextMusic->setText(tr("下一曲"));
    m_buttonNextMusic->move(500, 400);
    m_buttonNextMusic->show();
    connect(m_buttonPrevMusic, &QPushButton::clicked, this, &MainWindow::playPrevMusic);
    connect(m_buttonNextMusic, &QPushButton::clicked, this, &MainWindow::playNextMusic);

    // 暂停按钮
    m_buttonPauseMusic = new QPushButton(this);
    m_buttonPauseMusic->setText(tr("暂停/开始"));
    m_buttonPauseMusic->move(310, 400);
    m_buttonPauseMusic->show();
    connect(m_buttonPauseMusic, &QPushButton::clicked, this, &MainWindow::pauseMusic);

    // TODO
    // 音量设置
    // 当前播放音乐显示
    // 音乐列表


    // 设置窗口
    // 音乐列表(QListWidget)



    // 读取配置文件
    readConfig();


    // 读取本地音乐
    // 设置播放列表
    m_musicPlayer = new QMediaPlayer(this);
    m_musicPlayer->setVolume(50);
    m_musicPlayList = new QMediaPlaylist(m_musicPlayer);
    m_musicPlayer->setPlaylist(m_musicPlayList);
    for (auto&& musicPath : m_musicNames)
    {
        m_musicPlayList->addMedia(QUrl::fromLocalFile(musicPath));
    }

}


// 读取配置
void MainWindow::readConfig()
{
    if (m_config == nullptr)
    {
        QString configPath = ":/config/config.ini";
        if (!QFile::exists(configPath))
        {
            qDebug() << tr("配置文件不存在！");
            return;
        }
        else
        {
            m_config = new QSettings(configPath, QSettings::IniFormat, this);
        }
    }

    // 读取音乐路径
    QString musicPath = m_config->value("path/music_path").toString();
    if (musicPath == "")
    {
        m_config->setValue("path/music_path", "E:/music");
        musicPath = m_config->value("path/music_path").toString();
    }
    qDebug() << "musicPath: " << musicPath;


    // 读取本地音乐名
    readLocalMusic(musicPath);

}

// 读取音乐目录的所有音乐文件名
void MainWindow::readLocalMusic(QString musicPath)
{
    QDir musicDir(musicPath);
    if (musicDir.exists())
    {
        for (auto&& fileInfo : musicDir.entryInfoList(QDir::Files))
        {
            QString suffix = fileInfo.completeSuffix();
            if (suffix == "mp3")
            {
                m_musicNames.push_back(std::move(fileInfo.absoluteFilePath()));
                qDebug() << m_musicNames.back();
            }
        }
    }
}

// 使用音乐路径播放音乐
void MainWindow::playMusic(QString musicPath)
{
    m_musicPlayer->setMedia(QUrl::fromLocalFile(musicPath));
    m_musicPlayer->play();
    qDebug() << tr("开始播放音乐: %1").arg(musicPath);
}

// 播放播放列表中下标为 index 的音乐
void MainWindow::playMusic(int index)
{
    if (index < 0 || index >= m_musicPlayList->mediaCount())
    {
        qDebug() << tr("音乐播放下标 index: %1 非法").arg(index);
        return;
    }

    m_musicPlayList->setCurrentIndex(index);
    m_musicPlayer->play();
    qDebug() << tr("开始播放音乐: %1").arg(m_musicNames[index]);
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
    const int n = m_musicPlayList->mediaCount();
    if (n == 0) return;

    int index = (m_musicPlayList->currentIndex() + 1) % n;
    playMusic(index);
}

// 上一曲
void MainWindow::playPrevMusic()
{
    const int n = m_musicPlayList->mediaCount();
    if (n == 0) return;

    int index = (m_musicPlayList->currentIndex() - 1) % n;
    playMusic(index);
}

// 随机播放
void MainWindow::playRandomMusic()
{
    const int n = m_musicNames.size();
    if (n == 0) return;

    struct __run_once { __run_once(){ srand(0); } };
    static const __run_once __once{};

    int index = rand() % n;
    playMusic(index);
}
