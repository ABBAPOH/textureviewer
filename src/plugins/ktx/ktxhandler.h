#ifndef KTXHANDLER_H
#define KTXHANDLER_H

#include <TextureLib/TextureIOHandler>

#include <QtCore/QLoggingCategory>

class KtxHandler : public TextureIOHandler
{
public:
    KtxHandler() = default;

    bool read(Texture &texture) override;
};

Q_DECLARE_LOGGING_CATEGORY(ktxhandler)

#endif // KTXHANDLER_H
