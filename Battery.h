#include "Simulator.h"
#include "Plotter.h"

#include "VoltageSource.h"
#include "Diode.h"
#include "Resistor.h"
#include "Capacitor.h"
class Battery: public Device
{
public:

    // Constructor:

    Battery(int positive, int negative, double wh, double soc0);

    // Device interface:
    void Init();
    void Step(double t, double dt);
    

    // Viewable functions:
    double GetSOC();
    double GetTerminalVoltage();
    double GetTerminalCurrent();


    // Member variables:

    int positive;
    int negative;
    double wh;
    double soc0;

private:

    double soc;
    double RIN;
    double RT1;
    double RT2;
    double CT1;
    double CT2;
};

Battery::Battery(int positive, int negative, double wh, double soc0, double h)
{
    this->postive = positive;
    this->negative = negative;
    this->wh = wh;
    this->soc0 = soc0;
    this->h = h;
}

void Battery::Init()
{
    // model open circuit with 0 conductance:
    soc = soc0;
    AddJacobian(nodei, nodei, 0.0);
    AddJacobian(nodei, nodej, 0.0);
    AddJacobian(nodej, nodei, 0.0);
    AddJacobian(nodej, nodej, 0.0);
}

void Battery::Step(double t, double dt)
{   
    soc = GetSOC(soc, wh, dt);
    RIN = GetRIN(soc);
    RT1 = GetRT1(soc);
    RT2 = GetRT1(soc);
    CT1 = GetRT1(soc);
    CT2 = GetRT1(soc);

    double g = C / dt;
    double b = g * GetStateDifference(nodei, nodej);  // g * v(t)

    AddJacobian(nodei, nodei, g);
    AddJacobian(nodei, nodej, -g);
    AddJacobian(nodej, nodei, -g);
    AddJacobian(nodej, nodej, g);

    AddBEquivalent(nodei, b);
    AddBEquivalent(nodej, -b);
}


double Battery::GetTerminalVoltage()
{
    return GetStateDifference(nodei, nodej);
}

double Battery::GetTerminalCurrent()
{
    // current = g * v - b:
    return C / GetTimeStep() * GetVoltage() - GetBEquivalent(nodei);
}

double Battery::GetSOC(double socOld, double wH, double h)
{
    return socOld + (GetVoltage() * GetCurrent()*h)/(wH*3600);
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