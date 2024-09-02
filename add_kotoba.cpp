#include "add_kotoba.h"
#include "ui_add_kotoba.h"

add_kotoba::add_kotoba(bool* k, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::add_kotoba)
{
    ui->setupUi(this);
    setFixedSize(QSize(543, 363));
    this->k = k;
    bool lang_ruikei = load_gengo_ruikei();
    if (!lang_ruikei){
        #ifdef ja
        ui->cb->setItemText(0, "前置詞");
        #endif
        #ifdef en
        ui->cb->setItemText(0, "Preposition");
        #endif
        #ifdef ko
        ui->cb->setItemText(0, "전치사");
        #endif
    }
    connect(ui->add_btn, &QPushButton::clicked, this, &add_kotoba::_add);
    connect(ui->file_btn, &QPushButton::clicked, this, &add_kotoba::file_select);
}

add_kotoba::~add_kotoba()
{
    delete ui;
}

void add_kotoba::_add(){
    tuple add_dic;
    QString kotoba = ui->kotoba_line->text();
    add_dic.kotoba = kotoba.toStdString();
    QString kanji = ui->kanji_line->text();
    if (!(kanji.compare(""))){
        add_dic.kanji = kotoba.toStdString();
    } else {
        add_dic.kanji = kanji.toStdString();
    }
    int hinsi = ui->cb->currentIndex();
    add_dic.hinsi = hinsi;
    QString imi = ui->imi_line->toPlainText();
    add_dic.imi = imi.toStdString();
    QString bikou = ui->bikou_line->toPlainText();
    add_dic.bikou = bikou.toStdString();
    bool f;
    dic = new dic_exec;
    if (file(file_name)){
        f = dic->add_kotoba(add_dic);
    } else {
        f = dic->add_kotoba(add_dic, mp3);
    }
    if (!f){
        #ifdef ja
        QMessageBox::warning(nullptr, "お知らせ", "すでに存在することばです。");
        #endif
        #ifdef en
        QMessageBox::warning(nullptr, "Notification.", "It's already an existing word.");
        #endif
        #ifdef ko
        QMessageBox::warning(nullptr, "알림", "이미 존재하는 단어입니다.");
        #endif
    } else {
        *k = true;
    }
    delete dic;
    file_name = "";
}

void add_kotoba::file_select(){
    QString s = QCoreApplication::applicationDirPath();
    file_name = QFileDialog::getOpenFileName(this, "Select MP3 File", s, "MP3 files (*.mp3);;All files (*.*)");
    ui->mp3_path->setText(file_name);
}

bool add_kotoba::file(QString file_name){
    std::string name = file_name.toStdString();
    std::ifstream mp3_file(name, std::ios::binary);
    if (!mp3_file) {
        return true;
    }
    mp3_file.seekg(0, std::ios::end);
    std::streamsize size = mp3_file.tellg();
    mp3_file.seekg(0, std::ios::beg);
    mp3.resize(size);
    mp3_file.read(reinterpret_cast<char*>(mp3.data()), size);
    mp3_file.close();
    return false;
}

