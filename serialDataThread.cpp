#include "serialDataThread.h"

#include "asaEncoder.h"
#include "asadevice.h"
#include "loader.h"
void serialDataThread::dataHandling(const QByteArray raws) {
  static ASAEncoder::ASADecode decode;
  QString s;
  for (const char ch : raws) {
    if (!decode.put(ch)) {
      if (ch != '\r') s += ch;
      if (decode.isSync(ch)) {
        emit dataWrite("~ACK\n", strlen("~ACK\n"));
        s += u"~ACK\n"_qs;
      }
      putSync = ASAEncoder::ASAEncode::isSync(ch);
    }

    if (decode.isDone) {
      // ui->hmiText->moveCursor(QTextCursor::End);
      // ui->hmiText->insertPlainText(QString::fromStdString(decode.get()));
      emit hmiAppend(QString::fromStdString(decode.get()));
    }
  }
  emit portAppend(s);
}

void serialDataThread::dataSend(const std::string str) {
  ASAEncoder::ASAEncode encode;

  if (encode.put(str)) {
    auto datas = encode.get();
    if (putSync) {
      emit dataWrite("~ACK\n", strlen("~ACK\n"));
      emit portAppend(u"~ACK\n"_qs);
      putSync = false;
    }
    emit dataWrite(reinterpret_cast<char *>(datas.data()), datas.size());
  }
  
}

void serialDataThread::programming(QString portName, int deviceNum,
                                   QString hexFile) {
  using Loader::asa_dev_list;
  using Loader::Loader;
  typedef Loader::Loader::ProgMode ProgMode;

  // emit setEnable(false);
  auto serial = new QSerialPort(portName, this);
  serial->setBaudRate(QSerialPort::Baud38400);
  serial->setParity(QSerialPort::NoParity);
  serial->setDataBits(QSerialPort::Data8);
  serial->setStopBits(QSerialPort::OneStop);
  serial->setFlowControl(QSerialPort::NoFlowControl);
  serial->setReadBufferSize(500);
  auto is = serial->open(QIODevice::ReadWrite);

  if (is) {
    serial->setDataTerminalReady(true);
    serial->setRequestToSend(true);
    serial->clear();
    Loader l(*serial, deviceNum, ProgMode::FLASH, hexFile);
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
    emit setInfo(info);

    for (int i = 0; i < l.total_steps(); ++i) {
      l.do_step();
      emit progress(l.cur_step() * 100 / l.total_steps());
      // this->usleep(10000);
    }
  }
  serial->setDataTerminalReady(false);
  serial->setRequestToSend(false);

  serial->clear();

  serial->close();
  delete serial;
  emit setEnable(true);
}