#include "add_kotoba.h"
#include "ui_add_kotoba.h"

add_kotoba::add_kotoba(bool* k, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::add_kotoba)
{
    ui->setupUi(this);
    setFixedSize(QSize(543, 363));
    this->k = k;
    bool koutyakugo_f = koutyakugo();
    if (!koutyakugo_f){
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
    if (gen_tts != nullptr)
        delete gen_tts;
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
    if (file(file_name)){
        f = add_kotoba_(add_dic);
        if (gen_tts == nullptr)
            gen_tts = new QProcess();
        #ifdef ubuntu
        QString bin = "./gen_tts";
        #endif
        #ifdef appimage
        QString bin = "gen_tts";
        #endif
        #ifdef windows
        QString bin = "gen_tts.exe";
        #endif
        QStringList arguments;
        arguments << QString::fromStdString(load_lang()) << QString::fromStdString(add_dic.kotoba);
        gen_tts->start(bin, arguments);
    } else {
        f = add_kotoba_(add_dic, mp3);
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
    file_name = "";
}

void add_kotoba::file_select(){
    QString s = QCoreApplication::applicationDirPath();
    file_name = QFileDialog::getOpenFileName(this, "Select MP3 File", s, "MP3 files (*.mp3);;All files (*.*)");
    ui->mp3_path->setText(file_name);
}

bool add_kotoba::file(QString file_name){
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

