#pragma once

#include <QObject>
#include <QDialog>
#include <QLinearGradient>

class QPushButton;
class QLineEdit;
class QCheckBox;
class QStackedWidget;
class QLabel;

class LoginRegisterWindowOne : public QDialog
{
	Q_OBJECT

public:
    LoginRegisterWindowOne(QWidget* parent = nullptr, QString hintText = "");

    void handleRegisterResult(bool result);                                 // 处理注册结果
    void setTipMsg(const QString& msg, int msec = 3000);                    // 设置提示信息，默认3秒后清除
    Qt::CheckState isRememberPassword();                                    // 是否记住密码
	void setUserNameAndPwd(const QString& userName, const QString& pwd);	// 设置用户名和密码
    void setremPwdChxCheckState(Qt::CheckState state);                      // 设置记住密码复选框状态
    void setHintText(const QString& hintText);                              // 设置左侧提示文本

private:
    void initUi();                  // 初始化UI
    QWidget* createLoginPage();     // 创建登录页面
    QWidget* createRegPage();       // 创建注册页面
    void initStyleSheet();          // 初始化样式表

protected:
	void paintEvent(QPaintEvent* ev)override;

signals:
	void signalLoginRequest(const QString& userName, const QString& passwd);		// 登录请求信号
	void signalRegisterRequest(const QString& userName, const QString& passwd);		// 注册请求信号

private slots:
	void slotLoginBtnClicked();		// 登录按钮点击
	void onRegisterBtnClicked();	// 注册按钮点击

private:
    QStackedWidget* m_stkw{};           // 堆栈窗口
	
    QLineEdit* m_usernameEdit{};        // 用户名输入框
    QLineEdit* m_passwordEdit{};        // 密码输入框
    QCheckBox* m_remPwdChx{};           // 记住密码复选框
    QPushButton* m_goRegBtn{};          // 去注册按钮
    QPushButton* m_loginBtn{};          // 登录按钮

    QLineEdit* m_reg_usernameEdit{};    // 注册用户名输入框
    QLineEdit* m_reg_pwdEdit{};         // 注册密码输入框
    QLineEdit* m_reg_rePwdEdit{};       // 确认密码输入框
    QPushButton* m_reg_goLoginBtn{};    // 去登录按钮
    QPushButton* m_reg_regBtn{};        // 注册按钮

    QLabel* m_tipLab{};                 // 提示标签
    QTimer* m_timer{};                  // 提示信息定时器

    QLinearGradient m_lineGrad;         // 线性渐变
    QRect m_leftRect;                   // 左侧矩形区域
    QRect m_rightRect;                  // 右侧矩形区域
    QRect m_midRect;                    // 中间弦区域

    QString m_styleSheet{};             // 样式表
    QString m_hintText{};               // 左侧提示文本
};
