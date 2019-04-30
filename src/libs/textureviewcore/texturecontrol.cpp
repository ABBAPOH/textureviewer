#include "texturecontrol.h"
#include "texturedocument.h"

#include <TextureLib/Utils>

#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QResizeEvent>

namespace TextureViewer {

class TextureControlPrivate
{
    Q_DECLARE_PUBLIC(TextureControl)

public:
    using size_type = TextureControl::size_type;
    using TextureDocumentPointer = TextureControl::TextureDocumentPointer;
    using ItemPointer = TextureDocument::ItemPointer;

    explicit TextureControlPrivate(ObserverPointer<TextureControl> qq) noexcept : q_ptr(qq) {}

    ItemPointer currentItem() const;
    void onTextureChanged(const Texture &texture);
    void updateModel(const QImage &image);

    struct OpenGLData
    {
        std::unique_ptr<QOpenGLFunctions> functions;
        QMatrix4x4 projection;
        QMatrix4x4 view;
        QMatrix4x4 model;

        QOpenGLBuffer vbo {QOpenGLBuffer::VertexBuffer};
        QOpenGLBuffer ibo {QOpenGLBuffer::IndexBuffer};
        QOpenGLVertexArrayObject vao;

        std::unique_ptr<QOpenGLShaderProgram> program;
        std::unique_ptr<QOpenGLTexture> texture;

        void initializeGeometry();
        bool initializeShaders();
    };

    TextureDocumentPointer document;
    QSize size;
    size_type face {0};
    size_type layer {0};
    size_type level {0};
    bool textureDirty {false};
    OpenGLData glData;

protected:
    ObserverPointer<TextureControl> q_ptr;
};

void TextureControlPrivate::OpenGLData::initializeGeometry()
{
    GLfloat vertices[] = {
         // Positions         // Texture coordinates
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f,   // Top right
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f,   // Bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,   // Bottom left
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f    // Top left
    };

    GLuint indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&vao);

    vbo.create();
    vbo.bind();
    vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo.allocate(vertices, sizeof(vertices));

    ibo.create();
    ibo.bind();
    ibo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    ibo.allocate(indices, sizeof(indices));

    // Coordinates attribute
    functions->glEnableVertexAttribArray(0);
    functions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);

    // Texture attribute
    functions->glEnableVertexAttribArray(1);
    functions->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)));
}

bool TextureControlPrivate::OpenGLData::initializeShaders()
{
    program = std::make_unique<QOpenGLShaderProgram>();
    if (!program->addShaderFromSourceFile(
            QOpenGLShader::Vertex,
            QStringLiteral(":/textureviewcore/shaders/gles/vertex.glsl"))) {
        return false;
    }

    if (!program->addShaderFromSourceFile(
            QOpenGLShader::Fragment,
            QStringLiteral(":/textureviewcore/shaders/gles/fragment.glsl"))) {
        return false;
    }

    program->bindAttributeLocation("position", 0);
    program->bindAttributeLocation("texCoord", 1);

    return program->link();
}

TextureControlPrivate::ItemPointer TextureControlPrivate::currentItem() const
{
    return document ? document->item(face, layer, level) : nullptr;
}

void TextureControlPrivate::onTextureChanged(const Texture &texture)
{
    Q_UNUSED(texture);
    Q_Q(TextureControl);

    layer = 0;
    level = 0;
    face = 0;
    textureDirty = true;
    q->update();
}

void TextureControlPrivate::updateModel(const QImage &image)
{
    const auto w = size.width();
    const auto h = size.height();

    glData.model = QMatrix4x4();
    auto imageMax = std::max(image.width(), image.height());
    auto winMin = std::min(w, h);
    if (winMin >= imageMax) {
        glData.model.scale({float(imageMax) / image.width(), float(imageMax) / image.height(), 1});
        glData.model.scale({image.width() / float(w), image.height() / float(h), 1});
    } else {
        glData.model.scale({float(imageMax) / image.width(), float(imageMax) / image.height(), 1});
        glData.model.scale({image.width() / float(w), image.height() / float(h), 1});
    }
}

/*!
  \class TextureControl
  \brief A controller for the TextureView
*/

/*!
  \brief Constructs a TextureControl object with the given \a parent.
*/

TextureControl::TextureControl(QObject* parent)
    : QObject(parent)
    , d_ptr(new TextureControlPrivate(ObserverPointer<TextureControl>(this)))
{
    setDocument(TextureDocumentPointer(new TextureDocument(this)));
}

/*!
  \brief Destroys the TextureControl object.
*/
TextureControl::~TextureControl() = default;

/*!
  \property TextureDocumentPointer TextureControl::document
  \brief This property holds an assigned TextureDocument instance.
*/

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

    if (!document)
        document.reset(new TextureDocument(this));

    d->document = document;
    d->onTextureChanged(d->document->texture());

    connect(d->document.get(), &TextureDocument::textureChanged,
            this, [d](const Texture &texture) { d->onTextureChanged(texture); } );

    emit documentChanged(document);
    update();
}

int TextureControl::width() const
{
    Q_D(const TextureControl);
    return d->size.width();
}

int TextureControl::height() const
{
    Q_D(const TextureControl);
    return d->size.height();
}

QSize TextureControl::size() const
{
    Q_D(const TextureControl);
    return d->size;
}

auto TextureControl::level() const -> size_type
{
    Q_D(const TextureControl);
    return d->level;
}

void TextureControl::setLevel(size_type level)
{
    Q_D(TextureControl);
    if (d->level == level)
        return;
    d->level = level;
    d->textureDirty = true;
    emit levelChanged(d->level);
    update();
}

auto TextureControl::layer() const -> size_type
{
    Q_D(const TextureControl);
    return d->layer;
}

void TextureControl::setLayer(size_type layer)
{
    Q_D(TextureControl);
    if (d->layer == layer)
        return;
    d->layer = layer;
    d->textureDirty = true;
    emit layerChanged(d->layer);
    update();
}

auto TextureControl::face() const -> size_type
{
    Q_D(const TextureControl);
    return d->face;
}

void TextureControl::setFace(size_type face)
{
    Q_D(TextureControl);
    if (d->face == face)
        return;
    d->face = face;
    d->textureDirty = true;
    emit faceChanged(d->face);
    update();
}

/*!
  \brief Handles a resize event.
*/
void TextureControl::resizeEvent(QResizeEvent* event)
{
    Q_D(TextureControl);

    d->size = event->size();
    emit widthChanged(d->size.width());
    emit heightChanged(d->size.height());
}

/*!
  \brief Handles a mouse press event.
*/
void TextureControl::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
}

/*!
  \brief Handles a mouse move event.
*/
void TextureControl::mouseMoveEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
}

/*!
  \brief Handles a mouse release event.
*/
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

    d->glData.initializeGeometry();

    if (!d->glData.initializeShaders()) {
        qWarning() << "Can't initialize shaders";
    }
}

void TextureControl::resizeGL(int w, int h)
{
    Q_D(TextureControl);
    if (!d->glData.functions)
        return;

    d->glData.functions->glViewport(0, 0, w, h);
    d->glData.projection = QMatrix4x4();

    float left = -0.5f;
    float right = 0.5f;
    float bottom = -0.5f;
    float top = 0.5f;
    d->glData.projection.ortho(left, right, bottom, top, 0.1, 100.0);

    d->glData.view = QMatrix4x4();
    d->glData.view.translate({0, 0, -3.0f});

    auto item = d->currentItem();
    const auto &image = item ? item->thumbnail : QImage();
    if (!image.isNull()) {
        d->updateModel(image);
    }
}

void TextureControl::paintGL()
{
    Q_D(TextureControl);
    if (!d->glData.functions)
        return;

    if (d->textureDirty) {
        d->glData.texture.reset(); // delete tex here as we have a context
        auto item = d->currentItem();
        const auto &image = item ? item->thumbnail : QImage();
        if (!image.isNull()) {
            d->glData.texture.reset(new QOpenGLTexture(image));
            d->updateModel(image);
        }

        d->textureDirty = false;
    }

    d->glData.functions->glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    d->glData.functions->glClear(GL_COLOR_BUFFER_BIT);

    if (!d->glData.texture)
        return; // nothing to paint

    d->glData.program->bind();
    d->glData.texture->bind();

    d->glData.program->setUniformValue("ourTexture1", 0);
    d->glData.program->setUniformValue("projection", d->glData.projection);
    d->glData.program->setUniformValue("view", d->glData.view);
    d->glData.program->setUniformValue("model", d->glData.model);

    QOpenGLVertexArrayObject::Binder vaoBinder(&d->glData.vao);
    d->glData.functions->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    d->glData.texture->release();
    d->glData.program->release();
}

void TextureControl::update()
{
    emit updateRequested();
}

void TextureControl::nextLevel()
{
    Q_D(TextureControl);
    if (!d->document->levels())
        return;
    setLevel(std::min<size_type>(d->level + 1, d->document->levels() - 1));
}

void TextureControl::prevLevel()
{
    Q_D(TextureControl);
    setLevel(std::max<size_type>(0, d->level - 1));
}

void TextureControl::nextLayer()
{
    Q_D(TextureControl);
    if (!d->document->layers())
        return;
    setLayer(std::min<size_type>(d->layer + 1, d->document->layers() - 1));
}

void TextureControl::prevLayer()
{
    Q_D(TextureControl);
    setLayer(std::max<size_type>(0, d->layer - 1));
}

void TextureControl::nextFace()
{
    Q_D(TextureControl);
    if (!d->document->faces())
        return;
    setFace(std::min<size_type>(d->face + 1, d->document->faces() - 1));
}

void TextureControl::prevFace()
{
    Q_D(TextureControl);
    setFace(std::max<size_type>(0, d->face - 1));
}

} // namespace TextureViewer
