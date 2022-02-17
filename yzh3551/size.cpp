
#include "size.h"
//#include "ui_size.h"
#if 0
size::size(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::size)
{
    ui->setupUi(this);
    ui->big->setMaximum(100);
    ui->big->setMinimum(15);
    ui->small->setMinimum(150);
    ui->small->setMaximum(1000);
}

size::~size()
{
    delete ui;
}



int size::getbig()
{
    return ui->big->value();
}

int size::getsmall()
{
    return ui->small->value();
}

#endif
