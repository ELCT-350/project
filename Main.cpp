
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
	const double h = .005;
	const double tmax = 20;
	const double f = 1000;
	const double R = 0.5;
	const double C = 1e-3;
	const double soc0 = 0.9;
	double soc;

	Plotter plotter("Project", 1000, 600);
	plotter.SetLabels("vbat", "Terminal Current (A)", "SOC");

	Simulator simulator(1, 0);

	

	Resistor R1(1, 0, R);
	Battery B1(1, 0, 8.1, soc0, h);
	soc = soc0;

	simulator.AddDevice(B1);
	simulator.AddDevice(R1);
	simulator.Init(h, tmax);

	while (simulator.IsRunning())
	{
		plotter.AddRow(simulator.GetTime(), B1.GetTerminalVoltage(),
			B1.GetTerminalCurrent(), B1.soc);
		simulator.Step();
	}

	plotter.Plot();

	return 0;
}