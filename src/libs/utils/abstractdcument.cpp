#include "abstractdcument.h"
#include "abstractdcument_p.h"

#include <memory>

AbstractDocumentPrivate::~AbstractDocumentPrivate()
{
}

void AbstractDocumentPrivate::init()
{
    Q_Q(AbstractDocument);
    undoStack.reset(new QUndoStack);

    q->connect(undoStack.data(), &QUndoStack::canRedoChanged, q, &AbstractDocument::canRedoChanged);
    q->connect(undoStack.data(), &QUndoStack::canUndoChanged, q, &AbstractDocument::canUndoChanged);
}

/*!
    \class Document
    Base class for all documents.

    Provides base functionality like modified state or undo-redo stack.
*/

/*!
    Constructs an AbstractDocument with the given \a parent.
*/
AbstractDocument::AbstractDocument(QObject *parent) :
    QObject(parent),
    d_ptr(new AbstractDocumentPrivate(this))
{
    Q_D(AbstractDocument);
    d->init();
}

/*!
    Destroys the AbstractDocument object.
*/
AbstractDocument::~AbstractDocument()
{
}

/*!
    \property Document::isModified
    This property holds whether the document has been modified by the user.
    By default, this property is false.
    \sa modificationChanged()
*/

bool AbstractDocument::isModified() const
{
    Q_D(const AbstractDocument);
    return d->modified;
}

/*!
    \property Document::canRedo
    This property holds whether there are any commands to redo.
*/

bool AbstractDocument::canRedo() const
{
    Q_D(const AbstractDocument);
    return d->undoStack->canRedo();
}

/*!
    \property Document::canUndo
    This property holds whether there are any commands to redo.
*/

bool AbstractDocument::canUndo() const
{
    Q_D(const AbstractDocument);
    return d->undoStack->canUndo();
}

/*!
    Clears the undo redo stack.
*/
void AbstractDocument::clearUndoStack()
{
    Q_D(const AbstractDocument);
    return d->undoStack->clear();
}

void AbstractDocument::setModified(bool modified)
{
    Q_D(AbstractDocument);
    if (d->modified == modified)
        return;

    d->modified = modified;
    emit modificationChanged(modified);
}

/*!
    Undoes the last operation.
*/
void AbstractDocument::undo()
{
    Q_D(AbstractDocument);
    d->undoStack->undo();
}

/*!
    Redoes the current operation.
*/
void AbstractDocument::redo()
{
    Q_D(AbstractDocument);
    d->undoStack->redo();
}

/*!
    \fn Document::modificationChanged(bool changed)

    This signal is emitted whenever the content of the document
    changes in a way that affects the modification state. If \a
    changed is true, the document has been modified; otherwise it is
    false.

    For example, calling setModified(false) on a document and then
    changing a document causes the signal to get emitted. If you undo that
    operation, causing the document to return to its original
    unmodified state, the signal will get emitted again.
*/

/*!
    Constructs an AbstractDocument with the given \a parent and private class \a dd.
*/
AbstractDocument::AbstractDocument(AbstractDocumentPrivate &dd, QObject *parent) :
    QObject(parent),
    d_ptr(&dd)
{
}

/*!
    Returns pointer to the undo-redo stack.
    Can be used to insert undo-redo commands.
*/
QUndoStack *AbstractDocument::undoStack() const
{
    Q_D(const AbstractDocument);
    return d->undoStack.data();
}

