#include "textureview.h"
#include "textureview_p.h"

#include <TextureViewCoreLib/TextureControl>

#include <QtWidgets/QOpenGLWidget>

#include <QtGui/QResizeEvent>

#include <QtCore/QCoreApplication>

namespace TextureViewer {

class OpenGLWidget : public QOpenGLWidget
{
public:
    explicit OpenGLWidget(ObserverPointer<TextureControl> control, QWidget *parent = nullptr)
        : QOpenGLWidget(parent)
        , m_control(control)
    {
        setFocusPolicy(Qt::StrongFocus);
    }

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

    QObject::connect(control.get(), &TextureControl::updateRequested,
            q->viewport(), static_cast<void (QWidget::*)()>(&QWidget::update));
    QObject::connect(control.get(), &TextureControl::documentChanged,
            q, &TextureView::documentChanged);
    initActions();
}

void TextureViewPrivate::initActions()
{
    Q_Q(TextureView);

    std::unique_ptr<QAction> action;

    action = std::make_unique<QAction>();
    action->setShortcut(QKeySequence("Ctrl+Up"));
    action->setShortcutContext(Qt::WidgetShortcut);
    q->addAction(action.get());
    QObject::connect(action.get(), &QAction::triggered, q, &TextureView::nextLevel);
    actions[size_t(TextureView::Actions::NextLevel)] = std::move(action);

    action = std::make_unique<QAction>();
    action->setShortcut(QKeySequence("Ctrl+Down"));
    action->setShortcutContext(Qt::WidgetShortcut);
    q->addAction(action.get());
    QObject::connect(action.get(), &QAction::triggered, q, &TextureView::prevLevel);
    actions[size_t(TextureView::Actions::PrevLevel)] = std::move(action);
}

/*!
  \class TextureView
  \brief A view that displays a TextureDocument.
*/

/*!
  \brief Constructs a TextureView object with the given \a parent.
*/
TextureView::TextureView(QWidget *parent)
    : QAbstractScrollArea(parent)
    , d_ptr(new TextureViewPrivate(ObserverPointer<TextureView>(this)))
{
    Q_D(TextureView);
    d->init();
}

/*!
  \brief Destroys the TextureView object.
*/
TextureView::~TextureView() = default;

/*!
  \property TextureView::TextureDocumentPointer TextureView::document
  \brief This property holds an assigned TextureDocument instance.
*/

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

/*!
  \property TextureView::TextureControlPointer TextureView::control
  \brief This property holds an assigned TextureControl instance.
*/

TextureView::TextureControlPointer TextureView::control() const
{
    Q_D(const TextureView);
    return d->control;
}

bool TextureView::viewportEvent(QEvent* event)
{
    switch (event->type()) {
    case QEvent::Paint :
        return false;
    case QEvent::Resize : {
        Q_D(TextureView);
        d->control->resizeEvent(static_cast<QResizeEvent *>(event));
        return false;
    }
    default:
        break;
    }

    return QAbstractScrollArea::viewportEvent(event);
}

} // namespace TextureViewer
