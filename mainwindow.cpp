#include "mainwindow.h"

#include <QFileDialog>

#include "./ui_mainwindow.h"
#include "asadevice.h"
#include "loader.h"
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
  serial->setBaudRate(QSerialPort::Baud38400);
  serial->setParity(QSerialPort::NoParity);
  serial->setDataBits(QSerialPort::Data8);
  serial->setStopBits(QSerialPort::OneStop);
  serial->setFlowControl(QSerialPort::NoFlowControl);
  serial->setReadBufferSize(500);
  auto is = serial->open(QIODevice::ReadWrite);

  if (serial->isOpen()) {
    serial->setDataTerminalReady(true);
    serial->setRequestToSend(true);
    serial->clear();
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

void MainWindow::on_comboBox_currentTextChanged(const QString &arg1) {}

void switchTranslator(QTranslator &translator, const QString &filename) {
  // remove the old translator
  qApp->removeTranslator(&translator);

  // load the new translator
  QString path = QApplication::applicationDirPath();
  path.append("/languages/");
  if (translator.load(
          path + filename))  // Here Path and Filename has to be entered because
                             // the system didn't find the QM Files else
    qApp->installTranslator(&translator);
}

void MainWindow::loadLanguage(const QString &rLanguage) {
  QLocale locale = QLocale(rLanguage);
  QLocale::setDefault(locale);
  QString languageName = QLocale::languageToString(locale.language());
  switchTranslator(m_translator, QString("HMI_agent_%1.qm").arg(rLanguage));
  switchTranslator(m_translatorQt, QString("qt_%1.qm").arg(rLanguage));
}

void MainWindow::on_programButton_clicked() {
  if (serialOn(serial)) {
    Loader::Loader l(*serial, 0, true, ui->hexFile->text(), false, 0);
    for (int i = 0; i < l.total_steps(); ++i) {
      l.do_step();
      ui->progressBar->setValue((float)(i + 1) / l.total_steps());
    }
  }
}

void MainWindow::on_fileBrowseButton_clicked() {
  using Loader::Loader;
  typedef Loader::Loader::ProgMode ProgMode;

  auto hexFile = QFileDialog::getOpenFileName(this, tr("open file"), "",
                                              "Hex File (*hex)");
  if (!hexFile.isNull()) {
    auto l = Loader(*serial, ui->deviceComboBox_2->currentIndex(),
                    ProgMode::FLASH, hexFile);
    for (int i = 0; i < l.total_steps(); ++i) {
      l.do_step();
      ui->progressBar->setValue((i + 1) / l.total_steps());
    }
  }
}
