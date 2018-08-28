#include "texturedocument.h"

class TextureDocumentPrivate
{
public:
    Texture texture;
};

TextureDocument::TextureDocument(QObject *parent)
    : AbstractDocument(parent)
    , d_ptr(new TextureDocumentPrivate())
{
}

TextureDocument::~TextureDocument()
{
}

Texture TextureDocument::texture() const
{
    Q_D(const TextureDocument);
    return d->texture;
}

void TextureDocument::setTexture(const Texture &texture)
{
    Q_D(TextureDocument);
    if (d->texture == texture)
        return;
    d->texture = texture;
    emit textureChanged(texture);
}
