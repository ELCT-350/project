
#include "Simulator.h"
#include "Plotter.h"
#include "Resistor.h"
#include "Capacitor.h"
#include "Battery.h"

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
	const double R = 10;
	const double C = 1e-3;
	const double soc0 = 0.9;
	double soc;

	Plotter plotter("Project", 1000, 600);
	plotter.SetLabels("vin (V)", "iR (A)", "vout (V)");

	Simulator simulator(1, 0);

	

	Resistor R1(1, 0, R);
	Capacitor C1(1, 0, C);
	Battery B1(1, 0, 8.1, soc0, h);
	soc = soc0;

	simulator.AddDevice(B1);
	simulator.AddDevice(R1);
	simulator.AddDevice(C1);
	B1.Init();
	simulator.Init(h, tmax);

	while (simulator.IsRunning())
	{
		plotter.AddRow(simulator.GetTime(), B1.GetTerminalVoltage(),
			B1.GetTerminalCurrent(), B1.GetSOC(soc));
		simulator.Step();
		soc = B1.GetSOC(soc);
	}

	plotter.Plot();

	return 0;
}