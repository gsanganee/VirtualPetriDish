#ifndef SQUARE_H
#define SQUARE_H
#include <QPainter>
#include <QGraphicsItem>
#include <QDebug>
#include "node.h"

class Square : public QGraphicsItem
{
public:
    Square();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QList<node> getNodes();
};

#endif // SQUARE_H
