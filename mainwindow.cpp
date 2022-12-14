#include "mainwindow.h"

#include <QApplication>
#include <QFileDialog>
#include <iostream>

#include "./ui_mainwindow.h"
#include "asaEncoder.h"
#include "asadevice.h"
#include "data_create.h"
#include "loader.h"
using namespace std::string_literals;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      serial(new QSerialPort(this)) {
  ui->setupUi(this);
  ui->statusbar->setVisible(false);
  connect(ui->portComboBox, SIGNAL(popupShowing()), this,
          SLOT(on_portComboBoxPopupShowing()));
  connect(ui->portComboBox2, SIGNAL(popupShowing()), this,
          SLOT(on_portComboBoxPopupShowing()));
  emit ui->portComboBox->popupShowing();
  for (auto dev : Loader::asa_dev_list) ui->deviceSelect->addItem(dev.name, 0);
  ui->implicitText->addItems(QStringList{"CR", "LF", "CRLF", "None"});
  ui->implicitText->setCurrentIndex(1);
  ui->langSelect->addItems(QStringList{"zh_TW", "en_US"});
  ui->langSelect->setCurrentIndex(0);
  // connect(ui->, SIGNAL(hmiAppend(const QString &)), ui->hmiText,
  //         SLOT(appendPlainText(const QString &)));
  // connect(ui->, &portAppend, ui->portTextBrowser,
  //         [ui->portTextBrowser](const QString &s) {
  //           ui->portTextBrowser->moveCursor(QTextCursor::End);
  //           ui->portTextBrowser->insertPlainText(s);
  //         })
  connectProgrammer();
  connectTerminal();
  connect(this, SIGNAL(exited()), &thread, SLOT(quit()));
  thread.start();
}

MainWindow::~MainWindow() {
  delete ui;
  delete serial;
}

inline void MainWindow::connectProgrammer() {
  connect(&thread, SIGNAL(progress(int)), ui->progressBar, SLOT(setValue(int)));
  connect(&thread, SIGNAL(setInfo(const QString &)), ui->loaderInfo,
          SLOT(setText(const QString &)));
  connect(&thread, SIGNAL(setEnable(bool)), ui->programButton,
          SLOT(setEnabled(bool)));
  connect(&thread, SIGNAL(setEnable(bool)), ui->portComboBox2,
          SLOT(setEnabled(bool)));

  connect(&thread, &serialDataThread::dataWrite, serial,
          [port = serial](const char *s, qint64 len) { port->write(s, len); });
  connect(this, SIGNAL(programming(QString, int, QString)), &thread,
          SLOT(programming(QString, int, QString)));
}
inline void MainWindow::connectTerminal() {
  connect(&thread, SIGNAL(hmiAppend(const QString &)), ui->hmiText,
          SLOT(appendPlainText(const QString &)), Qt::QueuedConnection);  //
  connect(
      &thread, &serialDataThread::portAppend, ui->portText,
      [b = ui->portText](const QString &s) {
        b->moveCursor(QTextCursor::End);
        b->insertPlainText(s);
      },
      Qt::QueuedConnection);
  connect(this, SIGNAL(dataHandling(const QByteArray)), &thread,
          SLOT(dataHandling(const QByteArray)));
  connect(this, SIGNAL(dataSend(const std::string)), &thread,
          SLOT(dataSend(const std::string)));
}
inline void MainWindow::disconnectTerminal() {
  disconnect(&thread, SIGNAL(hmiAppend(const QString &)), ui->hmiText, nullptr);
  disconnect(&thread, &serialDataThread::portAppend, ui->portText, nullptr);
  disconnect(this, SIGNAL(dataHandling(const QByteArray)), &thread,
             SLOT(dataHandling(const QByteArray)));
  disconnect(this, SIGNAL(dataSend(const std::string)), &thread,
             SLOT(dataSend(const std::string)));
}
void MainWindow::closeEvent(QCloseEvent *event) {
  QMainWindow::closeEvent(event);
  emit exited();
}
void MainWindow::on_portButton_clicked() {
  static bool isOn = false;
  if (!isOn) {
    isOn = serialOn(ui->portComboBox->currentText());
    if (serial->isOpen()) {
      ui->portButton->setStyleSheet("color: rgb(115, 210, 22);");
      ui->portButton->setText("On");
      ui->portComboBox->setEnabled(false);
      QObject::connect(serial, SIGNAL(readyRead()), this, SLOT(serialRecv()),
                       Qt::QueuedConnection);
    } else {
      serialOff();
    }
  } else {
    QObject::disconnect(serial, SIGNAL(readyRead()), this, SLOT(serialRecv()));
    isOn = serialOff();
    // serial->disconnect(SIGNAL(readyRead()), this, SLOT(serialRecv()));
    ui->portButton->setStyleSheet("color: rgb(239, 41, 41);");
    ui->portButton->setText("Off");
    ui->portComboBox->setEnabled(true);
  }
}
bool MainWindow::serialOn(const QString &name) {
  // serial = new QSerialPort(name, this);
  serial->setPortName(name);
  serial->setBaudRate(QSerialPort::Baud38400);
  serial->setParity(QSerialPort::NoParity);
  serial->setDataBits(QSerialPort::Data8);
  serial->setStopBits(QSerialPort::OneStop);
  serial->setFlowControl(QSerialPort::NoFlowControl);
  serial->setReadBufferSize(500);
  auto is = serial->open(QIODevice::ReadWrite);
  serial->setDataTerminalReady(true);
  serial->setRequestToSend(true);
  serial->clear();
  return is;
}

bool MainWindow::serialOff() {
  serial->setDataTerminalReady(false);
  serial->setRequestToSend(false);
  serial->clear();

  serial->close();
  // delete serial;

  return false;
}

void MainWindow::serialRecv(void) { emit dataHandling(serial->readAll()); }

void MainWindow::on_portComboBoxPopupShowing() {
  SerialComboBox *box = qobject_cast<SerialComboBox *>(sender());
  if (box != NULL) {
    box->clear();
    for (auto port : QSerialPortInfo::availablePorts())
      box->addItem(port.portName(), 0);
  }
}
void MainWindow::portTextAppend(const QString &s) {
  ui->portText->moveCursor(QTextCursor::End);
  ui->portText->insertPlainText(s);
}

void MainWindow::on_enterbuttom_clicked() {
  auto text =
      ui->enterLine->text() + implicitText[ui->implicitText->currentIndex()];
  serial->write(text.toStdString().c_str(), text.size());
  ui->enterLine->clear();
  if (ui->echoCheckBox->isChecked()) {
    ui->portText->moveCursor(QTextCursor::End);
    ui->portText->insertPlainText(u"<< "_qs + text);
  }
}

void MainWindow::on_enterLine_returnPressed() { on_enterbuttom_clicked(); }

void MainWindow::on_clearButton_clicked() { ui->portText->clear(); }

void MainWindow::on_langSelect_currentTextChanged(const QString &arg1) {
  QApplication::instance()->removeTranslator(&m_translator);
  QString lang = ":/HMI_agent_" + QLocale(arg1).name();
  if (m_translator.load(lang)) {
    QApplication::instance()->installTranslator(&m_translator);
    ui->retranslateUi(this);
  }
}

void MainWindow::on_programButton_clicked() {
  using Loader::asa_dev_list;
  using Loader::Loader;
  typedef Loader::Loader::ProgMode ProgMode;

  ui->programButton->setEnabled(false);
  ui->portComboBox2->setEnabled(false);
  if (!ui->hexFile->text().isEmpty() &&
      !ui->portComboBox2->currentText().isEmpty()) {
    auto &&portName = ui->portComboBox2->currentText();
    int devNum = ui->deviceSelect->currentIndex();
    auto &&hexFile = ui->hexFile->text();

    emit programming(portName, devNum, hexFile);
  }
  // ui->programButton->setEnabled(true);
}

void MainWindow::on_fileBrowseButton_clicked() {
  auto file = QFileDialog::getOpenFileName(this, tr("open file"), "",
                                           "Hex File (*hex)");
  ui->hexFile->setText(file);
}

void MainWindow::on_newdataButton_clicked() {
  DataCreate data_create(this);
  if (data_create.exec() == QDialog::Accepted) {
    auto s = data_create.getFormat();
    ui->hmiText->appendPlainText(QString::fromStdString(s));
  }
}

void MainWindow::on_hmiSendButton_clicked() {
  auto cursorIdx = ui->hmiText->textCursor().position();
  std::string text = ui->hmiText->toPlainText().toStdString();
  auto matchs = ASAEncoder::ASAEncode::split(text);
  int num2send = 0;
  if (matchs.size() > 1) {
    int ret = QMessageBox::question(
        this, tr("multi format detect"),
        tr("There are multi format ready to send,\n"
           "Do you want to send all at ones?"),
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
        QMessageBox::No);
    if (ret == QMessageBox::Yes)
      num2send = matchs.size();
    else if (ret == QMessageBox::No)
      num2send = 1;
    else
      return;
  } else {
    num2send = 1;
  }
  if (serial->isOpen())
    for (int i = 0; i < num2send; i++) {
      emit dataSend(matchs[i].str);
    }
  // auto a=ui->hmiText->find(u"i16_5"_qs,QTextDocument::FindFlags::fromInt(0));
  // auto cur=ui->hmiText->textCursor();
  // cur.removeSelectedText();
  // ui->hmiText->setTextCursor(cur);
  // std::string s;
  // for (int i = num2send; i < matchs.size(); i++) s += matchs[i];
  auto cursor = ui->hmiText->textCursor();
  cursor.setPosition(0);
  // auto pos=matchs.position();
  cursor.setPosition(matchs[num2send].lastIndex, QTextCursor::KeepAnchor);
  // cursor.select(QTextCursor::LineUnderCursor);
  // auto cursor.selectedText();
  cursor.removeSelectedText();

  // ui->hmiText->insertPlainText(QString::fromStdString(s));
  ui->hmiText->setTextCursor(cursor);
}

void MainWindow::on_hmiClearButton_clicked() {
  // use clear() will clear both text and redo/undo history
  auto c = ui->hmiText->textCursor();
  c.select(QTextCursor::Document);
  c.removeSelectedText();
  ui->hmiText->setTextCursor(c);
}

void MainWindow::on_tabWidget_currentChanged(int index) {
  static bool portIsOn = false;
  if (index == 0) {
    if (portIsOn) {
      serialOn(ui->portComboBox->currentText());
      if (serial->isOpen()) {
        ui->portButton->setStyleSheet("color: rgb(115, 210, 22);");
        ui->portButton->setText("On");
        QObject::connect(serial, SIGNAL(readyRead()), this, SLOT(serialRecv()));
      } else {
        serialOff();
      }
    }
  } else {
    // std::cout<<serial<<std::endl;
    if (serial->isOpen()) {
      portIsOn = serial->isOpen();
      QObject::disconnect(serial, SIGNAL(readyRead()), this,
                          SLOT(serialRecv()));
      serialOff();
    } else
      portIsOn = false;
    ui->portButton->setStyleSheet("color: rgb(239, 41, 41);");
    ui->portButton->setText("Off");
  }
}
