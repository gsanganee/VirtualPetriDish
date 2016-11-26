#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "simulation.h"
#include <QList>

const int NUCLEUS = 1, CYTO = 2, IMEMBRANE = 3, MEMBRANE = 4, OMEMBRANE = 5;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    pData = new database();
    Gmsh = new gmsh("C:/Users/AkmalAriff/Desktop/gmsh-2.5.0-Windows/", pData);
    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(run()));
}

void MainWindow::run()
{
    Gmsh->write("C:/Users/AkmalAriff/Desktop/gmsh-2.5.0-Windows/writeTest.txt");
    Gmsh->readMesh("C:/Users/AkmalAriff/Desktop/gmsh-2.5.0-Windows/writeTest.msh");
    foreach(element* e, pData->vpElements)
    {
        scene->addItem(e);
        scene->update();
    }
    QCoreApplication::processEvents();
    Simulation Sim(pData);
    Sim.run(2,0.01,scene);
}

MainWindow::~MainWindow()
{
    delete ui;
}
