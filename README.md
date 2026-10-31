Automotive ECU Watchdog and Recovery Framework

1. Project Overview

The Automotive ECU Watchdog and Recovery Framework is a software-based prototype developed using QNX Momentics IDE 8.0.3 and the C programming language.

The project demonstrates how an automotive watchdog system monitors Electronic Control Units (ECUs), detects simulated failures, and performs recovery operations.

The system includes three simulated automotive ECUs:

- Engine ECU
- Brake ECU
- Airbag ECU

2. Project Objectives

- Monitor the health status of automotive ECUs.
- Simulate sensor failures and abnormal conditions.
- Detect ECU failures using a watchdog mechanism.
- Demonstrate automatic recovery operations.
- Display ECU status and diagnostic information.
- Improve understanding of automotive fault monitoring.

3. Technologies Used

- Programming Language: C
- IDE: QNX Momentics IDE 8.0.3
- Operating System: QNX Neutrino RTOS
- Development Environment: VMware
- Threading: POSIX Threads (if used in the source code)

4. System Modules

Engine ECU

Simulates engine-related data and operating status.

Brake ECU

Simulates brake-related data and operating status.

Airbag ECU

Simulates airbag system status.

Sensor Monitoring

Generates simulated sensor conditions and demonstrates abnormal-condition detection.

Watchdog Monitor

Monitors ECU health and detects simulated failures.

Recovery Manager

Demonstrates recovery by restoring the simulated ECU status to healthy.

5. Project Features

- Multiple simulated ECU services.
- Sensor failure simulation.
- ECU failure detection.
- Watchdog monitoring.
- Simulated ECU recovery.
- ECU status display.
- Diagnostic messages.

6. Application Commands

Command| Description
"sensor"| Simulates a sensor failure
"engine"| Simulates an engine ECU failure
"airbag"| Simulates an airbag ECU failure
"status"| Displays ECU status
"exit"| Exits the application

7. How to Build and Run

1. Open QNX Momentics IDE 8.0.3.
2. Import or open the project.
3. Configure the QNX build target.
4. Build the project.
5. Run the application on the configured QNX target.
6. Enter the supported commands in the application console.

Note: Build configuration and required libraries depend on the implementation.

8. Expected Output

The application demonstrates:

- ECU health status.
- Simulated sensor readings.
- Watchdog failure detection.
- Recovery notifications.
- ECU diagnostic information.

9. Limitations

This project is a software-based prototype. Sensor readings and ECU failures are simulated. Recovery operations demonstrate software-level state restoration and do not repair physical vehicle components or restart actual vehicle ECUs.

No external automotive hardware is required for the current simulation.

10. Future Enhancements

- Integration with real automotive sensors.
- CAN bus communication.
- Independent ECU processes using QNX IPC.
- Real Bluetooth hardware communication.
- Graphical monitoring dashboard.
- Advanced fault detection and logging.

11. Conclusion

This project demonstrates the basic working principles of an automotive ECU watchdog and recovery framework using a QNX software development environment. It provides a foundation for future development and integration with real automotive hardware.

12. Development Environment

Developed using QNX Momentics IDE 8.0.3 in a VMware environment.
