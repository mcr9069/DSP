// OPEN A NEW SKETCH WINDOW IN ARDUINO
// CLICK IN THIS BOX, CTL-A, CTL-C (Copy code)
// CLICK IN SKETCH, CTL-A, CTL-V (Paste code into sketch)

// file: Timing.ino
// created by: Clark Hochgraf Sept 15, 2015
// modified by: David Orlicki Sept 1, 2017
// modified by: Mark Thompson February 20, 2025
// purpose: Measuring execution time and memory usage as datatype is
//          changed from byte to integer, long, float 

const int DATA_LEN = 200; // number of data array values
unsigned long startUsec, endUsec, execUsec;
byte xv[DATA_LEN]={0}, seed = 1.01, res; // array initialized to zero

void setup()
{
  Serial.begin(115200);
  Serial.print("Data array length = "); Serial.println(DATA_LEN);
  
  res = seed;
  startUsec=micros();
  res=res+res; res=res+res; res=res+res; res=res+res; res=res+res;
  res=res+res; res=res+res; res=res+res; res=res+res; res=res+res;
  res=res+res; res=res+res; res=res+res; res=res+res; res=res+res;
  res=res+res; res=res+res; res=res+res; res=res+res; res=res+res; 
  endUsec = micros();
  execUsec = endUsec-startUsec;
  Serial.print("\nuSec per individual addition = ");
  Serial.println(execUsec/20.0,2);
  delay(20);
  
  //----------------------------------------------
  res = seed;
  startUsec=micros();
  res=res*res; res=res*res; res=res*res; res=res*res; res=res*res;
  res=res*res; res=res*res; res=res*res; res=res*res; res=res*res;
  res=res*res; res=res*res; res=res*res; res=res*res; res=res*res;
  res=res*res; res=res*res; res=res*res; res=res*res; res=res*res;
  execUsec = micros()-startUsec;
  Serial.print("uSec per individual multiplication = ");
  Serial.println(execUsec/20.0,2);
  
  //----------------------------------------------
  res = seed;
  startUsec=micros();
  res=res+res*res; res=res+res*res; res=res+res*res; res=res+res*res;
  res=res+res*res; res=res+res*res; res=res+res*res; res=res+res*res;
  res=res+res*res; res=res+res*res; res=res+res*res; res=res+res*res;
  res=res+res*res; res=res+res*res; res=res+res*res; res=res+res*res;
  res=res+res*res; res=res+res*res; res=res+res*res; res=res+res*res;
  execUsec = micros()-startUsec; 
  Serial.print("uSec per individual multiply and accumulate = ");
  Serial.println(execUsec/20.0,4);
  
  //----------------------------------------------
  res = seed;
  startUsec=micros();
  for (int i=0; i <DATA_LEN; i++)
  {
    res = res+res;
//    Serial.print(i); Serial.print('\t');
//    Serial.println(res);
  }
  execUsec = micros()-startUsec;
  Serial.print("uSec per loop addition = ");
  Serial.println((float)execUsec/DATA_LEN,2);
  
  //----------------------------------------------
  // multiply two values
  startUsec=micros();
  for (int i=0; i <DATA_LEN; i++){ res = res*res; }
  execUsec = micros()-startUsec;
  Serial.print("uSec per loop multiplication = ");
  Serial.println((float)execUsec/DATA_LEN,2);
  
  //----------------------------------------------
  // multiply two values and add to original value (accumulate) 
  startUsec=micros();
  for (int i=0; i <DATA_LEN; i++){ res = res+res*res; }
  execUsec = micros()-startUsec;
  Serial.print("uSec per loop multiply and accumulate = ");
  Serial.println((float)execUsec/DATA_LEN,2);
    
  //----------------------------------------------
  // Add two values using arrays
  startUsec=micros();
  for (int i=0; i <DATA_LEN; i++){ xv[i] = xv[i]+xv[i]; }
  execUsec = micros()-startUsec;
  Serial.print("uSec per loop array addition = ");
  Serial.println((float)execUsec/DATA_LEN,2);
  
  //----------------------------------------------
  // multiply two values using arrays
  startUsec=micros();
  for (int i=0; i <DATA_LEN; i++){ xv[i] = xv[i]*xv[i]; }
  execUsec = micros()-startUsec;
  Serial.print("uSec per loop array multiplication = ");
  Serial.println((float)execUsec/DATA_LEN,2);
  
  //----------------------------------------------
  // multiply two values and add to original value (accumulate) using arrays
  startUsec=micros();
  for (int i=0; i <DATA_LEN; i++){ xv[i] = xv[i]+xv[i]*xv[i]; }
  execUsec = micros()-startUsec;
  Serial.print("uSec per loop array multiply and accumulate = ");
  Serial.println((float)execUsec/DATA_LEN,4);
}
void loop(){ } // spin forever
