#include <OneWire.h>
#include <LiquidCrystal_I2C.h>
#include <DallasTemperature.h>
#include <IRremote.h>

#define DS_PIN 4
#define IR_PIN 3

static OneWire ow(DS_PIN);
static DallasTemperature ds(&ow);
static LiquidCrystal_I2C lcd(0x27, 16, 2);

int main() {
  DDRD |= 0b11000000;
  PORTD &= ~0b11000000;
  
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  ds.begin();

  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

  uint16_t time_cnt = 0;
 
  while (true) {
    if (time_cnt >= 10000) {
      time_cnt = 0;

      ds.requestTemperatures();
      float temp = ds.getTempCByIndex(0);

      lcd.setCursor(0, 0); 
      lcd.print("T: "); 
      lcd.print(temp); 
      lcd.print(" C");

      Serial.println(temp);
    } else {
      time_cnt += 1;
    }

    if (IrReceiver.decode()) {
      int cmd = IrReceiver.decodedIRData.command;
      
      Serial.print("cmd: 0x");
      Serial.println(cmd, HEX);
      
      IrReceiver.resume();
    }
    
    _delay_us(100);
  }

  return 0;
}