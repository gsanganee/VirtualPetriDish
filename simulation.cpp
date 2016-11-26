#include "simulation.h"
#include <cmath>
#include <QtCore>
#include <QColor>
#include <QDebug>

Simulation::Simulation(database* data)
{
    pData = data;
}

void Simulation::setEdgeDiffusivities()
{
    int counter=0; //the index of the element_neighList. Also corresponds to the index of the element in the element list

    //loop through every element
    foreach(element* e, pData->vpElements)
    {
        //Each element has at most 3 neighbours. Loop through them.
        for(int i= 0; i < 3; ++i)
        {
            element *n = pData->vvpElemsNeighElems[counter][i];  //the i-ith neighbour element for the counter-ith element

            if(n->Id==-2)                    //an ID of -2 for a neighbour means, it doesn't exist (the edge is on the boundary)
            {
                e->diffusivities[i+1]=0.0;   //set the diffusivity to 0 for that edge.
            }
            else                             //a neighbour on the ith side does exist
            {
                double dist_en=0.0;          //distance between centroids of neighbouring elements e and n.

                dist_en=sqrt((e->centre.x()-n->centre.x())*(e->centre.x()-n->centre.x())+(e->centre.y()-n->centre.y())*(e->centre.y()-n->centre.y()));

                //Now we need to find the distance bewteen their common edge and the centroid of the neighbour
                //following the same line between the two centroids.

                //Let line 1 be the line between the two centroids. Using y=mx+b notation, we have:
                double m1;
                double b1;
                if((n->centre.x()-e->centre.x())==0.00) //centroids are directly above each other
                {

                }
                else
                {
                    m1=(n->centre.y()-e->centre.y())/(n->centre.x()-e->centre.x());
                    b1=e->centre.y()-(m1*e->centre.x());
                }


                //Let line 2 be the line representing the edge between the two elements. Using y=mx+b notation, we have:
                double m2;
                double b2;
                if(e->sides[i].x()==0.00)
                {

                }
                else
                {
                    m2=(e->sides[i].y())/(e->sides[i].x());
                    b2=(e->nodes[i]->y)-(m2*e->nodes[i]->x);
                }


                //Given the two lines, find the intersection point:
                double x_int;
                double y_int;
                if((n->centre.x()-e->centre.x())==0.00)
                {
                    x_int=n->centre.x();
                    y_int=m2*(x_int-(e->nodes[i]->x))+(e->nodes[i]->y); //use any point along the side
                }
                else if(e->sides[i].x()==0.00)
                {
                    x_int=e->nodes[i]->x;
                    y_int=m1*(x_int-(n->centre.x()))+(n->centre.y());   //use any point on the line connecting the centroids
                }
                else
                {
                    x_int=(b2-b1)/(m1-m2);
                    y_int=m1*x_int+b1;
                }


                //Given the intersection point on the edge, find the distance from the edge to the centroid of the neighbour:
                double dist_sn=0.0;          //distance from side to neighbour
                dist_sn=sqrt((x_int-n->centre.x())*(x_int-n->centre.x())+(y_int-n->centre.y())*(y_int-n->centre.y()));

                //Using the to distances, calculate the distance ratio f:
                double f=dist_sn/dist_en;


                //Using this factor, calculate an approximate diffusivity at the edge:
                double De=e->diffusivities[0];      //the diffusivity at the centroid of the element
                double Dn=n->diffusivities[0];      //the diffusivity at the centroid of the neighbour element
                double Ds=0.00;                     //the diffusivity on the edge between e and n

                if(!De==0.00 && !Dn==0.00 && !(Dn*(1-f)+De*(f))==0.00)
                {
                    Ds=(De*Dn)/(Dn*(1-f)+De*(f));

                }

                e->diffusivities[i+1]=Ds;           //set the diffusivity for that edge

            }
        }
        counter=counter+1;
    }
}

void Simulation::run(int numSteps, double dt, QGraphicsScene* sceneToUpdate)
{
    qDebug() << "Simulation loop";

    new_masses_species1.clear();

    //Set the diffusivities at the edges of each element
    setEdgeDiffusivities();

    for(int it=0; it<numSteps; it++)
    {
        qDebug()<<"iteration: "<<it;
        for(int i=0; i<pData->vpElements.size(); ++i)             //loop through elements
        {
            new_masses_species1.append(0.00);
            element* e = pData->vpElements[i];

            double D_hat_total=0.00;                    //Sum of the dispersive fluxes
            for(int j=0; j<3; ++j)                      //loop through the ith elements neighbours
            {


                double D_hat=0.00;                      // dispersive flux across edge
                //AAAB - RE - OPTIMIZATION POSSIBILITY
                //FLUX ACROSS SOME(MAYBE MOST) EDGES BEING CALCULATED MORE THAN ONCE
                element *n = pData->vvpElemsNeighElems[i][j];    //the neighbour we are looking to
                if(n->Id==-2)                           //the neighbour doesn't exist. This edge is on the boundary
                {
                    D_hat=0.00;
                }
                else
                {

                    double dm_dn;                      // normal derivative dm/dn across edge
                    double dm_ds;                      // tangential derivative dm/ds along edge
                    double grad_m_x;                   // dm/dx component of gradient of m at the edge
                    double grad_m_y;                   // dm/dy component of gradient of m at the edge


                    // dm/dn is the difference of the centroid mass in the two neighbouring elements
                    // divided by the distance between the centroids (dist_en)


                    double dist_en=sqrt((e->centre.x()-n->centre.x())*(e->centre.x()-n->centre.x())+(e->centre.y()-n->centre.y())*(e->centre.y()-n->centre.y()));

                    dm_dn=(n->m_old_sp1-e->m_old_sp1)/dist_en;


                    //dm/ds is the difference of m at each node making up the edge divided by the length of the edge
                    //The m at each node is the weighted avg using area of all elements using that node


                    //AAAB - W - CALC MASS OF NODES AT EACH END OF EDGE
                    double totalArea = 0;
                    double totalMassArea = 0;
                    foreach(element* e, e->nodes[j]->vpNeighElements)
                    {
                        totalMassArea += (e->area)*(e->m_old_sp1);
                        totalArea += (e->area);
                    }
                    double m_node1 = totalMassArea/totalArea;

                    totalArea = 0;
                    totalMassArea = 0;
                    foreach(element* e, e->nodes[(j+1)%3]->vpNeighElements)
                    {
                        totalMassArea += (e->area)*(e->m_old_sp1);
                        totalArea += (e->area);
                    }
                    double m_node2 = totalMassArea/totalArea;

                    //if the gradient between the two nodes was very small - set it to zero. Feel free to change/remove
                    if(qAbs(m_node1-m_node2)<qPow(1, -4))
                    {
                        dm_ds=0.00;
                    }
                    else
                    {
                        dm_ds=(m_node1-m_node2)/(e->sides[j].length());
                    }

                    //Now we need to find the gradient dm/dx, dm/dy at the edge by projecting the
                    //normal and tangential derivatives

                    grad_m_x=dm_dn*(e->norms[j].normalized().x())+dm_ds*(e->sides[j].normalized().x());
                    grad_m_y=dm_dn*(e->norms[j].normalized().y())+dm_ds*(e->sides[j].normalized().y());


                    //To find D_hat, multiply the dm/dx, dm/dy by the diffusivity at the edge, dot with
                    //the norm to the edge, and multiply by the length of the side

                    D_hat=(e->sides[j].length())*(e->diffusivities[j+1])*((grad_m_x*e->norms[j].normalized().x())+(grad_m_y*e->norms[j].normalized().y()));
                }

                D_hat_total=D_hat_total+D_hat;


            }
            new_masses_species1[i]=e->m_old_sp1+(dt/e->area)*D_hat_total +dt*e->source_sp1;

            //correct for any -ve values
            if(new_masses_species1[i]<=0)
            {
                new_masses_species1[i]=0.00;
            }

        }

        setElementColour();

        for(int k=0; k<pData->vpElements.size(); ++k)
        {
            pData->vpElements[k]->m_old_sp1=new_masses_species1[k];
        }

        new_masses_species1.clear();

        foreach(element* e, pData->vpElements)
        {
            sceneToUpdate->removeItem(e);
            sceneToUpdate->update();
        }

        foreach(element* e, pData->vpElements)
        {
            sceneToUpdate->addItem(e);
            sceneToUpdate->update();
        }

        //AAAB - Make scene update. Makes program run faster to for some reason
        QCoreApplication::processEvents();

    }

}

//This function sets the colour for each of the elements in a mesh. Note each element has only one colour
//member variable. This could (and perhaps should) be changed to three different variables to accommodate
//three species.
void Simulation::setElementColour()
{
    if(!new_masses_species1.isEmpty())
    {
        //find the max and min masses for species 1
        findMaxAndMinMasses();

        QColor e_colour;
        int colourcode;

        //for each element in the mesh, determine its colour.
        for(int i=0; i<pData->vpElements.size(); ++i)
        {
            element *e=pData->vpElements[i];

            //if the max=min, then there is only one mass value. All elements are to be coloured
            // either white (if there is no mass), or red (otherwise).
            if(max_mass_sp1==min_mass_sp1)
            {
                if(new_masses_species1[i]<=0.00)
                {
                    e_colour=Qt::white;
                }
                else if(new_masses_species1[i]==max_mass_sp1)
                {
                    e_colour=Qt::red;
                }

            }
            //if max does not equal min, then if the mass value of the element is less than or equal to zero,
            //colour it white. If it is less than 0.05, colour it grey (deemed negligably small). If the
            //mass in the element is greater than 0.05, give it the appropriate HSV colour code. Just use
            //a linear map between the max and min mass values and 0-300 for the HSV colour codes.
            else
            {
                if(new_masses_species1[i]<=0.00)
                {
                    e_colour=Qt::white;
                }
                else if(new_masses_species1[i]<0.05)
                {
                    e_colour=Qt::gray;
                }
                else
                {
                    colourcode=(300.00/(min_mass_sp1-max_mass_sp1))*(new_masses_species1[i]-max_mass_sp1);
                    e_colour.setHsv(colourcode, 255, 255);
                }

            }
            e->elementColour=e_colour;

        }
    }
}

void Simulation::findMaxAndMinMasses()
{

    if(!new_masses_species1.isEmpty())          //if there are masses for species 1
    {
        max_mass_sp1=new_masses_species1[0];    //initially, assign the max mass variable the value of the first stored mass for the mesh
        min_mass_sp1=0.0;                       //initially, assign the min mass variable the value of 0

        //loop through all the mass values. Find the max.
        foreach(double m, new_masses_species1)
        {
            if(m>max_mass_sp1)
            {
                max_mass_sp1=m;
            }
        }

        //if the max mass value is some how less than zero, set it to zero.
        //the min mass value will remain zero (as initialized)
        if(max_mass_sp1<=0.00)
        {
            max_mass_sp1=0.00;
        }

        //If the max value is not zero, search through all the mass values for the min.
        //Note that here I picked that any mass value lower than 0.05 would be negliagable.
        //This is because you can have very, very, very small mass values and I'd rather lump
        //such small numbers together so that they don't conribute to setting up the colour bar.
        //Feel free to change this number. It appears once more in the set_element_colour function.
        else
        {
            min_mass_sp1=max_mass_sp1;
            foreach(double m, new_masses_species1)
            {
                if(m<min_mass_sp1 && m>0.05)
                {
                    min_mass_sp1=m;
                }
            }
        }

    }

}
