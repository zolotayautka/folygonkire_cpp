#include "modify_kotoba.h"
#include "ui_modify_kotoba.h"

modify_kotoba::modify_kotoba(tuple* t, bool* k, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::modify_kotoba)
{
    ui->setupUi(this);
    setFixedSize(QSize(543, 373));
    this->t = t;
    ui->kotoba_line->setText(QString::fromStdString(t->kotoba));
    QString hinsi;
    kanji_re();
    bool koutyakugo_f = koutyakugo();
    switch(t->hinsi){
    case 0:
        #ifdef ja
        if(koutyakugo_f)
            hinsi = "[助詞]";
        else
            hinsi = "[前置詞]";
        #endif
        #ifdef en
        if(koutyakugo_f)
            hinsi = "[Postposition]";
        else
            hinsi = "[Preposition]";
        #endif
        #ifdef ko
        if(koutyakugo_f)
            hinsi = "[조사]";
        else
            hinsi = "[전치사]";
        #endif
        break;
    case 1:
        #ifdef ja
        hinsi = "[名詞]";
        #endif
        #ifdef en
        hinsi = "[Noun]";
        #endif
        #ifdef ko
        hinsi = "[명사]";
        #endif
        break;
    case 2:
        #ifdef ja
        hinsi = "[動詞]";
        #endif
        #ifdef en
        hinsi = "[Verb]";
        #endif
        #ifdef ko
        hinsi = "[동사]";
        #endif
        break;
    case 3:
        #ifdef ja
        hinsi = "[形容詞]";
        #endif
        #ifdef en
        hinsi = "[Adjective]";
        #endif
        #ifdef ko
        hinsi = "[형용사]";
        #endif
        break;
    case 4:
        #ifdef ja
        hinsi = "[副詞]";
        #endif
        #ifdef en
        hinsi = "[Adverb]";
        #endif
        #ifdef ko
        hinsi = "[부사]";
        #endif
        break;
    case 5:
        #ifdef ja
        hinsi = "[その外]";
        #endif
        #ifdef en
        hinsi = "[Others]";
        #endif
        #ifdef ko
        hinsi = "[그 외]";
        #endif
        break;
    }
    ui->cb->setText(hinsi);
    ui->imi_line->setText(QString::fromStdString(t->imi));
    ui->bikou_line->setText(QString::fromStdString(t->bikou));
    ui->kanji_line->setText(QString::fromStdString(t->kanji));
    if(mp3_load(t->kotoba).size() < 5){
        ui->del_file->setEnabled(false);
    }
    this->k = k;
    connect(ui->modify_btn, &QPushButton::clicked, this, &modify_kotoba::_modify);
    connect(ui->file_btn, &QPushButton::clicked, this, &modify_kotoba::file_select);
    connect(ui->del_file, &QCheckBox::stateChanged, this, &modify_kotoba::del_f_edit);
}

modify_kotoba::~modify_kotoba()
{
    delete ui;
}

void modify_kotoba::_modify(){
    t->imi = ui->imi_line->toPlainText().toStdString();
    t->bikou = ui->bikou_line->toPlainText().toStdString();
    t->kanji = ui->kanji_line->text().toStdString();
    if (file(file_name)){
        modify_kotoba_(*t, del_f);
    } else {
        modify_kotoba_(*t, mp3);
    }
    *k = true;
}

void modify_kotoba::file_select(){
    QString s = QCoreApplication::applicationDirPath();
    file_name = QFileDialog::getOpenFileName(this, "Select MP3 File", s, "MP3 files (*.mp3);;All files (*.*)");
    ui->mp3_path->setText(file_name);
}

bool modify_kotoba::file(QString file_name){
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly)) {
        return true;
    }
    QByteArray mp3_file = file.readAll();
    mp3.resize(file.size());
    std::copy(mp3_file.begin(), mp3_file.end(), mp3.begin());
    file.close();
    return false;
}

void modify_kotoba::del_f_edit(){
    if (ui->del_file->isChecked()){
        del_f = true;
        ui->mp3_path->setText("");
        ui->mp3_path->setEnabled(false);
        ui->file_btn->setEnabled(false);
    } else {
        del_f = false;
        ui->mp3_path->setEnabled(true);
        ui->file_btn->setEnabled(true);
    }
}

inline void modify_kotoba::kanji_re(){
    if(!(!load_lang().compare("ja") || !load_lang().compare("ko") || !load_lang().compare("zh-cn") || !load_lang().compare("zh-tw"))){
        ui->kanji_line->hide();
        ui->label_2->hide();
        ui->imi_line->resize(QSize(471, 121));
        ui->bikou_line->resize(QSize(471, 111));
    }
}
