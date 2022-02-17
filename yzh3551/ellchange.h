#ifndef ELLCHANGE_H
#define ELLCHANGE_H

#include <QDialog>


namespace Ui {
class ellchange;
}



class ellchange : public QDialog
{
    Q_OBJECT

public:
    explicit ellchange(QWidget *parent = nullptr);
    ~ellchange();
    int getWide();
    int getHigh();

private:
    Ui::ellchange *ui;

};

#endif // ELLCHANGE_H




