#ifndef NODE_H
#define NODE_H

#include <QPainter>
#include <QGraphicsItem>
#include <QDebug>
#include <QList>

class element;

class node : public QGraphicsItem
{
public:

    int id;
    int cellId;
    int zoneId;
    double x;
    double y;
    int colour;
    QList<element*> vpNeighElements;

    node(int nodeID, double X, double Y, int cellID, int zoneID);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // NODE_H
