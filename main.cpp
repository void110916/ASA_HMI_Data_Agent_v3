#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "mainwindow.h"
// #include "serialDataThread.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  QTranslator translator;
  const QStringList uiLanguages = QLocale::system().uiLanguages();
  for (const QString &locale : uiLanguages) {
    const QString baseName = ":/HMI_agent_" + QLocale(locale).name();
    if (translator.load(baseName)) {
      a.installTranslator(&translator);
      break;
    }
  }
  MainWindow w;
  w.setWindowIcon(QIcon(":/icons/HMI_Agent_icon.png"));
  w.show();
  return a.exec();
}
