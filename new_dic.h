#ifndef NEW_DIC_H
#define NEW_DIC_H

#include <QDialog>
#include "folygonkire.h"

namespace Ui {
class new_dic;
}

class new_dic : public QDialog
{
    Q_OBJECT

public:
    explicit new_dic(bool* k, QWidget *parent = nullptr);
    ~new_dic();

private:
    Ui::new_dic *ui;
    bool* k;

private slots:
    void tukuru();
};

#endif // NEW_DIC_H
