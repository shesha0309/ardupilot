#include "Plane.h"

Mode::Number ModeLevelFlight::mode_number() const { return Mode::Number::LEVEL_FLIGHT; }

const char *ModeLevelFlight::name() const { return "LEVEL_FLIGHT"; }

const char *ModeLevelFlight::name4() const { return "LVFL"; }

void ModeLevelFlight::update() {
    // Set targets for level attitude (0 degrees roll and pitch)

    plane.nav_pitch_cd = 0;
    
    // Allow pilot input to override level attitude when stick moved
    // This will mix pilot input with our level targets
    plane.stabilize_stick_mixing_fbw();
    
    // Altitude hold is handled automatically by TECS when does_auto_throttle() = true
    // Target altitude was set in _enter() method

    plane.nav_roll_cd  = plane.roll_limit_cd / 3;
    plane.update_load_factor();
    plane.calc_nav_pitch();
    plane.calc_throttle();
}

void ModeLevelFlight::run() {
    // Call our update method
    update();
    
    // Use standard stabilization for roll and pitch (with level targets)
    plane.stabilize_roll();
    plane.stabilize_pitch();
    
    // NO yaw stabilization - this is key for Level_Flight mode
    // plane.stabilize_yaw();  // Commented out intentionally
    
    // Throttle is handled by TECS for altitude hold
    plane.calc_throttle();
}

bool ModeLevelFlight::_enter() {
    gcs().send_text(MAV_SEVERITY_INFO, "LEVEL_FLIGHT with altitude hold");
    
    // **ALTITUDE HOLD**: Capture current altitude as target
    plane.set_target_altitude_current();
        // the altitude to circle at is taken from the current altitude
    plane.next_WP_loc = plane.current_loc;

    return true;
}