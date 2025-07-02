#include "Plane.h"

Mode::Number ModeLevelFlight::mode_number() const { return Mode::Number::LEVEL_FLIGHT; }

const char *ModeLevelFlight::name() const { return "LEVEL_FLIGHT"; }

const char *ModeLevelFlight::name4() const { return "LVFL"; }

void ModeLevelFlight::update() {
    plane.level_flight_stabilize();
}

void ModeLevelFlight::run() {
    update();
    output_pilot_throttle();  // inherited from Mode
}

bool ModeLevelFlight::_enter() {
    gcs().send_text(MAV_SEVERITY_INFO, "LEVEL_FLIGHT");
    return true;
}

bool ModeLevelFlight::use_throttle_limits() const { return false; }

bool ModeLevelFlight::use_battery_compensation() const { return false; }
