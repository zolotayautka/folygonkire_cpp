#ifndef ADD_KOTOBA_H
#define ADD_KOTOBA_H

#include <QDialog>
#include "folygonkire.h"
#include <QFileDialog>
#include <QMessageBox>

namespace Ui {
class add_kotoba;
}

class add_kotoba : public QDialog
{
    Q_OBJECT

public:
    explicit add_kotoba(QWidget *parent = nullptr);
    ~add_kotoba();

private:
    Ui::add_kotoba *ui;
    dic_exec* dic;
    std::vector<unsigned char> mp3;
    QString file_name;

private slots:
    void _add();
    void file_select();
    bool file(QString file_name);
};

#endif // ADD_KOTOBA_H
