/*
 * File Name : README.txt
 * Author : ram krishnan (rkris@wisense.in)
 * Created : 1/9/2014
 */
 
 /*
  *****************************************************************************************
 File Name: rfd_app.c
 Description: Implements an RFD application in which the following data
              is reported to the gateway every 5 seconds. 
              
               - On board temperature sensor output (2 bytes)
               - On board Vcc sensor output (2 bytes)
               - External temperature sensor output "TMP102" (4 bytes)
               
              The RFD sleeps in low power mode and wakes up every 5 seconds to report 
              sensor data. When sleeping, all the components (MSP430 and cc2520 (radio) 
              are in low power / shutdown mode to reduce energy consumption thereby 
              increasing battery life. On waking up, this app wakes up triggers a single
              conversion each of the onboard temperature and Vcc sensor. It then sends 
              a data frame to the gateway, receives an ack and goes back to low power mode. 
                
              When building an RFD executable which includes rfd_app.c , these 
              macros need to enabled :-
              1) DEV_TYPE_RFD
              2) RFD_APP_0
              3) PLTFRM_ON_CHIP_TEMP_SENSOR_ENA
              4) PLTFRM_ON_CHIP_VCC_SENSE_ENA
              
              The data packet sent out by this app has the following format :-
              - 2 bytes (packet sequence number)
              - 1 byte (sensor bit mask with value 0x7)
              - 2 bytes (On chip Vcc sensor output (unsigned 2 byte value))
              - 2 bytes (On chip temperature sensor output (signed 2 byte value))
              
              The on chip temperature sensor and on chip Vcc sensor code is in
              pltfrm/src/temp_sensor.c and pltfrm/src/vcc_sense.c

 *****************************************************************************************
 */
 
  
 /*
  *****************************************************************************************
 File Name: rfd_app_1.c
 Description: Implements an RFD application in which the following data
              is reported to the gateway every 5 seconds. 
              
               - On board temperature sensor output (2 bytes)
               - On board Vcc sensor output (2 bytes)
               - External temperature sensor output "TMP102" (4 bytes)
               
              The RFD sleeps in low power mode and wakes up every 5 seconds to report 
              sensor data. When sleeping, all the components (MSP430, cc2520 (radio) 
              and the temperature sensor (TMP102)) are in low power / shutdown mode 
              to reduce energy consumption thereby increasing battery life. On waking
              up, this app wakes up the TMP102 and triggers a conversion. After a
              single conversion, the TMP102 goes back to shutdown mode.
                
              When building an RFD executable which includes rfd_app_1.c , these 
              macros need to enabled :-
              1) DEV_TYPE_RFD
              2) RFD_APP_1
              3) PLTFRM_ON_CHIP_TEMP_SENSOR_ENA
              4) PLTFRM_ON_CHIP_VCC_SENSE_ENA
              5) PLTFRM_TMP102_TEMP_SENSOR_ENA
              
              The data packet sent out by this app has the following format :-
              - 2 bytes (packet sequence number)
              - 1 byte (sensor bit mask with value 0x7)
              - 2 bytes (On chip Vcc sensor output (unsigned 2 byte value))
              - 2 bytes (On chip temperature sensor output (signed 2 byte value))
              - 4 bytes (TMP102 sensor output (signed 4 byte value))
              
              The TMP102 driver code is in pltfrm/src/lm75b.c.
 *****************************************************************************************
 */
  
  
/*
 *****************************************************************************************
 File Name: rfd_app_2.c
 Description: Implements an RFD application in which the following data
              is reported to the gateway every 5 seconds.
              
               - On board temperature sensor output (2 bytes)
               - On board Vcc sensor output (2 bytes)
               - External temperature sensor output "LM75B" (2 bytes)
               
              The RFD sleeps in low power mode and wakes up every 5 seconds to report 
              sensor data. Note that the LM75B is configured to operate in "NORMAL"
              mode. In this mode, the LM75B samples the ambient temperature once
              every 100 milli-seconds. The micro wakes up every 5 seconds and reads
              the latest temperature sampled by the LM75B, sends the data to the 
              gateway, shuts down the radio and goes to low power mode (LPM3). The
              LM75B is not put into low power/shutdown mode.               
                              
              When building an RFD executable which includes rfd_app_2.c , these 
              macros need to enabled :-
              1) DEV_TYPE_RFD
              2) RFD_APP_2
              3) PLTFRM_ON_CHIP_VCC_SENSE_ENA
              4) PLTFRM_LM75B_TEMP_SENSOR_ENA 
              
              The data packet sent out by this app has the following format :-
              - 2 bytes (packet sequence number)
              - 1 byte (sensor bit mask with value 0x7)
              - 2 bytes (On chip Vcc sensor output (unsigned 2 byte value))
              - 2 bytes (lm75b sensor output (signed 2 byte value))
              
              The LM75B driver code is in pltfrm/src/lm75b.c.
 *****************************************************************************************
 */
 
 
 /*
 *****************************************************************************************
 File Name: rfd_app_3.c
 Description: Implements an RFD application which sends the state of the attached
              reed switch whenever the switch opens or closes. Following data is 
              reported.
              
               - On board Vcc sensor output (2 bytes)
               - State of the reed switch (1 byte) - open(1), closed(2)

              The RFD stays for the most power in deep sleep (micro sleeps in LPM3 and
              the radio sleeps in LPM2). Whenever the reed switch opens/closes, it 
              interrupts the micro. The micro then debounces the switch after which the
              app sends the current state of the switch (open/close) to the gateway. The
              micro and radio go back to deep sleep after the message is sent and ack
              is received.                             

              When building an RFD executable which includes rfd_app_3.c , these 
              macros need to enabled :-
              1) DEV_TYPE_RFD
              2) RFD_APP_3
              3) PLTFRM_ON_CHIP_VCC_SENSE_ENA
              4) PLTFRM_REED_SWITCH_ENA
              
              The data packet sent out by this app has the following format :-
              - 2 bytes (packet sequence number)
              - 1 byte (sensor bit mask with value 0x7)
              - 2 bytes (On chip Vcc sensor output (unsigned 2 byte value))
              - 1 bytes (reed switch state (1 byte) - open(1), closed(2))
              
              The reed switch driver code is in pltfrm/src/reed_switch.c.
 *****************************************************************************************
 */
 
 /*
  *****************************************************************************************
  File Name: rfd_app_4.c
  Description: Implements an RFD application which sends the state of the attached
               PIR motion sensor (EKMC160111X) whenever the latter detects motion 
               in it's field of view. Following data is reported.
              
               - On board Vcc sensor output (2 bytes)
               - Count of the number of times the sensor has detected motion since the
                 node joined the network.

               The RFD stays for the most power in deep sleep (micro sleeps in LPM3 and
               the radio sleeps in LPM2). Whenever the motion sensor detects motion, it 
               interrupts the micro. The micro increments a variable which represents the
               number of times the motion has been detected. Interrupts from the motion 
               sensor are masked at this point. After the app reports this event to the 
               gateway and gets and gets an ack, it then debounces the motion sensor. On
               expiry of the debounce interval, the app re-enables interrupt handling from 
               the motion sensor and then puts the micro into LPM3.                   

               When building an RFD executable which includes rfd_app_4.c , these 
               macros need to enabled :-
               1) DEV_TYPE_RFD
               2) RFD_APP_3
               3) PLTFRM_ON_CHIP_VCC_SENSE_ENA
               4) PLTFRM_EKMC160111X_ENA
              
               The data packet sent out by this app has the following format :-
               - 2 bytes (packet sequence number)
               - 1 byte (sensor bit mask with value 0x7)
               - 2 bytes (On chip Vcc sensor output (unsigned 2 byte value))
               - 2 bytes (motion detected event count)
              
               The EKMC160111 PIR motion sensor driver code is in pltfrm/src/ekmc160111x.c
 *****************************************************************************************
 */
 
 
/*
 *****************************************************************************************
 File Name: rfd_app_5.c
 Description: Implements an RFD application in which the following data is reported to the 
              gateway every "RFD_APP_5_DFLT_WAKE_UP_INTERVAL_SECS" seconds. 

              - Light intensity in "Lux" from attached light sensor "TSL45315" (4 bytes)
              - On board Vcc sensor output (2 bytes)
 
               When building an RFD executable which includes rfd_app_5.c , these macros need 
               to enabled :-
               1) DEV_TYPE_RFD
               2) RFD_APP_3
               3) PLTFRM_ON_CHIP_VCC_SENSE_ENA
               4) PLTFRM_TSL45315_ENA
              
               The data packet sent out by this app has the following format :-
               - 2 bytes (packet sequence number)
               - 1 byte (sensor bit mask with value 0x7)
               - 2 bytes (On chip Vcc sensor output (unsigned 2 byte value))
               - 4 bytes (Light intensity in Lux)
              
               The TSL45315 driver code is in pltfrm/src/tsl45315.c
 *****************************************************************************************
 */
 