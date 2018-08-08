#include "converttool.h"
#include "exception.h"
#include "toolparser.h"

#include <TextureLib/TextureIO>

#include <QtGui/QImageReader>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

#include <OptionalType>

namespace TextureTool {

namespace {

const char toolId[] = "convert";

struct Options
{
    QString inputFile;
    QString inputMimeType;
    QString outputFile;
    QString outputMimeType;
};

Options parseOptions(const QStringList &arguments)
{
    ToolParser parser(toolId);
    QCommandLineOption inputTypeOption(QStringLiteral("input-type"),
                                       ConvertTool::tr("Input mime type (i.e. image/png)"),
                                       QStringLiteral("mime type"));
    QCommandLineOption outputTypeOption(QLatin1String("output-type"),
                                        ConvertTool::tr("Output mime type (i.e. image/png)"),
                                        QStringLiteral("mime type"));
    parser.addOption(inputTypeOption);
    parser.addOption(outputTypeOption);
    parser.addPositionalArgument(QStringLiteral("input"),
                                 ConvertTool::tr("Input filename"),
                                 QStringLiteral("input"));
    parser.addPositionalArgument(QStringLiteral("output"),
                                 ConvertTool::tr("Output filename"),
                                 QStringLiteral("output"));

    parser.process(arguments);

    const auto positional = parser.positionalArguments();
    if (positional.size() != 2) {
        ToolParser::showError(ConvertTool::tr("Incorrect input/output arguments"));
        parser.showHelp(EXIT_FAILURE);
    }

    Options options;
    options.inputFile = positional.at(0);
    options.outputFile = positional.at(1);
    options.inputMimeType = parser.value(inputTypeOption);
    options.outputMimeType = parser.value(outputTypeOption);
    return options;
}

QLatin1String mimeTypeToFormat(QStringView mimeType)
{
    if (mimeType == u"image/png")
        return QLatin1String("png");

    return {};
}

void convert(const Options &options)
{
    TextureIO io(options.inputFile);
    if (!options.inputMimeType.isEmpty())
        io.setMimeType(options.inputMimeType);
    const auto result = io.read();

    Optional<Texture> texture;
    if (result) {
        texture = *result;
    } else {
        if (result.error() == TextureIOError::UnsupportedMimeType) {
            QImageReader reader(options.inputFile);
            if (options.inputMimeType.isEmpty())
                reader.setFormat(mimeTypeToFormat(options.inputMimeType).data());
            QImage image;
            if (reader.read(&image)) {
                texture = Texture(image);
            } else if (reader.error() != QImageReader::UnsupportedFormatError) {
                throw RuntimeError(ConvertTool::tr("Can't read \"%1\": %2").
                                   arg(options.inputFile).
                                   arg(reader.errorString()));
            }
        }
    }

    if (!texture) {
        throw RuntimeError(ConvertTool::tr("Can't read \"%1\": %2").
                           arg(options.inputFile).
                           arg(toUserString(result.error())));
    }

    if (!options.outputMimeType.isEmpty())
        io.setMimeType(options.outputMimeType);
    io.setFileName(options.outputFile);
    const auto ok = io.write(*texture);
    if (!ok) {
        throw RuntimeError(ConvertTool::tr("Can't write texture %1: %2").
                           arg(options.outputFile, toUserString(ok.error())));
    }
}

} // namespace

/*!
    \class ConvertTool
    This is class implements image converting tool.
*/

/*!
    Constructs a ConvertTool instance.
*/
ConvertTool::ConvertTool() = default;

/*!
    \overload
*/
QByteArray ConvertTool::id() const
{
    return toolId;
}

/*!
    \overload
*/
QString ConvertTool::decription() const
{
    return ConvertTool::tr("Converts image files");
}

/*!
    \overload
*/
int ConvertTool::run(const QStringList &arguments)
{
    const auto options = parseOptions(arguments);
    convert(options);
    return 0;
}

} // namespace TextureTool
