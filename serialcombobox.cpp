#include "serialcombobox.h"
#include "mainwindow.h"
#include "./ui_mainwindow.h"
SerialComboBox::SerialComboBox(QWidget *parent) : QComboBox(parent) {}

void SerialComboBox::showPopup() {
  emit popupShowing();
  QComboBox::showPopup();
}