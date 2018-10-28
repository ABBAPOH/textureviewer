#pragma once

#include "utils_global.h"

#include <QtCore/QIODevice>
#include <QtCore/QMimeType>
#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QVariant>
#include <QtCore/QVector>

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
    // TODO (abbapoh): add undoRedoEnabled property

public:
    enum class State {
        Idle,
        Opening,
        Saving,
    };
    Q_ENUM(State)

    explicit AbstractDocument(QObject *parent = nullptr);
    AbstractDocument(AbstractDocument &&) = delete;
    ~AbstractDocument() override;
    AbstractDocument &operator=(AbstractDocument &&) = delete;

    QUrl url() const;
    Q_SIGNAL void urlChanged(const QUrl &url);

    State state() const;
    Q_SIGNAL void stateChanged(State state);

    bool isModified() const;
    bool canRedo() const;
    bool canUndo() const;

    void clearUndoStack();

    // NVI pattern
    bool waitForOpened() { return doWaitForOpened(); }
    bool waitForSaved() { return doWaitForSaved(); }

public slots:
    void open(const QUrl &url);
    void save(const QUrl &url = QUrl());
    void cancel();
    void clear();

    void setModified(bool modified);

    void undo();
    void redo();

signals:
    void openStarted();
    void openFinished(bool ok, const QString &error);
    void saveStarted();
    void saveFinished(bool ok, const QString &error);

    void modificationChanged(bool modified);
    void canRedoChanged(bool available);
    void canUndoChanged(bool available);

protected:
    explicit AbstractDocument(AbstractDocumentPrivate &dd, QObject *parent = Q_NULLPTR);

    QUndoStack *undoStack() const;

    virtual void doOpen(const QUrl &url) { Q_UNUSED(url); }
    void beginOpen();
    void endOpen(bool ok, const QString &error = QString());
    virtual void doSave(const QUrl &url) { Q_UNUSED(url); }
    void beginSave();
    void endSave(bool ok, const QString &error = QString());
    virtual void doClear() {}
    virtual void doCancel() {}

    virtual bool doWaitForOpened() { return false; }
    virtual bool doWaitForSaved() { return false; }

protected:
    QScopedPointer<AbstractDocumentPrivate> d_ptr;
};
