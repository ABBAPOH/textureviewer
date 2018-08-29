#ifndef TEXTUREVIEW_H
#define TEXTUREVIEW_H

#include "textureviewwidgets_global.h"

#include <QtWidgets/QAbstractScrollArea>

#include <ObserverPointer>

class TextureControl;
class TextureDocument;

class TextureViewPrivate;
class TextureView : public QAbstractScrollArea
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(TextureView)
    Q_DISABLE_COPY(TextureView)

public:
    using TextureControlPointer = ObserverPointer<TextureControl>;
    using TextureDocumentPointer = ObserverPointer<TextureDocument>;

    explicit TextureView(QWidget *parent = nullptr);
    ~TextureView() override;

    TextureDocumentPointer document() const;
    void setDocument(TextureDocumentPointer document);

    TextureControlPointer control() const;

signals:
    void documentChanged(TextureDocumentPointer document);

protected:
    bool viewportEvent(QEvent *event) override;

protected:
    QScopedPointer<TextureViewPrivate> d_ptr;
};

#endif // TEXTUREVIEW_H
