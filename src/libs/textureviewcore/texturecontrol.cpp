#include "texturecontrol.h"

class TextureControlPrivate
{
public:
    using TextureDocumentPointer = TextureControl::TextureDocumentPointer;

    TextureDocumentPointer document;
};

TextureControl::TextureControl(QObject* parent)
    : QObject(parent)
    , d_ptr(new TextureControlPrivate)
{
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
