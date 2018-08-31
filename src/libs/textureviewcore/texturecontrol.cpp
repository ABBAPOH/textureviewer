#include "texturecontrol.h"
#include "texturedocument.h"

class TextureControlPrivate
{
public:
    using TextureDocumentPointer = TextureControl::TextureDocumentPointer;

    void onTextureChanged(const Texture &texture);

    TextureDocumentPointer document;
};

void TextureControlPrivate::onTextureChanged(const Texture &texture)
{

}

TextureControl::TextureControl(QObject* parent)
    : QObject(parent)
    , d_ptr(new TextureControlPrivate)
{
    Q_D(TextureControl);
    d->document.reset(new TextureDocument(this));
}

TextureControl::~TextureControl()
{
}

auto TextureControl::document() const -> TextureDocumentPointer
{
    Q_D(const TextureControl);

    return d->document;
}

void TextureControl::setDocument(TextureControl::TextureDocumentPointer document)
{
    Q_D(TextureControl);

    if (d->document == document)
        return;

    d->document = document;
    d->onTextureChanged(d->document->texture());

    connect(d->document.get(), &TextureDocument::textureChanged,
            this, [d](const Texture &texture) { d->onTextureChanged(texture); } );

    emit documentChanged(document);
}

void TextureControl::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
}

void TextureControl::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
}

void TextureControl::mouseMoveEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
}

void TextureControl::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
}
