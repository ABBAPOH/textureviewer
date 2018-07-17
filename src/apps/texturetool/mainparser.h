#pragma once

#include "toolparser.h"
#include <QtCore/QCommandLineParser>
#include <QtCore/QMap>

namespace TextureTool {

class MainParser
{
public:
    using DescriptionMap = std::map<QByteArray, QString>;

    explicit MainParser(const DescriptionMap &map);

    void process(const QStringList &arguments);
    Q_NORETURN void showHelp(int code);
    Q_NORETURN void showVersion();

    inline QString toolName() { return _name; }
    inline QStringList arguments() const { return _arguments; }

    static void showMessage(const QString &message);
    static void showError(const QString &message);

private:
    Q_NORETURN void exit(int code);

private:
    DescriptionMap _map;
    QCommandLineParser parser;
    QCommandLineOption helpOption;
    QCommandLineOption versionOption;
    QString _name;
    QStringList _arguments;
};

} // namespace TextureTool
