#ifndef DATABASE_H
#define DATABASE_H
#include <QList>
#include "node.h"
#include "element.h"


class database
{
public:
    database();
    QList<node*> vpNodes;
    QList<element*> vpElements;
    QList<node*> vpPrevNodes;
    QList<element*> vpPrevElements;
    QList<QList<element*> > vvpNodesNeighElems;
    QList<QList<element*> > vvpElemsNeighElems;
};

#endif // DATABASE_H
