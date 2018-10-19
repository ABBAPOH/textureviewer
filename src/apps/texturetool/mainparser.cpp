#include "mainparser.h"
#include "exception.h"

#include <QtCore/QFileInfo>

namespace TextureTool {

/*!
    \class MainParser
    This is the parser for the arguments passed to main().
    Checks that input arguments has format ./texturetool [common options] \<tool\> [options] and
    that \<tool\> is valid tool.
*/

/*!
    Constructs MainParser object with the given DescriptionMap \a map.
*/
MainParser::MainParser(DescriptionMap map) :
    _map(std::move(map)),
    helpOption(parser.addHelpOption()),
    versionOption(parser.addVersionOption())
{
    parser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
}

/*!
    Processes options. If an error occured, or help, or version is required, prints help and
    throws ExitException, which leads to program exit.
*/
void MainParser::process(const QStringList &arguments)
{
    if (!parser.parse(arguments)) {
        showError(parser.errorText());
        showHelp(EXIT_FAILURE);
    }

    if (parser.isSet(versionOption))
        showVersion();

    const auto positional = parser.positionalArguments();
    if (!positional.isEmpty()) {
        _name = positional.first();
        _arguments = positional;
        if (parser.isSet(helpOption))
            _arguments.insert(1, "--help");
    } else {
        if (parser.isSet(helpOption))
            showHelp(EXIT_SUCCESS);
        else
            showHelp(EXIT_FAILURE);
    }
}

/*!
    Prints help and throws ExitException with the given \a code.
*/
void MainParser::showHelp(int code)
{
    auto text = parser.helpText();
    auto lines = text.split("\n");
    const auto filePath = qApp->applicationFilePath();
    lines[0] = QString("Usage: %1 [options] command [command options]")
            .arg(QFileInfo(filePath).fileName());
    lines.append("Commands:");
    for (const auto &tool: _map) {
        lines.append(QString("  %1 %2").
                     arg(QString::fromLatin1(tool.first), -10).
                     arg(tool.second));
    }
    lines.append(QString());
    lines.append("Use texturetool command --help to see command options");
    text = lines.join("\n");
    showMessage(text);
    exit(code);
}

/*!
    Prints program version and throws ExitException with zero code.
*/
void MainParser::showVersion()
{
    showMessage(QCoreApplication::applicationName() + QLatin1Char(' ')
                + QCoreApplication::applicationVersion());
    exit(EXIT_SUCCESS);
}

/*!
    Prints the given \a message to the sdtout stream.
*/
void MainParser::showMessage(const QString &message)
{
    ToolParser::showMessage(message);
}

/*!
    Prints the given \a message to the sdterr stream.
*/
void MainParser::showError(const QString &message)
{
    ToolParser::showError(message);
}

/*!
    \internal
    Throws ExitException with then given \a code.
*/
void MainParser::exit(int code)
{
    throw ExitException(code);
}

} // namespace TextureTool
