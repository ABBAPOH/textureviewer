#pragma once

#include "utils_global.h"

#include <QtCore/QIODevice>
#include <QtCore/QMimeType>
#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QVector>
#include <QtCore/QUrl>

#include <QtWidgets/QUndoStack>

#include <qfuturewatcher.h>

class AbstractDocumentPrivate;
class UTILS_EXPORT AbstractDocument : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AbstractDocument)
    Q_DISABLE_COPY(AbstractDocument)

    Q_PROPERTY(QUrl url READ url NOTIFY urlChanged)

    Q_PROPERTY(bool modified READ isModified WRITE setModified NOTIFY modificationChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY canRedoChanged)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
    // TODO: add undoRedoEnabled property

public:
    explicit AbstractDocument(QObject *parent = nullptr);
    ~AbstractDocument() override;

    QUrl url() const;
    Q_SIGNAL void urlChanged(const QUrl &url);

    bool isModified() const;
    bool canRedo() const;
    bool canUndo() const;

    void clearUndoStack();

public slots:
    void open(const QUrl &url);
    void save(const QUrl &url = QUrl());
    void clear();

    void setModified(bool modified);

    void undo();
    void redo();

signals:
    void openStarted();
    void openFinished(bool ok);

    void modificationChanged(bool modified);
    void canRedoChanged(bool available);
    void canUndoChanged(bool available);

protected:
    explicit AbstractDocument(AbstractDocumentPrivate &dd, QObject *parent = Q_NULLPTR);

    QUndoStack *undoStack() const;

    virtual void doOpen(const QUrl &url) { Q_UNUSED(url); }
    virtual void doSave(const QUrl &url) { Q_UNUSED(url); }

protected:
    QScopedPointer<AbstractDocumentPrivate> d_ptr;
};
