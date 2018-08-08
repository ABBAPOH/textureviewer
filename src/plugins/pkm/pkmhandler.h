#ifndef PKMHANDLER_H
#define PKMHANDLER_H

#include <QtCore/QLoggingCategory>
#include <TextureLib/TextureIOHandler>

class PkmHandler : public TextureIOHandler
{
    Q_DISABLE_COPY(PkmHandler)
public:
    PkmHandler() = default;
    PkmHandler(PkmHandler &&) = default;
    ~PkmHandler() override = default;

    PkmHandler &operator=(PkmHandler &&) = default;

public: // ImageIOHandler interface
    bool read(Texture &texture) override;
};

Q_DECLARE_LOGGING_CATEGORY(pkmhandler)

#endif // PKMHANDLER_H
