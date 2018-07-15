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
    _texture.reset();
    doneCurrent();
}

void Window::initializeGL()
{
    if (!context()) {
        throw std::runtime_error("Can't get OGL context");
    }
    _funcs = context()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    if (!_funcs) {
        qCritical() << "Can't get OGL 3.2";
        qApp->quit();
        return;
    }
    _funcs->initializeOpenGLFunctions();
    qInfo() << "real OGL version" << (char *)_funcs->glGetString(GL_VERSION);

    {
        GLint nrAttributes;
        _funcs->glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
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

        _vao.create();
        QOpenGLVertexArrayObject::Binder vaoBinder(&_vao);

        _vbo.create();
        _vbo.bind();
        _vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
        _vbo.allocate(vertices, sizeof(vertices));

        _ibo.create();
        _ibo.bind();
        _ibo.setUsagePattern(QOpenGLBuffer::StaticDraw);
        _ibo.allocate(indices, sizeof(indices));

        // Атрибут с координатами
        _funcs->glEnableVertexAttribArray(0);
        _funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), nullptr);

        // Атрибут с цветом
        _funcs->glEnableVertexAttribArray(1);
        _funcs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3* sizeof(GLfloat)));

        // Атрибут с текстурой
        _funcs->glEnableVertexAttribArray(2);
        _funcs->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    }

    _program = std::make_unique<QOpenGLShaderProgram>();
    _program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    _program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    _program->link();

    {
//        Texture image(u":/dds/BGRA_8888.dds");
//        Texture image(u":/dds/RGBX_8888.dds");
        Texture image(u":/vtf/BGRX8888.vtf");
//        Texture image(u":/vtf/RGB_888.vtf");
//        Texture image(u":/dds/DXT5.dds");

        if (image.isNull()) {
            qCritical() << "Can't load image";
            qApp->quit();
            return;
        }

        _texture = Utils::makeOpenGLTexture(image);
    }

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
    if (!_funcs)
        return;

    _funcs->glViewport(0, 0, w, h);
}

void Window::paintGL()
{
    if (!_funcs)
        return;

    _funcs->glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    _funcs->glClear(GL_COLOR_BUFFER_BIT);

    _program->bind();

    _funcs->glActiveTexture(GL_TEXTURE0);
    _texture->bind();
    _funcs->glUniform1i(_program->uniformLocation("ourTexture1"), 0);

    QOpenGLVertexArrayObject::Binder vaoBinder(&_vao);
    _funcs->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    _texture->release();
    _program->release();
}
