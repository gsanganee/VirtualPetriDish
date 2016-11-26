#ifndef ELEMENT_H
#define ELEMENT_H

#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsPolygonItem>
#include <QPainter>
#include <QGraphicsScene>
#include <QVector2D>
#include <QList>

class node;

class element : public QGraphicsPolygonItem
{
public:
    explicit element(int elmID, int cellID, int zoneID, node* n1, node* n2, node* n3);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    QPainterPath shape() const;

    int Id;
    int cellId;
    int zoneId;
    node* node1;
    node* node2;
    node* node3;
    QPolygonF triangle;

    QPointF centre;
    double area;

    double m_old_sp1;

    QList <double> diffusivities;   // center, side1, side2, side3
                                    //QList <double> diffusivities_sp2;
                                    //QList <double> diffusivities_sp3;


    double source_sp1;

    QVector2D norm1;
    QVector2D norm2;
    QVector2D norm3;
    QVector2D side1;
    QVector2D side2;
    QVector2D side3;
    QList <QVector2D> norms;
    QList <QVector2D> sides;
    QList <node*> nodes;
    QList <element*> vpNeighElements;

    QColor elementColour;

};

#endif // ELEMENT_H
