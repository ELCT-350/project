
#include "Simulator.h"
#include "Plotter.h"
#include "VoltageSource.h"
#include "Diode.h"
#include "Resistor.h"
#include "Capacitor.h"
#include "SensorCircuit.h"

/*

  Example circuit (rectifier):

     (1)   D1      (2)
       .--->|---.-------.
      +|        |       |     +
  vin (~)    R1 <   C1 ===   vout
       |        |       |     -
       '--------+-------'
               -+- (0)
                '

  time step = 1e-5 s
  simulation time = 5 ms
  
  vin = 10 * sin(2*pi*1000*t)
  R1 = 10 Ohm
  C1 = 1.0 mF

*/

int main()
{
	const double h = 1e-6;
	const double tmax = 5e-3;
	const double Va = 10;
	const double f = 1000;
	const double Rin = 10;
	const double Rt1 = 10;
	const double Rt2 = 10;
	const double Ct1 = 1e-3;
	const double Ct2 = 1e-3;

	Plotter plotter("Project", 1000, 600);
	plotter.SetLabels("vin (V)", "iR (A)", "vout (V)");

	Simulator simulator(4, 0);

	VoltageSource V1(1, 0, 0, Va, f);
	Resistor R1(0, 1, Rin);
	Resistor R2(1, 3, Rt1);
	Resistor R3(3, 4, Rt2);
	Capacitor C1(1, 3, Ct1);
	Capacitor C2(3, 4, Ct2);


	simulator.AddDevice(V1);
	simulator.AddDevice(R1);
	simulator.AddDevice(R2);
	simulator.AddDevice(R3);
	simulator.AddDevice(C1);
	simulator.AddDevice(C2);

	simulator.Init(h, tmax);
	while (simulator.IsRunning())
	{
		plotter.AddRow(simulator.GetTime(), V1.GetVoltage(),
		R1.GetCurrent(),
		R2.GetCurrent(), C1.GetVoltage(),
		R3.GetCurrent(), C2.GetVoltage());
		
		simulator.Step();
	}

	plotter.Plot();

	return 0;
}