#pragma execution_character_set("utf-8")

#include "MacAddressEdit.h"
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QKeyEvent>
#include <QTimer>

MacAddressEdit::MacAddressEdit(QWidget* parent) : QLineEdit(parent)
{
    // 设置输入掩码：HH:HH:HH:HH:HH:HH
    setInputMask("HH:HH:HH:HH:HH:HH");
    setPlaceholderText("00:00:00:00:00:00");

    // 设置最大长度（包括分隔符）
    setMaxLength(17);

    // 设置样式
    setStyleSheet("QLineEdit { "
        "padding: 2px 4px; "
        "font: 10pt 'Consolas'; "
        "border: 1px solid #ccc; "
        "border-radius: 3px; "
        "}");

    // 安装事件过滤器处理粘贴操作
    installEventFilter(this);

    // 连接文本变化信号
    connect(this, &QLineEdit::textChanged, this, &MacAddressEdit::formatText);
}

// 获取标准化的MAC地址（大写字母，冒号分隔）
QString MacAddressEdit::macAddress() const
{
    return text().toUpper();
}

// 验证MAC地址是否有效（12个十六进制字符）
bool MacAddressEdit::isValid() const
{
    QString mac = removeNonHex(text());
    return mac.length() == 12 && QRegularExpression("^[0-9A-F]{12}$", QRegularExpression::CaseInsensitiveOption).match(mac).hasMatch();
}

bool MacAddressEdit::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->matches(QKeySequence::Paste))
        {
            handlePaste();
            return true;
        }
    }

    return QLineEdit::eventFilter(obj, event);
}

void MacAddressEdit::keyPressEvent(QKeyEvent* event)
{
    // 先调用基类处理，确保基本输入功能
    QLineEdit::keyPressEvent(event);

    // 处理特殊按键
    int cursorPos = cursorPosition();

    // 处理跳转逻辑
    if (!event->text().isEmpty() && event->text().at(0).isLetterOrNumber()) 
    {
        // 延迟跳转，确保文本已更新
        QTimer::singleShot(0, this, &MacAddressEdit::jumpToNextSegment);
    }
    // 处理方向键和删除键的特殊跳转
    else if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Backspace)
    {
        if (cursorPos % 3 == 0 && cursorPos > 0) 
        {
            setCursorPosition(cursorPos - 1);
        }
    }
    else if (event->key() == Qt::Key_Right)
    {
        if (cursorPos % 3 == 2 && cursorPos < 16)
        {
            setCursorPosition(cursorPos + 1);
        }
    }
}

void MacAddressEdit::formatText()
{
    QString current = text();
    QString clean = removeNonHex(current);

    // 当有12个字符时自动格式化
    if (clean.length() == 12) 
    {
        setText(formatMacString(clean));
    }
}

// 处理粘贴的MAC地址（自动格式化）
void MacAddressEdit::handlePaste()
{
    QString clipboard = QApplication::clipboard()->text().trimmed();
    QString cleanMac = removeNonHex(clipboard);

    // 验证长度（必须是12个字符）
    if (cleanMac.length() != 12) return;

    // 格式化粘贴内容
    setText(formatMacString(cleanMac));
}

QString MacAddressEdit::removeNonHex(const QString& input) const
{
    return input.toUpper().remove(QRegularExpression("[^0-9A-F]"));
}

QString MacAddressEdit::formatMacString(const QString& input) const
{
    QString formatted;
    for (int i = 0; i < 12; i += 2) 
    {
        if (i > 0) formatted.append(':');
        formatted.append(input.mid(i, 2));
    }
    return formatted;
}

void MacAddressEdit::jumpToNextSegment()
{
    int pos = cursorPosition();

    // 当前位置在每组字符的第二个位置
    if ((pos == 2 || pos == 5 || pos == 8 || pos == 11 || pos == 14) && pos < 16) 
    {
        setCursorPosition(pos + 1);
    }
}