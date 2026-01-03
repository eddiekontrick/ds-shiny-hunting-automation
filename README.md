# ds-shiny-hunting-automation

A Python–Arduino automation system that performs visual detection and automated input control to identify rare in-game events on the Nintendo DS.

This repository demonstrates the system using a Pokémon Black 2 Cresselia encounter as a representative example. While individual hunts require different detection parameters and timing, the underlying automation pipeline remains consistent.

---

## Overview

The system combines software-based visual detection with embedded hardware control to autonomously reset the game, navigate menus, initiate encounters, and determine whether a shiny Pokémon has appeared.

A Python program performs screen-based detection and communicates its decision to an Arduino microcontroller over serial communication. Based on the response, the Arduino either halts execution (shiny found) or performs a soft reset and repeats the process.

---

## System Architecture

Python (Visual Detection
↓ Serial (USB)
Arduino (Input Controller)
↓
Nintendo DS Button Inputs

---

## Demonstration

Below is an example of a successful shiny detection during the Cresselia encounter in Pokémon Black 2:

![Shiny Cresselia Encounter](docs/cresselia_shiny.png)

---

## Tech Stack

- **Python** – Visual detection and decision logic  
- **Arduino (C/C++)** – Hardware input emulation and state control  
- **Serial Communication** – Coordination between software and hardware  
- **Embedded Timing Logic** – Precise input sequencing for DS interaction  

---

## Repository Structure

ds-shiny-hunting-automation/
├── python/
│ └── shiny_detector.py
├── arduino/
│ └── input_controller.ino
├── docs/
│ └── cresselia_shiny.png
└── README.md

---

## Notes

- Detection logic and timing are tailored to the demonstrated encounter.
- Individual hunts require different visual regions and delays, but no changes to the core control flow.
- This repository focuses on system design and hardware/software integration rather than game-specific mechanics.

---

## Future Improvements

- External configuration files for hunt parameters  
- Modular detection backends (multiple detection methods)  
- Improved fault tolerance and recovery logic  
- Extend the automation framework to support complex in-game sequences such as egg hatching and roamin
