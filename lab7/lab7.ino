#include <OneWire.h>
#include <LiquidCrystal_I2C.h>
#include <DallasTemperature.h>
#include <IRremote.h>

#define LED_EXTRA_PIN 7
#define LED_CHILL_PIN 6

#define DS_PIN 4
#define IR_PIN 3

int main() {
  init();

  DDRD |= 0b11000000;
  PORTD &= ~0b11000000;

  OneWire ow(DS_PIN);
  DallasTemperature ds(&ow);
  LiquidCrystal_I2C lcd(0x27, 16, 2);
  
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  ds.begin();

  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

  uint32_t last_ms = millis();
 
  while (true) {
    uint32_t now_ms = millis();
    
    if ((now_ms - last_ms) >= 1000) {
      last_ms = now_ms;

      ds.requestTemperatures();
      float temp = ds.getTempCByIndex(0);

      lcd.setCursor(0, 0); 
      lcd.print("T: "); 
      lcd.print(temp); 
      lcd.print(" C");

      Serial.println(temp);
    }

    if (IrReceiver.decode()) {
      int cmd = IrReceiver.decodedIRData.command;
      
      Serial.print("cmd: 0x");
      Serial.println(cmd, HEX);
      
      IrReceiver.resume();
    }
    
    _delay_us(10);
  }

  return 0;
}