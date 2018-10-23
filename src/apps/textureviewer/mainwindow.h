#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

#include <QtCore/QUrl>

#include <ObserverPointer>

#include <memory>

namespace Ui {
class MainWindow;
} // namespace Ui

namespace TextureViewer {
class TextureView;
class ThumbnailsModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    ObserverPointer<TextureView> view() const;

public slots:
    void convert();
    void showTextureFormatsDialog();

private:
    void initConnections();

private:
    std::unique_ptr<Ui::MainWindow> ui;

    TextureView *m_view {nullptr};
    std::unique_ptr<ThumbnailsModel> m_thumbModel;
};

} // namespace TextureViewer

#endif // MAINWINDOW_H
