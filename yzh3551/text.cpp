#include "ui_text.h"
#include "text.h"
#include <QFont>
#include <QPainter>
#include <QCursor>
#include <QKeyEvent>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QTextBlockFormat>
#include <iostream>
#include <text.h>

TextDialog::TextDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextDialog)
{
    ui->setupUi(this);
    ui->spinBox_text->setMaximum(100);
    ui->spinBox_text->setMinimum(15);
    ui->spinBox_Width->setMinimum(150);
    ui->spinBox_Width->setMaximum(1000);
    ui->spinBox_Rotation->setMinimum(0);
    ui->spinBox_Rotation->setMaximum(359);
}

TextDialog::~TextDialog()
{
    delete ui;
}

QString TextDialog::getFont()
{
    return ui->comboBox_text->currentText();
}

int TextDialog::getWidth()
{
    return ui->spinBox_text->value();
}

qreal TextDialog::getKWidth()
{
    return ui->spinBox_Width->value();
}

qreal TextDialog::getRotation()
{
    return ui->spinBox_Rotation->value();
}

TextItem::TextItem(QObject *parent) :
    QObject(parent)
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsMovable);

    QTextBlockFormat format;
    format.setAlignment(Qt::AlignCenter);
    //出现窗口，获得字体
    if(td.exec() == QDialog::Accepted)
    {
        tFont = td.getFont();
        tWeight = td.getWidth();
        myWidth = td.getKWidth();//文本框宽度
        myRotation = td.getRotation();
    }
    else
    {
        throw nullptr;
        // TODO: xxxxx
    }
    //尺寸
    adjust = 20;
    myFont.setFamily(tFont);
    myFont.setPointSize(tWeight);
    //文本
    text_item = new QGraphicsTextItem(this);
    text_item->setTextWidth(myWidth);
    text_item->setFont(myFont);
    text_item->setTextInteractionFlags(Qt::TextEditorInteraction);
    text_interaction = true;
    text_item->setPlainText(trUtf8("请在这里输入文字哦"));
    text_rect = text_item->boundingRect();
    text_item->setPos(520, 520);
    text_item->setTransform(text_item->transform().translate(-20,-20).rotate(myRotation).translate(20,20));
    //矩形
    rect_item = new QGraphicsRectItem(this);
    QPen *pen = new QPen(Qt::lightGray);
    rect_item->setPen(*pen);
    rect_item->setRect(500, 500,
                       text_rect.width() + 3*adjust,
                       text_rect.height() + 3*adjust);
    rect_item->setTransform(rect_item->transform().translate(500,500).rotate(myRotation).translate(-500,-500));

};

TextItem::~TextItem()
{
    delete text_item;
    delete rect_item;
}

QRectF TextItem::boundingRect(void) const //初始化长方形的位置，大小
{
    rect_item->setRect(500, 500,
                       text_rect.width() + 2*adjust,
                       text_rect.height() + 2*adjust);
    return rect_item->boundingRect();
}
void TextItem::paint(QPainter *painter,
                     const QStyleOptionGraphicsItem *option,
                     QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}

void TextItem::keyPressEvent(QKeyEvent *event)
{
    QGraphicsItem::keyPressEvent(event);
}
void TextItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //setFocus();
    QGraphicsItem::mousePressEvent(event);
}
void TextItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    setCursor(Qt::OpenHandCursor);
}
void TextItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    setCursor(Qt::ArrowCursor);
}
void TextItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

void TextItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu; //显示但不操作
    QAction *edit_action = menu.addAction(trUtf8("编辑开关"));
    QAction *selected_action = menu.exec(event->screenPos());
    if(selected_action == edit_action){
        if(text_interaction){
            text_item->setTextInteractionFlags(Qt::NoTextInteraction);
            text_interaction = false;
            reset_text_rect();
            update(); //更新
        }else{
            text_item->setTextInteractionFlags(Qt::TextEditorInteraction);//可编辑
            text_interaction = true;
        }
    }
}

void TextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    setFocus();
    QGraphicsItem::mouseDoubleClickEvent(event);
}
void TextItem::reset_text_rect(void)
{
    text_rect = text_item->boundingRect();
}


