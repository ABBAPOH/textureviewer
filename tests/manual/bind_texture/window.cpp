#include "window.h"

#include <QDebug>
#include <QCoreApplication>
#include <QTime>
#include <TextureLib/Texture>
#include <TextureLib/TextureIO>
#include <TextureLib/Utils>

const char* const vertexShaderSource = "\n"
"#version 330 core \n"

"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"layout (location = 2) in vec2 texCoord;\n"

"out vec3 ourColor;\n"
"out vec2 TexCoord;\n"

"void main()\n"
"{\n"
"    gl_Position = vec4(position, 1.0);\n"
"    ourColor = color;\n"
"    TexCoord = vec2(texCoord.x, 1.0f - texCoord.y);\n"
//"    TexCoord = texCoord;\n"
"}";

const char *const fragmentShaderSource =
"#version 330 core\n"

"in vec3 ourColor;\n"
"in vec2 TexCoord;\n"

"out vec4 color;\n"

"uniform sampler2D ourTexture1;\n"

"void main()\n"
"{\n"
"    color = texture(ourTexture1, TexCoord);\n"
"}"
;

Window::Window()
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
    m_funcs = context()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    if (!m_funcs) {
        qCritical() << "Can't get OGL 3.2";
        qApp->quit();
        return;
    }
    m_funcs->initializeOpenGLFunctions();
    qInfo() << "real OGL version" << (char *)m_funcs->glGetString(GL_VERSION);

    {
        GLint nrAttributes;
        m_funcs->glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
        qInfo() << "Maximum nr of vertex attributes supported: " << nrAttributes;
    }

    { // setup vertex data

        GLfloat vertices[] = {
            // Позиции          // Цвета             // Текстурные координаты
             0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // Верхний правый
             0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // Нижний правый
            -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // Нижний левый
            -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // Верхний левый
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
        m_funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), nullptr);

        // Атрибут с цветом
        m_funcs->glEnableVertexAttribArray(1);
        m_funcs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3* sizeof(GLfloat)));

        // Атрибут с текстурой
        m_funcs->glEnableVertexAttribArray(2);
        m_funcs->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    }

    m_program = std::make_unique<QOpenGLShaderProgram>();
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();

    {
        Texture image(u":/dds/RGBA_10101002.dds");
//        Texture image(u":/dds/RGBX_8888.dds");
//        Texture image(u":/vtf/BGR_888.vtf");

        if (image.isNull()) {
            qCritical() << "Can't load image";
            qApp->quit();
            return;
        }

        m_texture = Utils::makeOpenGLTexture(image);
    }

    m_funcs->glEnable(GL_BLEND);
    m_funcs->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//    {
//        QImage image("/Users/arch/Programming/qt5/opengl/textures/container.jpg");
//        if (image.isNull()) {
//            qCritical() << "Can't load image";
//            qApp->quit();
//            return;
//        }
//        image = image.convertToFormat(QImage::Format_RGB888);

//        GLuint tex;
//        _funcs->glGenTextures(1, &tex);
//        _funcs->glBindTexture(GL_TEXTURE_2D, tex);
//        _funcs->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
//        _funcs->glGenerateMipmap(GL_TEXTURE_2D);
//        _funcs->glBindTexture(GL_TEXTURE_2D, 0);
//        _texture1 = {GL_TEXTURE_2D, tex};
//    }
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
    m_funcs->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    m_texture->release();
    m_program->release();
}
