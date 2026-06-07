#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>

// ==================== НАЛАШТУВАННЯ WI-FI ====================
const char* ssid = "ESP32_Cyber_Car";
const char* password = "123456789";

// ==================== КОНФІГУРАЦІЯ ПІНІВ ====================
const int PIN_L_AIN2 = 12;
const int PIN_L_AIN1 = 14;
const int PIN_R_BIN1 = 27;
const int PIN_R_BIN2 = 26;

const int PIN_HEADLIGHTS = 2;   
const int PIN_TAIL_LIGHTS = 4;  
const int PIN_TURN_L = 16;      
const int PIN_TURN_R = 17;      
const int PIN_STOP = 19;        // Зарезервовано D19, звільнивши D22 під I2C
const int PIN_REVERSE = 18;     
const int PIN_HORN = 25;        

// ==================== ГЛОБАЛЬНІ ЗМІННІ СТАНУ ====================
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

bool parkingBrake = false;
bool hazardActive = false;

bool turnLeftActive = false;
bool turnRightActive = false;
unsigned long lastBlinkTime = 0;
bool blinkState = false;

char currentDir = 'S';
int currentSpeed = 0;
int currentBalance = 0;


// ==================== АПАРТАТНА ЛОГІКА РУХУ ====================
void executePhysicalMovement(char dir, int speed, int balance) {
 
}

// ==================== ОБРОБКА WEBSOCKET КОМАНД СЕРВЕРА ====================
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  
  // Діагностика підключень клієнтів
  if (type == WS_EVT_CONNECT) {
    Serial.printf("[WS] Нове підключення! Клієнт ID: %u | IP: %s\n", client->id(), client->remoteIP().toString().c_str());
  } 
  else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("[WS] Клієнт ID: %u відключився.\n", client->id());
  } 
  else if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      data[len] = 0;
      
      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, (char*)data);
      if (error) {
        Serial.print("[WS][ERROR] Помилка парсингу JSON: ");
        Serial.println(error.c_str());
        return;
      }

      String msgType = doc["type"].as<String>();

      // 1. Команда руху
      if (msgType == "move") {
        String dirStr = doc["dir"].as<String>();
        currentDir = (dirStr.length() > 0) ? dirStr[0] : 'S';
        currentSpeed = doc["speed"];
        currentBalance = doc["balance"];
        executePhysicalMovement(currentDir, currentSpeed, currentBalance);
      } 
      // 2. Команда систем безпеки
      else if (msgType == "safety") {
        String name = doc["name"].as<String>();
        bool state = doc["state"];
        Serial.printf("[SAFETY] Зміна стану безпеки: %s -> %s\n", name.c_str(), state ? "УВІМК" : "ВИМК");
        
        if (name == "brake") {
          parkingBrake = state;
          executePhysicalMovement(currentDir, currentSpeed, currentBalance);
        } else if (name == "hazard") {
          hazardActive = state;
          if (!state) { 
            digitalWrite(PIN_TURN_L, LOW);
            digitalWrite(PIN_TURN_R, LOW);
          }
        }
      }
      // 3. Ручне керування світлом
      else if (msgType == "light") {
        String name = doc["name"].as<String>();
        bool state = doc["state"];
        Serial.printf("[LIGHT] Ручне перемикання: %s -> %s\n", name.c_str(), state ? "ON" : "OFF");
        
        if (name == "head")        digitalWrite(PIN_HEADLIGHTS, state ? HIGH : LOW);
        else if (name == "tail")   digitalWrite(PIN_TAIL_LIGHTS, state ? HIGH : LOW);
        else if (name == "left")  { turnLeftActive = state;  if(!state) digitalWrite(PIN_TURN_L, LOW); }
        else if (name == "right") { turnRightActive = state; if(!state) digitalWrite(PIN_TURN_R, LOW); }
      } 
      // 4. Гудок
      else if (msgType == "horn") {
        bool state = doc["state"];
        Serial.printf("[HORN] Стан гудка -> %s\n", state ? "АКТИВНИЙ ЗВУК" : "ТИША");
        if (state) ledcWriteTone(4, 1000);
        else ledcWriteTone(4, 0);
      }
    }
  }
}

// ==================== ІНІЦІАЛІЗАЦІЯ СИСТЕМИ ====================
void setup() {
  Serial.begin(115200);
  delay(500); // Невелика затримка для стабілізації Serial порту після скидання

  Serial.println("\n==============================================");
  Serial.println("        ІНІЦІАЛІЗАЦІЯ РОБОМАШИНКИ ESP32       ");
  Serial.println("==============================================");
  Serial.printf("[SYSTEM] Версія ядра ESP32 SDK: %s\n", ESP.getSdkVersion());
  Serial.printf("[SYSTEM] Вільна пам'ять оперативки: %d байт\n", ESP.getFreeHeap());

  // Конфігурація цифрових виходів
  pinMode(PIN_HEADLIGHTS, OUTPUT);
  pinMode(PIN_TAIL_LIGHTS, OUTPUT);
  pinMode(PIN_TURN_L, OUTPUT);
  pinMode(PIN_TURN_R, OUTPUT);
  pinMode(PIN_STOP, OUTPUT);
  pinMode(PIN_REVERSE, OUTPUT);

  // Початковий стан фар
  digitalWrite(PIN_STOP, HIGH); 
  Serial.println("[LIGHT] Конфігурацію пінів виконано. Стоп-сигнал увімкнено за замовчуванням.");

  // Конфігурація ШІМ (Ядро 2.0.17)
  Serial.println("[PWM] Налаштування каналів ШІМ 0-3 (20 кГц) для двигунів...");
  ledcSetup(0, 20000, 8); ledcAttachPin(PIN_L_AIN1, 0);
  ledcSetup(1, 20000, 8); ledcAttachPin(PIN_L_AIN2, 1);
  ledcSetup(2, 20000, 8); ledcAttachPin(PIN_R_BIN1, 2);
  ledcSetup(3, 20000, 8); ledcAttachPin(PIN_R_BIN2, 3);
  
  Serial.println("[PWM] Налаштування каналу ШІМ 4 для п'єзодинаміка гудка...");
  ledcSetup(4, 2000, 8);  ledcAttachPin(PIN_HORN, 4);

  // Налаштування Wi-Fi Точки Доступу
  Serial.print("[WiFi] Створення точки доступу мережі: ");
  Serial.println(ssid);
  WiFi.softAP(ssid, password);
  
  Serial.print("[WiFi] Успішно! Точка доступу розгорнута. IP адреса машинки: ");
  Serial.println(WiFi.softAPIP());

  // Старт серверу та сокетів
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.begin();
  Serial.println("[SYSTEM] Асинхронний веб-сервер успішно запущено на порті 80.");
  Serial.println("==============================================\n[SYSTEM] Очікування підключення смартфона...");
}

// ==================== ОСНОВНИЙ ЦИКЛ БЛИМАННЯ ====================
void loop() {
  ws.cleanupClients();


}
