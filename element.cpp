#include "element.h"
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QGraphicsScene>
#include <QtGlobal>
#include <QList>
#include <QColor>
#include "node.h"

element::element(int elmID, int cellID, int zoneID, node *n1, node *n2, node *n3)
{
    Id = elmID;
    cellId = cellID;
    zoneId = zoneID;
    node1 = n1;
    node2 = n2;
    node3 = n3;

    nodes.append(n1);
    nodes.append(n2);
    nodes.append(n3);

    centre.setX((node1->x + node2->x + node3->x)/3.0);
    centre.setY((node1->y + node2->y + node3->y)/3.0);

    //The following side# variables are vectors representing the three sides of a triangular element.
    //The x and y coordinates for each sides are simply determined by calcualting the rise and run between nodes.
    //The vecotrs move in a clockwise fashion from node1 to node2 (side1), node2 to node3 (side2), and node3 to node1 (side3).
    side1.setX(node2->x-node1->x);
    side1.setY(node2->y-node1->y);
    side2.setX(node3->x-node2->x);
    side2.setY(node3->y-node2->y);
    side3.setX(node1->x-node3->x);
    side3.setY(node1->y-node3->y);

    //The three side vectors are stored in a list called 'sides'.
    sides.append(side1);
    sides.append(side2);
    sides.append(side3);

    //Area of a triangular element
    area=0.5*qAbs((side1.x()*side3.y())-(side3.x()*side1.y()));

    //The norms of the three sides. Simply calculatated by taking the negative reciprocal of the asscoiated side.
    norm1.setX(side1.y()/side1.length());
    norm1.setY((-1.0)*side1.x()/side1.length());
    norm2.setX(side2.y()/side2.length());
    norm2.setY((-1.0)*side2.x()/side2.length());
    norm3.setX(side3.y()/side3.length());
    norm3.setY((-1.0)*side3.x()/side3.length());

    //'norms' is a list containing the three norms of an element
    norms.append(norm1);
    norms.append(norm2);
    norms.append(norm3);

    //The mass of a species contained within an element, is a property of the element.
    //Initially we set the mass of species1 to 0.
    //Note, if more species are being modelled, you'll need to add a similar statement for each one.
    if(centre.x()>0)
    {
        m_old_sp1 = 500.0;
    }
    else
    {
        m_old_sp1 = 0.0;
    }
    //This is a list containing the diffusivity coefficient for species 1 at different points on the element.
    //You'll need separate lists for each species being modelled.
    diffusivities.append(10.0);        //center diffusivity
    diffusivities.append(0.0);        //diffusivity along side 1
    diffusivities.append(0.0);        //diffusivity along side 2
    diffusivities.append(0.0);        //diffusivity along side 3

    //Initially we set the source term for species 1 to 0.
    source_sp1=0.0;

    //Initially the colour of the element is set to white (meaning no mass)
    elementColour=Qt::white;

    //This is the triangle defining the element.
    triangle.append(QPointF(node1->x, (-1.0)*node1->y));
    triangle.append(QPointF(node2->x, (-1.0)*node2->y));
    triangle.append(QPointF(node3->x, (-1.0)*node3->y));
    triangle.append(QPointF(node1->x, (-1.0)*node1->y));

}

//The boudning rectangle of the element
QRectF element::boundingRect() const
{
    return triangle.boundingRect();

}

//The shape of the element. Overwriting the shape function helps for painting and if any collision detection is required
QPainterPath element::shape() const
{

    QPainterPath path;
    path.addPolygon(triangle);
    return path;

}

void element::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QBrush brush;
    QPen pen;
    brush.setColor(elementColour);
    brush.setStyle(Qt::SolidPattern);
    painter->setBrush(brush);
    if (zoneId == 0)
        pen.setColor(Qt::red);
    else if (zoneId == 1)
        pen.setColor(Qt::green);
    else if (zoneId == 2)
        pen.setColor(Qt::black);
    else
        pen.setColor(Qt::yellow);
    painter->setPen(pen);
    painter->drawPolygon(triangle);
}
