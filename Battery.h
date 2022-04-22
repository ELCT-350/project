#include "Simulator.h"
#include "Plotter.h"

#include "VoltageSource.h"
#include "Diode.h"
#include "Resistor.h"
#include "Capacitor.h"
class Battery : public Device
{
public:

    // Constructor:

    Battery(int positive, int negative, double wh, double soc0, double h);

    // Device interface:
    void Init();
    void Step(double t, double dt);


    // Viewable functions:
    double GetSOC(double socOld, double wh, double h);
    double GetTerminalVoltage();
    double GetTerminalCurrent();


    // Member variables:

    int positive;   //also int4
    int negative;
    double wh;
    double soc0;
    double h;

private:

    int int1;
    int int2;
    int int3;
    int int4;
    int int5;
    double soc;
    double RIN;
    double RT1;
    double RT2;
    double CT1;
    double CT2;

    //private functions
    double GetSOC(double socOld, double wH, double h);
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
        int4 = GetNextNode();
        int5 = GetNextNode();
    }

    void Battery::Step(double t, double h)
    {

        int4 = positive;

        soc = GetSOC(soc, wh, h);
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
        double b2 = g5 * GetStateDifference(int3, int4);  // g * v(t)
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
        //RT1
        AddJacobian(int3, int3, g3);
        AddJacobian(int3, int4, -g3);
        AddJacobian(int4, int3, -g3);
        AddJacobian(int4, int4, g3);
        //CT1
        AddJacobian(int2, int2, g4);
        AddJacobian(int2, int3, -g4);
        AddJacobian(int3, int2, -g4);
        AddJacobian(int3, int3, g4);
        //CT2
        AddJacobian(int3, int3, g5);
        AddJacobian(int3, int4, -g5);
        AddJacobian(int4, int3, -g5);
        AddJacobian(int4, int4, g5);
        //Vin
        AddJacobian(int1, int1, 0);
        AddJacobian(int1, int5, 1);
        AddJacobian(int5, int1, 1);
        AddJacobian(int5, int5, 0);

        AddBEquivalent(int1, 0);
        AddBEquivalent(int2, b1);
        AddBEquivalent(int3, b3);
        AddBEquivalent(int4, -b2);
        AddBEquivalent(int5, GetVIN(soc));
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

    double Battery::GetSOC(double socOld, double wH, double h)
    {
        return socOld + (GetTerminalVoltage() * GetTerminalCurrent() * h) / (wH * 3600);
    }

    double Battery::GetValue(double soc, double A, double k, double a0, double a1, double a2, double a3)
    {
        return A * exp(k * soc) + a0 + a1 * soc + a2 * (soc) ^ 2 + a3 * (soc) ^ 3;
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
