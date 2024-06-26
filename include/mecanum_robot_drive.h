#ifndef MECANUM_ROBOT_DRIVE_H
#define MECANUM_ROBOT_DRIVE_H

#define NUM_MOTORS 4

#define Kp 0.25
#define Ki 0.01
#define Kd 0.01
#define pidTau 0.1

#define M_ALPHA 0.1

#define MAX_FORWARD 20
#define MAX_ROTATE 30

void setupDrive();
void updateSetpoints(double forward, double sideways, double rotation);
void updatePIDs();

#endif // MECANUM_ROBOT_DRIVE_H