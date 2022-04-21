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
};

Battery::Battery(int positive, int negative, double wh, double soc0)
{
    this->postive = positive;
    this->negative = negative;
    this->wh = wh;
    this->soc0 = soc0;
}

void Battery::Init()
{
    // model open circuit with 0 conductance:

    AddJacobian(nodei, nodei, 0.0);
    AddJacobian(nodei, nodej, 0.0);
    AddJacobian(nodej, nodei, 0.0);
    AddJacobian(nodej, nodej, 0.0);
}

void Battery::Step(double t, double dt)
{
    double g = C / dt;
    double b = g * GetStateDifference(nodei, nodej);  // g * v(t)

    AddJacobian(nodei, nodei, g);
    AddJacobian(nodei, nodej, -g);
    AddJacobian(nodej, nodei, -g);
    AddJacobian(nodej, nodej, g);

    AddBEquivalent(nodei, b);
    AddBEquivalent(nodej, -b);
}

double Capacitor::GetVoltage()
{
    return GetStateDifference(nodei, nodej);
}

double Capacitor::GetCurrent()
{
    // current = g * v - b:
    return C / GetTimeStep() * GetVoltage() - GetBEquivalent(nodei);
}

double Capacitor::GetPower()
{
    return GetVoltage() * GetCurrent();
}
