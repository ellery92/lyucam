#include <QApplication>
#include <QDir>
#include "mainwindow.h"

#if defined(WIN32)
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  MainWindow mainwnd;
  mainwnd.show();
  return app.exec();
}
