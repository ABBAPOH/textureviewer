#include "window.h"

#include <QDebug>
#include <QCoreApplication>
#include <QFile>
#include <QTime>
#include <TextureLib/Texture>
#include <TextureLib/TextureIO>
#include <TextureLib/Utils>

Window::Window(const Texture& texture, bool coreProfile)
    : m_image(texture)
    , m_coreProfile(coreProfile)
{
    resize(640, 480);
}

Window::~Window()
{
    makeCurrent();
    m_texture.reset();
    doneCurrent();
}

void Window::initializeGL()
{
    if (!context()) {
        qCritical() << "Can't get OGL context";
        close();
        return;
    }

    m_funcs = context()->functions();
    if (!m_funcs) {
        qCritical() << "Can't get OGL functions";
        close();
        return;
    }

    m_funcs->initializeOpenGLFunctions();
    m_funcs->glEnable(GL_BLEND);
    m_funcs->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    qInfo() << "real OGL version" << reinterpret_cast<const char *>(m_funcs->glGetString(GL_VERSION));

    initializeGeometry();
    if (!initializeShaders()) {
        qCritical() << "Can't initialize shaders";
        close();
        return;
    }

    m_texture = Utils::makeOpenGLTexture(m_image);
}

void Window::resizeGL(int w, int h)
{
    if (!m_funcs)
        return;

    m_funcs->glViewport(0, 0, w, h);
    m_projection = QMatrix4x4();

    float left = -0.5f;
    float right = 0.5f;
    float bottom = -0.5f;
    float top = 0.5f;
    m_projection.ortho(left, right, bottom, top, 0.1, 100.0);

    m_view = QMatrix4x4();
    m_view.translate({0, 0, -3.0f});

    m_model = QMatrix4x4();
    auto max = std::max(m_image.width(), m_image.height());
    m_model.scale({float(max) / m_image.width(), float(max) / m_image.height(), 1});
    m_model.scale({m_image.width() / float(w), m_image.height() / float(h), 1});
}

void Window::paintGL()
{
    if (!m_funcs)
        return;

    m_funcs->glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    m_funcs->glClear(GL_COLOR_BUFFER_BIT);

    m_program->bind();

    m_texture->bind();
    m_program->setUniformValue("ourTexture1", 0);
    m_program->setUniformValue("projection", m_projection);
    m_program->setUniformValue("view", m_view);
    m_program->setUniformValue("model", m_model);

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_funcs->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    m_texture->release();
    m_program->release();
}

void Window::initializeGeometry()
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

    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    m_vbo.create();
    m_vbo.bind();
    m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vbo.allocate(vertices, sizeof(vertices));

    m_ibo.create();
    m_ibo.bind();
    m_ibo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_ibo.allocate(indices, sizeof(indices));

    // Coordinates attribute
    m_funcs->glEnableVertexAttribArray(0);
    m_funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);

    // Texture attribute
    m_funcs->glEnableVertexAttribArray(1);
    m_funcs->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)));
}

bool Window::initializeShaders()
{
    m_program = std::make_unique<QOpenGLShaderProgram>();
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                            m_coreProfile
                                            ? QStringLiteral(":/shaders/gl33/vertex.shader")
                                            : QStringLiteral(":/shaders/gles/vertex.shader"))) {
        return false;
    }

    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                            m_coreProfile
                                            ? QStringLiteral(":/shaders/gl33/fragment.shader")
                                            : QStringLiteral(":/shaders/gles/fragment.shader"))) {
        return false;
    }

    m_program->bindAttributeLocation("position", 0);
    m_program->bindAttributeLocation("texCoord", 1);

    return m_program->link();
}
