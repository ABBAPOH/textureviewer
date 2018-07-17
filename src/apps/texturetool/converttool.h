#pragma once

#include "abstracttool.h"
#include <QtCore/QCoreApplication>

namespace TextureTool {

class ConvertTool : public AbstractTool
{
    Q_DECLARE_TR_FUNCTIONS(ImageTool)
public:
    ConvertTool();

public: // AbstractTool interface
    QByteArray id() const override;
    QString decription() const override;
    int run(const QStringList &arguments) override;
};

} // namespace TextureTool
