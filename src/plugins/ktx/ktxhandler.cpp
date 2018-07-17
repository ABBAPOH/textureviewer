#include "ktxhandler.h"
#include "ktxheader.h"

bool KtxHandler::read(Texture& texture)
{
    KtxHeader header;

    {
        QDataStream s(device().get());
        s >> header;

        qCDebug(ktxhandler) << "header:" << header;

        if (s.status() != QDataStream::Ok) {
            qCWarning(ktxhandler) << "Can't read header: data stream status =" << s.status();
            return false;
        }
    }

    return false;
}

Q_LOGGING_CATEGORY(ktxhandler, "pluigns.textureformats.ktxhandler")
