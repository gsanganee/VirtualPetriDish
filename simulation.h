#ifndef SIMULATION_H
#define SIMULATION_H
#include "database.h"


class Simulation
{
public:
    Simulation(database* data);
    void setEdgeDiffusivities();
    void run(int numSteps, double dt, QGraphicsScene* sceneToUpdate);
    bool isPointInTriangle(node pt, node v1, node v2, node v3);
    float Sign(node p1, node p2, node p3);

    void findMaxAndMinMasses();
    void setElementColour();

    database* pData;
    QList<double> new_masses_species1;
    double max_mass_sp1;
    double min_mass_sp1;
};

#endif // SIMULATION_H
