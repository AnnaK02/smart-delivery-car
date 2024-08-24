#include <stdbool.h>
#include <iostream>
#include <string>
#include <sstream>
#include <Arduino.h>

using namespace std;
  
class DriveController {
  int pin1, pin2, pin3, pin4;
  
public:

  void motorPins(int p1, int p2, int p3, int p4){
    this->pin1 = p1;
    this->pin2 = p2;
    this->pin3 = p3;
    this->pin4 = p4;
  }
  

  
  /* Procedimento responsável por definir direção invocar o movimento */
  
  void makeMove(int amount, char _direction) {
    int seconds = amount * 1000; // transforamando em segundos (amount * 1000ms)
    
    switch(_direction){
      
      case 'S':
        cout << "\nStopping..." << flush;
        this->stopped();
        delay(seconds);
        break;
        
      case 'T': 
        cout << "\nMoving to top..." << flush;
        this->moveToTop(); 
        delay(seconds);
        break;
      
      case 'L': 
        cout << "\nTurning left..." << flush;
        this->moveToLeft();
        delay(seconds);
        break;
  
      case 'R': 
        cout << "\nTurning right..." << flush;
        this->moveToRight();
        delay(seconds);
        break;
  
      case 'B': 
        cout << "\nMoving to bottom..." << flush;
        this->moveToBottom();
        delay(seconds);
        break;
  
      default:
        cout << "\nNo action" << flush;
        delay(100);
        break;
    }
  }



/* Procedimentos responsáveis por mover o carrinho */

  void moveToTop() {
    analogWrite(pin1, LOW);
    analogWrite(pin2, 200);
    analogWrite(pin3, LOW);
    analogWrite(pin4, 200);
  }
  void moveToLeft() {
    analogWrite(pin1, LOW);
    analogWrite(pin2, LOW);
    analogWrite(pin3, LOW);
    analogWrite(pin4, 250);
  }
  void moveToRight() {
    analogWrite(pin1, LOW);
    analogWrite(pin2, 250);
    analogWrite(pin3, LOW);
    analogWrite(pin4, LOW);
  }
  void moveToBottom() {
    analogWrite(pin1, 200);
    analogWrite(pin2, LOW);
    analogWrite(pin3, 200);
    analogWrite(pin4, LOW);
  }
  void stopped() {
    analogWrite(pin1, LOW);
    analogWrite(pin2, LOW);
    analogWrite(pin3, LOW);
    analogWrite(pin4, LOW);
  }
};