#include "mainwindow.h"

#include "./ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  ui->statusbar->setVisible(false);
  // ui->serialTab.
  connect(ui->portComboBox, SIGNAL(popupShowing()), this,
          SLOT(on_portComboBoxPopupShowing()));
  for (auto port : QSerialPortInfo::availablePorts())
    ui->portComboBox->addItem(port.portName(), 0);
  ui->implicitText->addItems(QStringList{"CR", "LF", "CRLF", "None"});
  ui->implicitText->setCurrentIndex(1);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_portButton_clicked() {
  static bool isOn = false;
  if (!isOn)
    isOn = serialOn(this->serial);
  else
    isOn = serialOff(this->serial);
}

bool MainWindow::serialOn(QSerialPort *&serial) {
  serial = new QSerialPort(ui->portComboBox->currentText(), this);
  serial->setBaudRate(QSerialPort::Baud115200);
  serial->setParity(QSerialPort::NoParity);
  serial->setDataBits(QSerialPort::Data8);
  serial->setStopBits(QSerialPort::OneStop);
  serial->setReadBufferSize(500);
  auto is = serial->open(QIODevice::ReadWrite);

  if (serial->isOpen()) {
    ui->portButton->setStyleSheet("color: rgb(115, 210, 22);");
    ui->portButton->setText("On");
    QObject::connect(serial, SIGNAL(readyRead()), this, SLOT(serialRecv()));
    return true;
  } else {
    serialOff(serial);
    return false;
  }
}
void MainWindow::updateSize(int index) {}
void MainWindow::serialRecv(void) {
  auto text = serial->readAll();
  ui->portTextBrowser->moveCursor(QTextCursor::End);
  ui->portTextBrowser->insertPlainText(text);
}
bool MainWindow::serialOff(QSerialPort *&serial) {
  serial->disconnect(SIGNAL(readyRead()), this, SLOT(serialRecv()));
  // serial->clear();
  serial->close();
  delete serial;
  ui->portButton->setStyleSheet("color: rgb(239, 41, 41);");
  ui->portButton->setText("Off");
  return false;
}

void MainWindow::on_portComboBoxPopupShowing() {
  ui->portComboBox->clear();
  for (auto port : QSerialPortInfo::availablePorts())
    ui->portComboBox->addItem(port.portName(), 0);
}

void MainWindow::on_enterbuttom_clicked() {
  auto text =
      ui->enterLine->text() + implicitText[ui->implicitText->currentIndex()];
  serial->write(text.toStdString().c_str(), text.size());
  ui->enterLine->clear();
  if (ui->echoCheckBox->isChecked()) {
    ui->portTextBrowser->moveCursor(QTextCursor::End);
    ui->portTextBrowser->insertPlainText(text);
  }
}

void MainWindow::on_enterLine_returnPressed() { on_enterbuttom_clicked(); }

void MainWindow::on_tabWidget_currentChanged(int index) {
  for (int i = 0; i < ui->tabWidget->count(); i++)
    if (i != index)
      ui->tabWidget->widget(i)->setSizePolicy(QSizePolicy::Ignored,
                                              QSizePolicy::Ignored);

  ui->tabWidget->widget(index)->setSizePolicy(QSizePolicy::Preferred,
                                              QSizePolicy::Preferred);
  ui->tabWidget->widget(index)->resize(
      ui->tabWidget->widget(index)->minimumSizeHint());
  ui->tabWidget->widget(index)->adjustSize();
  resize(minimumSizeHint());
  adjustSize();
}

void MainWindow::on_clearButton_clicked() { ui->portTextBrowser->clear(); }
