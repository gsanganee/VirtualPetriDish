#include "node.h"
#include "element.h"

node::node(int nodeID, double X, double Y, int cellID, int zoneID)
{
    x = X;
    y = Y;
    id = nodeID;
    cellId = cellID;
    zoneId = zoneID;
}

QRectF node::boundingRect() const
{
    return QRectF (0, 0, 400, 250);
}

void node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QBrush brush(Qt::blue);
    QPen pen(Qt::red);
    painter->setBrush(brush);
    painter->setPen(pen);
    QPointF point(x,y);
    painter->drawPoint(point);
}
