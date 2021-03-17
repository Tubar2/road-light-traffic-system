# Road light Traffic System
Lab Sismic Final Project

Project is an attempt at using the TI msp430-f5529 to control a simple traffic light system.\
Simulation data is sent via bluetooth to a outside computer where it is displayed.\
A simulation overview is also displayed on a LCD screen

By data we mean the number of cars that passed on a green light and how many ran a red light

## Repo File Structure
There are 2 PDFs:
* One is the groups proposal: overview of what we wanted to make
* The other contains what we managed to achieve after the project's completion

main.c contains the simulation's setup and logic

The libs folder contains:
* Helper headers/functions for UART and I2C communication
* Helper headers/functions and types for using the LCD, GPIO and timer msp components
* A dedicated project library for specific functionality and handling interruptions
