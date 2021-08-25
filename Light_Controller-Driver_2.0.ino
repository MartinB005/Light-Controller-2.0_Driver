#include <LiquidCrystal_I2C.h>
#include <TimedAction.h>

class Light {

  public:
    
    int number;
    int colorPins[];
   
    void connect(int redPin, int greenPin, int bluePin){
      pinMode(redPin, OUTPUT);
      pinMode(greenPin, OUTPUT);
      pinMode(bluePin, OUTPUT);
      colorPins[0] = redPin;
      colorPins[1] = greenPin;
      colorPins[2] = bluePin;
    }

    void attachMotors(int xPin, int yPin){
      
    }

    void setRed(int value) {
      int voltage = map(value, 0, 255, 0, 150);
      analogWrite(colorPins[0], voltage);
    }

    void setGreen(int value) {
      int voltage = map(value, 0, 255, 0, 150);
      analogWrite(colorPins[1], voltage);
    }
    
    void setBlue(int value) {
      int voltage = map(value, 0, 255, 0, 150);
      analogWrite(colorPins[2], voltage);
    }

    void fadeOut(int colors[], int duration, int offset) {
      backgroundFade(colors, duration, offset);
  }


  private:
  
    void backgroundFade(int colors[], int duration, int offset) {
      for(int i = 0; i < 3; i++){
        if(colors[i] != 0) {
          int value = colors[i];
          float downFactor = duration / (value - offset);   
          while(value > offset) {
            if(i == 0) setRed(value);
            else if(i == 1) setGreen(value);
            else setBlue(value);
            value -= downFactor;
          }
        }
      }
    }
};

LiquidCrystal_I2C lcd(0x27,16,2); 
int counterPins[] = {50, 51, 52, 53};
int vuMeterPins[] = {48, 46, 44, 42, 49, 47};
int audioValues[] = {370, 400, 430, 460, 490, 520};
int currentBeat;
int limiter;
Light light[4];


void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(115200);
  for(int pin : counterPins) pinMode(pin, OUTPUT);
  for(int vuPin : vuMeterPins) pinMode(vuPin, OUTPUT);
  pinMode(A0,INPUT);
  light[0].connect(2, 3, 4);
  light[1].connect(8, 9, 10);
}


TimedAction readingThread = TimedAction(10, readAudio);

void readAudio() {
  int  input = analogRead(A0);
  int progress = map(input, 340, 510, 100, 0);
  String message = "-audio:" + String(progress);
  if(progress > 0) Serial.println(message);
  for(int i = 0; i < sizeof(vuMeterPins); i++) {
     digitalWrite(vuMeterPins[i], input < audioValues[i] ? HIGH : LOW);
  }
  updateAllLights();
  limiter = 0;
}

void loop() {
  String message = "";
  while(Serial.available()) {
      char c = Serial.read();
      message += c;
  }
  if (message.length() > 0) {
    Serial.read();
    String key = message.substring(message.indexOf("-")+1, message.indexOf(':'));
    String data = message.substring(message.indexOf(':') + 1);
    if(key.equals("beat")) {
        int number = data.toInt();
        if(number != 0) onBeat(number);
    }
    else if(key.equals("lcd")) lcd.print(data);
    delay(1);
  }
  readingThread.check();
}
 


void onBeat(int number) {
  currentBeat = number;
  updateAllLights();
}

void updateAllLights() {
   for(int i = 0; i < 4; i++){
      digitalWrite(counterPins[i], currentBeat == i+1 ? HIGH : LOW);
  }
  analogWrite(light[0].colorPins[0], currentBeat == 1 ? 150 : 0);
  analogWrite(light[0].colorPins[1], currentBeat == 2 ? 150 : 0);
  analogWrite(light[0].colorPins[2], currentBeat == 3 ? 150 : 0);
  //light[1].setGreen(currentBeat == 2 ? 255 : 0);
  //light[1].setBlue(currentBeat == 3 ? 255 : 0);
  light[0].setRed(currentBeat == 1 ? 255 : 0);
  light[0].setGreen(currentBeat == 2 ? 255 : 0);
  light[0].setBlue(currentBeat == 3 ? 255 : 0);
}
