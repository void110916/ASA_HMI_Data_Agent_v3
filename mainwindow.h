#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QtWidgets>

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

 private slots:
  void on_portButton_clicked();
  void on_portComboBox_popup_showing();

 private:
  Ui::MainWindow *ui;
  QSerialPort *serial;

  bool serialOn(QSerialPort *&serial);
  bool serialOff(QSerialPort *&serial);
  void serialRecv(void);
};
#endif  // MAINWINDOW_H
