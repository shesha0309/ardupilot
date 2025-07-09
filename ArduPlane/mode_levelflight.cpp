#include "Plane.h"

Mode::Number ModeLevelFlight::mode_number() const { 
    return Mode::Number::LEVEL_FLIGHT; 
}

const char *ModeLevelFlight::name() const { 
    return "LEVEL_FLIGHT"; 
}

const char *ModeLevelFlight::name4() const { 
    return "LVFL"; 
}

void ModeLevelFlight::update() {
    // Simple right turn logic
    switch (turn_state) {
        case TurnState::TURNING_RIGHT: {
            // Check if we've completed the 90-degree right turn
            float heading_error = wrap_180(target_heading_deg - ahrs.yaw_sensor * 0.01f);
            
            if (fabsf(heading_error) < 5.0f) {  // Within 5 degrees of target
                gcs().send_text(MAV_SEVERITY_INFO, "LEVEL_FLIGHT: Turn complete, entering level flight");
                turn_state = TurnState::LEVEL_FLIGHT;
                plane.nav_roll_cd = 0;  // Level the wings
            } else {
                // Continue turning right
                plane.nav_roll_cd = 3000;  // 30 degrees right bank
            }
            break;
        }
            
        case TurnState::LEVEL_FLIGHT: {
            // Maintain level flight
            plane.nav_roll_cd = 0;   // Keep wings level
            plane.nav_pitch_cd = 0;  // Keep nose level
            break;
        }
    }
    
    // Always maintain altitude control
    plane.calc_throttle();
}

void ModeLevelFlight::run() {
    // Call our update method
    update();
    
    // Use standard stabilization for roll and pitch
    plane.stabilize_roll();
    plane.stabilize_pitch();
    
    // NO yaw stabilization - this is key for Level_Flight mode
    // plane.stabilize_yaw();  // Commented out intentionally
}

void ModeLevelFlight::navigate() {
    // No complex navigation needed for this simple mode
    // Just maintain basic flight parameters
    plane.calc_nav_pitch();
}

bool ModeLevelFlight::_enter() {
    gcs().send_text(MAV_SEVERITY_INFO, "LEVEL_FLIGHT: Starting right turn maneuver");
    
    // **ALTITUDE HOLD**: Capture current altitude as target
    plane.set_target_altitude_current();
    
    // Calculate target heading (90 degrees to the right of current heading)
    float current_heading_deg = ahrs.yaw_sensor * 0.01f;  // Convert from centidegrees
    target_heading_deg = current_heading_deg + 90.0f;
    
    // Normalize to 0-360 range
    while (target_heading_deg >= 360.0f) target_heading_deg -= 360.0f;
    while (target_heading_deg < 0.0f) target_heading_deg += 360.0f;
    
    // Start in turning state
    turn_state = TurnState::TURNING_RIGHT;
    
    // Set initial navigation parameters
    plane.prev_WP_loc = plane.current_loc;
    plane.next_WP_loc = plane.current_loc;  // Not using waypoint navigation
    
    gcs().send_text(MAV_SEVERITY_INFO, "LEVEL_FLIGHT: Current heading %.1f°, target %.1f°", 
                    current_heading_deg, target_heading_deg);
    
    return true;
}