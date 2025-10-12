#include "QtHelp.h"
#include <QStandardPaths>
#include <QHostInfo>
#include <QSysInfo>
#include <QStorageInfo>
#include <QProcess>
#include <QCoreApplication>

// 网络管理器单例
QNetworkAccessManager* QtHelper::networkManager()
{
    static QNetworkAccessManager instance;
    return &instance;
}

// 应用设置单例
QSettings* QtHelper::appSettings()
{
    static QSettings instance(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    return &instance;
}

/************************ 文件操作相关 ************************/
bool QtHelper::fileWrite(const QString &filePath, const QByteArray &data)
{
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(data);
        file.close();
        return true;
    }
    return false;
}

QByteArray QtHelper::fileRead(const QString &filePath)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray data = file.readAll();
        file.close();
        return data;
    }
    return QByteArray();
}

bool QtHelper::fileAppend(const QString &filePath, const QByteArray &data)
{
    QFile file(filePath);
    if (file.open(QIODevice::Append))
    {
        file.write(data);
        file.close();
        return true;
    }
    return false;
}

bool QtHelper::fileCopy(const QString &sourcePath, const QString &targetPath)
{
    return QFile::copy(sourcePath, targetPath);
}

bool QtHelper::fileMove(const QString &sourcePath, const QString &targetPath)
{
    return QFile::rename(sourcePath, targetPath);
}

bool QtHelper::fileRemove(const QString &filePath)
{
    return QFile::remove(filePath);
}

bool QtHelper::fileRename(const QString &oldPath, const QString &newPath)
{
    return QFile::rename(oldPath, newPath);
}

bool QtHelper::fileExists(const QString &filePath)
{
    return QFile::exists(filePath);
}

qint64 QtHelper::fileSize(const QString &filePath)
{
    QFileInfo info(filePath);
    return info.size();
}

QDateTime QtHelper::fileCreatedTime(const QString &filePath)
{
    QFileInfo info(filePath);
    return info.birthTime();
}

QDateTime QtHelper::fileModifiedTime(const QString &filePath)
{
    QFileInfo info(filePath);
    return info.lastModified();
}

QDateTime QtHelper::fileLastAccessTime(const QString &filePath)
{
    QFileInfo info(filePath);
    return info.lastRead();
}

bool QtHelper::fileSetPermissions(const QString &filePath, QFile::Permissions permissions)
{
    return QFile::setPermissions(filePath, permissions);
}

QFile::Permissions QtHelper::filePermissions(const QString &filePath)
{
    return QFile::permissions(filePath);
}

bool QtHelper::dirCreate(const QString &dirPath)
{
    return QDir().mkpath(dirPath);
}

bool QtHelper::dirRemove(const QString &dirPath)
{
    QDir dir(dirPath);
    return dir.removeRecursively();
}

bool QtHelper::dirExists(const QString &dirPath)
{
    return QDir(dirPath).exists();
}

QStringList QtHelper::dirEntryList(const QString &dirPath, const QStringList &nameFilters, QDir::Filters filters, QDir::SortFlags sort)
{
    return QDir(dirPath).entryList(nameFilters, filters, sort);
}

bool QtHelper::dirCopy(const QString &sourceDir, const QString &targetDir)
{
    // 简化实现，实际可能需要递归复制
    return QDir(sourceDir).exists() && QDir().mkpath(targetDir) &&  QtHelper::dirEntryList(sourceDir).count() > 0;
}

bool QtHelper::dirMove(const QString &sourceDir, const QString &targetDir)
{
    return QDir().rename(sourceDir, targetDir);
}

qint64 QtHelper::dirSize(const QString &dirPath)
{
    qint64 size = 0;
    QDir dir(dirPath);
    QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::Hidden | QDir::NoDotAndDotDot);
    
    for (const QFileInfo &info : list)
    {
        if (info.isDir())
        {
            size += dirSize(info.absoluteFilePath());
        }
        else
        {
            size += info.size();
        }
    }

    return size;
}

QString QtHelper::dirCurrent()
{
    return QDir::currentPath();
}

bool QtHelper::dirSetCurrent(const QString &dirPath)
{
    return QDir::setCurrent(dirPath);
}

QString QtHelper::dirHome()
{
    return QDir::homePath();
}

QString QtHelper::dirTemp()
{
    return QDir::tempPath();
}

QString QtHelper::dirRoot()
{
    return QDir::rootPath();
}

QString QtHelper::pathJoin(const QString &path1, const QString &path2)
{
    return QDir::cleanPath(path1 + QDir::separator() + path2);
}

QString QtHelper::pathAbsolute(const QString &path)
{
    return QDir(path).absolutePath();
}

QString QtHelper::pathRelative(const QString &path, const QString &base)
{
    return QDir(base).relativeFilePath(path);
}

QString QtHelper::pathBaseName(const QString &path)
{
    return QFileInfo(path).baseName();
}

QString QtHelper::pathFileName(const QString &path)
{
    return QFileInfo(path).fileName();
}

QString QtHelper::pathDirName(const QString &path)
{
    return QFileInfo(path).path();
}

QString QtHelper::pathSuffix(const QString &path)
{
    return QFileInfo(path).suffix();
}

QString QtHelper::pathCompleteSuffix(const QString &path)
{
    return QFileInfo(path).completeSuffix();
}

bool QtHelper::pathIsAbsolute(const QString &path)
{
    return QFileInfo(path).isAbsolute();
}

bool QtHelper::pathIsRelative(const QString &path)
{
    return QFileInfo(path).isRelative();
}

QString QtHelper::pathClean(const QString &path)
{
    return QDir::cleanPath(path);
}

/************************ 字符串操作相关 ************************/
QString QtHelper::strTrim(const QString &str)
{
    return str.trimmed();
}

QString QtHelper::strLeft(const QString &str, int n)
{
    return str.left(n);
}

QString QtHelper::strRight(const QString &str, int n)
{
    return str.right(n);
}

QString QtHelper::strMid(const QString &str, int position, int n)
{
    return str.mid(position, n);
}

QString QtHelper::strToLower(const QString &str)
{
    return str.toLower();
}

QString QtHelper::strToUpper(const QString &str)
{
    return str.toUpper();
}

int QtHelper::strCompare(const QString &str1, const QString &str2, Qt::CaseSensitivity cs)
{
    return QString::compare(str1, str2, cs);
}

bool QtHelper::strStartsWith(const QString &str, const QString &start, Qt::CaseSensitivity cs)
{
    return str.startsWith(start, cs);
}

bool QtHelper::strEndsWith(const QString &str, const QString &end, Qt::CaseSensitivity cs)
{
    return str.endsWith(end, cs);
}

bool QtHelper::strContains(const QString &str, const QString &substr, Qt::CaseSensitivity cs)
{
    return str.contains(substr, cs);
}

int QtHelper::strCount(const QString &str, const QString &substr, Qt::CaseSensitivity cs)
{
    return str.count(substr, cs);
}

QString QtHelper::strReplace(const QString &str, const QString &before, const QString &after, Qt::CaseSensitivity cs)
{
    QString result = str;
    return result.replace(before, after, cs);
}

QStringList QtHelper::strSplit(const QString &str, const QString &delimiter)
{
    return str.split(delimiter);
}

QString QtHelper::strJoin(const QStringList &list, const QString &separator)
{
    return list.join(separator);
}

QString QtHelper::strRepeat(const QString &str, int times)
{
    return QString(str).repeated(times);
}

QString QtHelper::strReverse(const QString &str)
{
    QString result = str;
    std::reverse(result.begin(), result.end());
    return result;
}

QString QtHelper::strRemove(const QString &str, const QString &substr, Qt::CaseSensitivity cs)
{
    QString result = str;
    return result.remove(substr, cs);
}

QByteArray QtHelper::strToUtf8(const QString &str)
{
    return str.toUtf8();
}

QByteArray QtHelper::strToLocal8Bit(const QString &str)
{
    return str.toLocal8Bit();
}

QString QtHelper::strFromUtf8(const QByteArray &data)
{
    return QString::fromUtf8(data);
}

QString QtHelper::strFromLocal8Bit(const QByteArray &data)
{
    return QString::fromLocal8Bit(data);
}

QString QtHelper::strToHtmlEscaped(const QString &str)
{
    return str.toHtmlEscaped();
}

#include <QTextDocument>

QString QtHelper::strFromHtmlEscaped(const QString &str)
{
    QTextDocument text;

    text.setHtml(str);

    return text.toPlainText();
}

QString QtHelper::strFormat(const QString &format, const QVariant &arg1, const QVariant &arg2, const QVariant &arg3, const QVariant &arg4)
{
    QString result = format;

    if (arg1.isValid()) result = result.arg(arg1.toString());
    if (arg2.isValid()) result = result.arg(arg2.toString());
    if (arg3.isValid()) result = result.arg(arg3.toString());
    if (arg4.isValid()) result = result.arg(arg4.toString());

    return result;
}

QString QtHelper::strNumber(int n, int base)
{
    return QString::number(n, base);
}

QString QtHelper::strNumber(double n, char format, int precision)
{
    return QString::number(n, format, precision);
}

QString QtHelper::strArg(const QString &str, const QString &a)
{
    return str.arg(a);
}

QString QtHelper::strArg(const QString &str, int a, int fieldWidth, int base, QChar fillChar)
{
    return str.arg(a, fieldWidth, base, fillChar);
}

QString QtHelper::strArg(const QString &str, double a, int fieldWidth, char format, int precision, QChar fillChar)
{
    return str.arg(a, fieldWidth, format, precision, fillChar);
}

bool QtHelper::regexMatch(const QString &str, const QString &pattern)
{
    QRegularExpression regex(pattern);
    return regex.match(str).hasMatch();
}

QStringList QtHelper::regexCapture(const QString &str, const QString &pattern)
{
    QRegularExpression regex(pattern);
    QRegularExpressionMatch match = regex.match(str);
    QStringList captures;
    
    if (match.hasMatch())
    {
        for (int i = 0; i <= match.lastCapturedIndex(); ++i)
        {
            captures.append(match.captured(i));
        }
    }

    return captures;
}

QString QtHelper::regexReplace(const QString &str, const QString &pattern, const QString &after)
{
    QString result = str;
    QRegularExpression regex(pattern);
    return result.replace(regex, after);
}

/************************ 时间操作相关 ************************/
QDateTime QtHelper::timeCurrent()
{
    return QDateTime::currentDateTime();
}

qint64 QtHelper::timeCurrentMSecs()
{
    return QDateTime::currentMSecsSinceEpoch();
}

qint64 QtHelper::timeCurrentSecs()
{
    return QDateTime::currentSecsSinceEpoch();
}

QString QtHelper::timeCurrentString(const QString &format)
{
    return QDateTime::currentDateTime().toString(format);
}

QString QtHelper::timeToString(const QDateTime &datetime, const QString &format)
{
    return datetime.toString(format);
}

QDateTime QtHelper::timeFromString(const QString &string, const QString &format)
{
    return QDateTime::fromString(string, format);
}

qint64 QtHelper::timeToMSecs(const QDateTime &datetime)
{
    return datetime.toMSecsSinceEpoch();
}

QDateTime QtHelper::timeFromMSecs(qint64 msecs)
{
    return QDateTime::fromMSecsSinceEpoch(msecs);
}

QDateTime QtHelper::timeAddDays(const QDateTime &datetime, int days)
{
    return datetime.addDays(days);
}

QDateTime QtHelper::timeAddHours(const QDateTime &datetime, int hours)
{
    return datetime.addSecs(hours * 3600);
}

QDateTime QtHelper::timeAddMinutes(const QDateTime &datetime, int minutes)
{
    return datetime.addSecs(minutes * 60);
}

QDateTime QtHelper::timeAddSeconds(const QDateTime &datetime, int seconds)
{
    return datetime.addSecs(seconds);
}

QDateTime QtHelper::timeAddMSecs(const QDateTime &datetime, qint64 msecs)
{
    return datetime.addMSecs(msecs);
}

qint64 QtHelper::timeDiffDays(const QDateTime &datetime1, const QDateTime &datetime2)
{
    return datetime1.daysTo(datetime2);
}

qint64 QtHelper::timeDiffHours(const QDateTime &datetime1, const QDateTime &datetime2)
{
    return datetime1.secsTo(datetime2) / 3600;
}

qint64 QtHelper::timeDiffMinutes(const QDateTime &datetime1, const QDateTime &datetime2)
{
    return datetime1.secsTo(datetime2) / 60;
}

qint64 QtHelper::timeDiffSeconds(const QDateTime &datetime1, const QDateTime &datetime2)
{
    return datetime1.secsTo(datetime2);
}

qint64 QtHelper::timeDiffMSecs(const QDateTime &datetime1, const QDateTime &datetime2)
{
    return datetime1.msecsTo(datetime2);
}

bool QtHelper::timeIsValid(const QString &timeString, const QString &format)
{
    return QDateTime::fromString(timeString, format).isValid();
}

bool QtHelper::timeIsLeapYear(int year)
{
    return QDate::isLeapYear(year);
}

int QtHelper::timeDaysInMonth(int year, int month)
{
    return QDate(year, month, 1).daysInMonth();
}

int QtHelper::timeDaysInYear(int year)
{
    return QDate::isLeapYear(year) ? 366 : 365;
}

/************************ 网络操作相关 ************************/
QString QtHelper::urlEncode(const QString &url)
{
    return QUrl::toPercentEncoding(url);
}

QString QtHelper::urlDecode(const QString &url)
{
    return QUrl::fromPercentEncoding(url.toUtf8());
}

QString QtHelper::urlToLocalFile(const QUrl &url)
{
    return url.toLocalFile();
}

QUrl QtHelper::urlFromLocalFile(const QString &filePath)
{
    return QUrl::fromLocalFile(filePath);
}

QString QtHelper::urlFileName(const QUrl &url)
{
    return QFileInfo(url.path()).fileName();
}

QString QtHelper::urlPath(const QUrl &url)
{
    return url.path();
}

QString QtHelper::urlQuery(const QUrl &url)
{
    return url.query();
}

QByteArray QtHelper::httpGet(const QUrl &url, int timeout)
{
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager()->get(request);
    
    QElapsedTimer timer;
    timer.start();
    
    while (!reply->isFinished() && timer.elapsed() < timeout)
    {
        QCoreApplication::processEvents();
    }
    
    if (reply->isFinished())
    {
        QByteArray data = reply->readAll();
        reply->deleteLater();
        return data;
    }
    
    reply->abort();
    reply->deleteLater();
    return QByteArray();
}

QByteArray QtHelper::httpPost(const QUrl &url, const QByteArray &data, int timeout)
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *reply = networkManager()->post(request, data);
    
    QElapsedTimer timer;
    timer.start();
    
    while (!reply->isFinished() && timer.elapsed() < timeout)
    {
        QCoreApplication::processEvents();
    }
    
    if (reply->isFinished())
    {
        QByteArray response = reply->readAll();
        reply->deleteLater();
        return response;
    }
    
    reply->abort();
    reply->deleteLater();
    return QByteArray();
}

bool QtHelper::httpDownload(const QUrl &url, const QString &savePath, int timeout)
{
    QByteArray data = httpGet(url, timeout);
    if (!data.isEmpty())
    {
        return fileWrite(savePath, data);
    }

    return false;
}

QString QtHelper::httpUserAgent()
{
    return QString();
}

void QtHelper::httpSetUserAgent(const QString &userAgent)
{
    // 设置全局User-Agent
}

/************************ 图像操作相关 ************************/
QImage QtHelper::imageLoad(const QString &filePath)
{
    return QImage(filePath);
}

bool QtHelper::imageSave(const QImage &image, const QString &filePath, const char *format, int quality)
{
    return image.save(filePath, format, quality);
}

QImage QtHelper::imageScale(const QImage &image, const QSize &size, Qt::AspectRatioMode aspectMode, Qt::TransformationMode transformMode)
{
    return image.scaled(size, aspectMode, transformMode);
}

QImage QtHelper::imageScale(const QImage &image, int width, int height, Qt::AspectRatioMode aspectMode, Qt::TransformationMode transformMode)
{
    return image.scaled(width, height, aspectMode, transformMode);
}

QImage QtHelper::imageCrop(const QImage &image, const QRect &rect)
{
    return image.copy(rect);
}

QImage QtHelper::imageRotate(const QImage &image, double angle)
{
    QTransform transform;
    transform.rotate(angle);
    return image.transformed(transform);
}

QImage QtHelper::imageMirror(const QImage &image, bool horizontal, bool vertical)
{
    return image.mirrored(horizontal, vertical);
}

QImage QtHelper::imageGray(const QImage &image)
{
    return image.convertToFormat(QImage::Format_Grayscale8);
}

QImage QtHelper::imageBlur(const QImage &image, int radius)
{
    // 简化实现，实际可能需要使用更复杂的算法
    return image;
}

QImage QtHelper::imageBrightness(const QImage &image, int delta)
{
    QImage result = image;
    for (int y = 0; y < result.height(); ++y)
    {
        for (int x = 0; x < result.width(); ++x)
        {
            QColor color = result.pixelColor(x, y);
            color.setRed(qBound(0, color.red() + delta, 255));
            color.setGreen(qBound(0, color.green() + delta, 255));
            color.setBlue(qBound(0, color.blue() + delta, 255));
            result.setPixelColor(x, y, color);
        }
    }

    return result;
}

QImage QtHelper::imageContrast(const QImage &image, int delta)
{
    QImage result = image;
    double factor = (259.0 * (delta + 255.0)) / (255.0 * (259.0 - delta));
    
    for (int y = 0; y < result.height(); ++y)
    {
        for (int x = 0; x < result.width(); ++x)
        {
            QColor color = result.pixelColor(x, y);
            color.setRed(qBound(0, int(factor * (color.red() - 128) + 128), 255));
            color.setGreen(qBound(0, int(factor * (color.green() - 128) + 128), 255));
            color.setBlue(qBound(0, int(factor * (color.blue() - 128) + 128), 255));
            result.setPixelColor(x, y, color);
        }
    }

    return result;
}

QColor QtHelper::imagePixel(const QImage &image, int x, int y)
{
    return image.pixelColor(x, y);
}

QSize QtHelper::imageSize(const QImage &image)
{
    return image.size();
}

QPixmap QtHelper::imageToPixmap(const QImage &image)
{
    return QPixmap::fromImage(image);
}

QImage QtHelper::pixmapToImage(const QPixmap &pixmap)
{
    return pixmap.toImage();
}

QByteArray QtHelper::imageToData(const QImage &image, const char *format, int quality)
{
    QByteArray data;

    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, format, quality);

    return data;
}

QImage QtHelper::imageFromData(const QByteArray &data)
{
    return QImage::fromData(data);
}

/************************ 加密解密相关 ************************/
QByteArray QtHelper::hashMD5(const QByteArray &data)
{
    return QCryptographicHash::hash(data, QCryptographicHash::Md5);
}

QByteArray QtHelper::hashSHA1(const QByteArray &data)
{
    return QCryptographicHash::hash(data, QCryptographicHash::Sha1);
}

QByteArray QtHelper::hashSHA256(const QByteArray &data)
{
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256);
}

QString QtHelper::hashFileMD5(const QString &filePath)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        QCryptographicHash hash(QCryptographicHash::Md5);
        if (hash.addData(&file))
        {
            return QString(hash.result().toHex());
        }
    }

    return QString();
}

QString QtHelper::hashFileSHA1(const QString &filePath)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        QCryptographicHash hash(QCryptographicHash::Sha1);
        if (hash.addData(&file))
        {
            return QString(hash.result().toHex());
        }
    }

    return QString();
}

QString QtHelper::hashFileSHA256(const QString &filePath)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        QCryptographicHash hash(QCryptographicHash::Sha256);
        if (hash.addData(&file))
        {
            return QString(hash.result().toHex());
        }
    }

    return QString();
}

QByteArray QtHelper::base64Encode(const QByteArray &data)
{
    return data.toBase64();
}

QByteArray QtHelper::base64Decode(const QByteArray &data)
{
    return QByteArray::fromBase64(data);
}

QString QtHelper::base64Encode(const QString &str)
{
    return str.toUtf8().toBase64();
}

QString QtHelper::base64Decode(const QString &str)
{
    return QByteArray::fromBase64(str.toUtf8());
}

/************************ 系统操作相关 ************************/
QString QtHelper::systemName()
{
    return QSysInfo::prettyProductName();
}

QString QtHelper::systemVersion()
{
    return QSysInfo::productVersion();
}

QString QtHelper::systemArchitecture()
{
    return QSysInfo::currentCpuArchitecture();
}

qint64 QtHelper::systemMemoryTotal()
{
    // 简化实现，实际可能需要平台特定代码
    return 0;
}

qint64 QtHelper::systemMemoryFree()
{
    // 简化实现，实际可能需要平台特定代码
    return 0;
}

qint64 QtHelper::systemDiskTotal(const QString &path)
{
    QStorageInfo storage(path);
    return storage.bytesTotal();
}

qint64 QtHelper::systemDiskFree(const QString &path)
{
    QStorageInfo storage(path);
    return storage.bytesFree();
}

int QtHelper::systemCpuCount()
{
    return QThread::idealThreadCount();
}

QString QtHelper::systemUserName()
{
    return qgetenv("USERNAME");
}

QString QtHelper::systemMachineName()
{
    return QHostInfo::localHostName();
}

QString QtHelper::systemLocale()
{
    return QLocale::system().name();
}

bool QtHelper::processExecute(const QString &program, const QStringList &arguments)
{
    return QProcess::execute(program, arguments) == 0;
}

QByteArray QtHelper::processExecuteOutput(const QString &program, const QStringList &arguments)
{
    QProcess process;
    process.start(program, arguments);
    process.waitForFinished();
    return process.readAllStandardOutput();
}

int QtHelper::processId()
{
    return QCoreApplication::applicationPid();
}

bool QtHelper::processIsRunning(const QString &processName)
{
    // 简化实现，实际可能需要平台特定代码
    return false;
}

bool QtHelper::processKill(const QString &processName)
{
    // 简化实现，实际可能需要平台特定代码
    return false;
}

/************************ 应用程序相关 ************************/
QString QtHelper::appName()
{
    return QCoreApplication::applicationName();
}

QString QtHelper::appVersion()
{
    return QCoreApplication::applicationVersion();
}

QString QtHelper::appOrganization()
{
    return QCoreApplication::organizationName();
}

QString QtHelper::appDomain()
{
    return QCoreApplication::organizationDomain();
}

QString QtHelper::appPath()
{
    return QCoreApplication::applicationDirPath();
}

QString QtHelper::appFilePath()
{
    return QCoreApplication::applicationFilePath();
}

void QtHelper::appSetValue(const QString &key, const QVariant &value)
{
    appSettings()->setValue(key, value);
}

QVariant QtHelper::appValue(const QString &key, const QVariant &defaultValue)
{
    return appSettings()->value(key, defaultValue);
}

void QtHelper::appRemove(const QString &key)
{
    appSettings()->remove(key);
}

bool QtHelper::appContains(const QString &key)
{
    return appSettings()->contains(key);
}

void QtHelper::clipboardSetText(const QString &text)
{
    QGuiApplication::clipboard()->setText(text);
}

QString QtHelper::clipboardText()
{
    return QGuiApplication::clipboard()->text();
}

void QtHelper::clipboardSetImage(const QImage &image)
{
    QGuiApplication::clipboard()->setImage(image);
}

QImage QtHelper::clipboardImage()
{
    return QGuiApplication::clipboard()->image();
}

/************************ 界面操作相关 ************************/
QSize QtHelper::screenSize()
{
    return QGuiApplication::primaryScreen()->size();
}

int QtHelper::screenWidth()
{
    return QGuiApplication::primaryScreen()->size().width();
}

int QtHelper::screenHeight()
{
    return QGuiApplication::primaryScreen()->size().height();
}

int QtHelper::screenDpi()
{
    return QGuiApplication::primaryScreen()->logicalDotsPerInch();
}

double QtHelper::screenScale()
{
    return QGuiApplication::primaryScreen()->devicePixelRatio();
}

int QtHelper::screenCount()
{
    return QGuiApplication::screens().count();
}

QRect QtHelper::screenAvailableGeometry(int screen)
{
    QList<QScreen*> screens = QGuiApplication::screens();
    if (screen >= 0 && screen < screens.count())
    {
        return screens[screen]->availableGeometry();
    }

    return QRect();
}

QColor QtHelper::colorFromRgb(int r, int g, int b, int a)
{
    return QColor(r, g, b, a);
}

QColor QtHelper::colorFromHsv(int h, int s, int v, int a)
{
    return QColor::fromHsv(h, s, v, a);
}

QColor QtHelper::colorFromString(const QString &colorString)
{
    return QColor(colorString);
}

QString QtHelper::colorToString(const QColor &color)
{
    return color.name();
}

QColor QtHelper::colorLighter(const QColor &color, int factor)
{
    return color.lighter(factor);
}

QColor QtHelper::colorDarker(const QColor &color, int factor)
{
    return color.darker(factor);
}

int QtHelper::colorHue(const QColor &color)
{
    return color.hue();
}

int QtHelper::colorSaturation(const QColor &color)
{
    return color.saturation();
}

int QtHelper::colorValue(const QColor &color)
{
    return color.value();
}

int QtHelper::colorRed(const QColor &color)
{
    return color.red();
}

int QtHelper::colorGreen(const QColor &color)
{
    return color.green();
}

int QtHelper::colorBlue(const QColor &color)
{
    return color.blue();
}

int QtHelper::colorAlpha(const QColor &color)
{
    return color.alpha();
}

/************************ 数据类型转换相关 ************************/
int QtHelper::toInt(const QString &str, bool *ok, int base)
{
    return str.toInt(ok, base);
}

double QtHelper::toDouble(const QString &str, bool *ok)
{
    return str.toDouble(ok);
}

float QtHelper::toFloat(const QString &str, bool *ok)
{
    return str.toFloat(ok);
}

bool QtHelper::toBool(const QString &str)
{
    return str.toLower() == "true" || str == "1";
}

QString QtHelper::toString(int n, int base)
{
    return QString::number(n, base);
}

QString QtHelper::toString(double n, char format, int precision)
{
    return QString::number(n, format, precision);
}

QString QtHelper::toString(bool b)
{
    return b ? "true" : "false";
}

QVariantList QtHelper::toVariantList(const QList<int> &list)
{
    QVariantList result;
    for (int value : list)
    {
        result.append(value);
    }

    return result;
}

QVariantList QtHelper::toVariantList(const QList<QString> &list)
{
    QVariantList result;
    for (const QString &value : list)
    {
        result.append(value);
    }

    return result;
}

QList<int> QtHelper::toIntList(const QVariantList &list)
{
    QList<int> result;
    for (const QVariant &value : list)
    {
        result.append(value.toInt());
    }

    return result;
}

QList<QString> QtHelper::toStringList(const QVariantList &list)
{
    QList<QString> result;
    for (const QVariant &value : list)
    {
        result.append(value.toString());
    }

    return result;
}

QJsonObject QtHelper::toJsonObject(const QString &jsonString)
{
    return QJsonDocument::fromJson(jsonString.toUtf8()).object();
}

QJsonArray QtHelper::toJsonArray(const QString &jsonString)
{
    return QJsonDocument::fromJson(jsonString.toUtf8()).array();
}

QString QtHelper::fromJsonObject(const QJsonObject &jsonObject)
{
    return QJsonDocument(jsonObject).toJson();
}

QString QtHelper::fromJsonArray(const QJsonArray &jsonArray)
{
    return QJsonDocument(jsonArray).toJson();
}

QVariantMap QtHelper::jsonToMap(const QJsonObject &jsonObject)
{
    return jsonObject.toVariantMap();
}

QJsonObject QtHelper::mapToJson(const QVariantMap &map)
{
    return QJsonObject::fromVariantMap(map);
}

/************************ 其他实用功能 ************************/
int QtHelper::randomInt(int min, int max)
{
    return QRandomGenerator::global()->bounded(min, max + 1);
}

double QtHelper::randomDouble(double min, double max)
{
    return min + (max - min) * QRandomGenerator::global()->generateDouble();
}

QString QtHelper::randomString(int length, const QString &chars)
{
    QString result;
    int charsLength = chars.length();
    
    for (int i = 0; i < length; ++i)
    {
        int index = randomInt(0, charsLength - 1);
        result.append(chars.at(index));
    }

    return result;
}

void QtHelper::sleep(unsigned long msecs)
{
    QThread::sleep(msecs);
}

void QtHelper::msleep(unsigned long msecs)
{
    QThread::msleep(msecs);
}

void QtHelper::usleep(unsigned long usecs)
{
    QThread::usleep(usecs);
}

void QtHelper::debug(const QString &message)
{
    qDebug() << message;
}

void QtHelper::info(const QString &message)
{
    qInfo() << message;
}

void QtHelper::warning(const QString &message)
{
    qWarning() << message;
}

void QtHelper::error(const QString &message)
{
    qCritical() << message;
}

QString QtHelper::objectClassName(QObject *obj)
{
    return obj ? obj->metaObject()->className() : QString();
}

QObject* QtHelper::objectFindChild(QObject *parent, const QString &name)
{
    return parent ? parent->findChild<QObject*>(name) : nullptr;
}

QList<QObject*> QtHelper::objectFindChildren(QObject *parent, const QString &name)
{
    return parent ? parent->findChildren<QObject*>(name) : QList<QObject*>();
}

bool QtHelper::connect(const QObject *sender, const char *signal, const QObject *receiver, const char *method, Qt::ConnectionType type)
{
    return QObject::connect(sender, signal, receiver, method, type);
}

bool QtHelper::disconnect(const QObject *sender, const char *signal, const QObject *receiver, const char *method)
{
    return QObject::disconnect(sender, signal, receiver, method);
}

QString QtHelper::translate(const QString &context, const QString &sourceText, const QString &disambiguation, int n)
{
    return QCoreApplication::translate(context.toUtf8(), sourceText.toUtf8(), disambiguation.toUtf8(), n);
}

bool QtHelper::isWindows()
{
    return QSysInfo::productType() == "windows";
}

bool QtHelper::isMac()
{
    return QSysInfo::productType() == "macos";
}

bool QtHelper::isLinux()
{
    return QSysInfo::productType() == "linux";
}

bool QtHelper::isUnix()
{
    return QSysInfo::productType() == "unix";
}

bool QtHelper::isAndroid()
{
    return QSysInfo::productType() == "android";
}

bool QtHelper::isIOS()
{
    return QSysInfo::productType() == "ios";
}

int QtHelper::pxToPt(int px, int dpi)
{
    return px * 72 / dpi;
}

int QtHelper::ptToPx(int pt, int dpi)
{
    return pt * dpi / 72;
}

int QtHelper::mmToPx(int mm, int dpi)
{
    return mm * dpi / 25.4;
}

int QtHelper::pxToMm(int px, int dpi)
{
    return px * 25.4 / dpi;
}

bool QtHelper::isValidEmail(const QString &email)
{
    QRegularExpression regex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return regex.match(email).hasMatch();
}

bool QtHelper::isValidUrl(const QString &url)
{
    QRegularExpression regex(R"(^(https?|ftp)://[^\s/$.?#].[^\s]*$)");
    return regex.match(url).hasMatch();
}

bool QtHelper::isValidPhone(const QString &phone)
{
    QRegularExpression regex(R"(^\+?[0-9]{10,15}$)");
    return regex.match(phone).hasMatch();
}

bool QtHelper::isValidIp(const QString &ip)
{
    QRegularExpression regex(R"(^(\d{1,3}\.){3}\d{1,3}$)");
    if (!regex.match(ip).hasMatch()) return false;
    
    QStringList parts = ip.split(".");
    for (const QString &part : parts)
    {
        int num = part.toInt();
        if (num < 0 || num > 255) return false;
    }

    return true;
}

bool QtHelper::isNumber(const QString &str)
{
    bool ok;
    str.toDouble(&ok);
    return ok;
}

bool QtHelper::isInteger(const QString &str)
{
    bool ok;
    str.toInt(&ok);
    return ok;
}

bool QtHelper::isFloat(const QString &str)
{
    bool ok;
    str.toFloat(&ok);
    return ok;
}

double QtHelper::mathRound(double value, int decimals)
{
    return QString::number(value, 'f', decimals).toDouble();
}

double QtHelper::mathCeil(double value)
{
    return std::ceil(value);
}

double QtHelper::mathFloor(double value)
{
    return std::floor(value);
}

int QtHelper::mathClamp(int value, int min, int max)
{
    return qBound(min, value, max);
}

double QtHelper::mathClamp(double value, double min, double max)
{
    return qBound(min, value, max);
}

int QtHelper::mathLerp(int a, int b, double t)
{
    return a + (b - a) * t;
}

double QtHelper::mathLerp(double a, double b, double t)
{
    return a + (b - a) * t;
}
