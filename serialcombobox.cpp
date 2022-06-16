#include "serialcombobox.h"
#include "mainwindow.h"
#include "./ui_mainwindow.h"
SerialComboBox::SerialComboBox(QWidget *parent) : QComboBox(parent) {}

void SerialComboBox::showPopup() {
  // for (auto port : QSerialPortInfo::availablePorts()) Ui::MainWindow->portComboBox->addItem(port.portName(), 0);
  emit popupShowing();
  qDebug() << "ccc\n";
  QComboBox::showPopup();
}