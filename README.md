# 🚗 Car Black Box System using PIC16F877A

## 📌 Overview

The **Car Black Box System** is a real-time embedded application developed using the **PIC16F877A microcontroller**. The system continuously monitors and records important vehicle parameters such as **speed, gear position, and timestamp data**.

This project simulates the working principle of an automotive **Event Data Recorder (EDR)**, commonly known as a **Car Black Box**, which is widely used in modern vehicles for diagnostics, safety monitoring, and event tracking.

The system also provides a **secure password-protected interface** that allows users to:
- View stored logs
- Clear records
- Download logs through UART
- Set time
- Change passwords

---

# ❓ What is a Car Black Box?

A **Car Black Box** is an embedded electronic recording system installed in vehicles to continuously store important driving information such as:

- Vehicle speed
- Gear status
- Timestamp information
- Driver activity
- Critical events

Similar to an aircraft black box, it helps in:
- Accident analysis
- Vehicle diagnostics
- Safety monitoring
- Driver behavior tracking

---

# 🎯 Why Do We Need a Car Black Box?

Modern vehicles require reliable event monitoring systems for:

- Accident investigation
- Vehicle diagnostics
- Driver monitoring
- Data logging
- Safety analysis
- Event tracking

This system helps maintain important records that can later be analyzed for troubleshooting and safety improvements.

---

# ⚙️ How the System Works

The system continuously performs the following operations:

1. Reads speed input using ADC
2. Tracks gear/event status
3. Fetches current time using RTC (DS1307)
4. Displays information on CLCD
5. Stores logs into EEPROM memory
6. Allows users to access data through a password-protected menu

Whenever an event occurs, the system stores:
- Time
- Speed
- Gear status

The logs are stored inside EEPROM memory for later access.

---

# ✨ Features

- 🚘 Real-time vehicle event logging
- ⏱️ RTC-based timestamp recording
- 💾 EEPROM storage for 10 logs
- 🔐 Password-protected access system
- 📟 CLCD-based dashboard display
- ⌨️ Matrix keypad navigation
- 📥 UART log downloading at 9600 baud
- ⚡ Interrupt-driven event handling
- 🔄 Non-blocking real-time logging
- 🧹 Clear logs functionality
- 🕒 Time setting support

---

# 🛠️ Hardware Used

| Component | Description |
|-----------|-------------|
| PIC16F877A | Main microcontroller |
| CLCD Display | Displays system information |
| DS1307 RTC | Real-time clock module |
| EEPROM | Stores event logs |
| Matrix Keypad | User input interface |
| UART Interface | Serial communication |
| ADC Input | Speed sensor simulation |
| Timers | Timing operations |

---

# 💻 Software Used

| Software | Purpose |
|----------|---------|
| MPLAB X IDE | Embedded development |
| XC8 Compiler | Embedded C compilation |
| PICSimLab | Simulation and testing |
| GitHub | Version control and hosting |

---

# 🧠 Technologies Used

- Embedded C
- UART Communication
- ADC
- EEPROM
- RTC Interfacing
- I2C Protocol
- Timers & Interrupts
- Keypad Handling
- CLCD Interfacing
- State Machine Logic

---

# 📂 Project Structure

```text
CarBlackBox/
├── Header Files/
│   ├── adc.h
│   ├── clcd.h
│   ├── digital_keypad.h
│   ├── ds1307.h
│   ├── eeprom.h
│   ├── helper.h
│   ├── i2c.h
│   ├── timers.h
│   └── uart.h
│
├── Source Files/
│   ├── adc.c
│   ├── clcd.c
│   ├── digital_keypad.c
│   ├── ds1307.c
│   ├── eeprom.c
│   ├── helper.c
│   ├── i2c.c
│   ├── isr.c
│   ├── main.c
│   ├── timers.c
│   └── uart.c
│
├── Important Files/
├── Linker Files/
├── Libraries/
└── Loadables/
```

---

# 🔄 System Workflow

## 1️⃣ System Initialization
- Initialize CLCD
- Configure UART
- Configure ADC
- Initialize RTC
- Configure timers and interrupts

## 2️⃣ Real-Time Monitoring
- Read speed values
- Detect gear/event status
- Fetch current time

## 3️⃣ Event Logging
- Store:
  - Timestamp
  - Speed
  - Gear status
- Save data into EEPROM

## 4️⃣ User Operations
Using password-protected menu:
- View Logs
- Clear Logs
- Download Logs
- Set Time
- Change Password

---

# 📥 UART Output Example

```text
TIME       SPEED      GEAR
12:10:15     45        G1
12:10:20     52        G2
12:10:28     61        G3
```

---

# 📸 Screenshots

Add screenshots here:

```md
![Dashboard](images/dashboard.png)
![Menu](images/menu.png)
![UART Output](images/uart_output.png)
```

---

# 🚀 How to Run the Project

## Step 1: Clone Repository

```bash
git clone https://github.com/your-username/CarBlackBox-PIC16F877A.git
```

## Step 2: Open Project

- Open MPLAB X IDE
- Import project folder

## Step 3: Build the Project

- Select XC8 compiler
- Build the project

## Step 4: Run Simulation

- Open PICSimLab
- Load generated `.hex` file
- Start simulation

---

# ⚡ Challenges Faced

## 🔹 Real-Time Event Logging
Maintaining continuous logging without blocking the system execution.

### Solution
Implemented non-blocking event handling using timers and interrupts.

---

## 🔹 EEPROM Memory Management
Managing limited EEPROM storage efficiently.

### Solution
Implemented circular logging for storing latest records.

---

## 🔹 UART Communication Reliability
Maintaining reliable serial communication.

### Solution
Used interrupt-driven UART communication at 9600 baud rate.

---

## 🔹 Password Security
Securing menu operations while maintaining responsiveness.

### Solution
Implemented keypad-based password authentication.

---

# 📚 What I Learned

Through this project, I gained hands-on experience in:

- Embedded C programming
- PIC16F877A microcontroller programming
- UART communication
- RTC interfacing using I2C
- EEPROM memory handling
- Interrupt-driven systems
- Timer configuration
- State machine implementation
- Embedded UI development
- Real-time system design
- Debugging embedded applications

---

# 🎯 Future Improvements

- GPS integration
- GSM-based emergency alerts
- SD card storage
- CAN protocol support
- Cloud monitoring
- Mobile application integration
- Accident detection sensors

---

## 👨‍💻 Author

**Siddhant Babanagar**
🎓 Electronics & Telecommunication Engineer (2025)
💡 Interested in Embedded Systems & Low-Level Programming

---

