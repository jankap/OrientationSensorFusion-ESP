#include <Arduino.h>
#include <Wire.h>

#include <sstream>
#include <string>

// Sensor Fusion Headers
#include "sensor_fusion.h"      // top level magCal and sensor fusion interfaces
#include "control.h"  	        // Command processing and data Streaming interface
#include "status.h"   	        // Status indicator interface - application specific
#include "drivers.h"  	        // NXP sensor drivers OR customer-supplied drivers

// hardware-specific settings. Edit as needed for whatever board & sensors used.
#include "board.h"
#include "sensor_io_i2c_sensesp.h"  //I2C interfaces for ESP platform

#include "debug_print.h"  // provides ability to output debug messages via serial
 
// Sensor Fusion Global data structures
SensorFusionGlobals sfg;                ///< This is the primary sensor fusion data structure
struct ControlSubsystem controlSubsystem;      ///< used for serial communications
struct StatusSubsystem statusSubsystem;        ///< provides visual (usually LED) status indicator
struct PhysicalSensor sensors[3];              ///< This implementation uses up to 3 sensors

void setup() {
  // put your setup code here, to run once:
// init rs port
    Serial.begin(BOARD_DEBUG_UART_BAUDRATE); //initialize serial UART
    delay(200);

   // init wifi connection
/*   if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
     debug_log("Failed to configure network settings");
    }
   WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        debug_log("connecting to WiFi network");
        delay(500);
    }
   debug_log("connected to WiFi");
   debug_log("IP adddr: ");
   Serial.println(WiFi.localIP());
*/
   debug_log("waitasec...");  //delay not really necessary - gives me time to open a serial monitor
   delay(1000);

  //initialize the I2C system at max clock rate supported by sensors
 Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL, 400000 );
   debug_log("I2C initted");

  // initialize Sensor Fusion
  initializeControlPort(&controlSubsystem);  // configure pins and ports for the
                                             // control sub-system
  debug_log("Control Port OK");
  initializeStatusSubsystem(
      &statusSubsystem);  // configure pins and ports for the status sub-system
  debug_log("Status Subsystem OK");
  initSensorFusionGlobals(
      &sfg, &statusSubsystem,
      &controlSubsystem);  // Initialize sensor fusion structures
  debug_log("SFG OK");

// connect to the sensors we will be using.  Accelerometer and magnetometer are in same IC.
#if F_USING_ACCEL || F_USING_MAG
    sfg.installSensor(&sfg, &sensors[0], BOARD_ACCEL_MAG_I2C_ADDR, 1, NULL, NULL, FXOS8700_Init,  FXOS8700_Read);
   debug_log("Accel/Mag connected");
#endif
#if F_USING_GYRO
    sfg.installSensor(&sfg, &sensors[1], BOARD_GYRO_I2C_ADDR, 1, NULL, NULL, FXAS21002_Init, FXAS21002_Read);
   debug_log("Gyro connected");
#endif

   sfg.initializeFusionEngine(&sfg);	        // Initialize sensors and magnetic calibration
   debug_log("Fusion Engine OK");

    sfg.setStatus(&sfg, NORMAL);                // Set status state to NORMAL
   debug_log("Passing to main...");
   delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
    int last_call = millis();
    int loop_interval_ms = 1000 / FUSION_HZ;
    int i = 0;
    while (true) {
      if ((millis() - last_call) > loop_interval_ms) {
        //run the fusion routines every 25 ms (default, can change this but don't
        //overrun the ability of the UART to keep up)
        last_call += loop_interval_ms;

        sfg.readSensors(&sfg, (uint16_t)sfg.loopcounter);  // Reads sensors, applies HAL and does
                                   // averaging (if applicable)
//      debug_log("read sensors");
        sfg.conditionSensorReadings(&sfg);  // magCal (magnetic calibration) is part of this
//      debug_log("applied cal");
        sfg.runFusion(&sfg);                // Run the actual fusion algorithms
//      debug_log("fused");
        sfg.applyPerturbation(&sfg);  // apply debug perturbation (for testing only)
//      debug_log("applied perturbation");
        sfg.loopcounter++;  // loop counter is used to "serialize" mag cal
                            // operations and blink LEDs to indicate status
        i = i + 1;
        if (i >= 4) {   // Some status codes include a "blink" feature.  This loop
          i = 0;        // should cycle at least four times for that to operate
                        // correctly.
          sfg.updateStatus(
              &sfg);  // make pending status updates visible
//        debug_log("updated status");
        }

        sfg.queueStatus(
            &sfg,
            NORMAL);  // assume NORMAL status for next pass through the loop

//      debug_log("entering stream...");
        sfg.pControlSubsystem->stream(
            &sfg, sUARTOutputBuffer);  //Send stream data to the Sensor Fusion
                                       // Toolbox (default) or whatever UART is connected to.
      }
    }
    
}
