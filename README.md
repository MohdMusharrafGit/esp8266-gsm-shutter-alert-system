# 🚨 ESP8266 GSM Shutter Alert System

A **standalone GSM-based shutter monitoring and alert system** built using **ESP8266 + SIM800L**.

The system detects **shutter OPEN / CLOSE events** and sends **instant SMS alerts** without requiring Wi-Fi or internet connectivity.

Designed for **industrial, commercial, and remote security applications**.

---

## ✨ Features

- 🔔 Real-time **shutter OPEN / CLOSE detection**
- 📩 **Instant SMS alerts** using GSM (SIM800L)
- 🌐 **Web-based configuration portal** (ESP8266 AP mode)
- 💾 Mobile numbers stored in **EEPROM (non-volatile memory)**
- 🔄 Automatic restart after configuration
- 💡 Status LED indication
- 📡 Fully functional in **no-WiFi / remote locations**

---

## 🧠 Working Principle

1. ESP8266 continuously monitors shutter sensors  
2. On any state change (OPEN → CLOSE or vice-versa):
   - SMS alert is sent to configured mobile numbers  
3. Configuration mode:
   - Press the **config button**
   - ESP8266 creates a Wi-Fi hotspot
   - Open configuration web page
   - Save mobile numbers to EEPROM

---

## 🧰 Hardware Used

- ESP8266 (NodeMCU)
- SIM800L GSM module
- Shutter sensors (reed switch / limit switch)
- External **high-current power supply** for SIM800L
- Push button (configuration)
- Status LED

---

## 🔌 Pin Configuration (ESP8266)

### SIM800L
- RX → GPIO12 (D6)
- TX → GPIO13 (D7)

### Shutter Sensors
- Shutter 1 → GPIO5 (D1)
- Shutter 2 → GPIO4 (D2)
- Shutter 3 → GPIO14 (D5)

### Other
- Config Button → GPIO16 (D0)
- Status LED → GPIO2 (D4, Active LOW)

⚠️ **Important**
- SIM800L requires **2A peak current**
- Use a **voltage divider** for SIM800L RX
- GPIO16 (D0) does **not support interrupts**

---

## 🌐 Configuration Portal

- Wi-Fi SSID: **ESP_SHUTTER_CFG**
- Configure up to **3 mobile numbers**
- Stored in EEPROM
- Device auto-restarts after saving

---

## 📩 SMS Alert Format

- **Shutter OPEN**
- **Shutter CLOSED**

SMS is sent **only when state changes**.

---

## 🛠️ Software Stack

- Arduino IDE
- Embedded C / C++
- ESP8266WiFi
- ESP8266WebServer
- SoftwareSerial
- EEPROM
- SIM800L AT Commands

---

## 📂 Project Structure
esp8266-gsm-shutter-alert-system
├── esp8266ShutterWithGsm.ino
└── README.md


---

## 🔮 Future Enhancements

- Cloud logging
- Web dashboard
- Password-protected portal
- Voice call alerts
- Battery backup

---

## 👤 Author

**Mohd Musharraf**  
Embedded Systems & IoT Developer  

---

## ⭐ Support

If you find this project useful, please **⭐ star the repository**.


