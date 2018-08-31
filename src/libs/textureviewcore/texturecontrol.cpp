#include "texturecontrol.h"
#include "texturedocument.h"

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QMatrix4x4>

class TextureControlPrivate
{
public:
    using TextureDocumentPointer = TextureControl::TextureDocumentPointer;
    using ItemPointer = TextureDocument::ItemPointer;

    ItemPointer currentItem() const;
    void onTextureChanged(const Texture &texture);

    struct OpenGLData
    {
        std::unique_ptr<QOpenGLFunctions> functions;
        QMatrix4x4 projection;
        QMatrix4x4 view;
        QMatrix4x4 model;
    };

    TextureDocumentPointer document;
    int face {0};
    int layer {0};
    int level {0};
    OpenGLData glData;
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
    d->glData.functions = std::make_unique<QOpenGLFunctions>();
    if (!d->glData.functions)
        return;

    d->glData.functions->initializeOpenGLFunctions();

    d->glData.functions->glEnable(GL_BLEND);
    d->glData.functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void TextureControl::resizeGL(int w, int h)
{
    Q_D(TextureControl);
    if (!d->glData.functions)
        return;

    d->glData.projection = QMatrix4x4();

    float left = -0.5f;
    float right = 0.5f;
    float bottom = -0.5f;
    float top = 0.5f;
    d->glData.projection.ortho(left, right, bottom, top, 0.1, 100.0);

    d->glData.view = QMatrix4x4();
    d->glData.view.translate({0, 0, -3.0f});

    auto item = d->currentItem();
    const auto &image = item->texture;

    d->glData.model = QMatrix4x4();
    auto max = std::max(image.width(), image.height());
    d->glData.model.scale({float(max) / image.width(), float(max) / image.height(), 1});
    d->glData.model.scale({image.width() / float(w), image.height() / float(h), 1});
}

void TextureControl::paintGL()
{
    Q_D(TextureControl);
    if (!d->glData.functions)
        return;

    d->glData.functions->glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    d->glData.functions->glClear(GL_COLOR_BUFFER_BIT);
}
