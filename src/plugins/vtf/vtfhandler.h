#ifndef VTFHANDLER_H
#define VTFHANDLER_H

#include <QtCore/QLoggingCategory>
#include <TextureLib/TextureIOHandler>
#include "vtfheader.h"

class VTFHandler : public TextureIOHandler
{
public:
    VTFHandler() = default;

public: // ImageIOHandler interface
    bool read(Texture &texture) override;
};

Q_DECLARE_LOGGING_CATEGORY(vtfhandler)

#endif // VTFHANDLER_H
