#pragma execution_character_set("utf-8")

#include "LoginRegisterWindowOne.h"
#include "EventFilterObject.h"
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QStaticText>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QFile>
#include <QBoxLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QTimer>
#include <QApplication>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>

LoginRegisterWindowOne::LoginRegisterWindowOne(QWidget* parent, QString hintText) : QDialog(parent), m_hintText(std::move(hintText))
{
    initUi();
    initStyleSheet();
}

void LoginRegisterWindowOne::handleRegisterResult(bool result)
{
    if (result)
    {
        m_stkw->setCurrentIndex(0);
        setTipMsg("注册成功，请登录");
    }
    else
    {
        setTipMsg("注册失败，账号已存在");
    }
}

void LoginRegisterWindowOne::slotLoginBtnClicked()
{
    //检查合法性
    auto username = m_usernameEdit->text();
    auto password = m_passwordEdit->text();

    if (username.isEmpty() || password.isEmpty())
    {
        setTipMsg("账号或密码不能为空");
        return;
    }

    Q_EMIT signalLoginRequest(username, password);
}

void LoginRegisterWindowOne::onRegisterBtnClicked()
{
    auto username = m_reg_usernameEdit->text();
    auto password = m_reg_pwdEdit->text();
    auto rePassword = m_reg_rePwdEdit->text();

    if (username.isEmpty() || password.isEmpty())
    {
        setTipMsg("账号或密码不能为空");
        return;
    }

    if (password != rePassword)
    {
        setTipMsg("两次密码输入不一致");
        return;
    }

    Q_EMIT signalRegisterRequest(username, password);
}

void LoginRegisterWindowOne::initUi()
{
    //去掉边框
    setWindowFlag(Qt::FramelessWindowHint);

    //背景透明
    setAttribute(Qt::WA_TranslucentBackground);

    //安装过滤器(实现窗口拖动)
    installEventFilter(new EventFilterObject(this));

    //调整窗口大小
    setFixedSize(740, 470);

    // 设置窗口置顶
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    //初始化矩形
    m_rightRect = { 5,40,width() - 2 * 5,height() - 40 * 2 };
    m_leftRect = { 5,5,185,height() - 2 * 5 };
    m_midRect = { 5,5,370,height() - 2 * 5 };

    //初始化线性渐变
    m_lineGrad.setCoordinateMode(QGradient::ObjectMode);
    m_lineGrad.setStart(0, 0);
    m_lineGrad.setFinalStop(1, 1);

    m_lineGrad.setStart(m_midRect.x(), m_midRect.y());
    m_lineGrad.setFinalStop(m_midRect.right(), m_midRect.bottom());
    m_lineGrad.setColorAt(0, QColor(0xe0c3fc));
    m_lineGrad.setColorAt(1, QColor(0x8ec5fc));

    //设置图形特效(阴影)
    auto effect = new QGraphicsDropShadowEffect(this);
    effect->setOffset(0);
    effect->setBlurRadius(10);
    setGraphicsEffect(effect);

    //关闭按钮
    auto closeBtn = new QPushButton(this);
    closeBtn->setObjectName("closeBtn");
    closeBtn->move(width() - 50, y() + 50);
    closeBtn->clearFocus();
    closeBtn->setFocusPolicy(Qt::NoFocus);
    connect(closeBtn, &QPushButton::clicked, this, &LoginRegisterWindowOne::close);

    //堆栈窗口
    m_stkw = new QStackedWidget(this);
    m_stkw->setGeometry(434, 150, 200, 240);
    m_stkw->addWidget(createLoginPage());
    m_stkw->addWidget(createRegPage());

    m_timer = new QTimer(this);
    m_timer->callOnTimeout([this]()
                           {
                               m_tipLab->clear();
                               m_timer->stop();
                           });

    m_tipLab = new QLabel(this);
    m_tipLab->setGeometry(434, 390, 200, 30);
    m_tipLab->setAlignment(Qt::AlignCenter);
    m_tipLab->setStyleSheet("color:red");
}

void LoginRegisterWindowOne::paintEvent(QPaintEvent* ev)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    //绘制大背景
    painter.fillRect(m_rightRect, Qt::white);

    //绘制左边矩形
    painter.fillRect(m_leftRect, m_lineGrad);

    //绘制弦
    painter.setPen(Qt::NoPen);
    painter.setBrush(m_lineGrad);
    painter.drawChord(m_midRect, -90 * 16, 180 * 16);

    //绘制文本
    painter.setPen(QPen(Qt::white, 1, Qt::PenStyle::SolidLine));
    painter.setFont(QFont("微软雅黑", 30));
    painter.drawStaticText(48, 60, QStaticText("欢迎使用"));

    painter.setFont(QFont("微软雅黑", 10));
    painter.drawStaticText(52, 120, QStaticText(m_hintText));

    //绘制图片
    painter.drawPixmap(QRect(40, 200, 250, 250), QPixmap(":/login/cai.png"));
    painter.drawPixmap(QPoint(423, 70), QPixmap(":/login/dunkai.png"));
}

QWidget* LoginRegisterWindowOne::createLoginPage()
{
    QWidget* w = new QWidget;
    m_usernameEdit = new QLineEdit;
    m_passwordEdit = new QLineEdit;
    m_remPwdChx = new QCheckBox("记住密码");
    m_goRegBtn = new QPushButton("没有账号？去注册");
    m_loginBtn = new QPushButton("登 录");

    auto hlayout = new QHBoxLayout;
    hlayout->addWidget(m_remPwdChx);
    hlayout->addWidget(m_goRegBtn);

    auto vlayout = new QVBoxLayout(w);
    vlayout->setContentsMargins(0, 0, 0, 0);
    vlayout->addWidget(m_usernameEdit);
    vlayout->addWidget(m_passwordEdit);
    vlayout->addLayout(hlayout);
    vlayout->addWidget(m_loginBtn);

    connect(m_goRegBtn, &QPushButton::clicked, [this]() { m_stkw->setCurrentIndex(1); });
    connect(m_loginBtn, &QPushButton::clicked, this, &LoginRegisterWindowOne::slotLoginBtnClicked);

    m_usernameEdit->setPlaceholderText("用户名");
    m_passwordEdit->setPlaceholderText("密码");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_remPwdChx->setObjectName("remPwdChx");
    m_goRegBtn->setObjectName("goRegBtn");
    m_goRegBtn->setFocusPolicy(Qt::NoFocus);

    m_loginBtn->setObjectName("loginBtn");
    m_loginBtn->setDefault(true);

    return w;
}

QWidget* LoginRegisterWindowOne::createRegPage()
{
    QWidget* w = new QWidget;
    m_reg_usernameEdit = new QLineEdit;
    m_reg_pwdEdit = new QLineEdit;
    m_reg_rePwdEdit = new QLineEdit;
    m_reg_goLoginBtn = new QPushButton("去登录");
    m_reg_regBtn = new QPushButton("注册");

    auto hlayout = new QHBoxLayout;
    hlayout->setSpacing(0);
    hlayout->addWidget(m_reg_regBtn);
    hlayout->addWidget(m_reg_goLoginBtn);

    auto vlayout = new QVBoxLayout(w);
    vlayout->setContentsMargins(0, 0, 0, 0);
    vlayout->addWidget(m_reg_usernameEdit);
    vlayout->addWidget(m_reg_pwdEdit);
    vlayout->addWidget(m_reg_rePwdEdit);
    vlayout->addLayout(hlayout);

    connect(m_reg_goLoginBtn, &QPushButton::clicked, [this]() { m_stkw->setCurrentIndex(0); });
    connect(m_reg_regBtn, &QPushButton::clicked, this, &LoginRegisterWindowOne::onRegisterBtnClicked);

    m_reg_usernameEdit->setPlaceholderText("用户名");

    m_reg_pwdEdit->setPlaceholderText("密码");
    m_reg_rePwdEdit->setPlaceholderText("确认密码");
    m_reg_pwdEdit->setEchoMode(QLineEdit::Password);
    m_reg_rePwdEdit->setEchoMode(QLineEdit::Password);

    m_reg_goLoginBtn->setObjectName("goLoginBtn");
    m_reg_regBtn->setObjectName("regBtn");
    return w;
}

void LoginRegisterWindowOne::setTipMsg(const QString& msg, int sec)
{
    m_tipLab->setText(msg);

    if (m_timer->isActive())
    {
        m_timer->stop();
    }

    m_timer->start(sec);
}

Qt::CheckState LoginRegisterWindowOne::isRememberPassword()
{
    return m_remPwdChx->checkState();
}

void LoginRegisterWindowOne::setUserNameAndPwd(const QString& userName, const QString& pwd)
{
    m_usernameEdit->setText(userName);
    m_passwordEdit->setText(pwd);
}

void LoginRegisterWindowOne::setremPwdChxCheckState(Qt::CheckState state)
{
    m_remPwdChx->setCheckState(state);
}

void LoginRegisterWindowOne::setHintText(const QString &hintText)
{
    m_hintText = hintText;
}

void LoginRegisterWindowOne::initStyleSheet()
{
    m_styleSheet = R"(
QPushButton#closeBtn {
    border:none;
    background-image:url(:/login/close.png);
    width:32px;
    height:32px;
}

QPushButton#closeBtn:hover {
    background-image:url(:/login/close-hover.png);
}

QLineEdit{
    border:none;
    border-bottom:1px solid rgb(223, 223, 223);
    height:40px;
    font-size:12pt;
}

QLineEdit:hover{
    border-color:rgb(11,194,255);
}

QCheckBox {
    color:rgb(127,127,127);
}
QCheckBox:hover {
    color:rgb(110,110,110);
}
QCheckBox::indicator{
    border-image:url(:/login/uncheck.png);
}

QCheckBox::indicator:checked{
    border-image:url(:/login/check.png);
}

QPushButton#goRegBtn{
    border:none;
    color:rgb(127,127,127);
}

QPushButton#goRegBtn:hover{
    color:rgb(11,194,255);
}

QPushButton#loginBtn {
    border: none;
    border-radius: 5px;
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(11,194,255), stop:1 rgb(0,182,250));
    color: white;
    height: 30px;
    font-size:12pt;
}

QPushButton#loginBtn:hover {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(34,203,255), stop:1 rgb(26,194,251));
}


QPushButton#regBtn, QPushButton#goLoginBtn
{
    border:none;
    height:30px;
    color:white;
    font-size:12pt;
}

QPushButton#regBtn
{
    width:120px;
    background-color:rgb(217, 31, 31);
    border-top-left-radius:5px;
}

QPushButton#regBtn:hover
{
    background-color:rgb(229, 74, 74);
}

QPushButton#goLoginBtn
{
    background-color:rgb(39, 178, 239);
    border-bottom-right-radius:5px;
}
QPushButton#goLoginBtn:hover
{
    background-color:rgb(26,194,251);
})";

    this->setStyleSheet(m_styleSheet);
}
