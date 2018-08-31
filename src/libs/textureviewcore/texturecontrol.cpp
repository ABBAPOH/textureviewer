#include "texturecontrol.h"
#include "texturedocument.h"

#include <TextureLib/Utils>

#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLVertexArrayObject>
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

        QOpenGLBuffer vbo {QOpenGLBuffer::VertexBuffer};
        QOpenGLBuffer ibo {QOpenGLBuffer::IndexBuffer};
        QOpenGLVertexArrayObject vao;

        std::unique_ptr<QOpenGLShaderProgram> program;
        std::unique_ptr<QOpenGLTexture> texture;

        void initializeGeometry();
        bool initializeShaders();
    };

    TextureDocumentPointer document;
    int face {0};
    int layer {0};
    int level {0};
    bool textureDirty {true};
    OpenGLData glData;
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
    layer = 0;
    level = 0;
    face = 0;
    textureDirty = true;
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

    if (!document)
        document.reset(new TextureDocument(this));

    d->document = document;
    d->onTextureChanged(d->document->texture());

    connect(d->document.get(), &TextureDocument::textureChanged,
            this, [d](const Texture &texture) { d->onTextureChanged(texture); } );

    emit documentChanged(document);
}

int TextureControl::level() const
{
    Q_D(const TextureControl);
    return d->level;
}

void TextureControl::setLevel(int level)
{
    Q_D(TextureControl);
    if (d->level == level)
        return;
    d->level = level;
    d->textureDirty = true;
    emit levelChanged(d->level);
}

int TextureControl::layer() const
{
    Q_D(const TextureControl);
    return d->layer;
}

void TextureControl::setLayer(int layer)
{
    Q_D(TextureControl);
    if (d->layer == layer)
        return;
    d->layer = layer;
    d->textureDirty = true;
    emit layerChanged(d->layer);
}

int TextureControl::face() const
{
    Q_D(const TextureControl);
    return d->face;
}

void TextureControl::setFace(int face)
{
    Q_D(TextureControl);
    if (d->face == face)
        return;
    d->face = face;
    d->textureDirty = true;
    emit faceChanged(d->face);
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
    const auto &image = item ? item->texture : Texture();

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

    if (d->textureDirty) {
        d->glData.texture.reset(); // delete tex here as we have a context
        auto item = d->currentItem();
        d->glData.texture = Utils::makeOpenGLTexture(item ? item->texture : Texture());
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

void TextureControl::nextLevel()
{
    Q_D(TextureControl);
    setLevel(std::min(d->level + 1, d->document->levels()));
}

void TextureControl::prevLevel()
{
    Q_D(TextureControl);
    setLevel(std::max(0, d->level - 1));
}

void TextureControl::nextLayer()
{
    Q_D(TextureControl);
    setLayer(std::min(d->layer + 1, d->document->layers()));
}

void TextureControl::prevLayer()
{
    Q_D(TextureControl);
    setLayer(std::max(0, d->layer - 1));
}

void TextureControl::nextFace()
{
    Q_D(TextureControl);
    setFace(std::min(d->face + 1, d->document->faces()));
}

void TextureControl::prevFace()
{
    Q_D(TextureControl);
    setFace(std::max(0, d->face - 1));
}
