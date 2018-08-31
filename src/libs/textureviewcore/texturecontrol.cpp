#include "texturecontrol.h"
#include "texturedocument.h"

#include <QtGui/QOpenGLFunctions>

class TextureControlPrivate
{
public:
    using TextureDocumentPointer = TextureControl::TextureDocumentPointer;
    using ItemPointer = TextureDocument::ItemPointer;

    ItemPointer currentItem() const;
    void onTextureChanged(const Texture &texture);

    TextureDocumentPointer document;
    int face {0};
    int layer {0};
    int level {0};

    std::unique_ptr<QOpenGLFunctions> glFunctions;
};

TextureControlPrivate::ItemPointer TextureControlPrivate::currentItem() const
{
    return document ? document->item(face, layer, level) : nullptr;
}

void TextureControlPrivate::onTextureChanged(const Texture &texture)
{
    layer = 0;
    level = 0;
    face = 0;
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

void TextureControl::initializeGL()
{
    Q_D(TextureControl);
    d->glFunctions = std::make_unique<QOpenGLFunctions>();
    if (!d->glFunctions)
        return;
}

void TextureControl::resizeGL(int w, int h)
{
    Q_D(TextureControl);
    if (!d->glFunctions)
        return;
}

void TextureControl::paintGL()
{
    Q_D(TextureControl);
    if (!d->glFunctions)
        return;

    d->glFunctions->glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    d->glFunctions->glClear(GL_COLOR_BUFFER_BIT);
}
