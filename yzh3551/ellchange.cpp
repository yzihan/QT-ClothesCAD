#include "ellchange.h"
#include "ui_ellchange.h"

ellchange::ellchange(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ellchange)
{
    ui->setupUi(this);
    ui->doublehigh->setMaximum(100);
    ui->doublehigh->setMinimum(15);
    ui->doublewide->setMinimum(150);
    ui->doublewide->setMaximum(1000);
}

ellchange::~ellchange()
{
    delete ui;
}



int ellchange::getWide()
{
    return ui->doublewide->value();
}

int ellchange::getHigh()
{
    return ui->doublehigh->value();
}


