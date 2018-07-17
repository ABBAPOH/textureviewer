#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QString>

namespace TextureTool {

class AbstractTool
{
    Q_DISABLE_COPY(AbstractTool)
public:
    AbstractTool() = default;
    virtual ~AbstractTool();

    virtual QByteArray id() const = 0;
    virtual QString decription() const = 0;
    virtual int run(const QStringList &arguments) = 0;
};

} // namespace TextureTool
