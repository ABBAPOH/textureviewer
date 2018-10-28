#pragma once

#include "abstractdocument.h"

class AbstractDocumentPrivate
{
    Q_DECLARE_PUBLIC(AbstractDocument)
    Q_DISABLE_COPY(AbstractDocumentPrivate)

protected:
    AbstractDocument *q_ptr;

public:
    explicit AbstractDocumentPrivate(AbstractDocument *qq) : q_ptr(qq) {}
    virtual ~AbstractDocumentPrivate();

    void init();
    void setState(AbstractDocument::State state);

public:
    QUrl url;
    AbstractDocument::State state {AbstractDocument::State::Idle};
    bool modified {false};
    QScopedPointer<QUndoStack> undoStack;
};
