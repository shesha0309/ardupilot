#include "Plane.h"
#include "AP_Math/AP_Math.h"
#ifndef constrain
#define constrain(amt, low, high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#endif
#include <AC_AttitudeControl/AC_AttitudeControl_Multi.h>

extern AC_AttitudeControl_Multi *attitude_control;

Mode::Number ModeLevelFlight::mode_number() const { return Mode::Number::LEVEL_FLIGHT; }

const char *ModeLevelFlight::name() const { return "LEVEL_FLIGHT"; }

const char *ModeLevelFlight::name4() const { return "LVFL"; }

bool ModeLevelFlight::_enter() {
    gcs().send_text(MAV_SEVERITY_INFO, "LEVEL_FLIGHT");

    _target_altitude_cm = plane.get_rel_altitude_cm();

    return true;
}

void ModeLevelFlight::update()
{
    // Maintain pitch and roll using existing Plane attitude controllers
    plane.level_flight_stabilize();

    // Maintain altitude using a simple P controller
    int32_t current_alt_cm = plane.get_rel_altitude_cm();
    int32_t error_cm = _target_altitude_cm - current_alt_cm;

    float kP = 0.003f;
    float base_throttle = 0.5f;
    float throttle_correction = kP * error_cm;

    float desired_throttle = constrain(base_throttle + throttle_correction, 0.0f, 1.0f);

    attitude_control->set_throttle_out(desired_throttle, true, false);
}


void ModeLevelFlight::run() {
    update();
}

bool ModeLevelFlight::use_throttle_limits() const { return false; }

bool ModeLevelFlight::use_battery_compensation() const { return false; }
