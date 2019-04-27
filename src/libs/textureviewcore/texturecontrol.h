#ifndef TEXTURECONTROL_H
#define TEXTURECONTROL_H

#include "textureviewcore_global.h"

#include <TextureLib/Texture>

#include <QtCore/QObject>

#include <ObserverPointer>

class QResizeEvent;
class QMouseEvent;

namespace TextureViewer {

class TextureDocument;

class TextureControlPrivate;
class TEXTUREVIEWCORE_EXPORT TextureControl : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(TextureControl)
    Q_DECLARE_PRIVATE(TextureControl)

    Q_PROPERTY(TextureDocumentPointer document READ document WRITE setDocument NOTIFY documentChanged)

    Q_PROPERTY(int width READ width NOTIFY widthChanged)
    Q_PROPERTY(int height READ height NOTIFY heightChanged)

    Q_PROPERTY(size_type level READ level WRITE setLevel NOTIFY levelChanged)
    Q_PROPERTY(size_type layer READ layer WRITE setLayer NOTIFY layerChanged)
    Q_PROPERTY(size_type face READ face WRITE setFace NOTIFY faceChanged)

public:
    using size_type = Texture::size_type;
    using TextureDocumentPointer = ObserverPointer<TextureDocument>;

    explicit TextureControl(QObject *parent = nullptr);
    ~TextureControl() override;

    TextureDocumentPointer document() const;
    void setDocument(TextureDocumentPointer document);

    int width() const;
    int height() const;
    QSize size() const;

    size_type level() const;
    void setLevel(size_type level);

    size_type layer() const;
    void setLayer(size_type layer);

    size_type face() const;
    void setFace(size_type face);

    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    // OpenGL API. TODO (abbapoh): move to a separate class aka QuickItem?
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

public slots:
    void update();

    void nextLevel();
    void prevLevel();

    void nextLayer();
    void prevLayer();

    void nextFace();
    void prevFace();

signals:
    void updateRequested();

    void widthChanged(int width);
    void heightChanged(int height);

    void documentChanged(TextureDocumentPointer document);
    void levelChanged(size_type level);
    void layerChanged(size_type layer);
    void faceChanged(size_type face);

private:
    QScopedPointer<TextureControlPrivate> d_ptr;
};

} // namespace TextureViewer

#endif // TEXTURECONTROL_H
