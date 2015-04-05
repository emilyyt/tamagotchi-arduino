#include <ST7565.h>

int ledPin =  13;    // LED connected to digital pin 13
int thermistor = A5; //normal room temp value is around 250
int photocell = A1; //above 700
int foodSwitch = 4;

#define THERM_ABOVE 800
#define THERM_BELOW 760
#define PHOTO_ABOVE 450
#define PHOTO_BELOW 430
int stage = 1; //4 stages: Egg, Toddler, Teenager, Adult
#define PROGRESS_MAX 11
int progress = 0; //when reach progress level 10, the pet will move to next stage of life
int mood = 1; // 0 = sad, 1 = indifferent and 2 = happy
int happy = 0;
int indifferent = 0;
int sad = 0;

int switchVal = 0;
int thermistorVal = 0;
int photocellVal = 0;
#define COUNTER_MAX 3
int counter = COUNTER_MAX/2;
int deathWarning = 0;
int num = 0;
// pin 9 - Serial data out (SID)
// pin 8 - Serial clock out (SCLK)
// pin 7 - Data/Command select (RS or A0)
// pin 6 - LCD reset (RST)
// pin 5 - LCD chip select (CS)
ST7565 glcd(9, 8, 7, 6, 5);

#define GRASS_HEIGHT 8
#define GRASS_WIDTH  13 
// a bitmap of a 13x8 grass
static unsigned char __attribute__ ((progmem)) grass_bmp[]={
  0xc0, 0xe0, 0xf2, 0xfc, 0xc0, 0xe0, 0xf4, 0xf8, 0xc0, 0xe0, 0xf2, 0xfc, 0xe0 };

#define EGG_HEIGHT 16
#define EGG_WIDTH 12
#define EGG_X 54
#define EGG_Y 47

static unsigned char __attribute__ ((progmem)) egg_bmp[]={
  0xf0, 0x08, 0x04, 0x02, 0x01, 0x01, 0x01, 0x01, 0x02, 0x04, 0x08, 0xf0,
  0x07, 0x08, 0x10, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x08, 0x07};

#define CRACK_WIDTH 10
#define CRACK_HEIGHT 4
#define CRACK_X EGG_X+1
#define CRACK_Y EGG_Y+4
static unsigned char __attribute__ ((progmem)) crack_bmp[] ={
  0x8, 0x4, 0x2, 0x4, 0x8, 0x4, 0x2, 0x4, 0x8, 0x4
};

#define ADULT_WIDTH 12
#define ADULT_HEIGHT 16
static unsigned char __attribute__ ((progmem)) adult_sad[]={
  0x78, 0x84, 0x02, 0x21, 0x15, 0x11, 0x11, 0x15, 0x21, 0x02, 0x84, 0x78, 
  0x30, 0x28, 0x25, 0xa3, 0xe5, 0x29, 0x29, 0xe5, 0xa3, 0x25, 0x28, 0x30 
};
static unsigned char __attribute__ ((progmem)) adult_happy[]={
  0x78, 0x84, 0x02, 0x11, 0x25, 0x21, 0x21, 0x25, 0x11, 0x02, 0x84, 0x78,
  0x31, 0x2a, 0x25, 0xa3, 0xe5, 0x29, 0x29, 0xe5, 0xa3, 0x25, 0x2a, 0x31 
};
static unsigned char __attribute__ ((progmem)) adult_indifferent[]={
  0x78, 0x84, 0x02, 0x01, 0x25, 0x21, 0x21, 0x25, 0x01, 0x02, 0x84, 0x78, 
  0x32, 0x2a, 0x27, 0xa3, 0xe1, 0x21, 0x21, 0xe1, 0xa3, 0x27, 0x2a, 0x32
};

#define TEENAGER_WIDTH 14
#define TEENAGER_HEIGHT 16
static unsigned char __attribute__ ((progmem)) teenager_happy[]={
  0x00, 0xc0, 0x20, 0x10, 0x08, 0x48, 0x08, 0x08, 0x48, 0x08, 0x10, 0x20, 0xc0, 0x00, 
  0x10, 0x0b, 0x04, 0x08, 0x91, 0xe2, 0x22, 0x22, 0xe2, 0x91, 0x08, 0x04, 0x0b, 0x10
};
static unsigned char __attribute__ ((progmem)) teenager_sad[]={
  0x00, 0xc0, 0x20, 0x10, 0x08, 0x48, 0x08, 0x08, 0x48, 0x08, 0x10, 0x20, 0xc0, 0x00,
  0x10, 0x0b, 0x04, 0x08, 0x92, 0xe1, 0x21, 0x21, 0xe1, 0x92, 0x08, 0x04, 0x0b, 0x10 
};
static unsigned char __attribute__ ((progmem)) teenager_indifferent[]={
  0x00, 0xc0, 0x20, 0x10, 0x08, 0x48, 0x08, 0x08, 0x48, 0x08, 0x10, 0x20, 0xc0, 0x00, 
  0x10, 0x0b, 0x04, 0x08, 0x90, 0xe2, 0x22, 0x22, 0xe2, 0x90, 0x08, 0x04, 0x0b, 0x10
};

#define TODDLER_HEIGHT 16
#define TODDLER_WIDTH 12
static unsigned char __attribute__ ((progmem)) toddler_sad[]={
  0xf0, 0x08, 0x04, 0x42, 0x29, 0x21, 0x21, 0x29, 0x42, 0x04, 0x08, 0xf0,
  0x00, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08, 0x08, 0x04, 0x02, 0x01, 0x00
};
static unsigned char __attribute__ ((progmem)) toddler_happy[]={
  0xf0, 0x08, 0x04, 0x22, 0x49, 0x41, 0x41, 0x49, 0x22, 0x04, 0x08, 0xf0,
  0x00, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08, 0x08, 0x04, 0x02, 0x01, 0x00
};
static unsigned char __attribute__ ((progmem)) toddler_indifferent[]={
  0xf0, 0x08, 0x04, 0x02, 0x49, 0x41, 0x41, 0x49, 0x02, 0x04, 0x08, 0xf0,
  0x00, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08, 0x08, 0x04, 0x02, 0x01, 0x00
};

void setup()   { 

  Serial.begin(9600);
//  Serial.println(freeRam());
  pinMode(photocell, INPUT);
  pinMode(thermistor, INPUT);
  pinMode(foodSwitch, INPUT);


  // initialize and set the contrast to 0x18
  glcd.begin(0x18);
  glcd.clear(); //clear preset splash screen
  background();
  
//  drawEgg();
  glcd.display();
  //
  //  drawEgg();
  //  glcd.display();
  //  delay(200);

}

void loop()
{

  thermistorVal = analogRead(thermistor);
  photocellVal = analogRead(photocell);
  switchVal = digitalRead(foodSwitch);
  Serial.print("photocell: ");
  Serial.println(photocellVal);
//  Serial.print("thermistor: ");
//  Serial.println(thermistorVal); 
//  Serial.print("counter: ");
//  Serial.println(counter);
//  Serial.print("mood: ");
//  Serial.println(mood);
//  Serial.print("progress: ");
//  Serial.println(progress);
//  delay(500);

  petSetup();

  ////   drawEgg();
  //  drawPet(toddler_sad, TODDLER_WIDTH, TODDLER_HEIGHT);
  //  glcd.clear();
  //  drawAdult("sad");
}

void petSetup(){
  updateCounter();
  updateMood();
  updateStage();
  if(stage == 1){
    drawEgg();
    background();
  }
  else if(stage == 2){
    background();
    if(mood == 0){
      drawPet(toddler_sad, TODDLER_WIDTH, TODDLER_HEIGHT);
    } 
    else if(mood == 1){
      drawPet(toddler_indifferent, TODDLER_WIDTH, TODDLER_HEIGHT);
    }
    else if(mood ==2){
      drawPet(toddler_happy, TODDLER_WIDTH, TODDLER_HEIGHT);
    }
  }
  else if(stage == 3){
    background();
    if(mood == 0){
      glcd.clear();
      glcd.drawbitmap(EGG_X, EGG_Y-2, teenager_sad, TEENAGER_WIDTH, TEENAGER_HEIGHT, BLACK);
      background();
      glcd.display();  
  } 
    else if(mood == 1){
      glcd.clear();
      glcd.drawbitmap(EGG_X, EGG_Y-2, teenager_indifferent, TEENAGER_WIDTH, TEENAGER_HEIGHT, BLACK);
      background();
      glcd.display();  
  }
    else if(mood == 2){
      glcd.clear();
      glcd.drawbitmap(EGG_X, EGG_Y-2, teenager_happy, TEENAGER_WIDTH, TEENAGER_HEIGHT, BLACK);
      background();
      glcd.display();  
  }
  }
  else if(stage == 4){
    background();
    glcd.clear();
    if(mood == 0){
      glcd.drawbitmap(EGG_X, EGG_Y-3, adult_sad, ADULT_WIDTH, ADULT_HEIGHT, BLACK);
      background();
      glcd.display();  
      } 
    else if(mood == 1){
      glcd.drawbitmap(EGG_X, EGG_Y-3, adult_indifferent, ADULT_WIDTH, ADULT_HEIGHT, BLACK);
      background();
      glcd.display();
    }
    else if(mood == 2){
      glcd.drawbitmap(EGG_X, EGG_Y-3, adult_happy, ADULT_WIDTH, ADULT_HEIGHT, BLACK);
      background();
      glcd.display();
    }
  }

}

//need to update
void updateCounter(){
  //light count
  if (photocellVal >= PHOTO_ABOVE && counter <= COUNTER_MAX){
    counter++;
  }else if(photocellVal <= PHOTO_BELOW && counter>0){
    counter-=2;
  }
  //thermistor count
  if (thermistorVal >= THERM_ABOVE && counter <= COUNTER_MAX){
    counter++;
  } 
  else if(thermistorVal <= THERM_BELOW && counter>0){
       counter--;
  }
}

void updateMood(){
  if(mood == 0 && counter >= COUNTER_MAX){
    mood = 1;
    counter = 1;
  }
  else if(mood == 1 && counter >= COUNTER_MAX){
    mood = 2;
    counter = 1;
  } 
  else if(mood == 2 && counter >= COUNTER_MAX && progress <= PROGRESS_MAX){
    progress++;
    counter = 1;
  } 
  else if (mood == 1 && counter == 0){
    mood = 0;
  }
  else if (mood == 2 && counter == 0){
    mood = 1;
  } 
}


void updateStage(){
  if(stage == 1 && progress == PROGRESS_MAX){
    stage = 2;
    progress = 5;
    glcd.clear();
    glcd.display();
  } 
  else if(stage == 2 && progress == PROGRESS_MAX){
    stage = 3;
    progress = 0;
    glcd.clear();
    glcd.display();
  } 
  else if(stage == 3 && progress == PROGRESS_MAX){
    stage = 4;
    progress = 0;
    glcd.clear();
    glcd.display();
  }
}

void drawProgress(){
  int x = 3;
  int y = 3;
    glcd.drawrect(0,0,3,12,BLACK);
    glcd.drawline(1, 0, 1, progress, BLACK);
    if(progress > PROGRESS_MAX-1){
      glcd.drawline(1, 0, 1, progress, 0);
    }
 

}

//Need testing
//draws and toggles pet
void drawPet(unsigned char pet_bitmap[], int w, int h){
  glcd.drawbitmap(EGG_X, EGG_Y, pet_bitmap, w, h, BLACK);
  glcd.display();
  delay(500);
  glcd.drawbitmap(EGG_X, EGG_Y, pet_bitmap, w, h, 0);
  glcd.display();
  glcd.drawbitmap(EGG_X, EGG_Y-1, pet_bitmap, w, h, BLACK);
  glcd.display();
  delay(500);
  glcd.drawbitmap(EGG_X, EGG_Y-1, pet_bitmap, w, h, 0);
  background();
  glcd.display();
}

void drawEgg(){

  glcd.drawbitmap(EGG_X, EGG_Y, egg_bmp,  EGG_WIDTH, EGG_HEIGHT, BLACK);
  glcd.drawbitmap(CRACK_X, CRACK_Y, crack_bmp,  CRACK_WIDTH-(10-progress), CRACK_HEIGHT, BLACK);
  glcd.display();
}

void background(){
  if(stage < 4){
    drawProgress();
  } else{
    glcd.clear();
  }
  for (int i=0; i<=128; i++){
    if (i % 13 == 0){
      glcd.drawbitmap(i, 56, grass_bmp,  GRASS_WIDTH, GRASS_HEIGHT, BLACK);
    }
  }
  
}

// this handy function will return the number of bytes currently free in RAM, great for debugging! 
int freeRam(void)
{
  extern int  __bss_end; 
  extern int  *__brkval; 
  int free_memory; 
  if((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__bss_end); 
  }
  else {
    free_memory = ((int)&free_memory) - ((int)__brkval); 
  }
  return free_memory; 
} 

