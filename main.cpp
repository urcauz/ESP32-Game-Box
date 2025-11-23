#include <WiFi.h>
#include <WebServer.h>
#include <vector>
#include <string>

const int NUM_BTNS = 6;
const int NUM_LEDS = 6;
const int BTN_PINS[NUM_BTNS] = {19, 18, 21, 22, 23, 33};
const int LED_PINS[NUM_LEDS] = {12, 13, 25, 26, 27, 32};

const char* ssid = "ESP32-GameHub";
const char* password = "game1234";

WebServer server(80);

bool ledStates[NUM_LEDS] = {false, false, false, false, false, false};
bool btnStates[NUM_BTNS] = {false, false, false, false, false, false};
int currentGame = 0;
bool gameActive = false;

int gameScores[6] = {0, 0, 0, 0, 0, 0};
std::vector<int> gameSequence;
int sequenceIndex = 0;
unsigned long gameTimer = 0;
int gameLevel = 1;
bool waitingForInput = false;

enum GameState {
  IDLE,
  GAME_1_SIMON_SAYS,
  GAME_2_WHACK_A_MOLE,
  GAME_3_MEMORY_MATCH,
  GAME_4_REACTION_TEST,
  GAME_5_PATTERN_RACE,
  GAME_6_LED_CHASE
};

GameState gameState = IDLE;

void handleRoot();
void handleMonitor();
void handleGame();
void handleLEDControl();
void handleStatus();
void handleStartGame();
void handleGameInput();
void handleStopGame();
void startupAnimation();
void readButtons();
void updateGame();
void setLED(int index, bool state);
void allLEDsOff();
void updateSimonSays();
void updateWhackAMole();
void updateMemoryMatch();
void updateReactionTest();
void updatePatternRace();
void updateLEDChase();
void flashError();
void endGame();

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < NUM_BTNS; i++) {
    pinMode(BTN_PINS[i], INPUT_PULLUP);
  }

  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }

  WiFi.softAP(ssid, password);
  Serial.println("WiFi AP Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/monitor", handleMonitor);
  server.on("/game", handleGame);
  server.on("/api/led", HTTP_POST, handleLEDControl);
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/start-game", HTTP_POST, handleStartGame);
  server.on("/api/game-input", HTTP_POST, handleGameInput);
  server.on("/api/stop-game", HTTP_POST, handleStopGame);

  server.begin();
  Serial.println("HTTP server started");

  startupAnimation();
}

void loop() {
  server.handleClient();
  readButtons();
  updateGame();
}

void startupAnimation() {
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(LED_PINS[i], HIGH);
    delay(100);
  }
  delay(200);
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(LED_PINS[i], LOW);
    delay(100);
  }
}

void readButtons() {
  for (int i = 0; i < NUM_BTNS; i++) {
    btnStates[i] = !digitalRead(BTN_PINS[i]);
  }
}

void setLED(int index, bool state) {
  if (index >= 0 && index < NUM_LEDS) {
    ledStates[index] = state;
    digitalWrite(LED_PINS[index], state ? HIGH : LOW);
  }
}

void allLEDsOff() {
  for (int i = 0; i < NUM_LEDS; i++) {
    setLED(i, false);
  }
}

void updateGame() {
  if (!gameActive) return;

  switch (gameState) {
    case GAME_1_SIMON_SAYS:
      updateSimonSays();
      break;
    case GAME_2_WHACK_A_MOLE:
      updateWhackAMole();
      break;
    case GAME_3_MEMORY_MATCH:
      updateMemoryMatch();
      break;
    case GAME_4_REACTION_TEST:
      updateReactionTest();
      break;
    case GAME_5_PATTERN_RACE:
      updatePatternRace();
      break;
    case GAME_6_LED_CHASE:
      updateLEDChase();
      break;
    default:
      break;
  }
}

void updateSimonSays() {
  if (!waitingForInput && millis() - gameTimer > 1000) {
    if (sequenceIndex < gameSequence.size()) {
      int ledToShow = gameSequence[sequenceIndex];
      setLED(ledToShow, true);
      delay(500);
      setLED(ledToShow, false);
      delay(300);
      sequenceIndex++;
    } else {
      waitingForInput = true;
      sequenceIndex = 0;
      gameTimer = millis();
    }
  }

  if (waitingForInput) {
    for (int i = 0; i < NUM_BTNS; i++) {
      if (btnStates[i]) {
        if (i == gameSequence[sequenceIndex]) {
          setLED(i, true);
          delay(200);
          setLED(i, false);
          sequenceIndex++;

          if (sequenceIndex >= gameSequence.size()) {
            gameScores[0]++;
            gameSequence.push_back(random(0, NUM_LEDS));
            sequenceIndex = 0;
            waitingForInput = false;
            gameTimer = millis();
          }
        } else {
          flashError();
          endGame();
        }
        delay(200);
        break;
      }
    }
  }
}

void updateWhackAMole() {
  if (millis() - gameTimer > 1500) {
    allLEDsOff();
    int mole = random(0, NUM_LEDS);
    setLED(mole, true);
    gameTimer = millis();
    sequenceIndex = mole;
  }

  for (int i = 0; i < NUM_BTNS; i++) {
    if (btnStates[i]) {
      if (i == sequenceIndex && ledStates[i]) {
        gameScores[1]++;
        setLED(i, false);
      } else if (ledStates[i] == false) {
        gameScores[1] = max(0, gameScores[1] - 1);
      }
      delay(200);
      break;
    }
  }
}

void updateMemoryMatch() {
  static int firstPick = -1;
  static int secondPick = -1;
  static unsigned long pickTimer = 0;

  if (firstPick >= 0 && secondPick >= 0) {
    if (millis() - pickTimer > 1000) {
      setLED(firstPick, false);
      setLED(secondPick, false);

      if (firstPick % 3 == secondPick % 3 && firstPick != secondPick) {
        gameScores[2]++;
      }

      firstPick = -1;
      secondPick = -1;
    }
    return;
  }

  for (int i = 0; i < NUM_BTNS; i++) {
    if (btnStates[i] && !ledStates[i]) {
      if (firstPick == -1) {
        firstPick = i;
        setLED(i, true);
      } else if (secondPick == -1 && i != firstPick) {
        secondPick = i;
        setLED(i, true);
        pickTimer = millis();
      }
      delay(200);
      break;
    }
  }
}

void updateReactionTest() {
  if (!waitingForInput && millis() - gameTimer > random(2000, 5000)) {
    int led = random(0, NUM_LEDS);
    setLED(led, true);
    sequenceIndex = led;
    waitingForInput = true;
    gameTimer = millis();
  }

  if (waitingForInput) {
    for (int i = 0; i < NUM_BTNS; i++) {
      if (btnStates[i]) {
        if (i == sequenceIndex) {
          unsigned long reactionTime = millis() - gameTimer;
          if (reactionTime < 500) {
            gameScores[3] += 10;
          } else if (reactionTime < 1000) {
            gameScores[3] += 5;
          } else {
            gameScores[3] += 1;
          }
        }
        allLEDsOff();
        waitingForInput = false;
        gameTimer = millis();
        delay(200);
        break;
      }
    }

    if (millis() - gameTimer > 3000) {
      allLEDsOff();
      waitingForInput = false;
      gameTimer = millis();
    }
  }
}

void updatePatternRace() {
  static int targetPattern = 0;

  if (!waitingForInput && millis() - gameTimer > 2000) {
    targetPattern = random(0, 64);
    for (int i = 0; i < NUM_LEDS; i++) {
      setLED(i, (targetPattern >> i) & 1);
    }
    waitingForInput = true;
    gameTimer = millis();
  }

  if (waitingForInput) {
    int currentPattern = 0;
    for (int i = 0; i < NUM_BTNS; i++) {
      if (btnStates[i]) {
        currentPattern |= (1 << i);
      }
    }

    if (currentPattern == targetPattern && currentPattern != 0) {
      gameScores[4]++;
      allLEDsOff();
      waitingForInput = false;
      gameTimer = millis();
    }
  }
}

void updateLEDChase() {
  if (millis() - gameTimer > 300) {
    allLEDsOff();
    sequenceIndex = (sequenceIndex + 1) % NUM_LEDS;
    setLED(sequenceIndex, true);
    gameTimer = millis();
  }

  for (int i = 0; i < NUM_BTNS; i++) {
    if (btnStates[i] && i == sequenceIndex) {
      gameScores[5]++;
      delay(100);
      break;
    }
  }
}

void flashError() {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < NUM_LEDS; j++) {
      setLED(j, true);
    }
    delay(100);
    allLEDsOff();
    delay(100);
  }
}

void endGame() {
  gameActive = false;
  gameState = IDLE;
  allLEDsOff();
}

void handleRoot() {
  String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>body{font-family:Arial;text-align:center;padding:20px;background:#0f172a;color:#fff;}";
  html += "a{display:block;margin:20px auto;padding:20px;background:#3b82f6;color:#fff;text-decoration:none;border-radius:10px;max-width:300px;font-size:20px;}</style></head>";
  html += "<body><h1>ESP32 Game Hub</h1>";
  html += "<a href='/game'>1 Play Games</a>";
  html += "<a href='/monitor'>2 Monitor & Debug</a>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleMonitor() {
  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no'>";
  html += "<style>*{margin:0;padding:0;box-sizing:border-box;}body{font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',sans-serif;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);min-height:100vh;padding:20px;color:#fff;}";
  html += ".container{max-width:500px;margin:0 auto;}.header{text-align:center;margin-bottom:30px;}.header h1{font-size:28px;font-weight:700;margin-bottom:5px;}";
  html += ".header p{font-size:14px;opacity:0.9;}.section{background:rgba(255,255,255,0.15);backdrop-filter:blur(10px);border-radius:20px;padding:20px;margin-bottom:20px;box-shadow:0 8px 32px rgba(0,0,0,0.1);}";
  html += ".section-title{font-size:16px;font-weight:600;margin-bottom:15px;text-transform:uppercase;letter-spacing:1px;}.led-grid,.btn-grid{display:grid;grid-template-columns:repeat(3,1fr);gap:12px;}";
  html += ".led-item,.btn-item{background:rgba(255,255,255,0.1);border-radius:15px;padding:15px;text-align:center;transition:all 0.3s ease;}.led-item{cursor:pointer;border:2px solid rgba(255,255,255,0.2);}";
  html += ".led-item:active{transform:scale(0.95);}.led-item.active{background:#10b981;border-color:#10b981;box-shadow:0 0 20px rgba(16,185,129,0.5);}";
  html += ".led-label,.btn-label{font-size:12px;opacity:0.8;margin-bottom:8px;}.led-status,.btn-status{font-size:24px;font-weight:700;}";
  html += ".btn-item.pressed{background:#ef4444;border:2px solid #140808ff;box-shadow:0 0 20px rgba(239,68,68,0.5);}.status-bar{display:flex;justify-content:space-between;padding:12px;background:rgba(255,255,255,0.1);border-radius:10px;font-size:13px;margin-bottom:20px;}";
  html += ".control-btn{width:100%;padding:15px;background:#3b82f6;border:none;border-radius:12px;color:#fff;font-size:16px;font-weight:600;cursor:pointer;transition:all 0.3s;}";
  html += ".control-btn:active{transform:scale(0.98);background:#2563eb;}.back-btn{background:rgba(255,255,255,0.2);margin-top:10px;}</style></head>";
  html += "<body><div class='container'><div class='header'><h1>Monitor</h1><p>Real-time hardware control</p></div>";
  html += "<div class='status-bar'><span>Connected</span><span id='uptime'>0s</span></div>";
  html += "<div class='section'><div class='section-title'>LED Control</div><div class='led-grid'>";

  for (int i = 0; i < NUM_LEDS; i++) {
    html += "<div class='led-item' onclick='toggleLED(" + String(i) + ")' id='led" + String(i) + "'>";
    html += "<div class='led-label'>LED " + String(i + 1) + "</div>";
    html += "<div class='led-status'>o</div></div>";
  }

  html += "</div></div><div class='section'><div class='section-title'>Button Status</div><div class='btn-grid'>";

  for (int i = 0; i < NUM_BTNS; i++) {
    html += "<div class='btn-item' id='btn" + String(i) + "'>";
    html += "<div class='btn-label'>BTN " + String(i + 1) + "</div>";
    html += "<div class='btn-status'>o</div></div>";
  }

  html += "</div></div><button class='control-btn' onclick='allOff()'>Turn All LEDs OFF</button>";
  html += "<button class='control-btn back-btn' onclick='location.href=\"/\"'>Back to Home</button></div>";
  html += "<script>let startTime=Date.now();function updateStatus(){fetch('/api/status').then(r=>r.json()).then(d=>{";
  html += "for(let i=0;i<6;i++){document.getElementById('led'+i).classList.toggle('active',d.leds[i]);";
  html += "document.getElementById('btn'+i).classList.toggle('pressed',d.buttons[i]);}";
  html += "document.getElementById('uptime').textContent=Math.floor((Date.now()-startTime)/1000)+'s';});";
  html += "}function toggleLED(i){fetch('/api/led',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({led:i})}).then(()=>updateStatus());}";
  html += "function allOff(){for(let i=0;i<6;i++){fetch('/api/led',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({led:i,state:0})});}setTimeout(updateStatus,100);}";
  html += "setInterval(updateStatus,200);updateStatus();</script></body></html>";

  server.send(200, "text/html", html);
}

void handleGame() {
  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no'>";
  html += "<style>*{margin:0;padding:0;box-sizing:border-box;}body{font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',sans-serif;background:linear-gradient(135deg,#f093fb 0%,#f5576c 100%);min-height:100vh;padding:20px;color:#fff;}";
  html += ".container{max-width:500px;margin:0 auto;}.header{text-align:center;margin-bottom:30px;}.header h1{font-size:32px;font-weight:700;margin-bottom:5px;}";
  html += ".header p{font-size:14px;opacity:0.9;}.game-grid{display:grid;gap:15px;margin-bottom:20px;}";
  html += ".game-card{background:rgba(255,255,255,0.2);backdrop-filter:blur(10px);border-radius:20px;padding:20px;cursor:pointer;transition:all 0.3s;border:2px solid rgba(255,255,255,0.3);position:relative;overflow:hidden;}";
  html += ".game-card:active{transform:scale(0.98);}.game-card::before{content:'';position:absolute;top:0;left:0;right:0;height:4px;background:linear-gradient(90deg,#10b981,#3b82f6,#8b5cf6);}";
  html += ".game-header{display:flex;align-items:center;margin-bottom:10px;}.game-icon{font-size:32px;margin-right:15px;}.game-info{flex:1;}";
  html += ".game-title{font-size:18px;font-weight:700;margin-bottom:3px;}.game-desc{font-size:13px;opacity:0.85;line-height:1.4;}";
  html += ".game-score{text-align:right;font-size:24px;font-weight:700;color:#fbbf24;}.control-panel{background:rgba(255,255,255,0.15);backdrop-filter:blur(10px);border-radius:20px;padding:20px;margin-bottom:20px;text-align:center;}";
  html += ".status{font-size:16px;margin-bottom:15px;font-weight:600;}.control-btn{width:100%;padding:15px;border:none;border-radius:12px;color:#fff;font-size:16px;font-weight:600;cursor:pointer;transition:all 0.3s;margin-bottom:10px;}";
  html += ".start-btn{background:#10b981;}.start-btn:active{background:#059669;}.stop-btn{background:#ef4444;}.stop-btn:active{background:#dc2626;}";
  html += ".back-btn{background:rgba(255,255,255,0.2);}.back-btn:active{background:rgba(255,255,255,0.3);}</style></head>";
  html += "<body><div class='container'><div class='header'><h1>Game Hub</h1><p>Choose your game and play!</p></div>";
  html += "<div class='control-panel'><div class='status' id='status'>Select a game to start</div>";
  html += "<button class='control-btn stop-btn' onclick='stopGame()'>Stop Current Game</button></div>";
  html += "<div class='game-grid'>";

  String games[6][3] = {
    {"1", "Simon Says", "Remember and repeat the LED sequence"},
    {"2", "Whack-a-Mole", "Hit the LED as fast as you can"},
    {"3", "Memory Match", "Find matching LED pairs"},
    {"4", "Reaction Test", "Test your reaction speed"},
    {"5", "Pattern Race", "Match the LED pattern quickly"},
    {"6", "LED Chase", "Catch the moving light"}
  };

  for (int i = 0; i < 6; i++) {
    html += "<div class='game-card' onclick='startGame(" + String(i) + ")'>";
    html += "<div class='game-header'><div class='game-icon'>" + games[i][0] + "</div>";
    html += "<div class='game-info'><div class='game-title'>" + games[i][1] + "</div>";
    html += "<div class='game-desc'>" + games[i][2] + "</div></div>";
    html += "<div class='game-score' id='score" + String(i) + "'>0</div></div></div>";
  }

  html += "</div><button class='control-btn back-btn' onclick='location.href=\"/\"'>Back to Home</button></div>";
  html += "<script>function updateScores(){fetch('/api/status').then(r=>r.json()).then(d=>{";
  html += "for(let i=0;i<6;i++){document.getElementById('score'+i).textContent=d.scores[i];}";
  html += "document.getElementById('status').textContent=d.gameActive?'Game Active: '+d.gameName:'No game running';});}";
  html += "function startGame(g){fetch('/api/start-game',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({game:g})}).then(()=>updateScores());}";
  html += "function stopGame(){fetch('/api/stop-game',{method:'POST'}).then(()=>updateScores());}";
  html += "setInterval(updateScores,500);updateScores();</script></body></html>";

  server.send(200, "text/html", html);
}

void handleLEDControl() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    int ledIndex = -1;
    int state = -1;

    int ledPos = body.indexOf("\"led\":");
    int statePos = body.indexOf("\"state\":");

    if (ledPos > -1) {
      ledIndex = body.substring(ledPos + 6, body.indexOf(",", ledPos)).toInt();
    }

    if (statePos > -1) {
      state = body.substring(statePos + 8, body.indexOf("}", statePos)).toInt();
    } else {
      state = ledStates[ledIndex] ? 0 : 1;
    }

    if (ledIndex >= 0 && ledIndex < NUM_LEDS) {
      setLED(ledIndex, state == 1);
    }
  }

  server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleStatus() {
  String json = "{\"leds\":[";
  for (int i = 0; i < NUM_LEDS; i++) {
    json += ledStates[i] ? "true" : "false";
    if (i < NUM_LEDS - 1) json += ",";
  }
  json += "],\"buttons\":[";
  for (int i = 0; i < NUM_BTNS; i++) {
    json += btnStates[i] ? "true" : "false";
    if (i < NUM_BTNS - 1) json += ",";
  }
  json += "],\"scores\":[";
  for (int i = 0; i < 6; i++) {
    json += String(gameScores[i]);
    if (i < 5) json += ",";
  }
  json += "],\"gameActive\":";
  json += gameActive ? "true" : "false";
  json += ",\"gameName\":\"";

  String gameNames[] = {"Simon Says", "Whack-a-Mole", "Memory Match", "Reaction Test", "Pattern Race", "LED Chase"};
  if (gameActive && currentGame >= 0 && currentGame < 6) {
    json += gameNames[currentGame];     
  } else {
    json += "None";
  }
  json += "\"}";

  server.send(200, "application/json", json);
}

void handleStartGame() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    int gamePos = body.indexOf("\"game\":");
    if (gamePos > -1) {
      int game = body.substring(gamePos + 7, body.indexOf("}", gamePos)).toInt();
      currentGame = game;
      gameActive = true;
      gameScores[game] = 0;
      gameSequence.clear();
      sequenceIndex = 0;
      gameTimer = millis();
      waitingForInput = false;
      allLEDsOff();

      switch (game) {
        case 0:
          gameState = GAME_1_SIMON_SAYS;
          gameSequence.push_back(random(0, NUM_LEDS));
          break;
        case 1:
          gameState = GAME_2_WHACK_A_MOLE;
          break;
        case 2:
          gameState = GAME_3_MEMORY_MATCH;
          break;
        case 3:
          gameState = GAME_4_REACTION_TEST;
          break;
        case 4:
          gameState = GAME_5_PATTERN_RACE;
          break;
        case 5:
          gameState = GAME_6_LED_CHASE;
          break;
      }
    }
  }
  server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleGameInput() {
  server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleStopGame() {
  endGame();
  server.send(200, "application/json", "{\"status\":\"ok\"}");
}
