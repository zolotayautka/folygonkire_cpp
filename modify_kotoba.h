#ifndef MODIFY_KOTOBA_H
#define MODIFY_KOTOBA_H

#include <QDialog>
#include "folygonkire.h"
#include <QFileDialog>

namespace Ui {
class modify_kotoba;
}

class modify_kotoba : public QDialog
{
    Q_OBJECT

public:
    explicit modify_kotoba(tuple* t, bool* k, QWidget *parent = nullptr);
    ~modify_kotoba();

private:
    Ui::modify_kotoba *ui;
    std::vector<unsigned char> mp3;
    QString file_name;
    tuple* t;
    bool* k;
    bool del_f = false;
    bool file(QString file_name);
    inline void kanji_re();

private slots:
    void _modify();
    void file_select();
    void del_f_edit();
};

#endif // MODIFY_KOTOBA_H
