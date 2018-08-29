#ifndef TEXTURECONTROL_H
#define TEXTURECONTROL_H

#include <QtCore/QObject>

#include <ObserverPointer>

class QResizeEvent;
class QMouseEvent;
class TextureDocument;

class TextureControlPrivate;
class TextureControl : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(TextureControl)
    Q_DECLARE_PRIVATE(TextureControl)

public:
    using TextureDocumentPointer = ObserverPointer<TextureDocument>;

    explicit TextureControl(QObject *parent = nullptr);
    ~TextureControl() override;

    TextureDocumentPointer document() const;
    void setDocument(TextureDocumentPointer document);

    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void documentChanged(TextureDocumentPointer document);

private:
    QScopedPointer<TextureControlPrivate> d_ptr;
};

#endif // TEXTURECONTROL_H
