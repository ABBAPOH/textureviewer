#ifndef STARTUPDIALOG_H
#define STARTUPDIALOG_H

#include <QDialog>

namespace Ui {
class StartupDialog;
}

namespace TextureViewer {

class StartupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartupDialog(QWidget *parent = nullptr);
    ~StartupDialog() override;

private:
    Q_SLOT void openTexture();

private:
    std::unique_ptr<Ui::StartupDialog> ui;
};

} // namespace TextureViewer

#endif // STARTUPDIALOG_H
