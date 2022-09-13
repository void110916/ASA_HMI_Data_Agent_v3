#include "mainwindow.h"

#include <QApplication>
#include <QFileDialog>

#include "./ui_mainwindow.h"
#include "asaEncoder.h"
#include "asadevice.h"
#include "loader.h"
using namespace std::string_literals;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),serial(new QSerialPort(this)) {
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
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_portButton_clicked() {
  static bool isOn = false;
  if (!isOn) {
    isOn = serialOn(ui->portComboBox->currentText());
    if (serial->isOpen()) {
      ui->portButton->setStyleSheet("color: rgb(115, 210, 22);");
      ui->portButton->setText("On");
      QObject::connect(serial, SIGNAL(readyRead()), this, SLOT(serialRecv()));
    } else {
      serialOff();
    }
  } else {
    isOn = serialOff();
    serial->disconnect(SIGNAL(readyRead()), this, SLOT(serialRecv()));
    ui->portButton->setStyleSheet("color: rgb(239, 41, 41);");
    ui->portButton->setText("Off");
  }
}
bool MainWindow::serialOn(const QString &name) {
  serial = new QSerialPort(name, this);
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
  delete serial;

  return false;
}

void MainWindow::serialRecv(void) {
  static ASAEncoder::ASADecode decode;
  char ch;
  QString s;
  static QString sync;
  // serial->waitForReadyRead(3000);
  auto size = serial->bytesAvailable();
  while (serial->read(&ch, 1)) {
    if (!decode.put(ch)) {
      s += ch;
      if (decode.isSync(ch)) {
        auto l = serial->write("~ACK\n");
        s += u"~ACK\n"_qs;
      }
      putSync = ASAEncoder::ASAEncode::isSync(ch);
    }

    if (decode.isDone) {
      // ui->hmiText->moveCursor(QTextCursor::End);
      // ui->hmiText->insertPlainText(QString::fromStdString(decode.get()));
      auto ss=decode.get();
      ui->hmiText->appendPlainText(QString::fromStdString(ss));
    }
  }
  ui->portTextBrowser->moveCursor(QTextCursor::End);
  ui->portTextBrowser->insertPlainText(s);
}

void MainWindow::on_portComboBoxPopupShowing() {
  SerialComboBox *box = qobject_cast<SerialComboBox *>(sender());
  if (box != NULL) {
    box->clear();
    for (auto port : QSerialPortInfo::availablePorts())
      box->addItem(port.portName(), 0);
  }
}

void MainWindow::on_enterbuttom_clicked() {
  auto text =
     u">> "_qs+ ui->enterLine->text() + implicitText[ui->implicitText->currentIndex()];
  serial->write(text.toStdString().c_str(), text.size());
  ui->enterLine->clear();
  if (ui->echoCheckBox->isChecked()) {
    ui->portTextBrowser->moveCursor(QTextCursor::End);
    ui->portTextBrowser->insertPlainText(text);
  }
}

void MainWindow::on_enterLine_returnPressed() { on_enterbuttom_clicked(); }

void MainWindow::on_clearButton_clicked() { ui->portTextBrowser->clear(); }

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
  if (!ui->hexFile->text().isEmpty() &&
      !ui->portComboBox2->currentText().isEmpty()) {
    if (serialOn(ui->portComboBox2->currentText())) {
      Loader l(*serial, ui->deviceSelect->currentIndex(), ProgMode::FLASH,
               ui->hexFile->text());
      QString info;
      info += tr("Device is ") +
              QString("\"%1\"").arg(asa_dev_list[l.device_type()].name) + "\n";
      info += tr("Flash hex size is %1 KB (%2 bytes)")
                  .arg((float)l.flash_size() / 1024, 4)
                  .arg(l.flash_size()) +
              "\n";
      info += tr("Externel Flash hex size is %1 KB (%2 bytes)")
                  .arg((float)l.ext_flash_size() / 1024, 4)
                  .arg(l.ext_flash_size()) +
              "\n";
      info += tr("EEPROM hex size is %1 bytes.").arg(l.eep_size()) + "\n";
      info += tr("Estimated time is %1 s.").arg(l.prog_time()) + "\n";
      ui->loaderInfo->setText(info);

      for (int i = 0; i < l.total_steps(); ++i) {
        l.do_step();
        ui->progressBar->setValue(l.cur_step() * 100 / l.total_steps());
      }
    }
    serialOff();
  }
  ui->programButton->setEnabled(true);
}

void MainWindow::on_fileBrowseButton_clicked() {
  auto file = QFileDialog::getOpenFileName(this, tr("open file"), "",
                                           "Hex File (*hex)");
  ui->hexFile->setText(file);
}

void MainWindow::on_newdataButton_clicked() {}

void MainWindow::on_sendDataButton_clicked() {
  std::string text = ui->hmiText->toPlainText().toStdString();
  ASAEncoder::ASAEncode encode;
  auto matchs = encode.split(text);
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
  for (int i = 0; i < num2send; i++) {
    if (encode.put(matchs[i])) {
      auto datas = encode.get();
      if (serial->isOpen()) {
        if (putSync) {
          serial->write("~ACK\n");
          ui->portTextBrowser->moveCursor(QTextCursor::End);
          ui->portTextBrowser->insertPlainText(u"~ACK\n"_qs);
          putSync = false;
        }
        serial->write(reinterpret_cast<char *>(datas.data()), datas.size());
      }
    }
  }
  std::string s;
  for(int i=num2send;i<matchs.size();i++) s+=matchs[i];
  ui->hmiText->setPlainText(QString::fromStdString(s));
}
