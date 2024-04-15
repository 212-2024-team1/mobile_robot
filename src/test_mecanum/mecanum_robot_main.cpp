#include <Arduino.h>
#include "mecanum_robot_drive.h"
#include "wireless.h"
#include "util.h"
#include "mecanum_robot_motion_control.h"

void setup() {
    Serial.begin(115200);
    setupDrive();
    setupWireless();
    setupIMU();
}

void loop() {
    // Update velocity setpoints based on trajectory at 50Hz
    EVERY_N_MILLIS(20) {
        followTrajectory();
    }

    // Update PID at 200Hz
    EVERY_N_MILLIS(5) {
        updatePIDs();
    }

    // Send and print robot values at 20Hz
    EVERY_N_MILLIS(50) {
        updateOdometry();
        sendRobotData();

        // Serial.printf("x: %.2f, y: %.2f, theta: %.2f\n",
                    // robotMessage.x, robotMessage.y, robotMessage.theta);
    }
  
}