# AUTOMOTIVE ECU WATCHDOG & RECOVERY FRAMEWORK

**Team Name:** CODE TITANS  
**Platform:** QNX Neutrino RTOS  
**IDE:** QNX Momentics IDE 8.0.3  
**Execution Environment:** VMware – Software Simulation  
**Programming Language:** C

## 1. Project Overview

The Automotive ECU Watchdog & Recovery Framework is a software-based project designed to monitor automotive Electronic Control Unit (ECU) services, detect unresponsive processes, and automatically recover failed services.

The framework monitors three ECU services: Battery, Motor Control, and Battery Management System (BMS). Each service periodically sends heartbeat messages to the watchdog supervisor. If a service stops responding, the watchdog detects the heartbeat timeout, terminates the failed process, and restarts only that service.

Other ECU services continue running during the recovery process.

## 2. Problem Statement

Create a supervisory framework that monitors multiple simulated ECU services for missed heartbeats, deadlocks and execution overruns. Use CAN/UART for ECU health messages and GPIO LEDs for service health, failure and recovery indication. Failed services must be restarted safely.

## 3. Project Objectives

- Monitor the health of multiple ECU services.
- Detect unresponsive services using heartbeat timeouts.
- Terminate failed processes using `SIGKILL`.
- Restart only the affected ECU service.
- Maintain the operation of healthy ECU services.
- Measure fault detection and recovery delays.
- Demonstrate process monitoring and recovery using QNX.

## 4. System Architecture

The system consists of the following components:

### 4.1 ECU Services

Three independent services are simulated:

- **Battery Service:** Represents battery-related monitoring.
- **Motor Control Service:** Represents motor control functionality.
- **BMS Service:** Represents battery management functionality.

Each service sends periodic heartbeat messages to the supervisor.

### 4.2 QNX IPC

QNX Inter-Process Communication (IPC) enables communication between ECU processes and the watchdog supervisor.

The project uses QNX message-passing functions such as `MsgSend()`, `MsgReceive()`, and `MsgReply()`.

### 4.3 Watchdog Supervisor

The watchdog supervisor continuously monitors the heartbeat status of each ECU service. If a service exceeds the configured heartbeat timeout, the supervisor initiates recovery.

### 4.4 Recovery Manager

The recovery mechanism terminates the unresponsive process using `SIGKILL`, collects its termination status using `waitpid()`, and attempts to restart the affected service.

### 4.5 QNX Operating System

QNX provides process management, message passing, scheduling, and operating-system services required by the application.

### 4.6 VMware Software Simulation

The project can be developed and tested in a virtualized software environment without requiring physical automotive ECUs.

## 5. Working Principle

1. The supervisor starts the Battery, Motor Control, and BMS services.
2. Each service sends heartbeat messages periodically.
3. The supervisor updates the last heartbeat time for each service.
4. A fault is injected by stopping the selected service process.
5. The watchdog detects the missed heartbeat after the timeout.
6. The supervisor terminates the failed process using `SIGKILL`.
7. The failed service is restarted independently.
8. The first heartbeat from the restarted process confirms that communication has resumed.
9. The supervisor reports the recovery status and timing.
10. Other ECU services continue operating without being restarted.

## 6. Fault Detection and Recovery

The configured heartbeat timeout is **4 seconds**.

The watchdog checks whether a service has stopped sending heartbeats. When the timeout condition is met, the supervisor records the detection delay and starts the recovery process.

The recovery mechanism targets only the failed process. The reported recovery time depends on process termination, process startup, and the restoration of heartbeat communication.

## 7. Technologies Used

| Technology | Purpose |
|---|---|
| C Programming | Implements monitoring and recovery logic |
| QNX Neutrino RTOS | Provides the operating-system environment |
| QNX Momentics IDE 8.0.3 | Development and debugging |
| QNX IPC | Inter-process communication |
| `SIGSTOP` | Injects a simulated process hang |
| `SIGKILL` | Forcefully terminates the failed process |
| `waitpid()` | Collects process termination status |
| VMware | Software simulation environment |

## 8. Available Commands

The program provides a command-line interface for fault injection and status monitoring.

| Command | Description |
|---|---|
| `battery` | Injects a fault into the Battery service |
| `motor` | Injects a fault into the Motor Control service |
| `bms` | Injects a fault into the BMS service |
| `status` | Displays service status and restart counts |
| `exit` | Stops the application and its ECU processes |

A fault command should be issued after the selected service has become healthy.

## 9. Testing and Validation

The following tests can be performed in the QNX software simulation environment:

### Test Case 1: Normal Operation

**Input:** Start the application.

**Expected Result:** All three ECU services start and send heartbeat messages.

### Test Case 2: Fault Injection

**Input:** Enter `bms`.

**Expected Result:** The BMS process is paused to simulate an unresponsive service.

### Test Case 3: Fault Detection

**Input:** Allow the heartbeat timeout to occur.

**Expected Result:** The watchdog reports the BMS heartbeat timeout and detection delay.

### Test Case 4: Automatic Recovery

**Input:** Allow the supervisor to complete recovery.

**Expected Result:** The failed BMS process is terminated and a replacement process is started.

### Test Case 5: Service Continuity

**Input:** Monitor the Battery and Motor Control services during BMS recovery.

**Expected Result:** The other services are not intentionally restarted by the BMS recovery operation.

### Test Case 6: Status Monitoring

**Input:** Enter `status`.

**Expected Result:** The program displays the current service states and restart counts.

*Note: These are planned validation cases. Record actual results from your QNX Momentics and VMware outputs before marking each test as passed.*

## 10. Expected Results

- Detection of simulated ECU process hangs.
- Identification of missed heartbeats.
- Termination of the failed process.
- Automatic restart of the affected service.
- Reporting of fault detection and recovery timing.
- Continued operation of other ECU services.

## 11. Applications

- Automotive ECU health monitoring.
- Embedded process supervision.
- Fault recovery in distributed services.
- Industrial control software.
- Real-time operating-system demonstrations.
- Software reliability and availability experiments.

## 12. Advantages

- Independent service recovery.
- Reduced interruption to healthy services.
- Automatic failure detection.
- Process-level monitoring.
- Useful fault-injection and recovery demonstration.
- No physical ECU hardware required for the software simulation.

## 13. Limitations

- The current project is a software simulation, not a complete vehicle ECU implementation.
- Heartbeat timeout detection does not identify every possible software fault.
- Recovery time depends on operating-system scheduling and process startup.
- Successful process restart does not guarantee that the service is functionally correct.
- Automotive safety certification and hard real-time guarantees are outside the scope of this demonstration.

## 14. Future Enhancements

- Add more simulated ECU services.
- Introduce configurable heartbeat intervals and timeouts.
- Record fault and recovery events in a log file.
- Add recovery statistics and service availability measurements.
- Integrate hardware interfaces such as CAN or UART.
- Test the framework on supported QNX target hardware.

## 15. Conclusion

The Automotive ECU Watchdog & Recovery Framework demonstrates a process-level fault detection and recovery mechanism using QNX. By monitoring heartbeat messages, detecting unresponsive ECU services, and restarting only the failed process, the framework illustrates how service availability can be improved without intentionally restarting the entire system.

## 16. Team

**Team Name:** CODE TITANS

**Project Title:** Automotive ECU Watchdog & Recovery Framework

**Development Platform:** QNX Momentics IDE 8.0.3
