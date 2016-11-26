#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QList>
#include "square.h"
#include "node.h"
#include "element.h"
#include "database.h"
#include "gmsh.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);  
    ~MainWindow();
    database* pData;
    gmsh* Gmsh;

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    Square* square;

private slots:
    void run();
};

#endif // MAINWINDOW_H
