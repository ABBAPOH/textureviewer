#ifndef TEXTURECONTROL_H
#define TEXTURECONTROL_H

#include "textureviewcore_global.h"

#include <QtCore/QObject>

#include <ObserverPointer>

class QResizeEvent;
class QMouseEvent;
class TextureDocument;

class TextureControlPrivate;
class TEXTUREVIEWCORE_EXPORT TextureControl : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(TextureControl)
    Q_DECLARE_PRIVATE(TextureControl)

    Q_PROPERTY(TextureDocumentPointer document READ document WRITE setDocument NOTIFY documentChanged)

    Q_PROPERTY(int level READ level WRITE setLevel NOTIFY levelChanged)
    Q_PROPERTY(int layer READ layer WRITE setLayer NOTIFY layerChanged)
    Q_PROPERTY(int face READ face WRITE setFace NOTIFY faceChanged)

public:
    using TextureDocumentPointer = ObserverPointer<TextureDocument>;

    explicit TextureControl(QObject *parent = nullptr);
    ~TextureControl() override;

    TextureDocumentPointer document() const;
    void setDocument(TextureDocumentPointer document);

    int level() const;
    void setLevel(int level);

    int layer() const;
    void setLayer(int layer);

    int face() const;
    void setFace(int face);

    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    // OpenGL API. TODO (abbapoh): move to a separate class aka QuickItem?
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

signals:
    void documentChanged(TextureDocumentPointer document);
    void levelChanged(int level);
    void layerChanged(int layer);
    void faceChanged(int face);

private:
    QScopedPointer<TextureControlPrivate> d_ptr;
};

#endif // TEXTURECONTROL_H
