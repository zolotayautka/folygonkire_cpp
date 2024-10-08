#ifndef MAINQT_H
#define MAINQT_H

#include <QMainWindow>
#include "folygonkire.h"
#include "add_kotoba.h"
#include "modify_kotoba.h"
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>
#include <sys/stat.h>
#include <QMessageBox>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "new_dic.h"

QT_BEGIN_NAMESPACE
namespace Ui { class mainQT; }
QT_END_NAMESPACE

class mainQT : public QMainWindow
{
    Q_OBJECT

public:
    mainQT(QWidget *parent = nullptr);
    ~mainQT();

private:
    Ui::mainQT *ui;
    int* count_p;
    add_kotoba* add_ui;
    modify_kotoba* modify_ui;
    int count[7];
    std::vector<tuple> list;
    std::string cname = "";
    std::vector<tuple> book_list;
    std::vector<std::string> rireki_list;
    void count_view();
    void load_book();
    void load_history();
    QtCharts::QPieSeries *hinsipi = nullptr;
    QtCharts::QChart *pi = nullptr;
    QtCharts::QChartView *pi_view = nullptr;
    void Pi();
    bool koutyakugo_f;
    bool hisf = true;

private slots:
    void sagasu();
    void _add();
    void _modify();
    void _del();
    void imi_out();
    void book_view();
    void reset_history();
    void add_book();
    void del_book();
    void play_mp3();
    void tab_henkou();
};

#endif // MAINQT_H
