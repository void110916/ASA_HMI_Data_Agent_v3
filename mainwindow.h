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
  void on_portComboBoxPopupShowing();
  void serialRecv(void);

  void on_enterbuttom_clicked();

  void on_enterLine_returnPressed();
  void updateSize(int index);

  void on_tabWidget_currentChanged(int index);

  void on_clearButton_clicked();

private:
  Ui::MainWindow *ui;
  QSerialPort *serial;

  bool serialOn(QSerialPort *&serial);
  bool serialOff(QSerialPort *&serial);

  const QStringList implicitText = {"\r", "\n", "\r\n", ""};
  // bool event(QEvent *ev);
};
#endif  // MAINWINDOW_H
