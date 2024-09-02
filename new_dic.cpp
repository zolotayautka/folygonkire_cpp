#include "new_dic.h"
#include "ui_new_dic.h"

new_dic::new_dic(bool* k, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::new_dic)
{
    ui->setupUi(this);
    setFixedSize(QSize(320, 60));
    this->k = k;
    connect(ui->btn, &QPushButton::clicked, this, &new_dic::tukuru);
}

new_dic::~new_dic()
{
    delete ui;
}

void new_dic::tukuru(){
    QString lang = ui->cb->currentText();
    create_dic();
    set_lang(lang.toStdString());
    *k = false;
    close();
}
