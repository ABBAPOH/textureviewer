#include "texturecontrol.h"
#include "texturedocument.h"

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

        void initializeGeometry();
        bool initializeShaders();
    };

    TextureDocumentPointer document;
    int face {0};
    int layer {0};
    int level {0};
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

    d->glData.functions->glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    d->glData.functions->glClear(GL_COLOR_BUFFER_BIT);
}
