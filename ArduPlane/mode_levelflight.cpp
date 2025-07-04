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
    _start_time_ms = AP_HAL::millis();
    _phase2_started = false;

    return true;
}

void ModeLevelFlight::update()
{
    uint32_t now = AP_HAL::millis();
    uint32_t elapsed = now - _start_time_ms;

    // PHASE 1: Straight flight for 5 seconds
    if (elapsed < 5000) {
        plane.level_flight_stabilize(); // pitch & roll stabilization
    }

    // PHASE 2: Right turn for next 5 seconds (L-shape)
    else if (elapsed < 10000) {
        if (!_phase2_started) {
            gcs().send_text(MAV_SEVERITY_INFO, "LEVEL_FLIGHT: Turning Right (L-shape)");
            _phase2_started = true;
        }
        plane.set_nav_roll_cd(2000); // Apply 20 deg roll
        plane.level_flight_stabilize();
    }

    // PHASE 3: Resume level flight
    else {
        plane.level_flight_stabilize();
    }

    // Altitude Hold Logic
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
