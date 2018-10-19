#pragma once

#include <QtCore/QCommandLineParser>

namespace TextureTool {

class ToolParser : public QCommandLineParser
{
public:
    explicit ToolParser(QByteArray toolName);

    void process(const QStringList &arguments);
    Q_NORETURN void showHelp(int code);

    static void showMessage(const QString &message);
    static void showError(const QString &message);

private:
    Q_NORETURN void exit(int code);

private:
    QByteArray _toolId;
    QCommandLineOption helpOption;
};

} // namespace TextureTool
