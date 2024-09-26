#ifndef ADD_KOTOBA_H
#define ADD_KOTOBA_H

#include <QDialog>
#include "folygonkire.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>

namespace Ui {
class add_kotoba;
}

class add_kotoba : public QDialog
{
    Q_OBJECT

public:
    explicit add_kotoba(bool* k, QWidget *parent = nullptr);
    ~add_kotoba();

private:
    Ui::add_kotoba *ui;
    std::vector<unsigned char> mp3;
    QString file_name;
    bool* k;
    bool file(QString file_name);
    QProcess *gen_tts = nullptr;

private slots:
    void _add();
    void file_select();
};

#endif // ADD_KOTOBA_H
