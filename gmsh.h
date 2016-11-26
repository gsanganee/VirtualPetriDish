#ifndef GMSH_H
#define GMSH_H
#include <QString>
#include <QList>
#include "node.h"
#include "element.h"
#include "database.h"
#include <QFileSystemWatcher>

class gmsh : public QObject
{
    Q_OBJECT

public:
    gmsh(QString gmshDIR, database* pDataArg);
    void readMesh(QString mshFilename);
    void parseMesh(QString mshFilename);
    void createElemsNeighElemsList();
    void createNodesNeighElemsList();

    //AAAB - testing writing to Gmsh
    void write(QString gmshGeoFilename);

    database* pData;
    QString gmshDir;
    QString meshFilename;
    int timercount;
    QFileSystemWatcher* pWatcher;
    int move;
};

#endif // GMSH_H
