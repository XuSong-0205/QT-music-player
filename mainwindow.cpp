#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QUrl>
#include <QIcon>
#include <QFile>
#include <QLabel>
#include <QDebug>
#include <QSlider>
#include <QWidget>
#include <QPixmap>
#include <QPalette>
#include <QFileDialog>
#include <qsettings.h>
#include <QPushButton>
#include <QListWidget>
#include <QMediaContent>
#include <QMediaPlayer>
#include <QMediaPlaylist>

#include <ctime>


// 窗口总大小
static const QSize WINDOW_SIZE(720, 480);

// 显示界面的位置
static constexpr int WIDGET_X = 160;
static constexpr int WIDGET_Y = 40;
// 显示界面的大小
static const QSize WIDGET_SIZE(WINDOW_SIZE.width() - WIDGET_X, 350);


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
    this->resize(WINDOW_SIZE);

    // this->setWindowOpacity(0.8);
    // this->setWindowFlags(Qt::FramelessWindowHint);
    // this->setAttribute(Qt::WA_TranslucentBackground);

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

    // 读取音乐路径
    QString musicPath = getStringConfigValue("path/music", "E:/music");
    // 读取本地音乐
    readLocalMusic(musicPath);

    // 设置音量
    const int volume = getIntConfigValue("music/volume", 40);
    m_musicPlayer->setVolume(volume);

    // 设置背景图片
    QString backgroundPath = getStringConfigValue("path/background");
    if (!backgroundPath.isEmpty())
    {
        setBackground(backgroundPath);
    }

    // 设置窗口透明度
    const int windowTransprent = getIntConfigValue("window/transparent", 100);
    this->setWindowOpacity((windowTransprent + 0.0) / 100);

}

// 读取配置
void MainWindow::readConfig()
{
    if (m_config == nullptr)
    {
        QString configPath = "./config/config.ini";
        m_config = new QSettings(configPath, QSettings::IniFormat, this);
        m_config->setIniCodec("UTF-8");
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

// 初始化 ui
void MainWindow::initUi()
{
    //////////////////////////////////////////////////
    // 1. 设置按钮
    m_buttonSetup = new QPushButton(this);
    m_buttonSetup->setText(tr("设置"));
    m_buttonSetup->move(40, 40);
    // 按钮透明
    m_buttonSetup->setFlat(true);
    m_buttonSetup->show();
    // 设置界面
    m_widgetSetup = new QWidget(this);
    m_widgetSetup->resize(WIDGET_SIZE);
    m_widgetSetup->move(WIDGET_X, WIDGET_Y);
    m_widgetSetup->hide();
    m_widgetSetup->setAttribute(Qt::WA_TranslucentBackground, true);
    connect(m_buttonSetup, &QPushButton::clicked, this, [this](){
        changeShowWidget(m_widgetSetup);
    });

    // 设置功能
    // 1.1 选择音乐路径
    m_buttonChoicePath = new QPushButton(m_widgetSetup);
    m_buttonChoicePath->setText(tr("音乐路径"));
    m_buttonChoicePath->move(40, 0);
    m_buttonChoicePath->setFlat(true);
    m_buttonChoicePath->show();
    // 音乐路径标签
    m_labelMusicPath = new QLabel(m_widgetSetup);
    m_labelMusicPath->setText(getStringConfigValue("path/music"));
    m_labelMusicPath->resize(m_labelMusicPath->width(), m_buttonChoicePath->height());
    m_labelMusicPath->move(m_buttonChoicePath->x() + m_buttonChoicePath->width() + 10,
                           m_buttonChoicePath->y());
    m_labelMusicPath->show();
    connect(m_buttonChoicePath, &QPushButton::clicked, this, [this](){
        QString dir = QDir("D:").exists() ? "D:" : "C:";
        QString musicPath = QFileDialog::getExistingDirectory(m_widgetSetup, tr("选择音乐路径"), dir);
        if (musicPath.isEmpty()) return;
        m_config->setValue("path/music", musicPath);
        m_labelMusicPath->setText(musicPath);
    });
    // 1.2 选择背景图片
    m_buttonChoiceBackground = new QPushButton(m_widgetSetup);
    m_buttonChoiceBackground->setText(tr("背景图片"));
    m_buttonChoiceBackground->move(40, 40);
    m_buttonChoiceBackground->setFlat(true);
    m_buttonChoiceBackground->show();
    // 背景图片标签
    m_labelBackgroundPath = new QLabel(m_widgetSetup);
    m_labelBackgroundPath->setText(getStringConfigValue("path/background"));
    m_labelBackgroundPath->resize(m_labelBackgroundPath->width(), m_buttonChoiceBackground->height());
    m_labelBackgroundPath->move(m_buttonChoiceBackground->x() + m_buttonChoiceBackground->width() + 10, m_buttonChoiceBackground->y());
    m_labelBackgroundPath->show();
    connect(m_buttonChoiceBackground, &QPushButton::clicked, this, [this](){
        QString dir = QDir("D:").exists() ? "D:" : "C:";
        QString backgroundPath = QFileDialog::getOpenFileName(m_widgetSetup, tr("选择背景图片"), dir, tr("图片文件(*png *jpg)"));
        if (backgroundPath.isEmpty()) return;
        m_config->setValue("path/background", backgroundPath);
        setBackground(backgroundPath);
        m_labelBackgroundPath->setText(backgroundPath);
    });
    // 1.3 设置窗口透明度
    m_labelTransparent = new QLabel(m_widgetSetup);
    m_labelTransparent->setText(tr("窗口透明度"));
    m_labelTransparent->move(40, 80);
    m_labelTransparent->show();
    // 窗口透明度值
    m_labelTransparentValue = new QLabel(m_widgetSetup);
    const int windowTransprent = getIntConfigValue("window/transparent", 100);
    m_labelTransparentValue->setText(QString("%1%").arg(windowTransprent));
    m_labelTransparentValue->resize(100, m_labelTransparent->height());
    m_labelTransparentValue->move(m_labelBackgroundPath->x(), m_labelTransparent->y());
    m_labelTransparentValue->show();
    // 滚动条
    m_sliderTransparent = new QSlider(Qt::Horizontal, m_widgetSetup);
    m_sliderTransparent->setMinimum(0);
    m_sliderTransparent->setMaximum(100);
    m_sliderTransparent->setValue(windowTransprent);
    m_sliderTransparent->move(m_labelTransparentValue->x() + m_labelTransparentValue->width() + 40, m_labelTransparentValue->y());
    connect(m_sliderTransparent, &QSlider::sliderMoved, this, [this](int value){
        this->setWindowOpacity((value + 0.0) / 100);
        m_labelTransparentValue->setText(QString("%1%").arg(value));
        m_config->setValue("window/transparent", value);
    });
    // todo
    // 1.4 设置字体颜色




    // 2. 本地音乐按钮
    m_buttonLocalMusic = new QPushButton(this);
    m_buttonLocalMusic->setText(tr("本地音乐"));
    m_buttonLocalMusic->move(40, 100);
    m_buttonLocalMusic->setFlat(true);
    m_buttonLocalMusic->show();
    // 本地音乐列表
    m_listWidgetLocal = new QListWidget(this);
    m_listWidgetLocal->resize(WIDGET_SIZE);
    m_listWidgetLocal->move(WIDGET_X, WIDGET_Y);
    m_listWidgetLocal->setStyleSheet("background-color: transparent;");
    m_listWidgetLocal->sortItems();
    m_listWidgetLocal->hide();
    // todo
    // 播放列表右键功能
    // a. 添加到某歌单
    // b. 从列表中删除
    // x. ...
    QMediaPlaylist* playlist = m_musicPlayer->playlist();
    if (playlist == nullptr) return;
    for (int i = 0; i < playlist->mediaCount(); ++i)
    {
        QMediaContent content = playlist->media(i);
        QString musicName = content.canonicalUrl().fileName().split("/").back();
        musicName = musicName.split(".").front();
        QListWidgetItem* widgetItem = new QListWidgetItem(m_listWidgetLocal);
        widgetItem->setText(musicName);
        widgetItem->setSizeHint(QSize(-1, 30));
        m_listWidgetLocal->insertItem(m_listWidgetLocal->count() + 1, widgetItem);
    }
    // 显示/隐藏 本地音乐列表
    connect(m_buttonLocalMusic, &QPushButton::clicked, this, [&](bool){
        changeShowWidget(m_listWidgetLocal);
    });
    // 双击音乐列表切换音乐
    connect(m_listWidgetLocal, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item){
        playMusic(item->listWidget()->currentRow());
    });


    // 3. 我喜欢
    m_buttonMyLike = new QPushButton(this);
    m_buttonMyLike->setText(tr("我喜欢"));
    m_buttonMyLike->move(40, 160);
    m_buttonMyLike->setFlat(true);
    m_buttonMyLike->show();


    // todo
    // 4. 最近播放


    // todo
    // 5. 我的歌单
    // 可无限制创建自定义歌单


    //////////////////////////////////////////////////
    // 按钮
    // 上一曲按钮
    m_buttonPrevMusic = new QPushButton(this);
    m_buttonPrevMusic->setText(tr("上一曲"));
    m_buttonPrevMusic->resize(60, 20);
    m_buttonPrevMusic->move(220, 430);
    m_buttonPrevMusic->setFlat(true);
    m_buttonPrevMusic->show();
    // 下一曲按钮
    m_buttonNextMusic = new QPushButton(this);
    m_buttonNextMusic->setText(tr("下一曲"));
    m_buttonNextMusic->resize(60, 20);
    m_buttonNextMusic->move(440, 430);
    m_buttonNextMusic->setFlat(true);
    m_buttonNextMusic->show();
    connect(m_buttonPrevMusic, &QPushButton::clicked, this, &MainWindow::playPrevMusic);
    connect(m_buttonNextMusic, &QPushButton::clicked, this, &MainWindow::playNextMusic);

    // 暂停按钮
    m_buttonPauseMusic = new QPushButton(this);
    m_buttonPauseMusic->setText(tr("暂停/开始"));
    m_buttonPauseMusic->resize(80, 20);
    m_buttonPauseMusic->move(320, 430);
    m_buttonPauseMusic->setFlat(true);
    m_buttonPauseMusic->show();
    connect(m_buttonPauseMusic, &QPushButton::clicked, this, &MainWindow::pauseMusic);

    // todo
    // 播放模式
    // 1. 列表循环
    // 2. 随机播放
    // 3. 单曲循环

    // todo
    // 歌词显示
    // 显示在显示界面

    // todo
    // 歌词按钮
    // 单独一个透明可移动窗口显示歌词






    //////////////////////////////////////////////////
    // 音量条
    m_volumeBar = new QSlider(Qt::Horizontal, this);
    m_volumeBar->setMinimum(0);
    m_volumeBar->setMaximum(100);
    m_volumeBar->setValue(m_musicPlayer->volume());
    m_volumeBar->resize(100, 20);
    m_volumeBar->move(560, 430);
    m_volumeBar->show();
    connect(m_volumeBar, &QSlider::valueChanged, m_musicPlayer, [this](int value){
        m_musicPlayer->setVolume(value);
        m_config->setValue("music/volume", value);
    });


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

}

// 使用音乐路径播放音乐
void MainWindow::playMusic(QString musicPath)
{
    m_musicPlayer->setMedia(QUrl::fromLocalFile(musicPath));
    m_musicPlayer->play();
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

// 切换当前显示的界面
void MainWindow::changeShowWidget(QWidget *widget)
{
    if (m_widgetCurrShow)
    {
        if (m_widgetCurrShow != widget)
        {
            m_widgetCurrShow->hide();
        }
    }

    if (widget->isHidden())
    {
        widget->show();
        m_widgetCurrShow = widget;
    }
    else
    {
        widget->hide();
        m_widgetCurrShow = nullptr;
    }
}

// 设置背景图片
void MainWindow::setBackground(QString backgroundPath)
{
    this->setAutoFillBackground(true);
    QPixmap pixmap(backgroundPath);
    QPalette palette;
    palette.setBrush(QPalette::Window, QBrush(pixmap));
    this->setPalette(palette);
}

// 设置窗口透明
void MainWindow::setWidgetTransparent(QWidget *widget, double level)
{
//    QPalette palette;
//    palette.setColor(QPalette::Background, QColor(0x00, 0x00, 0x00, alpha));
//    widget->setPalette(palette);

    widget->setWindowOpacity(level);
}

// 获取配置文件中的值
int MainWindow::getIntConfigValue(QString key, int defaultValue)
{
    QVariant var = m_config->value(key);
    if (var.isNull())
    {
        m_config->setValue(key, defaultValue);
    }

    return m_config->value(key).toInt();
}

bool MainWindow::getBoolConfigValue(QString key, bool defaultValue)
{
    QVariant var = m_config->value(key);
    if (var.isNull())
    {
        m_config->setValue(key, defaultValue);
    }

    return m_config->value(key).toBool();
}

double MainWindow::getDoubleConfigValue(QString key, double defaultValue)
{
    QVariant var = m_config->value(key);
    if (var.isNull())
    {
        m_config->setValue(key, defaultValue);
    }

    return m_config->value(key).toDouble();
}

QString MainWindow::getStringConfigValue(QString key, QString defaultValue)
{
    QVariant var = m_config->value(key);
    if (var.isNull())
    {
        m_config->setValue(key, defaultValue);
    }

    return m_config->value(key).toString();
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
