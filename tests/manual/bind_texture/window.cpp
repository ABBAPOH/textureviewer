#include "window.h"

#include <QDebug>
#include <QCoreApplication>
#include <QFile>
#include <QTime>
#include <TextureLib/Texture>
#include <TextureLib/TextureIO>
#include <TextureLib/Utils>

namespace {

QByteArray readAll(QStringView path)
{
    QFile file(path.toString());
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Can't open file:" << file.errorString() << file.error();
        return QByteArray();
    }
    return file.readAll();
}

} //  namespace

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
        throw std::runtime_error("Can't get OGL context");
    }
    m_funcs = context()->functions();
    if (!m_funcs) {
        qCritical() << "Can't get OGL functions";
        qApp->quit();
        return;
    }
    m_funcs->initializeOpenGLFunctions();
    qInfo() << "real OGL version" << reinterpret_cast<const char *>(m_funcs->glGetString(GL_VERSION));

    { // setup vertex data

        GLfloat vertices[] = {
            // Позиции           // Текстурные координаты
             0.5f,  0.5f, 0.0f,   1.0f, 1.0f,   // Верхний правый
             0.5f, -0.5f, 0.0f,   1.0f, 0.0f,   // Нижний правый
            -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,   // Нижний левый
            -0.5f,  0.5f, 0.0f,   0.0f, 1.0f    // Верхний левый
        };

        GLuint indices[] = {  // Помните, что мы начинаем с 0!
            0, 1, 3,   // Первый треугольник
            1, 2, 3    // Второй треугольник
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

        // Атрибут с координатами
        m_funcs->glEnableVertexAttribArray(0);
        m_funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);

        // Атрибут с текстурой
        m_funcs->glEnableVertexAttribArray(1);
        m_funcs->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)));
    }

    m_program = std::make_unique<QOpenGLShaderProgram>();
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, readAll(
                                           m_coreProfile
                                           ? u":/shaders/gl33/vertex.shader"
                                           : u":/shaders/gles/vertex.shader"));
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, readAll(
                                           m_coreProfile
                                           ? u":/shaders/gl33/fragment.shader"
                                           : u":/shaders/gles/fragment.shader"));

    m_program->bindAttributeLocation("position", 0);
    m_program->bindAttributeLocation("texCoord", 1);

    m_program->link();

    m_texture = Utils::makeOpenGLTexture(m_image);

    m_funcs->glEnable(GL_BLEND);
    m_funcs->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Window::resizeGL(int w, int h)
{
    if (!m_funcs)
        return;

    m_funcs->glViewport(0, 0, w, h);
}

void Window::paintGL()
{
    if (!m_funcs)
        return;

    m_funcs->glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    m_funcs->glClear(GL_COLOR_BUFFER_BIT);

    m_program->bind();

    m_funcs->glActiveTexture(GL_TEXTURE0);
    m_texture->bind();
    m_funcs->glUniform1i(m_program->uniformLocation("ourTexture1"), 0);

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_funcs->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    m_texture->release();
    m_program->release();
}
