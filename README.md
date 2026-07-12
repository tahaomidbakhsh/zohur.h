# 📺 ZOHUR TV Library

کتابخونه کامل تلویزیون هوشمند برای آردوینو
بدون نیاز به هیچ کتابخونه‌ای (OLED + IR)

## ✨ قابلیت‌ها
- کنترل کامل OLED 0.96"
- گیرنده IR داخلی
- ۶ کانال تلویزیونی
- منوی کامل
- بدون نیاز به کتابخونه اضافی

## 📥 نصب
1. دانلود از گیت‌هاب
2. انتقال به پوشه `libraries` آردوینو
3. ریستارت Arduino IDE

## 🔌 اتصالات
| قطعه | پین |
|-------|-----|
| OLED SDA | A4 |
| OLED SCL | A5 |
| گیرنده IR | 11 |

## 🎮 مثال استفاده

\`\`\`cpp
#include <ZOHUR.h>

ZOHUR tv;

void setup() {
  tv.begin(11, A4, A5,
           KEY_POWER, KEY_MENU, KEY_PLUS, KEY_MINUS, KEY_OK,
           KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6);
}

void loop() {
  tv.loop();
}
\`\`\`
