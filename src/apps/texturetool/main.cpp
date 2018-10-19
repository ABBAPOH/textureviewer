#include "abstracttool.h"
#include "converttool.h"
#include "exception.h"
#include "mainparser.h"
#include "showtool.h"
#include "toolparser.h"

#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>

#include <TextureLib/TextureIO>

#include <map>
#include <memory>

using ExitException = TextureTool::ExitException;
using RuntimeError = TextureTool::RuntimeError;
using AbstractTool = TextureTool::AbstractTool;
using ConvertTool = TextureTool::ConvertTool;
using ShowTool = TextureTool::ShowTool;
using MainParser = TextureTool::MainParser;

using ToolsMap = std::map<QByteArray, std::unique_ptr<AbstractTool>>;
using DescriptionMap = MainParser::DescriptionMap;

static ToolsMap createTools()
{
    auto convertTool = std::make_unique<ConvertTool>();
    auto showTool = std::make_unique<ShowTool>();
    ToolsMap result;
    result[convertTool->id()] = std::move(convertTool);
    result[showTool->id()] = std::move(showTool);
    return result;
}

static MainParser::DescriptionMap getDescriptions(const ToolsMap &map)
{
    MainParser::DescriptionMap result;
    for (const auto &item: map)
        result[item.first] = item.second->decription();
    return result;
}

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(extramimetypes);
    try {
        QCoreApplication app(argc, argv);
        QCoreApplication::setApplicationName("texturetool");
        QCoreApplication::setApplicationVersion("1.0");
        QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath() + TextureIO::pluginsDirPath());

        const auto tools = createTools();

        MainParser parser(getDescriptions(tools));
        parser.process(QCoreApplication::arguments());

        const auto toolName = parser.toolName();
        const auto it = tools.find(toolName.toLatin1());
        if (it == tools.end()) {
            MainParser::showError(QCoreApplication::translate("TextureTool", "Unknown command %1").arg(toolName));
            parser.showHelp(EXIT_FAILURE);
        }

        return it->second->run(parser.arguments());
    } catch (const ExitException &ex) {
        return ex.code();
    } catch (const RuntimeError &ex) {
        MainParser::showError(ex.message());
        return 1;
    } catch (const std::exception &ex) {
        MainParser::showError(QString::fromLatin1(ex.what()));
        return 1;
    }
}

