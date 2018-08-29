#pragma once

#include "utils_global.h"

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QUrl>
#include <QtCore/QIODevice>
#include <QtCore/QMimeType>
#include <QtCore/QVector>
#include <QtWidgets/QUndoStack>

class AbstractDocumentPrivate;
class UTILS_EXPORT AbstractDocument : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AbstractDocument)
    Q_DISABLE_COPY(AbstractDocument)

    Q_PROPERTY(bool modified READ isModified WRITE setModified NOTIFY modificationChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY canRedoChanged)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
    // TODO: add undoRedoEnabled property

public:
    explicit AbstractDocument(QObject *parent = nullptr);
    ~AbstractDocument();

    bool isModified() const;
    bool canRedo() const;
    bool canUndo() const;

    void clearUndoStack();

public slots:
    void setModified(bool modified);

    void undo();
    void redo();

signals:
    void modificationChanged(bool modified);
    void canRedoChanged(bool available);
    void canUndoChanged(bool available);

protected:
    explicit AbstractDocument(AbstractDocumentPrivate &dd, QObject *parent = Q_NULLPTR);

    QUndoStack *undoStack() const;

protected:
    QScopedPointer<AbstractDocumentPrivate> d_ptr;
};
