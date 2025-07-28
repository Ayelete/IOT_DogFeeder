# Smart Dog Feeder 🐶📲

An IoT-powered smart dog feeder combining embedded hardware (ESP32, load cell, OLED, stepper motor) with a connected Flutter app and Firebase backend.

Designed to give your pup the best care — even when you’re not home.

---

## 💡 How It Works

1. ⏰ Schedule up to **5 meals per day** from the mobile app  
2. ⚙️ The ESP32-controlled motor dispenses food, tracked live with a **load cell**  
3. 📡 Feeding data is synced to **Firebase** in real time  
4. 📲 You get a **notification** when your dog finishes their food  
5. 📊 A stats page shows exactly how much your dog ate — even if they grazed throughout the day  

Smart enough to only add the **difference** to the bowl — so no double feeding if leftovers remain.

---

## 📱 Mobile App Features

- Set up to **5 custom meal times**
- View **daily feeding stats** based on real weight
- Get **push notifications** when meals are completed
- Uses **Firebase** for cloud sync across devices
- Built with Flutter and designed for simplicity

---

## ⚙️ ESP32 Firmware Highlights

- **Wi-Fi Auto-Reconnect + AP Fallback**  
  If no saved Wi-Fi, launches its own access point with a browser config page.

- **Motor Control with AccelStepper**  
  Smooth stepper motor action with automatic shutoff when the target weight is reached.

- **HX711 Load Cell Integration**  
  Reads live weight from the bowl and pushes updates to Firebase.

- **OLED Feedback**  
  Displays live messages for connection status, feeding progress, and errors.

- **NTP Time Sync**  
  Pulls time from Israeli and global NTP servers for accurate daily resets.

---

## 📂 Folder Structure


---

## 🛠 Tech Stack

- **Microcontroller**: ESP32
- **App**: Flutter (Dart)
- **Backend**: Firebase Realtime DB
- **Sensors**: HX711 Load Cell
- **Motor Driver**: AccelStepper
- **Display**: SH1106 OLED
- **Clock Sync**: NTP (Technion + pool.ntp.org)

---

## 📎 Notes

- This project was built collaboratively and includes contributions across firmware, mobile development, and Firebase integration.
- All previous versions and dev experiments are saved in the `/archive` folder.
