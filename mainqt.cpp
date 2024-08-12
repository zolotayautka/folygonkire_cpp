#include "mainqt.h"
#include "ui_mainqt.h"

bool fileExists(const std::string& filePath) {
    struct stat buffer;
    return (stat(filePath.c_str(), &buffer) == 0);
}

mainQT::mainQT(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::mainQT)
{
    ui->setupUi(this);
    setFixedSize(QSize(821, 561));
    if (!(fileExists("dic.db"))){
        create_dic();
    }
    connect(ui->sagasu_btn, &QPushButton::clicked, this, &mainQT::sagasu);
    connect(ui->sagasu_list, &QListWidget::clicked, this, &mainQT::imi_out);
    connect(ui->sagasu_list, &QListWidget::currentItemChanged, this, &mainQT::imi_out);
    connect(ui->mp3_btn, &QPushButton::clicked, this, &mainQT::play_mp3);
    connect(ui->add_kotoba_btn, &QPushButton::clicked, this, &mainQT::_add);
    connect(ui->modify_kotoba_btn, &QPushButton::clicked, this, &mainQT::_modify);
    connect(ui->del_kotoba_btn, &QPushButton::clicked, this, &mainQT::_del);
    connect(ui->bookmark_list, &QListWidget::clicked, this, &mainQT::book_view);
    connect(ui->bookmark_list, &QListWidget::currentItemChanged, this, &mainQT::book_view);
    connect(ui->add_book_btn, &QPushButton::clicked, this, &mainQT::add_book);
    connect(ui->del_book_btn, &QPushButton::clicked, this, &mainQT::del_book);
    connect(ui->history_reset_btn, &QPushButton::clicked, this, &mainQT::reset_history);
    connect(ui->tab, &QTabWidget::currentChanged, this, &mainQT::tab_henkou);
    count_view();
    load_book();
    load_history();
    Pi();
    sagasu();
}

mainQT::~mainQT()
{
    delete ui;
}

void mainQT::sagasu(){
    ui->sagasu_list->clear();
    dic = new dic_exec();
    QString qkotoba = ui->kotoba_line->text();
    std::string kotoba = qkotoba.toStdString();
    list = dic->search(kotoba);
    delete dic;
    int l = list.size();
    for(int i = 0; i < l; i++){
        if (!(list[i].kanji.compare(list[i].kotoba))){
            QString kanji = QString::fromStdString(list[i].kanji);
            ui->sagasu_list->addItem(kanji);
        } else {
            QString kanji = QString::fromStdString(list[i].kanji);
            QString kotoba = QString::fromStdString(list[i].kotoba);
            QString q = kanji + "  " + kotoba;
            ui->sagasu_list->addItem(q);
        }
    }
}

void mainQT::imi_out(){
    ui->naiyou->setText("");
    cname = "";
    int n = ui->sagasu_list->currentRow();
    if (n<0){
        return;
    }
    QString kotoba = QString::fromStdString(list[n].kotoba);
    if (!(list[n].kotoba.compare(list[n].kanji))){
        ui->naiyou->append(kotoba);
    } else {
        QString kanji = QString::fromStdString(list[n].kanji);
        QString q = kanji + "  " + kotoba;
        ui->naiyou->append(q);
    }
    int h = list[n].hinsi;
    QString hinsi;
    switch(h){
    case 0:
        #ifdef ja
            hinsi = "[助詞]";
        #endif
        #ifdef en
            hinsi = "[Preposition]";
        #endif
        #ifdef ko
            hinsi = "[조사]";
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
    ui->naiyou->append(hinsi);
    QString imi = QString::fromStdString(list[n].imi);
    QString bikou = QString::fromStdString(list[n].bikou);
    ui->naiyou->append(imi);
    ui->naiyou->append(bikou);
    rireki = new history(list[n].kotoba, list[n].imi);
    delete rireki;
    load_history();
    cname = list[n].kotoba;
}

void mainQT::count_view(){
    dic = new dic_exec;
    count_p = dic->count_kotoba();
    count[0] = *count_p;
    for(int i = 1; i < 7; i++){
        count_p++;
        count[i] = *count_p;
    }
    delete dic;
    ui->count_lcd->display(count[0]);
}

void mainQT::play_mp3(){
    if (!(cname.compare(""))){
        return;
    }
    dic = new dic_exec();
    std::vector<unsigned char> mp3 = dic->mp3_load(cname);
    delete dic;
    if (mp3.size() < 4){
        return;
    }
    std::ofstream tfile("temp.mp3", std::ios::binary);
    tfile.write(reinterpret_cast<const char*>(mp3.data()), mp3.size());
    tfile.close();
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Mix_OpenAudio failed: " << Mix_GetError() << std::endl;
        SDL_Quit();
        return;
    }
    Mix_Music *music = Mix_LoadMUS("temp.mp3");
    if (!music) {
        std::cerr << "Mix_LoadMUS failed: " << Mix_GetError() << std::endl;
        Mix_CloseAudio();
        SDL_Quit();
        return;
    }
    if (Mix_PlayMusic(music, 1) == -1) {
        std::cerr << "Mix_PlayMusic failed: " << Mix_GetError() << std::endl;
        Mix_FreeMusic(music);
        Mix_CloseAudio();
        SDL_Quit();
        return;
    }
    while (Mix_PlayingMusic()) {
        SDL_Delay(100);
    }
    Mix_FreeMusic(music);
    Mix_CloseAudio();
    SDL_Quit();
    if (fileExists("temp.mp3")){
        remove("temp.mp3");
    }
}

void mainQT::_add(){
    bool k = false;
    add_ui = new add_kotoba(&k);
    add_ui->exec();
    delete add_ui;
    if (k){
        count_view();
        Pi();
        sagasu();
    }
}

void mainQT::_modify(){
    int n = ui->sagasu_list->currentRow();
    if (n<0){
        return;
    }
    tuple b;
    bool k = false;
    modify_ui = new modify_kotoba(list[n], &b, &k);
    modify_ui->exec();
    delete modify_ui;
    if (k){
        ui->sagasu_list->clear();
        ui->naiyou->setText("");
        cname = "";
        sagasu();
        book = new bookmark();
        if (book->kaburu_check(b.kotoba)){
            book->del_bookmark(b.kotoba);
            book->add_bookmark(b);
            delete book;
            load_book();
        } else {
            delete book;
        }
    }
}

void mainQT::_del(){
    int n = ui->sagasu_list->currentRow();
    if (n<0){
        return;
    }
    #ifdef ja
    QMessageBox::StandardButton f = QMessageBox::question(this, "警告", "本当に消してもいいですか？",
                                      QMessageBox::Yes | QMessageBox::No);
    #endif
    #ifdef en
    QMessageBox::StandardButton f = QMessageBox::question(this, "Warning", "Are you sure you want to delete this?",
                                      QMessageBox::Yes | QMessageBox::No);
    #endif
    #ifdef ko
    QMessageBox::StandardButton f = QMessageBox::question(this, "경고", "정말로 삭제 하겠습니까?",
                                      QMessageBox::Yes | QMessageBox::No);
    #endif
    if (f == QMessageBox::No){
        return;
    }
    dic = new dic_exec();
    dic->del_kotoba(list[n].kotoba);
    delete dic;
    count_view();
    ui->sagasu_list->clear();
    ui->naiyou->setText("");
    cname = "";
    book = new bookmark();
    if (book->kaburu_check(list[n].kotoba)){
        book->del_bookmark(list[n].kotoba);
        delete book;
        load_book();
    } else {
        delete book;
    }
    Pi();
    sagasu();
}

void mainQT::load_book(){
    ui->bookmark_list->clear();
    book = new bookmark();
    book_list = book->bookmark_load();
    book_count = book->bookmark_count();
    delete book;
    ui->bookmark_count_lcd->display(book_count);
    for(int i = 0; i < book_count; i++){
        if (!(book_list[i].kanji.compare(book_list[i].kotoba))){
            QString kanji = QString::fromStdString(book_list[i].kanji);
            ui->bookmark_list->addItem(kanji);
        } else {
            QString kanji = QString::fromStdString(book_list[i].kanji);
            QString kotoba = QString::fromStdString(book_list[i].kotoba);
            QString q = kanji + "  " + kotoba;
            ui->bookmark_list->addItem(q);
        }
    }
}

void mainQT::book_view(){
    ui->naiyou->setText("");
    cname = "";
    int n = ui->bookmark_list->currentRow();
    if (n<0){
        return;
    }
    QString kotoba = QString::fromStdString(book_list[n].kotoba);
    if (!(book_list[n].kotoba.compare(book_list[n].kanji))){
        ui->naiyou->append(kotoba);
    } else {
        QString kanji = QString::fromStdString(book_list[n].kanji);
        QString q = kanji + "  " + kotoba;
        ui->naiyou->append(q);
    }
    int h = book_list[n].hinsi;
    QString hinsi;
    switch(h){
    case 0:
        #ifdef ja
            hinsi = "[助詞]";
        #endif
        #ifdef en
            hinsi = "[Preposition]";
        #endif
        #ifdef ko
            hinsi = "[조사]";
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
    ui->naiyou->append(hinsi);
    QString imi = QString::fromStdString(book_list[n].imi);
    QString bikou = QString::fromStdString(book_list[n].bikou);
    ui->naiyou->append(imi);
    ui->naiyou->append(bikou);
    cname = book_list[n].kotoba;
}

void mainQT::add_book(){
    int n = ui->sagasu_list->currentRow();
    if (n<0){
        return;
    }
    tuple new_book;
    new_book.kotoba = list[n].kotoba;
    new_book.hinsi = list[n].hinsi;
    new_book.imi = list[n].imi;
    new_book.bikou = list[n].bikou;
    new_book.kanji = list[n].kanji;
    book = new bookmark();
    int f = book->add_bookmark(new_book);
    delete book;
    load_book();
    if (f == 0){
        #ifdef ja
        QMessageBox::warning(nullptr, "お知らせ", "すでに存在することばです。");
        #endif
        #ifdef en
        QMessageBox::warning(nullptr, "Notification.", "It's already an existing word.");
        #endif
        #ifdef ko
        QMessageBox::warning(nullptr, "알림", "이미 존재하는 단어입니다.");
        #endif
    }
}

void mainQT::del_book(){
    int n = ui->bookmark_list->currentRow();
    if (n<0){
        return;
    }
    ui->naiyou->setText("");
    cname = "";
    book = new bookmark();
    book->del_bookmark(book_list[n].kotoba);
    delete book;
    load_book();
}

void mainQT::load_history(){
    ui->history_view->clear();
    rireki = new history();
    rireki_list = rireki->read_history();
    delete rireki;
    int l = rireki_list.size();
    for(int i = 0; i < l; i++){
        std::string s = rireki_list[i];
        QString t = QString::fromStdString(s);
        ui->history_view->append(t);
    }
}

void mainQT::reset_history(){
    if (fileExists("history.txt")){
        remove("history.txt");
    }
    load_history();
}

void mainQT::Pi(){
    if (!hinsipi) {
            hinsipi = new QtCharts::QPieSeries();
    } else {
            hinsipi->clear();
    }
    #ifdef ja
    std::string a = "助詞";
    std::string b = "名詞";
    std::string c = "動詞";
    std::string d = "形容詞";
    std::string e = "副詞";
    std::string f = "その外";
    #endif
    #ifdef en
    std::string a = "Preposition";
    std::string b = "Noun";
    std::string c = "Verb";
    std::string d = "Adjective";
    std::string e = "Adverb";
    std::string f = "Others";
    #endif
    #ifdef ko
    std::string a = "조사";
    std::string b = "명사";
    std::string c = "동사";
    std::string d = "형용사";
    std::string e = "부사";
    std::string f = "그 외";
    #endif
    QtCharts::QPieSlice *s1 = hinsipi->append(QString::fromStdString(a), count[1]);
    s1->setBrush(Qt::yellow);
    s1->setLabelVisible(false);
    QtCharts::QPieSlice *s2 = hinsipi->append(QString::fromStdString(b), count[2]);
    s2->setBrush(Qt::magenta);
    s2->setLabelVisible(false);
    QtCharts::QPieSlice *s3 = hinsipi->append(QString::fromStdString(c), count[3]);
    s3->setBrush(Qt::cyan);
    s3->setLabelVisible(false);
    QtCharts::QPieSlice *s4 = hinsipi->append(QString::fromStdString(d), count[4]);
    s4->setBrush(Qt::green);
    s4->setLabelVisible(false);
    QtCharts::QPieSlice *s5 = hinsipi->append(QString::fromStdString(e), count[5]);
    s5->setBrush(Qt::lightGray);
    s5->setLabelVisible(false);
    QtCharts::QPieSlice *s6 = hinsipi->append(QString::fromStdString(f), count[6]);
    s6->setBrush(Qt::gray);
    s6->setLabelVisible(false);
    if (pi == nullptr) {
        pi = new QtCharts::QChart();
    }
    pi->addSeries(hinsipi);
    if (count[0]>0){
        s1->setLabelFont(QFont("Noto Sans CJK JP", 7));
        s1->setLabelVisible();
        s1->setLabelColor(Qt::black);
        s2->setLabelFont(QFont("Noto Sans CJK JP", 7));
        s2->setLabelVisible();
        s2->setLabelColor(Qt::black);
        s3->setLabelFont(QFont("Noto Sans CJK JP", 7));
        s3->setLabelVisible();
        s3->setLabelColor(Qt::black);
        s4->setLabelFont(QFont("Noto Sans CJK JP", 7));
        s4->setLabelVisible();
        s4->setLabelColor(Qt::black);
        s5->setLabelFont(QFont("Noto Sans CJK JP", 7));
        s5->setLabelVisible();
        s5->setLabelColor(Qt::black);
        s6->setLabelFont(QFont("Noto Sans CJK JP", 7));
        s6->setLabelVisible();
        s6->setLabelColor(Qt::black);
    }
    pi->legend()->setVisible(false);
    if (!pi_view) {
        pi_view = new QtCharts::QChartView(pi);
        pi_view->setRenderHint(QPainter::Antialiasing);
        ui->layer->addWidget(pi_view);
    } else {
        pi_view->setChart(pi);
    }
}

void mainQT::tab_henkou(){
    int t = ui->tab->currentIndex();
    if (t == 0){
        imi_out();
        ui->mp3_btn->setEnabled(true);
        ui->add_book_btn->setEnabled(true);
        ui->add_kotoba_btn->setEnabled(true);
        ui->modify_kotoba_btn->setEnabled(true);
        ui->del_kotoba_btn->setEnabled(true);
    } else if (t == 1) {
        book_view();
        ui->mp3_btn->setEnabled(true);
        ui->add_book_btn->setEnabled(false);
        ui->add_kotoba_btn->setEnabled(false);
        ui->modify_kotoba_btn->setEnabled(false);
        ui->del_kotoba_btn->setEnabled(false);
    } else {
        ui->naiyou->setText("");
        ui->mp3_btn->setEnabled(false);
        ui->add_book_btn->setEnabled(false);
        ui->add_kotoba_btn->setEnabled(false);
        ui->modify_kotoba_btn->setEnabled(false);
        ui->del_kotoba_btn->setEnabled(false);
    }
}

