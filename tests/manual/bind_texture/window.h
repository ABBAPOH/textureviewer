#ifndef WINDOW_H
#define WINDOW_H

#include <TextureLib/Texture>

#include <QtGui/QOpenGLWindow>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLShaderProgram>

#include <QOpenGLFunctions_3_3_Core>

#include <memory>

class Window : public QOpenGLWindow
{
public:
    explicit Window(const Texture &texture, bool coreProfile = false);
    ~Window();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    void initializeGeometry();
    bool initializeShaders();

private:
    Texture m_image;
    bool m_coreProfile {false};
    QOpenGLFunctions *m_funcs {nullptr};
    QOpenGLBuffer m_vbo {QOpenGLBuffer::VertexBuffer};
    QOpenGLBuffer m_ibo {QOpenGLBuffer::IndexBuffer};
    QOpenGLVertexArrayObject m_vao;
    std::unique_ptr<QOpenGLShaderProgram> m_program;
    std::unique_ptr<QOpenGLTexture> m_texture;
};

#endif // WINDOW_H
