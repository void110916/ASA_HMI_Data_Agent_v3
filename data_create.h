#ifndef DATA_CREATE_H
#define DATA_CREATE_H
// #include "./ui_data_create.h"
#include <QDialog>
#include <QtWidgets>
#include <string>
QT_BEGIN_NAMESPACE
namespace Ui {
class DataCreate;
}
QT_END_NAMESPACE
class DataCreate : public QDialog {
  Q_OBJECT
  enum typeId : int { array, matrix, structure };

 public:
  explicit DataCreate(QWidget *parent = nullptr);
  ~DataCreate();
  std::string getFormat();

 private slots:
  void on_structCombobox_currentIndexChanged(int index);
  void on_addButton_clicked();
  void on_delButton_clicked();
  void on_upButton_clicked();
  void on_downButton_clicked();
  void on_DataCreate_accepted();
 private:
  Ui::DataCreate *ui;
  inline void insertRow(int row);
  inline void insertRow(int row, QWidget *comboBox, QList<QTableWidgetItem *> &items);
  inline QList<QTableWidgetItem *> takeRow(int row);
  inline void setRow(int row, QList<QTableWidgetItem *> &items);
};

#endif  // DATA_CREATE_H
