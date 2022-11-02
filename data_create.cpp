#include "data_create.h"

#include "./ui_data_create.h"
#include "asaEncoder.h"

DataCreate::DataCreate(QWidget *parent)
    : QDialog(parent), ui(new Ui::DataCreate) {  //
  ui->setupUi(this);
  ui->structCombobox->addItems(QStringList{"array", "matrix", "struct"});
  ui->structCombobox->setCurrentIndex(typeId::array);
  on_structCombobox_currentIndexChanged(typeId::array);
  on_addButton_clicked();
}

DataCreate::~DataCreate() { delete ui; }

std::string DataCreate::getFormat() {
  using namespace ASAEncoder;
  string format;
  ASADecode decode;
  auto idx = ui->structCombobox->currentIndex();
  uint8_t type, num,num2;
  if (idx == typeId::array) {
    type=qobject_cast<QComboBox *>(ui->sizeTable->cellWidget(0, 0))->currentIndex();
    auto size_text=ui->sizeTable->item(0,1)->text().toStdString();
    num=std::stoi(size_text);
    decode.putArray(type,num);
  } else if (idx == typeId::matrix) {
    type=qobject_cast<QComboBox *>(ui->sizeTable->cellWidget(0, 0))->currentIndex();
    auto size_text=ui->sizeTable->item(0,1)->text().toStdString();
    num=std::stoi(size_text);
    size_text=ui->sizeTable->item(0,2)->text().toStdString();
    num2=std::stoi(size_text);
    decode.putMatrix(type,num,num2);
  } else if (idx == typeId::structure) {
    // decode.putStruct(ui->sizeTable->item(0,1)->text());
    string st_str;
    for(int i=0;i<ui->sizeTable->rowCount();i++)
    {
      st_str+=decode.getTypeStr(qobject_cast<QComboBox *>(ui->sizeTable->cellWidget(i, 0))->currentIndex());
      st_str+="_"+ui->sizeTable->item(i,1)->text().toStdString()+",";
    }
    decode.putStruct(st_str);
  } else
    return format;
  format = decode.get();
  return format;
}

void DataCreate::on_structCombobox_currentIndexChanged(int index) {
  QStringList hHeaderlabel;
  if (index == typeId::array) {
    hHeaderlabel << "index"
                 << "size";
    ui->sizeTable->setColumnHidden(2, true);
    ui->addButton->setVisible(false);
    ui->delButton->setVisible(false);
    ui->upButton->setVisible(false);
    ui->downButton->setVisible(false);
  } else if (index == typeId::matrix) {
    hHeaderlabel << "index"
                 << "dia1"
                 << "dia2";
    ui->sizeTable->setColumnHidden(2, false);
    ui->addButton->setVisible(false);
    ui->delButton->setVisible(false);
    ui->upButton->setVisible(false);
    ui->downButton->setVisible(false);
  } else if (index == typeId::structure) {
    hHeaderlabel << "index"
                 << "size";
    ui->sizeTable->setColumnHidden(2, true);
    ui->addButton->setVisible(true);
    ui->delButton->setVisible(true);
    ui->upButton->setVisible(true);
    ui->downButton->setVisible(true);
  }
  ui->sizeTable->setHorizontalHeaderLabels(hHeaderlabel);
}
inline void DataCreate::insertRow(int row) {
  ui->sizeTable->insertRow(row);
  QComboBox *combobox = new QComboBox();
  combobox->addItems(QStringList{"int8", "int16", "int32", "int64", "uint8",
                                 "uint16", "uint32", "uint64", "float32",
                                 "float64"});
  ui->sizeTable->setCellWidget(row, 0, combobox);
}
inline void DataCreate::insertRow(int row, QWidget *comboBox,
                                  QList<QTableWidgetItem *> &items) {
  if (items.isEmpty())
    insertRow(row);
  else {
    ui->sizeTable->insertRow(row);
    ui->sizeTable->setCellWidget(row, 0, comboBox);
    setRow(row, items);
  }
}
inline QList<QTableWidgetItem *> DataCreate::takeRow(int row) {
  QList<QTableWidgetItem *>
      items;  // =QList<QTableWidgetItem *>(ui->sizeTable->columnCount())
  for (int i = 1; i < ui->sizeTable->columnCount(); i++)
    items << ui->sizeTable->takeItem(row, i);
  return items;
}
inline void DataCreate::setRow(int row, QList<QTableWidgetItem *> &items) {
  for (int i = 1; i < ui->sizeTable->columnCount(); i++)
    ui->sizeTable->setItem(row, i, items[i - 1]);
}
void DataCreate::on_addButton_clicked() {
  insertRow(ui->sizeTable->rowCount());
}

void DataCreate::on_delButton_clicked() {
  ui->sizeTable->removeRow(ui->sizeTable->currentRow());
}

void DataCreate::on_upButton_clicked() {
  int currentRow = ui->sizeTable->currentRow();
  if (currentRow > 0) {
    QComboBox *beforeBox =
        qobject_cast<QComboBox *>(ui->sizeTable->cellWidget(currentRow, 0));
    auto beforeItmes = takeRow(currentRow);
    insertRow(currentRow - 1, beforeBox, beforeItmes);
    ui->sizeTable->removeRow(currentRow + 1);
    ui->sizeTable->setCurrentCell(currentRow - 1, 0);
  }
}

void DataCreate::on_downButton_clicked() {
  int currentRow = ui->sizeTable->currentRow();
  if (currentRow < ui->sizeTable->rowCount() - 1) {
    QComboBox *beforeBox =
        qobject_cast<QComboBox *>(ui->sizeTable->cellWidget(currentRow, 0));
    auto beforeItmes = takeRow(currentRow);
    insertRow(currentRow + 2, beforeBox, beforeItmes);
    ui->sizeTable->removeRow(currentRow);
    ui->sizeTable->setCurrentCell(currentRow + 1, 0);
  }
}

void DataCreate::on_DataCreate_accepted() {}
