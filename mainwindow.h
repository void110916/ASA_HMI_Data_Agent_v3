#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QtWidgets>

#include "serialcombobox.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
  QTranslator m_translator;    // contains the translations for this application
  QTranslator m_translatorQt;  // contains the translations for qt
 private slots:
  void on_portButton_clicked();
  void on_portComboBoxPopupShowing();
  // void on_portComboBox2PopupShowing();
  void serialRecv(void);

  void on_enterbuttom_clicked();

  void on_enterLine_returnPressed();

  void on_clearButton_clicked();

  void on_langSelect_currentTextChanged(const QString &arg1);
  void on_programButton_clicked();
  void on_fileBrowseButton_clicked();
  void on_newdataButton_clicked();
  void on_sendDataButton_clicked();

 private:
  Ui::MainWindow *ui;
  QSerialPort *serial;
  bool putSync=false;
  bool serialOn(const QString &name);
  bool serialOff();
  const QStringList implicitText = {"\r", "\n", "\r\n", ""};
  // bool event(QEvent *ev);
};
#endif  // MAINWINDOW_H
