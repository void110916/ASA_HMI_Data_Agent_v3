#include "mainwindow.h"

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  auto portlist = QSerialPortInfo::availablePorts();

  for (auto port : portlist) ui->portComboBox->addItem(port.portName(), 0);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_pushButton_clicked() {}
