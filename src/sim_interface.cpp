#include "sim_interface.h"

#include "4xSimulation/include/terminal.h"
#include "4xSimulation/include/simulation.h"

void sim_interface::start() {
  simulation::start();
  terminal::initialize();
}

void sim_interface::kill() {
  simulation::kill();
  terminal::kill();
}

