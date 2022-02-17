#include "pointmove.h"
#include "ui_pointmove.h"


pointmove::pointmove(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::pointmove)
{
    ui->setupUi(this);
    ui->mleft->setMaximum(1000);
    ui->mleft->setMinimum(10);
    ui->mright->setMaximum(1000);
    ui->mright->setMinimum(10);
    ui->mdown->setMaximum(1000);
    ui->mdown->setMinimum(10);
    ui->mup->setMaximum(1000);
    ui->mup->setMinimum(10);

}

pointmove::~pointmove()
{
    delete ui;
}



int pointmove::getleft()
{
    return ui->mleft->value();
}


int pointmove::getright()
{
    return ui->mright->value();
}

int pointmove::getdown()
{
    return ui->mdown->value();
}

int pointmove::getup()
{
    return ui->mup->value();
}
