// Simplified Battery Model

#include "Simulator.h"

class Battery : public Device

{

    /*

               (int1)   R(soc)

             vi  o------VVV---o (nodepos)  vpos

                +|

       Vin(soc) ( ) o (int2) ibatt

                 |

                 '------------o (nodeneg)  vneg

        output vector:



           quantities:   indices:

            [ vpos  ]    nodepos

        x = [ vneg  ]    nodeneg

            [ vi    ]    int1

            [ ibatt ]    int2

    */



public:

    // Constructor (external nodes and params):

    Battery(int nodepos, int nodeneg, double soci);

    // Device interface:

    void Step(double t, double dt);

    void Init();

    // Viewable functions:

    double GetVoltage();

    double GetCurrent();

    double GetSOC();

    // f(soc) functions:

    double GetVin(double soc);

    double GetRin(double soc);

    // Member variables:

    int nodei;

    int nodej;

    int int1;

    int int3;

    const double C1 = 1e-6;

    const double C2 = 1e-6;

    const double C3 = 1e-6;

    const double h = 1e-2;

    double soci;

    double soc;  // state of charge

};

Battery::Battery(int nodepos, int nodeneg, double soci)

{

    this->nodei = nodepos;

    this->nodej = nodeneg;

    this->soci = soci;

}

void Battery::Init()

{

    int1 = GetNextNode();

    int3 = GetNextNode();

    soc = soci;

}

void Battery::Step(double t, double h)

{

    double Vin = GetVin(soc);

    double gin = 1 / GetRin(soc);

    double g1 = 1 / GetRt1(soc);

    double g2 = 1 / GetRt2(soc);

    double wh = 8.1;

    // Rin:

    AddJacobian(0, 0, gin);

    AddJacobian(0, 1, -gin);

    AddJacobian(1, 0, -gin);

    AddJacobian(1, 1, gin);

    //Vin

    AddJacobian(2, 0, 1);

    AddJacobian(0, 2, 1);

    AddBEquivalent(2, Vin);



    //R2||C1

    AddJacobian(1, 1, g1 + (C1 / h));

    AddJacobian(1, 3, -g1 - (C1 / h));

    AddJacobian(3, 1, -g1 - (C1 / h));

    AddJacobian(3, 3, g1 + (C1 / h));

    AddBEquivalent(1, (C1 / h) * int1);

    AddBEquivalent(3, (C1 / h) * int1);

    //R3||C2

    AddJacobian(3, 3, g2 + (C2 / h));

    AddJacobian(3, 4, -g2 - (C2 / h));

    AddJacobian(4, 3, -g2 - (C2 / h));

    AddJacobian(4, 4, g2 + (C2 / h));

    AddBEquivalent(3, (C2 / h) * int3);

    AddBEquivalent(4, (-C2 / h) * int3);

    //

    // update soc:

    soc = soc + GetVoltage() * GetCurrent() * h / (wh * 3600);

}

double Battery::GetVoltage()

{

    return GetStateDifference(4, 2);

}

double Battery::GetCurrent()

{

    return GetState(int3);

}

double Battery::GetSOC()

{

    return soc;

}

double Battery::GetVin(double soc)

{

    return 3.8 * soc;  // simple linear model

}

double Battery::GetRin(double soc)

{
    return 0.1 + (1 - soc) * 0.01;  // simple linear model
}
