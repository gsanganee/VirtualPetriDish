#include "square.h"

Square::Square()
{

}

QRectF Square::boundingRect() const
{
    return QRectF(0,0, 20,20);
}

void Square::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF rec = boundingRect();
    QBrush brush(Qt::blue);
    QPen pen(Qt::red);
    painter->setBrush(brush);
    painter->setPen(pen);
    painter->drawRect(rec);
}

QList<node> Square::getNodes()
{
}


