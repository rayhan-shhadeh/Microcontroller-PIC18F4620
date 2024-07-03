# PIC18F4620 Microcontroller Project

This project utilizes the PIC18F4620 microcontroller to interface with an LCD, a temperature sensor (LM35DZ), and an ultrasonic sensor. It also includes serial communication capabilities for data logging and debugging.

## Table of Contents

1. [Overview](#overview)
2. [Hardware](#hardware)
3. [Software](#software)
4. [Usage](#usage)
5. [Circuit Diagram](#circuit-diagram)

## Overview

The project demonstrates how to set up and control various peripherals with the PIC18F4620 microcontroller. It includes the following features:
- Displaying information on a 16x2 LCD
- Reading temperature data from an LM35DZ sensor
- Measuring distance using an ultrasonic sensor
- Serial communication for sending and receiving data

## Hardware

### Components

- PIC18F4620 Microcontroller
- 16x2 LCD Display
- LM35DZ Temperature Sensor
- Ultrasonic Sensor (HC-SR04)
- Push Buttons
- 8 MHz Crystal Oscillator
- Various Resistors and Capacitors
- L7805 Voltage Regulator
- FTDI Module for USB-to-Serial Communication

### Connections

Refer to the circuit diagram below for detailed connections.

1. **PIC18F4620 Pins**:
    - **MCLR/VPP/RE3**: Connected to a push button and Vcc through a 10kΩ resistor.
    - **Oscillator (OSC1/CLKI and OSC2/CLKO)**: Connected to an 8 MHz crystal oscillator with 15pF capacitors.
    - **LCD**: Connected to PORTD and PORTE.
    - **Temperature Sensor (LM35DZ)**: Connected to RA1/AN1.
    - **Ultrasonic Sensor**: Trigger pin connected to RC0, Echo pin connected to RC1.
    - **FTDI Module**: Connected to RC6 (TX) and RC7 (RX).

2. **Voltage Regulation**:
    - Input voltage (>= 7.5V) is regulated to 5V using the L7805 voltage regulator.
  
### Circuit Diagram
![image](https://github.com/rayhan-shhadeh/Microcontroller-PIC18F4620/assets/51097934/c1d16070-64b1-48ef-8e0b-b20cbde65f31)

## Software

### Configuration Bits

The configuration bits are set to:
```c
// CONFIG1H
#pragma config OSC = XT
#pragma config FCMEN = OFF
#pragma config IESO = OFF

// CONFIG2L
#pragma config PWRT = OFF
#pragma config BOREN = SBORDIS
#pragma config BORV = 3

// CONFIG2H
#pragma config WDT = ON
#pragma config WDTPS = 32768

// CONFIG3H
#pragma config CCP2MX = PORTC
#pragma config PBADEN = ON
#pragma config LPT1OSC = OFF
#pragma config MCLRE = ON

// CONFIG4L
#pragma config STVREN = ON
#pragma config LVP = ON
#pragma config XINST = OFF

// CONFIG5L
#pragma config CP0 = OFF
#pragma config CP1 = OFF
#pragma config CP2 = OFF
#pragma config CP3 = OFF

// CONFIG5H
#pragma config CPB = OFF
#pragma config CPD = OFF

// CONFIG6L
#pragma config WRT0 = OFF
#pragma config WRT1 = OFF
#pragma config WRT2 = OFF
#pragma config WRT3 = OFF

// CONFIG6H
#pragma config WRTC = OFF
#pragma config WRTB = OFF
#pragma config WRTD = OFF

// CONFIG7L
#pragma config EBTR0 = OFF
#pragma config EBTR1 = OFF
#pragma config EBTR2 = OFF
#pragma config EBTR3 = OFF

// CONFIG7H
#pragma config EBTRB = OFF
```

## Main Code
The main code initializes the ports, sets up serial communication, initializes the LCD, and reads data from the sensors. It displays the data on the LCD and sends it via serial communication. The distance is measured using the ultrasonic sensor.

## Functions
void initPorts(void): Initializes the ports.
unsigned int getDistance(): Measures distance using the ultrasonic sensor.
void delay_ms(unsigned int n): Provides a delay in milliseconds.
void main(void): Main function that sets up the system and continuously reads sensor data, updates the LCD, and handles serial communication.
Usage
Connect the hardware components as per the circuit diagram.
Flash the PIC18F4620 microcontroller with the provided code.
Power the circuit with a suitable DC power source (>= 7.5V).
The system will display the initial message on the LCD, read the temperature and distance, and update the display.
Data can be sent and received via the serial port using an FTDI module connected to a PC.
Circuit Diagram

Ensure that all connections are made correctly as per the diagram to avoid any damage to the components or incorrect readings.

## Notes
The system uses an 8 MHz crystal oscillator. Ensure that the crystal and capacitors are correctly connected to the oscillator pins.
The temperature sensor (LM35DZ) and ultrasonic sensor must be properly connected to the respective analog and digital pins as shown in the circuit diagram.
