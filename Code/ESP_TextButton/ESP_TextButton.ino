#define TITLE           "P10 RGB TEXT DISPLAY BUTTON"
#include <PxMatrix.h>

// Pins for LED MATRIX
#define P_LAT 22
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 5
#define P_E 15
#define P_OE 2
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

#define matrix_width 64
#define matrix_height 16

#define GREEN_PIN 25 
#define RED_PIN 26

int L, bstate, lbstate, bstate2, lbstate2;
bool full = false;
unsigned long previousMillis = 0;   // Stores last time temperature was publishe
const long interval = 200;
uint8_t display_draw_time = 60; //30-70 is usually fine, bigger will crash esp
PxMATRIX display(64,16,P_LAT, P_OE,P_A,P_B,P_C);

// Some standard colors
uint16_t myRED = display.color565(255, 0, 0);
uint16_t myGREEN = display.color565(0, 255, 0);
uint16_t myBLUE = display.color565(0, 0, 255);
uint16_t myWHITE = display.color565(255, 255, 255);
uint16_t myYELLOW = display.color565(255, 255, 0);
uint16_t myCYAN = display.color565(0, 255, 255);
uint16_t myMAGENTA = display.color565(255, 0, 255);
uint16_t myBLACK = display.color565(0, 0, 0);

uint16_t myCOLORS[8]={myRED,myGREEN,myBLUE,myWHITE,myYELLOW,myCYAN,myMAGENTA,myBLACK};

void IRAM_ATTR display_updater(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  display.display(display_draw_time);
  portEXIT_CRITICAL_ISR(&timerMux);
}

void display_update_enable(bool is_enable)
{
  if (is_enable)
  {
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &display_updater, true);
    timerAlarmWrite(timer, 4000, true);
    timerAlarmEnable(timer);
  }
  else
  {
    timerDetachInterrupt(timer);
    timerAlarmDisable(timer);
  }
}

void setup() {

 Serial.begin(115200);
  // Define your display layout here, e.g. 1/8 step, optional SPI pins begin(row_pattern, CLK, MOSI, MISO, SS)
  display.begin(4);
  display.setPanelsWidth(2);  //display.begin(8, 14, 13, 12, 4);
  display.clearDisplay();
  display.setTextColor(myCYAN);
  display.setCursor(2,4);
  display.print("Pixel");
  display.setTextColor(myMAGENTA);
  display.setCursor(33,4);
  display.print("Time");
  display_update_enable(true);
 // display.setMuxDelay(1,1,1,1,0);

  pinMode(GREEN_PIN, INPUT_PULLUP);
  pinMode(RED_PIN, INPUT_PULLUP);
  bstate = digitalRead(GREEN_PIN);
  bstate2 = digitalRead(RED_PIN);
  delay(3000);

  Serial.println("Vacant");
  // Define multiplex implemention here {BINARY, STRAIGHT} (default is BINARY)
  //display.setMuxPattern(BINARY);

  // Set the multiplex pattern {LINE, ZIGZAG,ZZAGG, ZAGGIZ, WZAGZIG, VZAG, ZAGZIG} (default is LINE)
  display.setScanPattern(ZAGZIG);

}

void show_text(uint8_t xpos, uint8_t ypos, String text, uint8_t colorR, uint8_t colorG, uint8_t colorB)
{
    display.setTextWrap(true);  // we don't wrap text so it scrolls nicely
    //display.setTextSize(1);
    display.setTextColor(display.color565(colorR,colorG,colorB));
    display.clearDisplay();
    display.setCursor(xpos,ypos); 
    display.println(text);
    yield();
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    display.clearDisplay();
   
   //-----GREEN is vacant
   lbstate = bstate;      // save the last state
   bstate = digitalRead(GREEN_PIN); // read new state
  
   if (lbstate == HIGH && bstate == LOW) {
     if (full){
        delay(300);
        Serial.println("Vacant");
        full = false;
     }
   }
  
   //-----RED is full
   lbstate2 = bstate2;      
   bstate2 = digitalRead(RED_PIN);
  
   if (lbstate2 == HIGH && bstate2 == LOW) {
     if (!full){
        delay(300);
        Serial.println("Full");
        full = true;
     }
   }

  //-----display text
   if (!full){
      show_text(15, 5,"VACANT",0,250,0); //8 zagzig
   }
   else{
      show_text(22, 5,"FULL",250,0,0);
   }
 }
}
