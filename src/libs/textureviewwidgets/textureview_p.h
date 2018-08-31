#ifndef TEXTUREVIEW_P_H
#define TEXTUREVIEW_P_H

#include "textureview.h"

#include <TextureViewCoreLib/TextureControl>

#include <QtWidgets/QAction>

class TextureViewPrivate
{
    Q_DISABLE_COPY(TextureViewPrivate)
    Q_DECLARE_PUBLIC(TextureView)
public:
    using TextureControlPointer = TextureView::TextureControlPointer;

    TextureViewPrivate(ObserverPointer<TextureView> qq) : q_ptr(qq) {}

    void init();
    void initActions();

    TextureControlPointer control;
    std::unique_ptr<QAction> actions[size_t(TextureView::Actions::Count)];

private:
    ObserverPointer<TextureView> q_ptr;
};

#endif // TEXTUREVIEW_P_H
