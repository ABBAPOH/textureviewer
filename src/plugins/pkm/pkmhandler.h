#ifndef PKMHANDLER_H
#define PKMHANDLER_H

#include <QtCore/QLoggingCategory>
#include <TextureLib/TextureIOHandler>

class PkmHandler : public TextureIOHandler
{
    Q_DISABLE_COPY(PkmHandler)
public:
    PkmHandler() noexcept = default;
    PkmHandler(PkmHandler &&) noexcept = default;
    ~PkmHandler() noexcept override = default;
    PkmHandler &operator=(PkmHandler &&) noexcept = default;

public: // ImageIOHandler interface
    bool read(Texture &texture) override;
};

Q_DECLARE_LOGGING_CATEGORY(pkmhandler)

#endif // PKMHANDLER_H
