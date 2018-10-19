#include "showtool.h"
#include "exception.h"
#include "toolparser.h"

#include <TextureLib/TextureIO>
#include <TextureLib/TextureModel>

#include <QtCore/QDebug>

namespace TextureTool {

namespace {

const char toolId[] = "show";

struct Options
{
    bool showInfo {false};
    QString fileName;
};

Options parseOptions(const QStringList &arguments)
{
    ToolParser parser(toolId);
    parser.addPositionalArgument(QStringLiteral("file"),
                                 ShowTool::tr("Input filename"),
                                 QStringLiteral("[file]"));

    parser.process(arguments);

    Options result;

    const auto positional = parser.positionalArguments();
    if (positional.size() > 1) {
        ToolParser::showError(ShowTool::tr("Too many file arguments"));
        parser.showHelp(EXIT_FAILURE);
    } else if (positional.isEmpty()) {
        ToolParser::showError(ShowTool::tr("File argument missing"));
        parser.showHelp(EXIT_FAILURE);
    } else {
        result.showInfo = true;
        result.fileName = positional.at(0);
    }

    return result;
}

QString modelToText(QAbstractTableModel *model)
{
    QStringList result;
    for (int row = 0, rowCount = model->rowCount(); row < rowCount; ++row) {
        const auto keyIndex = model->index(row, 0);
        const auto valueIndex = model->index(row, 1);
        result.append(ShowTool::tr("%1 %2").
                      arg(model->data(keyIndex).toString(), -15).
                      arg(model->data(valueIndex).toString()));
    }
    return result.join("\n");
}

void showImageInfo(const QString &filePath)
{
    TextureIO io(filePath);
    const auto result = io.read();
    if (!result) {
        throw RuntimeError(ShowTool::tr("Can't read image %1: %2").
                           arg(filePath, toUserString(result.error())));
    }

    TextureModel model;
    model.setTexture(*result);
    ToolParser::showMessage(modelToText(&model));
}

} // namespace

/*!
    \class ShowTool
    This is class implements image showing tool.
*/

/*!
    Constructs a ShowTool instance.
*/
ShowTool::ShowTool() = default;

/*!
    \overload
*/
QByteArray ShowTool::id() const
{
    return toolId;
}

/*!
    \overload
*/
QString ShowTool::decription() const
{
    return ShowTool::tr("Shows information about image file", "TextureTool");
}

/*!
    \overload
*/
int ShowTool::run(const QStringList &arguments)
{
    const auto options = parseOptions(arguments);

    if (options.showInfo)
        showImageInfo(options.fileName);

    return 0;
}

} // namespace TextureTool
