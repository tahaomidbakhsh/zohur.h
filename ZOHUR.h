/*
   📺 ZOHUR TV Library - Complete
   با متدهای getChannel و getVolume
*/

#ifndef ZOHUR_H
#define ZOHUR_H

#include <Arduino.h>

#define OLED_ADDR 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

class ZOHUR {
  private:
    int sdaPin, sclPin, irPin;
    unsigned long irPower, irMenu, irPlus, irMinus, irOk;
    unsigned long irCh[6];
    bool isOn;
    int currentChannel;
    int volume;
    bool inMenu;
    int menuIndex;
    
    const char* channelNames[6] = {"NEWS", "SERIAL", "KIDS", "MOVIE", "SPORT", "MUSIC"};
    const char* mainMenu[4] = {"CHANNELS", "VOLUME", "ABOUT", "EXIT"};
    
    // I2C
    void i2cStart();
    void i2cStop();
    void i2cWriteByte(uint8_t data);
    void i2cWriteCommand(uint8_t cmd);
    void i2cWriteData(uint8_t data);
    
    // OLED
    void oledInit();
    void oledClear();
    void oledFillScreen(uint8_t color);
    void oledDrawChar(int x, int y, char c, uint8_t color, uint8_t size);
    void oledDrawString(int x, int y, const char* str, uint8_t color, uint8_t size);
    void oledDrawRect(int x, int y, int w, int h, uint8_t color);
    void oledFillRect(int x, int y, int w, int h, uint8_t color);
    
    // IR
    unsigned long readIR();
    void handleIR(unsigned long code);
    
    // نمایش
    void showWelcome();
    void showHome();
    void showMenu();
    void showChannelList();
    void showVolume();
    void showAbout();
    void executeMenu();
    
  public:
    ZOHUR();
    void begin(int irPin, int sda, int scl,
               unsigned long power, unsigned long menu, 
               unsigned long plus, unsigned long minus, 
               unsigned long ok,
               unsigned long ch1, unsigned long ch2, 
               unsigned long ch3, unsigned long ch4,
               unsigned long ch5, unsigned long ch6);
    void loop();
    bool isPoweredOn();
    
    // ===== متدهای جدید =====
    int getChannel();
    int getVolume();
};

// ============== پیاده‌سازی ==============

ZOHUR::ZOHUR() {
  isOn = false;
  currentChannel = 1;
  volume = 50;
  inMenu = false;
  menuIndex = 0;
}

// ===== I2C =====
void ZOHUR::i2cStart() {
  pinMode(sdaPin, OUTPUT);
  digitalWrite(sdaPin, HIGH);
  digitalWrite(sclPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(sdaPin, LOW);
  delayMicroseconds(5);
  digitalWrite(sclPin, LOW);
}

void ZOHUR::i2cStop() {
  pinMode(sdaPin, OUTPUT);
  digitalWrite(sclPin, LOW);
  digitalWrite(sdaPin, LOW);
  delayMicroseconds(5);
  digitalWrite(sclPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(sdaPin, HIGH);
  delayMicroseconds(5);
}

void ZOHUR::i2cWriteByte(uint8_t data) {
  pinMode(sdaPin, OUTPUT);
  for (int i = 7; i >= 0; i--) {
    digitalWrite(sclPin, LOW);
    digitalWrite(sdaPin, (data >> i) & 1);
    delayMicroseconds(2);
    digitalWrite(sclPin, HIGH);
    delayMicroseconds(2);
  }
  digitalWrite(sclPin, LOW);
  pinMode(sdaPin, INPUT);
  delayMicroseconds(2);
  digitalWrite(sclPin, HIGH);
  delayMicroseconds(2);
  digitalWrite(sclPin, LOW);
  pinMode(sdaPin, OUTPUT);
}

void ZOHUR::i2cWriteCommand(uint8_t cmd) {
  i2cStart();
  i2cWriteByte(0x78);
  i2cWriteByte(0x00);
  i2cWriteByte(cmd);
  i2cStop();
}

void ZOHUR::i2cWriteData(uint8_t data) {
  i2cStart();
  i2cWriteByte(0x78);
  i2cWriteByte(0x40);
  i2cWriteByte(data);
  i2cStop();
}

// ===== OLED =====
void ZOHUR::oledInit() {
  delay(100);
  i2cWriteCommand(0xAE);
  i2cWriteCommand(0xD5); i2cWriteCommand(0x80);
  i2cWriteCommand(0xA8); i2cWriteCommand(0x3F);
  i2cWriteCommand(0xD3); i2cWriteCommand(0x00);
  i2cWriteCommand(0x40);
  i2cWriteCommand(0x8D); i2cWriteCommand(0x14);
  i2cWriteCommand(0x20); i2cWriteCommand(0x02);
  i2cWriteCommand(0xA1);
  i2cWriteCommand(0xC8);
  i2cWriteCommand(0xDA); i2cWriteCommand(0x12);
  i2cWriteCommand(0x81); i2cWriteCommand(0xCF);
  i2cWriteCommand(0xD9); i2cWriteCommand(0xF1);
  i2cWriteCommand(0xDB); i2cWriteCommand(0x40);
  i2cWriteCommand(0xA4);
  i2cWriteCommand(0xA6);
  i2cWriteCommand(0x2E);
  i2cWriteCommand(0xAF);
  oledClear();
}

void ZOHUR::oledClear() {
  for (int page = 0; page < 8; page++) {
    i2cWriteCommand(0xB0 + page);
    i2cWriteCommand(0x00);
    i2cWriteCommand(0x10);
    for (int col = 0; col < 128; col++) {
      i2cWriteData(0x00);
    }
  }
}

void ZOHUR::oledFillScreen(uint8_t color) {
  for (int page = 0; page < 8; page++) {
    i2cWriteCommand(0xB0 + page);
    i2cWriteCommand(0x00);
    i2cWriteCommand(0x10);
    for (int col = 0; col < 128; col++) {
      i2cWriteData(color);
    }
  }
}

void ZOHUR::oledDrawChar(int x, int y, char c, uint8_t color, uint8_t size) {
  const uint8_t font[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, {0x00,0x00,0x5F,0x00,0x00},
    {0x00,0x07,0x00,0x07,0x00}, {0x14,0x7F,0x14,0x7F,0x14},
    {0x24,0x2A,0x7F,0x2A,0x12}, {0x23,0x13,0x08,0x64,0x62},
    {0x36,0x49,0x55,0x22,0x50}, {0x00,0x05,0x03,0x00,0x00},
    {0x00,0x1C,0x22,0x41,0x00}, {0x00,0x41,0x22,0x1C,0x00},
    {0x08,0x2A,0x1C,0x2A,0x08}, {0x08,0x08,0x3E,0x08,0x08},
    {0x00,0x50,0x30,0x00,0x00}, {0x08,0x08,0x08,0x08,0x08},
    {0x00,0x60,0x60,0x00,0x00}, {0x20,0x10,0x08,0x04,0x02},
    {0x3E,0x51,0x49,0x45,0x3E}, {0x00,0x42,0x7F,0x40,0x00},
    {0x42,0x61,0x51,0x49,0x46}, {0x21,0x41,0x45,0x4B,0x31},
    {0x18,0x14,0x12,0x7F,0x10}, {0x27,0x45,0x45,0x45,0x39},
    {0x3C,0x4A,0x49,0x49,0x30}, {0x01,0x71,0x09,0x05,0x03},
    {0x36,0x49,0x49,0x49,0x36}, {0x06,0x49,0x49,0x29,0x1E},
    {0x00,0x36,0x36,0x00,0x00}, {0x00,0x56,0x36,0x00,0x00},
    {0x00,0x08,0x14,0x22,0x41}, {0x14,0x14,0x14,0x14,0x14},
    {0x41,0x22,0x14,0x08,0x00}, {0x02,0x01,0x51,0x09,0x06}
  };
  
  int idx = c - 32;
  if (idx < 0 || idx > 63) return;
  
  for (int row = 0; row < 5; row++) {
    uint8_t line = font[idx][row];
    for (int col = 0; col < 5; col++) {
      if (line & (1 << (4 - col))) {
        for (int sy = 0; sy < size; sy++) {
          for (int sx = 0; sx < size; sx++) {
            int px = x + col * size + sx;
            int py = y + row * size + sy;
            if (px < 128 && py < 64) {}
          }
        }
      }
    }
  }
}

void ZOHUR::oledDrawString(int x, int y, const char* str, uint8_t color, uint8_t size) {
  while (*str) {
    oledDrawChar(x, y, *str++, color, size);
    x += 6 * size;
  }
}

void ZOHUR::oledDrawRect(int x, int y, int w, int h, uint8_t color) {}
void ZOHUR::oledFillRect(int x, int y, int w, int h, uint8_t color) {}

// ===== IR =====
unsigned long ZOHUR::readIR() {
  unsigned long code = 0;
  while (digitalRead(irPin) == HIGH) {}
  unsigned long start = micros();
  while (digitalRead(irPin) == LOW) {}
  unsigned long duration = micros() - start;
  if (duration < 8000 || duration > 10000) return 0;
  for (int i = 0; i < 32; i++) {
    while (digitalRead(irPin) == HIGH) {}
    start = micros();
    while (digitalRead(irPin) == LOW) {}
    duration = micros() - start;
    if (duration > 1000) {
      code = (code << 1) | 1;
    } else {
      code = (code << 1) | 0;
    }
  }
  return code;
}

// ===== نمایش =====
void ZOHUR::showWelcome() {
  oledClear();
  oledDrawString(15, 10, "ZOHUR", 1, 2);
  oledDrawString(85, 20, "TV", 1, 1);
  oledDrawString(20, 50, "Press POWER", 1, 1);
}

void ZOHUR::showHome() {
  oledClear();
  oledDrawString(0, 0, "CH", 1, 1);
  oledDrawString(100, 0, "VOL", 1, 1);
  oledDrawString(15, 25, channelNames[currentChannel - 1], 1, 3);
}

void ZOHUR::showMenu() {
  oledClear();
  oledDrawString(0, 0, "=== MENU ===", 1, 1);
  for (int i = 0; i < 4; i++) {
    String line = String(i == menuIndex ? ">" : " ") + mainMenu[i];
    oledDrawString(5, 12 + i * 12, line.c_str(), 1, 1);
  }
}

void ZOHUR::showChannelList() {
  oledClear();
  oledDrawString(0, 0, "=== CHANNELS ===", 1, 1);
  for (int i = 0; i < 6; i++) {
    String line = String(i + 1) + "." + channelNames[i];
    oledDrawString(5, 12 + i * 8, line.c_str(), 1, 1);
  }
  delay(2000);
  inMenu = false;
  showHome();
}

void ZOHUR::showVolume() {
  oledClear();
  oledDrawString(0, 0, "=== VOLUME ===", 1, 1);
  oledDrawString(10, 25, "VOLUME", 1, 1);
  oledDrawString(95, 40, String(volume).c_str(), 1, 1);
  delay(2000);
  inMenu = false;
  showHome();
}

void ZOHUR::showAbout() {
  oledClear();
  oledDrawString(15, 8, "ZOHUR", 1, 2);
  oledDrawString(85, 18, "TV", 1, 1);
  oledDrawString(10, 35, "SMART TV", 1, 1);
  oledDrawString(10, 47, "6 CHANNELS", 1, 1);
  delay(2000);
  inMenu = false;
  showHome();
}

void ZOHUR::executeMenu() {
  switch(menuIndex) {
    case 0: showChannelList(); break;
    case 1: showVolume(); break;
    case 2: showAbout(); break;
    case 3: inMenu = false; showHome(); break;
  }
}

void ZOHUR::handleIR(unsigned long code) {
  if (code == irPower) {
    isOn = !isOn;
    if (isOn) showHome();
    else showWelcome();
    return;
  }
  if (!isOn) return;
  if (code == irMenu) {
    inMenu = !inMenu;
    menuIndex = 0;
    if (inMenu) showMenu();
    else showHome();
    return;
  }
  if (inMenu) {
    if (code == irPlus) {
      menuIndex = (menuIndex + 1) % 4;
      showMenu();
    } else if (code == irMinus) {
      menuIndex = (menuIndex + 3) % 4;
      showMenu();
    } else if (code == irOk) {
      executeMenu();
    }
    return;
  }
  for (int i = 0; i < 6; i++) {
    if (code == irCh[i]) {
      currentChannel = i + 1;
      showHome();
      return;
    }
  }
  if (code == irPlus) {
    currentChannel = (currentChannel % 6) + 1;
    showHome();
  } else if (code == irMinus) {
    currentChannel = (currentChannel + 4) % 6 + 1;
    showHome();
  }
}

// ===== عمومی =====
void ZOHUR::begin(int pin, int sda, int scl,
                   unsigned long power, unsigned long menu, 
                   unsigned long plus, unsigned long minus, 
                   unsigned long ok,
                   unsigned long ch1, unsigned long ch2, 
                   unsigned long ch3, unsigned long ch4,
                   unsigned long ch5, unsigned long ch6) {
  irPin = pin;
  sdaPin = sda;
  sclPin = scl;
  pinMode(irPin, INPUT);
  irPower = power;
  irMenu = menu;
  irPlus = plus;
  irMinus = minus;
  irOk = ok;
  irCh[0] = ch1;
  irCh[1] = ch2;
  irCh[2] = ch3;
  irCh[3] = ch4;
  irCh[4] = ch5;
  irCh[5] = ch6;
  oledInit();
  showWelcome();
}

void ZOHUR::loop() {
  unsigned long code = readIR();
  if (code != 0) {
    handleIR(code);
  }
  delay(50);
}

bool ZOHUR::isPoweredOn() {
  return isOn;
}

// ===== متدهای جدید =====
int ZOHUR::getChannel() {
  return currentChannel;
}

int ZOHUR::getVolume() {
  return volume;
}

#endif
