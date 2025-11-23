# ESP32 Game Box 🎮

A web-based interactive gaming system built on ESP32 featuring 6 physical buttons and 6 LEDs, controlled through a beautiful mobile-responsive web interface. Perfect for parties, education, or just having fun!

## 🌟 Features

- **6 Classic Games** - Each with unique gameplay mechanics
- **Web-Based Interface** - No app installation required
- **Real-Time Monitoring** - Debug panel for hardware testing
- **Wireless Control** - Access via WiFi on any device
- **Beautiful UI** - Modern, gradient-based design with smooth animations
- **Standalone System** - ESP32 creates its own WiFi access point

## 🎯 Games Included

### 1. 🧠 Simon Says
Remember and repeat increasingly complex LED sequences. One mistake and you're out!

### 2. 🔨 Whack-a-Mole
Hit the lit LED as fast as you can. Miss or hit the wrong button and lose points!

### 3. 🃏 Memory Match
Find matching LED pairs based on position patterns. Test your memory skills!

### 4. ⚡ Reaction Test
How fast are your reflexes? Hit the correct button when the LED lights up to score points.

### 5. 🎨 Pattern Race
Memorize and recreate LED patterns as quickly as possible. Accuracy is key!

### 6. 🏃 LED Chase
Catch the moving light by pressing the button at just the right moment!

## 🔧 Hardware Requirements

### Components
- **ESP32 Development Board** (any variant)
- **6x Push Buttons** (normally open, momentary)
- **6x LEDs** (any color, 5mm recommended)
- **6x 1kΩ Resistors** (for LEDs)
- **Breadboard and Jumper Wires**
- **Power Supply** (USB or 5V adapter)

### Pin Configuration

| Component | GPIO Pins |
|-----------|-----------|
| Buttons   | 19, 18, 21, 22, 23, 33 |
| LEDs      | 12, 13, 25, 26, 27, 32 |

**Button Wiring:** Connect buttons between GPIO pins and GND (using internal pull-up resistors)

**LED Wiring:** Connect LEDs through 220Ω resistors to GPIO pins (anode to GPIO, cathode to GND)

## 📋 Software Requirements

- Arduino IDE (1.8.x or 2.x)
- ESP32 Board Package
- Required Libraries:
  - `WiFi.h` (included with ESP32 package)
  - `WebServer.h` (included with ESP32 package)

## 🚀 Installation

### 1. Install Arduino IDE
Download from [arduino.cc](https://www.arduino.cc/en/software)

### 2. Add ESP32 Board Support
1. Open Arduino IDE
2. Go to `File` → `Preferences`
3. Add to "Additional Board Manager URLs":
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Go to `Tools` → `Board` → `Boards Manager`
5. Search "ESP32" and install "esp32 by Espressif Systems"

### 3. Upload the Code
1. Open the `.ino` file in Arduino IDE
2. Select your ESP32 board: `Tools` → `Board` → `ESP32 Arduino` → (your board)
3. Select the correct port: `Tools` → `Port`
4. Click **Upload** button

### 4. Connect to ESP32
1. Power on your ESP32
2. Connect to WiFi network: `ESP32-GameHub`
3. Password: `game1234`
4. Open browser and navigate to: `http://192.168.4.1`

## 🎮 Usage

### Main Menu
Two options available:
- **Play Games** - Access the game selection screen
- **Monitor & Debug** - Real-time hardware testing interface

### Playing Games
1. Select a game from the game hub
2. Game starts automatically
3. Physical buttons control gameplay
4. Watch your score increase on the web interface
5. Press "Stop Current Game" to end

### Monitor Mode
- View real-time button states
- Toggle LEDs individually
- Test hardware connectivity
- Perfect for debugging and setup

## 🛠️ Configuration

### WiFi Settings
Edit these lines in the code to change network credentials:
```cpp
const char* ssid = "ESP32-GameHub";      // Network name
const char* password = "game1234";        // Network password
```

### Pin Customization
Modify these arrays to change GPIO assignments:
```cpp
const int BTN_PINS[NUM_BTNS] = {19, 18, 21, 22, 23, 33};
const int LED_PINS[NUM_LEDS] = {12, 13, 25, 26, 27, 32};
```

### Game Difficulty
Adjust timing parameters in individual game update functions to modify difficulty.

## 📱 Web Interface

### Responsive Design
- Optimized for mobile phones and tablets
- Works on any modern browser
- Touch-friendly controls
- No installation required

### Features
- Real-time score updates
- Live game status
- Hardware state visualization
- Beautiful gradient animations

## 🐛 Troubleshooting

### Can't Connect to WiFi
- Ensure ESP32 is powered on
- Check if "ESP32-GameHub" network appears
- Try forgetting the network and reconnecting
- Verify password is correct: `game1234`

### LEDs Not Working
- Check resistor values (220Ω recommended)
- Verify LED polarity (long leg = anode)
- Test in Monitor mode to isolate issues
- Confirm GPIO pin assignments

### Buttons Not Responding
- Ensure buttons are wired to GND correctly
- Check for loose connections
- Test individual buttons in Monitor mode
- Verify internal pull-ups are enabled (they are by default)

### Page Won't Load
- Confirm IP address is `192.168.4.1`
- Try different browser
- Clear browser cache
- Check serial monitor for errors

## 🔍 Serial Monitor

Open serial monitor (115200 baud) to see:
- WiFi AP status
- IP address confirmation
- HTTP server status
- Connection logs

## 📊 API Endpoints

The system provides REST API endpoints:

- `GET /` - Main menu
- `GET /game` - Game selection interface
- `GET /monitor` - Hardware monitor interface
- `GET /api/status` - System status JSON
- `POST /api/led` - Control individual LEDs
- `POST /api/start-game` - Start a game
- `POST /api/stop-game` - Stop current game

## 🎨 Customization Ideas

- Add sound effects with a buzzer
- Implement difficulty levels
- Create multiplayer modes
- Add score persistence
- Design custom enclosure
- Add RGB LEDs for color effects
- Create tournament mode

## 📄 License

This project is open source and available under the MIT License.

## 🤝 Contributing

Contributions, issues, and feature requests are welcome! Feel free to check issues page.

## 👨‍💻 Author

urcauz

## 🙏 Acknowledgments

- Built with ESP32 Arduino Core
- Inspired by classic arcade games
- UI design influenced by modern web trends

## 📸 Screenshots

[![Screenshot-from-2025-11-23-18-52-59.png](https://i.postimg.cc/rmgP69xv/Screenshot-from-2025-11-23-18-52-59.png)](https://postimg.cc/Ppp4YY64)
[![Screenshot-from-2025-11-23-18-53-05.png](https://i.postimg.cc/gJBHqtvx/Screenshot-from-2025-11-23-18-53-05.png)](https://postimg.cc/vgfnGz2y)
[![Screenshot-from-2025-11-23-18-53-12.png](https://i.postimg.cc/BncycstD/Screenshot-from-2025-11-23-18-53-12.png)](https://postimg.cc/dh34wb2t)
[![Screenshot-from-2025-11-23-18-57-35.png](https://i.postimg.cc/W3r0gpb7/Screenshot-from-2025-11-23-18-57-35.png)](https://postimg.cc/VrfSc8tS)

---

**Enjoy gaming with your ESP32! 🎮✨**

*If you found this project helpful, please consider giving it a ⭐!*
