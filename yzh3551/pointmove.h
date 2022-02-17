#ifndef POINTMOVE_H
#define POINTMOVE_H

#include <QDialog>


namespace Ui {
class pointmove;
}



class pointmove : public QDialog
{
    Q_OBJECT

public:
    explicit pointmove(QWidget *parent = nullptr);
    ~pointmove();
    int getleft();
    int getright();
    int getdown();
    int getup();


private:
    Ui::pointmove *ui;

};

#endif // POINTMOVE_H



