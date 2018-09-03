#ifndef TEXTUREVIEW_H
#define TEXTUREVIEW_H

#include "textureviewwidgets_global.h"

#include <TextureViewCoreLib/TextureControl>

#include <QtWidgets/QAbstractScrollArea>

#include <ObserverPointer>

namespace TextureViewer {

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

    enum class Actions {
        NextLevel,
        PrevLevel,
        Count,
    };

    explicit TextureView(QWidget *parent = nullptr);
    ~TextureView() override;

    TextureDocumentPointer document() const;
    void setDocument(TextureDocumentPointer document);

    TextureControlPointer control() const;

public slots:
    void nextLevel() { control()->nextLevel(); }
    void prevLevel() { control()->prevLevel(); }

    void nextLayer() { control()->nextLayer(); }
    void prevLayer() { control()->prevLayer(); }

    void nextFace() { control()->nextFace(); }
    void prevFace() { control()->prevFace(); }


signals:
    void documentChanged(TextureDocumentPointer document);

protected:
    bool viewportEvent(QEvent *event) override;

protected:
    QScopedPointer<TextureViewPrivate> d_ptr;
};

} // namespace TextureViewer

#endif // TEXTUREVIEW_H
