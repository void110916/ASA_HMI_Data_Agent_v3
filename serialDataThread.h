#ifndef SERIALDATATHREAD_H
#define SERIALDATATHREAD_H

#include <QSerialPort>
#include <QThread>
QT_BEGIN_NAMESPACE
class serialDataThread : public QThread {
  Q_OBJECT

 public:
  bool isDone = false;
  bool putSync = false;
  QString sBuffer;
  QString hmiBuffer;
  serialDataThread(QObject* parent = nullptr) : QThread(parent) {}
  //   void connectSignal(QWidget *parent = nullptr);
 signals:
  void progress(int value);
  void dataWrite(const char* data, qint64 maxSize);
  void hmiAppend(const QString& s);
  void portAppend(const QString s);
  void setInfo(const QString& text);
  void setEnable(bool enable);
 public slots:
  void dataHandling(const QByteArray raws);
  void programming(QString portName, int deviceNum, QString hexFile);
};
QT_END_NAMESPACE
#endif  // SERIALDATATHREAD_H