/*
 * File Name : msp430_lpm.c
 * Author : ram krishnan (rkris@wisense.in)
 * Created : 9/30/2013
 */


/*
 * The clock system is very flexible and  allows the MSP430 to operate optimally
 * from a single 32KHz crystal – with the internal digitally controlled oscillator
 * (DCO) used for the CPU and high-speed peripherals. A low-frequency Auxiliary Clock
 * (ACLK) is driven directly from a common 32KHz watch crystal with no additional
 * external components. The ACLK enables the MSP430’s ultra-low power standby mode
 * (LPM3) and an embedded real-time clock function. In LPM3, the MSP430 typically
 * consumes current in the 1µA range.
 * The integrated high-speed DCO can source the master clock (MCLK) used by the CPU
 * and high-speed peripherals. By design, the DCO is active and fully stable in less
 * than 6 µs with no intermediate steps. This enables “instant on” high-performance
 * processing – no long start-up for a second crystal or 2-speed start-up required.
 * Because the DCO is digitally adjustable with software and hardware, stability over
 * time and temperature is assured.
 * To service interrupt driven events, the software efficiently uses the 16-bit RISC
 * CPU’s performance in very short “burst” intervals. Transition from standby to fully
 * active is less than 6 µs. This results in a combination of ultra-low power consumption
 * and very high performance immediately when needed.
 */


/*
 * <MSP430G2553 spec>
 *  - Low Supply-Voltage Range: 1.8 V to 3.6 V
 *  - Ultra-Low Power Consumption
 *     > Active Mode: 230 μA at 1 MHz, 2.2 V
 *     > Standby Mode: 0.5 μA
 *     > Off Mode (RAM Retention): 0.1 μA
 *  - Five Power-Saving Modes
 *  - Ultra-Fast Wake-Up From Standby Mode in less Than 1 microsecs.
 *
 *  LPM3 (LFXT1) mode current consumption (Typical 0.7 microamps, max 1.5 microamps)
 *  fACLK of 32768 Hz, Vcc - 2.2 volts
 *
 *
 *  Operating Modes
 *    The MSP430 has one active mode and five software selectable low-power modes of operation.
 *    An interrupt event can wake up the device from any of the low-power modes, service the
 *    request, and restore back to the low-power mode on return from the interrupt program.
 *    > AM
 *    > LPM0
 *    > LPM1
 *    > LPM2
 *    > LPM3  (CPU disabled and only ACLK active)
 *    > LPM4  (CPU disabled and all clocks too)
 */




