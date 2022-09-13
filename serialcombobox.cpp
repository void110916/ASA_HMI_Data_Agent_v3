#include "serialcombobox.h"

#include "./ui_mainwindow.h"
#include "mainwindow.h"
SerialComboBox::SerialComboBox(QWidget *parent) : QComboBox(parent) {}

void SerialComboBox::showPopup() {
  emit popupShowing();
  QComboBox::showPopup();
}
