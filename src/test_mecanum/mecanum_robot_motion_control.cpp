#include <Arduino.h>
#include "util.h"
#include "mecanum_robot_drive.h"
#include "EncoderVelocity.h"
#include "wireless.h"
#include "mecanum_robot_motion_control.h"
#include "mecanum_robot_pinout.h"
#include "imu.h"
#include "EulerAngles.h"

// #define UTURN
// #define CIRCLE
#define JOYSTICK
// #define YOUR_TRAJECTORY

extern RobotMessage robotMessage;
extern ControllerMessage controllerMessage;
IMU imu(BNO08X_RESET, BNO08X_CS, BNO08X_INT);

int state = 0;
double robotVelocity = 0; // velocity of robot, in m/s
double k = 0; // k is 1/radius from center of rotation circle

extern EncoderVelocity encoders[NUM_MOTORS];
double currPhiL = 0;
double currPhiR = 0;
double prevPhiL = 0;
double prevPhiR = 0;

void setupIMU() {
    imu.setup();
}

// Sets the desired wheel velocities based on desired robot velocity in m/s
// and k curvature in 1/m representing 1/(radius of curvature)
void setWheelVelocities(float robotVelocity, float k){
    double left = (robotVelocity - k*WHEEL_B*robotVelocity)/WHEEL_R;
    double right = 2*robotVelocity/WHEEL_R  - left;
    updateSetpoints(left, right, 0.0);
}

// Makes robot follow a trajectory
void followTrajectory() {

    #ifdef JOYSTICK
    if (freshWirelessData) {
        // Serial.printf("Joystick1: %.2f, %.2f, Joystick2: %.2f, %.2f, BL: %u, BR:%u\n",
        //                 controllerMessage.joystick1.x, controllerMessage.joystick1.y,
        //                 controllerMessage.joystick2.x, controllerMessage.joystick2.y,
        //                 controllerMessage.buttonL, controllerMessage.buttonR);
        double forward = abs(controllerMessage.joystick1.y) < 0.1 ? 0 : mapDouble(abs(controllerMessage.joystick1.y), 0.1, 1, 0, MAX_FORWARD);
        forward = controllerMessage.joystick1.y > 0 ? forward : -forward;
        double sideways = abs(controllerMessage.joystick1.x) < 0.1 ? 0 : -mapDouble(abs(controllerMessage.joystick1.x), 0.1, 1, 0, MAX_FORWARD);
        sideways = controllerMessage.joystick1.x > 0 ? sideways : -sideways;
        double rotation = abs(controllerMessage.joystick2.x) < 0.1 ? 0 : -mapDouble(abs(controllerMessage.joystick2.x), 0.1, 1, 0, MAX_ROTATE);
        rotation = controllerMessage.joystick2.x > 0 ? rotation : -rotation;
        // Serial.printf("forward (x): %.2f, sideways (y): %.2f, rotation (theta): %.2f\n", forward, sideways, rotation);
        updateSetpoints(forward, sideways, rotation);
    }
    #endif 

    #ifdef CIRCLE
    robotVelocity = 0.2;
    k = 1/0.5;
    setWheelVelocities(robotVelocity, k);
    #endif 

    #ifdef UTURN
    switch (state) {
        case 0: 
            // Until robot has achieved a x translation of 1m
            if (robotMessage.x <= 1.0) {
                // Move in a straight line forward
                robotVelocity = 0.2;
                k = 0;
            } else {
                // Move on to next state
                state++;
            }
            break;

        case 1:
            // Until robot has achieved a 180 deg turn in theta
            if (robotMessage.theta <= M_PI) {
                // Turn in a circle with radius 25cm 
                robotVelocity = 0.2;
                k = 1/0.25;
            } else {
                state++;
            }
            break;

        case 2:
            // Until robot has achieved a x translation of -1m
            if (robotMessage.x >= 0) {
                // Move in a straight line forward
                robotVelocity = 0.2;
                k = 0;
            } else {
                // Move on to next state
                state++;
            }
            break;

        default: 
            // If none of the states, robot should just stop
            robotVelocity = 0;
            k = 0;
            break;
    }
    setWheelVelocities(robotVelocity, k);
    #endif

    #ifdef YOUR_TRAJECTORY
    // TODO: Create a state machine to define your custom trajectory!



    setWheelVelocities(robotVelocity, k);
    #endif 

}

void updateOdometry() {
    // take angles from traction wheels only since they don't slip
    // currPhiL = encoders[2].getPosition();
    // currPhiR = -encoders[3].getPosition();
    
    // double dPhiL = currPhiL - prevPhiL;
    // double dPhiR = currPhiR - prevPhiR;
    // prevPhiL = currPhiL;
    // prevPhiR = currPhiR;

    // float dtheta = r/(2*b)*(dPhiR-dPhiL);
    // float dx = r/2.0 * (cos(robotMessage.theta)*dPhiR + cos(robotMessage.theta)*dPhiL);
    // float dy = r/2.0 * (sin(robotMessage.theta)*dPhiR + sin(robotMessage.theta)*dPhiL);

    imu.update();
    EulerAngles euler_angles = imu.getEulerAngles();

    // Update robot message 
    robotMessage.millis = millis();
    robotMessage.x = 0.0;
    robotMessage.y = 0.0;
    robotMessage.theta = 0.0;
}

