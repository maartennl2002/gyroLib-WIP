#include "accelerometer.h"

#include "defines.h"
#include "quaternion.h"
#include <cmath>

//TODO: support different callibration schemes, single-point calibration works quite poorly


double range(double val){
  return fmod((val + M_PI), (2 * M_PI)) - M_PI;
}

void Accelerometer::calibrate(int& rawX, int& rawY, int& rawZ, int samplesize, bool changeOffset){
  long accelXTotal = 0;
  long accelYTotal = 0;
  long accelZTotal = 0;
  int accelXdata, accelYdata, accelZdata;

  //Set all three axes offsets to 0
  setOffset(0, 0, 0);

  for(int i = 0; i < samplesize; i++){
    while(!isReady()){}
    read(accelXdata, accelYdata, accelZdata);
    accelXTotal += accelXdata;
    accelYTotal += accelYdata;
    accelZTotal += accelZdata;
  }

  rawX = accelXTotal / samplesize;
  rawY = accelYTotal / samplesize;
  rawZ = accelZTotal / samplesize;

  //Set all three axes offsets
  if(changeOffset){
    setOffset(zeroX-rawX, zeroY-rawY, zeroZ-rawZ);
  }
}

void Accelerometer::calibrate(int samplesize, bool changeOffset){
  int rawX, rawY, rawZ;
  calibrate(rawX, rawY, rawZ, samplesize, changeOffset);
}

Euler Accelerometer::calcEuler(int rawX, int rawY, int rawZ){
  Euler e;
  e.roll = range(atan2(-rawY, rawZ)+M_PI);                      //from the internet
  e.pitch = -atan2(rawX, sqrt(rawY*rawY + rawZ*rawZ));          //from the internet
  return e;
}

Euler Accelerometer::calcEuler(){
  int rawX, rawY, rawZ;
  read(rawX, rawY, rawZ);
  return calcEuler(rawX, rawY, rawZ);
}

void Accelerometer::step(){
  if(isReady()){
    rotation = calcEuler();
  }
}

Quaternion Accelerometer::calcQuaternion(int rawX, int rawY, int rawZ, double yaw){
  Euler e = calcEuler(rawX, rawY, rawZ);
  
  return Quaternion(yaw, e.pitch, e.roll);
}

Quaternion Accelerometer::calcQuaternion(double yaw){
  Euler e = calcEuler();
  
  return Quaternion(yaw, e.pitch, e.roll);
}

void Accelerometer::setZeroReading(double zeroX, double zeroY, double zeroZ){
  this->zeroX = zeroX;
  this->zeroY = zeroY;
  this->zeroZ = zeroZ;
}

void Accelerometer::getZeroReading(double& zeroX, double& zeroY, double& zeroZ){
  zeroX = this->zeroX;
  zeroY = this->zeroY;
  zeroZ = this->zeroZ;
}

void Accelerometer::setOffset(double offsetX, double offsetY, double offsetZ){
  this->offsetX = offsetX;
  this->offsetY = offsetY;
  this->offsetZ = offsetZ;
}

void Accelerometer::getOffset(double& offsetX, double& offsetY, double& offsetZ){
  offsetX = this->offsetX;
  offsetY = this->offsetY;
  offsetZ = this->offsetZ;
}

Quaternion Accelerometer::getQuaternion(double yaw){
  return Quaternion(yaw, rotation.pitch, rotation.roll);
}

void Accelerometer::setQuaternion(Quaternion q){
  double yaw;
  q.getEuler(yaw, rotation.pitch, rotation.roll);
}

void Accelerometer::setEuler(Euler e){
  rotation = e;
}

Euler Accelerometer::getEuler(){
  return rotation;
}

bool Accelerometer::isReady(){
  return true;
}