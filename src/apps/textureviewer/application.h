#ifndef APPLICATION_H
#define APPLICATION_H

#include <QtWidgets/QApplication>

namespace TextureViewer {

class Application: public QApplication
{
public:
    Application(int &argc, char **argv);

    static QString showOpenDialog();
    static bool open();
    static bool openPath(const QString &path);

    static QStringList recentFiles();
};

} // namespace TextureViewer

#endif // APPLICATION_H
