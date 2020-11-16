/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright (c) 2016-2017 NXP
 * Copyright (c) 2020 Bjarne Hansen
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*! \file driver_sensors.h
    \brief Provides function prototypes for driver level interfaces
    It does not have a corresponding .c or .cpp file. Rather, it declares
    functions contained in hardware-specific files.
   
    To use, for example, the NXP FXOS8700, include the driver_FXOS8700.c source
    file in the build and ensure its Init and Read methods for the sensor are 
    declared here.

*/

#ifndef DRIVER_SENSORS_H
#define DRIVER_SENSORS_H

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations of structures (redefined in sensor_fusion.h, but needed now for pointer definitions)
typedef struct SensorFusionGlobals SensorFusionGlobals;
typedef struct PhysicalSensor PhysicalSensor;

/// @name Sensor Interface Prototypes
/// Each physical sensor must be provided with one initialization function
/// and one "read" function.  These must be installed by the user using the
/// installSensor method defined in SensorFusionGlobals.  By "physical sensor",
/// we mean either individual sensor type (such as a 3-axis accelerometer) or
/// a combo-sensor such as the NXP FXOS8700 6-axis accel plus mag.  The init()
/// function for each sensor is responsible for initializing all sensors contained
/// in that package.  The read() function is responsible for reading those same
/// sensors and moving the results into the standard structures contained within
/// the SensorFusionGlobals object.
int8_t FXOS8700_Init(PhysicalSensor *sensor, SensorFusionGlobals *sfg);
int8_t FXAS21002_Init(PhysicalSensor *sensor, SensorFusionGlobals *sfg);

int8_t FXOS8700_Read(PhysicalSensor *sensor, SensorFusionGlobals *sfg);
int8_t FXAS21002_Read(PhysicalSensor *sensor, SensorFusionGlobals *sfg);

int8_t FXOS8700_Idle(PhysicalSensor *sensor, SensorFusionGlobals *sfg);
int8_t FXAS21002_Idle(PhysicalSensor *sensor, SensorFusionGlobals *sfg);

#ifdef __cplusplus
}
#endif

#endif // DRIVER_SENSORS_H
