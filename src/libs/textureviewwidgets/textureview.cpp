#include "textureview.h"
#include "textureview_p.h"

#include <TextureViewCoreLib/TextureControl>

#include <QtWidgets/QOpenGLWidget>

#include <QtGui/QResizeEvent>

class OpenGLWidget : public QOpenGLWidget
{
public:
    explicit OpenGLWidget(ObserverPointer<TextureControl> control, QWidget *parent = nullptr)
        : QOpenGLWidget(parent)
        , m_control(control)
    {}

    void initializeGL() override { m_control->initializeGL(); }
    void resizeGL(int w, int h) override { m_control->resizeGL(w, h); }
    void paintGL() override { m_control->paintGL(); }

private:
    ObserverPointer<TextureControl> m_control;
};

void TextureViewPrivate::init()
{
    Q_Q(TextureView);
    control.reset(new TextureControl);
    q->setViewport(new OpenGLWidget(control, q));

    q->connect(control.get(), &TextureControl::documentChanged, q, &TextureView::documentChanged);
}

TextureView::TextureView(QWidget *parent)
    : QAbstractScrollArea(parent)
    , d_ptr(new TextureViewPrivate(ObserverPointer<TextureView>(this)))
{
    Q_D(TextureView);
    d->init();
}

TextureView::~TextureView()
{
}

TextureView::TextureDocumentPointer TextureView::document() const
{
    Q_D(const TextureView);
    return d->control->document();
}

void TextureView::setDocument(TextureDocumentPointer document)
{
    Q_D(TextureView);
    d->control->setDocument(document);
}

TextureView::TextureControlPointer TextureView::control() const
{
    Q_D(const TextureView);
    return d->control;
}

bool TextureView::viewportEvent(QEvent* event)
{
    switch (event->type()) {
    case QEvent::Resize : {
        Q_D(TextureView);
        d->control->resizeEvent(static_cast<QResizeEvent *>(event));
    }
    default:
        break;
    }

    return QAbstractScrollArea::viewportEvent(event);
}

