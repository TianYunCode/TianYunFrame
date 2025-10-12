#pragma once

#include <QLineEdit>
#include <QRegularExpression>
#include <QKeyEvent>

class MacAddressEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit MacAddressEdit(QWidget* parent = nullptr);

    // 获取标准化的MAC地址（大写字母，冒号分隔）
    QString macAddress() const;

    // 验证MAC地址是否有效（12个十六进制字符）
    bool isValid() const;

protected:
    // 事件过滤器处理粘贴操作
    bool eventFilter(QObject* obj, QEvent* event) override;

    // 处理键盘事件（自动跳转和删除）
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    // 格式化文本
    void formatText();

private:
    // 处理粘贴的MAC地址（自动格式化）
    void handlePaste();

    // 移除所有非十六进制字符
    QString removeNonHex(const QString& input) const;

    // 格式化MAC地址字符串
    QString formatMacString(const QString& input) const;

    // 跳转到下一个输入段
    void jumpToNextSegment();
};