#include "Simulator.h"
#include "Plotter.h"
#include <math.h>

class Battery : public Device
{
public:

    // Constructor:

    Battery(int positive, int negative, double wh, double soc0, double h);

    // Device interface:
    void Init();
    void Step(double t, double dt);


    // Viewable functions:
    void UpdateSOC();
    double GetTerminalVoltage();
    double GetTerminalCurrent();


    // Member variables:

    int positive;   //also positive
    int negative;   //also negative
    double wh;
    double soc0;
    double h;
    double soc;

private:

    int int1;
    int int2;
    int int3;
    int int5;
  
    
    double RIN;
    double RT1;
    double RT2;
    double CT1;
    double CT2;

    //private functions
    
    double GetValue(double soc, double A, double k, double a0, double a1, double a2, double a3);
    double GetVIN(double soc);
    double GetRIN(double soc);
    double GetRT1(double soc);
    double GetRT2(double soc);
    double GetCT1(double soc);
    double GetCT2(double soc);
};

    Battery::Battery(int positive, int negative, double wh, double soc0, double h)
    {
        this->positive = positive;
        this->negative = negative;
        this->wh = wh;
        this->soc0 = soc0;
        this->h = h;
    }

    void Battery::Init()
    {
        // model open circuit with 0 conductance:
        soc = soc0;
        int1 = GetNextNode();
        int2 = GetNextNode();
        int3 = GetNextNode();
        //positive = GetNextNode();
        int5 = GetNextNode();
        //negative = GetNextNode();
    }

    void Battery::Step(double t, double h)
    {

        //positive = positive;
        //negative = negative;

        UpdateSOC();
        RIN = GetRIN(soc);
        RT1 = GetRT1(soc);
        RT2 = GetRT1(soc);
        CT1 = GetRT1(soc);
        CT2 = GetRT1(soc);

        double g1 = 1 / RIN;
        double g2 = 1 / RT1;
        double g3 = 1 / RT2;
        double g4 = CT1 / h;
        double g5 = CT2 / h;
        double b1 = g4 * GetStateDifference(int2, int3);  // g * v(t)
        double b2 = g5 * GetStateDifference(int3, positive);  // g * v(t)
        double b3 = -b1 + b2;

        //RIN
        AddJacobian(int1, int1, g1);
        AddJacobian(int1, int2, -g1);
        AddJacobian(int2, int1, -g1);
        AddJacobian(int2, int2, g1);
        //RT1
        AddJacobian(int2, int2, g2);
        AddJacobian(int2, int3, -g2);
        AddJacobian(int3, int2, -g2);
        AddJacobian(int3, int3, g2);
        //RT2
        AddJacobian(int3, int3, g3);
        AddJacobian(int3, positive, -g3);
        AddJacobian(positive, int3, -g3);
        AddJacobian(positive, positive, g3);
        //CT1
        AddJacobian(int2, int2, g4);
        AddJacobian(int2, int3, -g4);
        AddJacobian(int3, int2, -g4);
        AddJacobian(int3, int3, g4);
        //CT2
        AddJacobian(int3, int3, g5);
        AddJacobian(int3, positive, -g5);
        AddJacobian(positive, int3, -g5);
        AddJacobian(positive, positive, g5);
        //Vin
        AddJacobian(int1, int1, 0);
        AddJacobian(int1, int5, 1);
        AddJacobian(int5, int1, 1);
        AddJacobian(int5, int5, 0);

        AddJacobian(int5, negative, -1);
        AddJacobian(negative, int5, -1);


        AddBEquivalent(int1, 0);
        AddBEquivalent(int2, b1);
        AddBEquivalent(int3, b3);
        AddBEquivalent(positive, -b2);
        AddBEquivalent(int5, GetVIN(soc));
        AddBEquivalent(negative, 0);
    }


    double Battery::GetTerminalVoltage()
    {
        return GetStateDifference(positive, negative);
    }

    double Battery::GetTerminalCurrent()
    {
        // current = g * v - b:
        return int5;
    }

    void Battery::UpdateSOC()
    {
        soc += (GetTerminalVoltage() * GetTerminalCurrent() * h) / (wh * 3600);
    }

    double Battery::GetValue(double soc, double A, double k, double a0, double a1, double a2, double a3)
    {
        return A * exp(k * soc) + a0 + a1 * soc + a2 * pow(soc,2.0) + a3 * pow(soc,3.0);
    }

    double Battery::GetVIN(double soc)
    {
        return GetValue(soc, -1.031, -35.0, 3.685, 0.2156, -0.1178, 0.3201);
    }

    double Battery::GetRIN(double soc)
    {
        return GetValue(soc, 0.1562, -24.37, 0.07446, 0, 0, 0);
    }

    double Battery::GetRT1(double soc)
    {
        return GetValue(soc, 0.3208, -29.14, 0.04669, 0, 0, 0);
    }

    double Battery::GetRT2(double soc)
    {
        return GetValue(soc, 6.603, -155.2, 0.04984, 0, 0, 0);
    }

    double Battery::GetCT1(double soc)
    {
        return GetValue(soc, -752.9, -13.51, 703.6, 0, 0, 0);
    }

    double Battery::GetCT2(double soc)
    {
        return GetValue(soc, -6056.0, -27.12, 4475.0, 0, 0, 0);
    }
