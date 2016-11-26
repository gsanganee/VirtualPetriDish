#include "gmsh.h"
#include <QTextStream>
#include <QFile>
#include <QTimer>
#include <QProcess>
#include <QDir>
#include <QtCore>
#include <QDebug>



gmsh::gmsh(QString gmshDIR, database* pDataArg)
{
    pData = pDataArg;
    gmshDir = gmshDIR;

    pWatcher = new QFileSystemWatcher();
    pWatcher->addPath("C:\\Users\\AkmalAriff\\Desktop\\gmsh-2.5.0-Windows\\writeTest.msh");
    move = 0;

}

void gmsh::readMesh(QString mshFilename)
{
    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
    QObject::connect(pWatcher, SIGNAL(fileChanged(QString)), &loop, SLOT(quit()));
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    timer.start(10000);
    loop.exec();

    if(timer.isActive())
    {
        timer.stop();
        qDebug() <<"file changed";
        parseMesh(mshFilename);
    }
    else
    {
        qDebug() <<"timer timeout";
        parseMesh(mshFilename);
    }
}

void gmsh::parseMesh(QString mshFilename)
{
    QFile mshFile(mshFilename);

    if(mshFile.exists())
    {
        QList<element*>* pvpElements = &(pData->vpElements);
        QList<node*>* pvpNodes = &(pData->vpNodes);

        pvpElements->clear();
        pvpNodes->clear();
        
        
        //Attempt to open the file for reading
        if(!mshFile.open(QFile::ReadOnly| QFile::Text))
        {
            qDebug()<<"Can't open "<<mshFilename;
        }
        
        //Read in the first line of the file. If it is not empty, proceed with reading the file.
        QTextStream in(&mshFile);
        QString line = in.readLine();
        int linecount=1;
        while (!line.isNull())
        {
            if(line=="$Nodes")                          //the first section of information in a .msh file pertains to the nodes
            {
                line = in.readLine();
                ++linecount;
                if(!line.isNull())
                {
                    pvpNodes->reserve(line.toInt());     //The msh file states the # of nodes. Reserve that many spots in the node list
                    line = in.readLine();
                    ++linecount;
                    while(!(line.isNull()|| line=="$EndNodes"))         //until a null line of $EndNodes is reached...
                    {
                        QStringList nodeinfo = line.split(" ");         //split the info on a line using the space
                        int nodeId=nodeinfo[0].toInt();                 //first is the node ID
                        double nodex=nodeinfo[1].toDouble();            //Node x coordinate
                        double nodey=(1.0)*(nodeinfo[2].toDouble());    //Node y coordinate
                        node* pNewNode=new node(nodeId, nodex, nodey, 1, 1);  //create the node
                        pvpNodes->append(pNewNode);                       //append it to the node list
                        line = in.readLine();                           //read the next line
                        ++linecount;
                    }
                    
                    //Search for "$Elements" which begins the element section of the file
                    while(!(line.isEmpty()))
                    {
                        if(line=="$Elements")
                        {
                            line = in.readLine();
                            ++linecount;
                            if(!line.isNull())
                            {
                                pvpElements->reserve(line.toInt());
                                line = in.readLine();
                                ++linecount;
                                //Read element info until null or end of section
                                while(!(line.isNull()|| line=="$EndElements"))
                                {
                                    
                                    QStringList elementinfo = line.split(" ");  //split line info based on space
                                    int elementId=elementinfo[0].toInt();       //element ID is first on a line
                                    int cellId = 2;
                                    int zoneId = 2;
                                    //                                int cellId;
                                    //                                if (elementinfo[3].left(3) == "111")
                                    //                                    cellId = elementinfo[3].right(elementinfo[3].length() - 3).toInt();          //the ID of the cell to which it belongs
                                    //                                else
                                    //                                    cellId = elementinfo[3].toInt();
                                    //                                int zoneId;
                                    //                                if (elementinfo[4].right(4) == "1110")
                                    //                                    zoneId =0;      //take mod 3 to determine if in membrane, cytoplasm, or nucleus
                                    //                                if (elementinfo[4].right(4) == "1111")
                                    //                                    zoneId =1;      //take mod 3 to determine if in membrane, cytoplasm, or nucleus
                                    //                                if (elementinfo[4].right(4) == "1112")
                                    //                                    zoneId =2;      //take mod 3 to determine if in membrane, cytoplasm, or nucleus
                                    //                                if (elementinfo[4].right(4) == "1113")
                                    //                                    zoneId =3;      //take mod 3 to determine if in membrane, cytoplasm, or nucleus
                                    int n1index=elementinfo[5].toInt()-1;   //The nodelist index of the 1st node of the element
                                    int n2index=elementinfo[6].toInt()-1;   //The nodelist index of the 2nd node of the element
                                    int n3index=elementinfo[7].toInt()-1;   //The nodelist index of the 3rd node of the element
                                    
                                    element* pNewElement= new element(elementId, cellId, zoneId, (*pvpNodes)[n1index], (*pvpNodes)[n2index], (*pvpNodes)[n3index]);
                                    pvpElements->append(pNewElement);
                                    
                                    line = in.readLine();
                                    ++linecount;
                                }
                                //finished elements
                                
                            }
                        }
                        else
                        {
                            line = in.readLine();
                            ++linecount;
                        }
                    }
                }
                
            }
            
            line = in.readLine();
            ++linecount;
        }
        mshFile.close();

        //When finished reading in the file, and IF elements exist, create the element neightbours list and
        //node neighbour list.
        if(!pvpElements->isEmpty())
        {
            createElemsNeighElemsList();
            if(!pvpNodes->isEmpty())
            {
                createNodesNeighElemsList();
            }
        }
    }
    else
    {
        readMesh(mshFilename);
    }

}

void gmsh::createElemsNeighElemsList()
{
    pData->vvpElemsNeighElems.clear();                 //this list holds the element neighbours for all the elements
    node* dummynode = new node(-2,0,0,-2,-2);     //dummy node created for the dummy element
    element* dummyelement = new element(-2, -2, -2, dummynode, dummynode, dummynode); //dummy element
    QList<element*> temp_neigh_list;

    //for each element in the element list, find it's neighbours. This element is the base element for
    //single iteration through the loop.
    foreach (element* ebase, pData->vpElements)
    {
        temp_neigh_list.clear();                //a temp list that will hold the three neighbours
        temp_neigh_list.append(dummyelement);   //initialize with three dummy elements
        temp_neigh_list.append(dummyelement);
        temp_neigh_list.append(dummyelement);

        //loop through the element list looking for neighbours
        foreach(element* eneigh, pData->vpElements)
        {
            if(ebase->node1==eneigh->node1 || ebase->node1==eneigh->node2 || ebase->node1==eneigh->node3)
            {
                if(ebase->node2==eneigh->node1 || ebase->node2==eneigh->node2 || ebase->node2==eneigh->node3)
                {
                    if(!(ebase->node3==eneigh->node1 || ebase->node3==eneigh->node2 || ebase->node3==eneigh->node3))
                    {
                        //if an element shares node1 and node2 with the base element, but not all three
                        //nodes (aka, is not the base element itself), then it is the first neighbour
                        temp_neigh_list[0]=eneigh;
                    }
                }
                else if(ebase->node3==eneigh->node1 || ebase->node3==eneigh->node2 || ebase->node3==eneigh->node3)
                {
                    //if an element shares node1 and node3 with the base element, then it is the 3rd neighbour
                    temp_neigh_list[2]=eneigh;
                }

            }
            else if(ebase->node2==eneigh->node1 || ebase->node2==eneigh->node2 || ebase->node2==eneigh->node3)
            {
                if(ebase->node3==eneigh->node1 || ebase->node3==eneigh->node2 || ebase->node3==eneigh->node3)
                {
                    //if an element shares node2 and node3 with the base element, then it is the 2nd neighbour
                    temp_neigh_list[1]=eneigh;
                }

            }

            //If none of the neighbour elements in the temp list have the dummy ID anymore, then three
            //neighbours were found and we can stop searching for more.
            if(!temp_neigh_list[0]->Id==-2 && !temp_neigh_list[1]->Id==-2 && !temp_neigh_list[2]->Id==-2)
            {
                break;
            }
        }

        //add the temp list to the master list of element neighbours
        pData->vvpElemsNeighElems.append(temp_neigh_list);

    }
    //qDebug()<<"Done Element Neighbours";
}

void gmsh::createNodesNeighElemsList()
{
    pData->vvpNodesNeighElems.clear(); //the master list of all the elements adjacent to every node

    QList<element*> temp_neigh_list;

    //For each node, look for elements that use this node
    foreach (node* nbase, pData->vpNodes)
    {
        temp_neigh_list.clear();    //a temp list that holds elements that use the base node

        //loop through the element list looking for those that use the base node
        foreach(element* eneigh, pData->vpElements)
        {
            if(nbase==eneigh->node1 || nbase==eneigh->node2 || nbase==eneigh->node3)
            {
                //if one of the three nodes of an element, match the base node, add the element to the
                //temp list. It is a neighbour to the node.
                temp_neigh_list.append(eneigh);
            }
        }
        //append the temp list to the master list holding the neighbours for all the nodes.
        pData->vvpNodesNeighElems.append(temp_neigh_list);
        nbase->vpNeighElements = temp_neigh_list;
    }
}

void gmsh::write(QString gmshGeoFilename)
{
    move++;
    qDebug()<<"move: "<<move;
    node p1(1,100,100,1,1);
    node p2(2,100,-100,1,1);
    node p3(3,-100,-100,1,1);
    node p4(4,-100,100,1,1);

    node p5(5,20+move,20,1,2);
    node p6(6,20+move,-20,1,2);
    node p7(7,-20+move,-20,1,2);
    node p8(8,-20+move,20,1,2);

    QList<node*> path1;
    path1 << &p1 << &p2 << &p3 << &p4;

    QList<node*> path2;
    path2 << &p5 << &p6 << &p7 << &p8;

    QList<QList<node*> > vPaths;
    vPaths << path1 << path2;


    double cl = 10;

    QFile geoFile(gmshGeoFilename);
    geoFile.open(QFile::WriteOnly| QFile::Text);
    QTextStream geoOut(&geoFile);

    foreach(QList<node*> path, vPaths)
    {
        foreach(node* point, path)
        {
            geoOut << "Point(" << point->id << ") = {" << point->x << "," << point->y << ",0," << cl << "};\n";
        }

        for(int i=0;i<path.length();i++)
        {
            geoOut << "Line(" << path[i]->id << ") = {" << path[i]->id << "," << path[(i+1)%path.length()]->id << "};\n";
        }

        geoOut << "Line Loop(" << path[0]->cellId << path[0]->zoneId << ") = {";
        for(int i=0;i<path.length();i++)
        {
            geoOut << path[i]->id;
            if(i!=path.length()-1)
            {
                geoOut << ",";
            }
            else
            {
                geoOut << "};\n";
            }
        }
    }
    geoOut << "Plane Surface(" << path1[0]->cellId << path1[0]->zoneId << ") = {" << path1[0]->cellId << path1[0]->zoneId << "," << path2[0]->cellId << path2[0]->zoneId << "};\n";
    geoOut << "Physical Surface(" << path1[0]->cellId << path1[0]->zoneId << ") = {" << path1[0]->cellId << path1[0]->zoneId << "};\n";

    geoFile.close();

    QProcess *p = new QProcess();

    QString original_directory = QDir::currentPath();

    //Set the current directory to that in which Gmsh is located so the cmd window opens in that directory
    QString gmsh_directory("C:/Users/AkmalAriff/Desktop/gmsh-2.5.0-Windows/");
    bool dirflag=QDir::setCurrent("C:/Users/AkmalAriff/Desktop/gmsh-2.5.0-Windows/");
    p->start("C:\\Users\\AkmalAriff\\Desktop\\gmsh-2.5.0-Windows\\gmsh.exe -2 writeTest.txt -o writeTest.msh");



}
