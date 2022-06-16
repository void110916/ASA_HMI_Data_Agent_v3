#ifndef SERIALCOMBOBOX_H
#define SERIALCOMBOBOX_H

#include <QComboBox>
#include <QMainWindow>
#include <QWidget>

class SerialComboBox : public QComboBox {
  Q_OBJECT
 public:
  explicit SerialComboBox(QWidget *parent = nullptr);
  virtual void showPopup() override;
signals:
   void popupShowing();
//  private:
//   MainWindow *ui;
};

#endif  // SERIALCOMBOBOX_H
