#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
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

  void on_comboBox_currentTextChanged(const QString &arg1);
  void on_programButton_clicked();
  void on_fileBrowseButton_clicked();

 private:
  Ui::MainWindow *ui;
  QSerialPort *serial;
  QTranslator m_translator;    // contains the translations for this application
  QTranslator m_translatorQt;  // contains the translations for qt

  bool serialOn(QSerialPort *&serial);
  bool serialOff(QSerialPort *&serial);
  void loadLanguage(const QString &rLanguage);
  const QStringList implicitText = {"\r", "\n", "\r\n", ""};
  // bool event(QEvent *ev);
};
#endif  // MAINWINDOW_H
