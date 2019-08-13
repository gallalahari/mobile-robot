#include <EnableInterrupt.h>
#include <Stepper.h>
#include <Servo.h>

#define SERIAL_PORT_SPEED 57600
#define RC_NUM_CHANNELS  6

#define RC_CH1  0
#define RC_CH2  1
#define RC_CH3  2
#define RC_CH4  3
#define RC_CH5  4
#define RC_CH6  5

#define RC_CH1_INPUT  A8
#define RC_CH2_INPUT  A9
#define RC_CH3_INPUT  A10
#define RC_CH4_INPUT  A11
#define RC_CH5_INPUT  A12
#define RC_CH6_INPUT  A13

Servo servo1;
Servo servo2;
Servo servo3;

int en1 = 2;
int en2 = 3;

int en3 = 4;
int en4 = 5;

int enA = 10;
int enB = 11;

int bs =200;
int pos1 = 90, pos2 = 0;
//for stepper motor
const int stepsPerRevolution = 200; 
//connection for motordriver to arduinoor orderwise
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

//for rc receiver 
uint16_t rc_values[RC_NUM_CHANNELS];
uint32_t rc_start[RC_NUM_CHANNELS];
volatile uint16_t rc_shared[RC_NUM_CHANNELS];

void rc_read_values() {
  noInterrupts();
  memcpy(rc_values, (const void *)rc_shared, sizeof(rc_shared));
  interrupts();
}

void calc_input(uint8_t channel, uint8_t input_pin) {
  if (digitalRead(input_pin) == HIGH) {
    rc_start[channel] = micros();
  } else {
    uint16_t rc_compare = (uint16_t)(micros() - rc_start[channel]);
    rc_shared[channel] = rc_compare;
  }
}

void calc_ch1() { calc_input(RC_CH1, RC_CH1_INPUT); }
void calc_ch2() { calc_input(RC_CH2, RC_CH2_INPUT); }
void calc_ch3() { calc_input(RC_CH3, RC_CH3_INPUT); }
void calc_ch4() { calc_input(RC_CH4, RC_CH4_INPUT); }
void calc_ch5() { calc_input(RC_CH5, RC_CH5_INPUT); }
void calc_ch6() { calc_input(RC_CH6, RC_CH6_INPUT); }

void setup() {
  Serial.begin(SERIAL_PORT_SPEED);
  servo1.attach(12);
  servo2.attach(13);
  servo3.attach(14);
  myStepper.setSpeed(50);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(RC_CH1_INPUT, INPUT);
  pinMode(RC_CH2_INPUT, INPUT);
  pinMode(RC_CH3_INPUT, INPUT);
  pinMode(RC_CH4_INPUT, INPUT);
  pinMode(RC_CH5_INPUT, INPUT);
  pinMode(RC_CH6_INPUT, INPUT);
  enableInterrupt(RC_CH1_INPUT, calc_ch1, CHANGE);
  enableInterrupt(RC_CH2_INPUT, calc_ch2, CHANGE);
  enableInterrupt(RC_CH3_INPUT, calc_ch3, CHANGE);
  enableInterrupt(RC_CH4_INPUT, calc_ch4, CHANGE);
  enableInterrupt(RC_CH5_INPUT, calc_ch5, CHANGE);
  enableInterrupt(RC_CH6_INPUT, calc_ch6, CHANGE);
}

void loop() {
  rc_read_values();

  pinMode(8, LOW);
  pinMode(9, LOW);
  pinMode(10, LOW);
  pinMode(11, LOW);

  Serial.print("CH1:"); Serial.print(rc_values[RC_CH1]); Serial.print("\t");
  Serial.print("CH2:"); Serial.print(rc_values[RC_CH2]); Serial.print("\t");
  Serial.print("CH3:"); Serial.print(rc_values[RC_CH3]); Serial.print("\t");
  Serial.print("CH4:"); Serial.print(rc_values[RC_CH4]); Serial.print("\t");
  Serial.print("CH5:"); Serial.print(rc_values[RC_CH5]); Serial.print("\t");
  Serial.print("CH6:"); Serial.println(rc_values[RC_CH6]);


  //setting motion to channel 6 value
  if(rc_values[RC_CH5<1500]){
    //Forward
    if(rc_values[RC_CH4]>1700){
      digitalWrite(en1, HIGH);   // Left wheel forward
      digitalWrite(en2, LOW);
      analogWrite(enA, bs);
      digitalWrite(en3, HIGH);  // Right wheel forward
      digitalWrite(en4, LOW);
      analogWrite(enB, bs);
      Serial.println("forward");
      }
    //Backward
    else if(rc_values[RC_CH4]<1300){
      digitalWrite(en1, LOW);   // Left wheel forward
      digitalWrite(en2, HIGH);
      analogWrite(enA, bs);
      digitalWrite(en3, LOW);  // Right wheel forward
      digitalWrite(en4, HIGH);
      analogWrite(enB, bs);
      Serial.println("backward");
      }
    //Left turn
    else if(rc_values[RC_CH3]<1300){
      digitalWrite(en1, HIGH);   // Left wheel forward
      digitalWrite(en2, LOW);
      analogWrite(enA, bs);
      digitalWrite(en3, HIGH);  // Right wheel forward
      digitalWrite(en4, LOW);
      analogWrite(enB, bs);
      Serial.println("left");
      }
    //Right turn
    else if(rc_values[RC_CH3]>1700){
      digitalWrite(en1, LOW);   // Left wheel forward
      digitalWrite(en2, HIGH);
      analogWrite(enA, bs);
      digitalWrite(en3, LOW);  // Right wheel forward
      digitalWrite(en4, HIGH);
      analogWrite(enB, bs);
      Serial.println("right");
      }
    //Stop
    else if((rc_values[RC_CH4]<1700) && (rc_values[RC_CH4]>1300)){
      digitalWrite(en1, LOW);   // Left wheel forward
      digitalWrite(en2, LOW);
      analogWrite(enA, bs);
      digitalWrite(en3, LOW);  // Right wheel forward
      digitalWrite(en4, LOW);
      analogWrite(enB, bs);
      Serial.println("forward");
      }
    }
  //setting robotics to channel 6 value
  else if(rc_values[RC_CH5>1500]){
    if(rc_values[RC_CH1]<1300){
      pinMode(8, HIGH);
      pinMode(9, HIGH);
      pinMode(10, HIGH);
      pinMode(11, HIGH);
      myStepper.step(-stepsPerRevolution); 
      }
    else if(rc_values[RC_CH1]>1700){
      pinMode(8, HIGH);
      pinMode(9, HIGH);
      pinMode(10, HIGH);
      pinMode(11, HIGH);
      myStepper.step(stepsPerRevolution); 
      }
    else if(rc_values[RC_CH1]>1700){
      pos1 = pos1+5;
      if(pos1>180){
        pos1 =180;
        }
      servo1.write(pos1);
      servo2.write(pos1);
      }
    else if(rc_values[RC_CH1]<1300){
      pos1 = pos1-5;
      if(pos1<0){
        pos1=0;
      }
      servo1.write(pos1);
      servo2.write(pos1);
      }
    else if(rc_values[RC_CH2]>1700){
      pos2 = pos2+5;
      if(pos2>180){
        pos2 = 180;
        }
      servo3.write(pos2);
      }
    else if(rc_values[RC_CH2]<1300){
      pos2 = pos2-5;
      if(pos2<0){
        pos2 = 0;
        }
      servo3.write(pos2);
      }
    }
}
