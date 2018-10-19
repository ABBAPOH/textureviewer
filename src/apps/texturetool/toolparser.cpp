#include "toolparser.h"
#include "exception.h"

#include <QDir>

namespace TextureTool {

/*!
    \class ToolParser
    This is helper class that simplifies usage of the QCommandLineParser.
    Provides built-in procesing of the help option which prints tool help.
*/

/*!
    Constructs a ToolParser instance with the given \a toolName.
*/
ToolParser::ToolParser(const QByteArray &toolName) :
    _toolId(toolName),
    helpOption(addHelpOption())
{
    if (_toolId.isEmpty())
        throw RuntimeError("Tool name is empty");
}

/*!
    Parses and handles the default options (help). Prints help and throws ExitException in case
    of parse error.
*/
void ToolParser::process(const QStringList &arguments)
{
    if (!parse(arguments)) {
        showError(errorText());
        showHelp(EXIT_FAILURE);
    }

    if (isSet(helpOption))
        showHelp(EXIT_SUCCESS);
}

/*!
   Prints help and throws ExitException with the given \a code.
*/
void ToolParser::showHelp(int code)
{
    const auto filePath = qApp->applicationFilePath();
    const auto text = helpText().replace(QDir::toNativeSeparators(filePath),
                                         QString("%1 %2").
                                         arg(QFileInfo(filePath).fileName(), QString::fromLatin1(_toolId)));
    showMessage(text);
    exit(code);
}

/*!
    Prints the given \a message to the sdtout stream.
*/
void ToolParser::showMessage(const QString &message)
{
    fputs(qPrintable(message + "\n"), stdout);
}

/*!
    Prints the given \a message to the sdterr stream.
*/
void ToolParser::showError(const QString &message)
{
    fputs(qPrintable(message + "\n"), stderr);
}

/*!
    \internal
    Throws ExitException with then given \a code.
*/
void ToolParser::exit(int code)
{
    throw ExitException(code);
}

} // namespace TextureTool
