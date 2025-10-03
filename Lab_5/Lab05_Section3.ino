// OPEN NEW ARDUINO SKETCH.
// CLICK IN THIS TEXT BOX. CTRL-A, CTRL-C.
// CLICK IN SKETCH. CTRL-A, CTRL-V.

// file: Roundoff_2.ino
// created by: Clark Hochgraf  Sept 15, 2015
// modified by: David Orlicki Sept 1, 2017
// modified by: Mark Thompson December 31, 2019
// modified by: Mark Thompson Feb 20, 2025
// purpose: Illustration of unexpected behavior due to roundoff error 

float A = 18000002.0;
float B = 18000001.0;
//long A = 18000002;
//long B = 18000001;
//long A = 18000002.0;
//long B = 18000001.0;

void setup()
{
  Serial.begin(9600);
  Serial.print("A = float(18000002.0) value printed out is "); Serial.println(A); 
  Serial.print("B = float(18000001.0) value printed out is "); Serial.println(B); 
  Serial.print("A - B  is "); Serial.println(A-B); 

  //-------------------------------------------------------------------
  Serial.println("\nAnother illustration:  Counting up by 1 "); 
  Serial.println("A is the counter value"); 
  Serial.println("B is the counter value minus 1"); 
  Serial.println(); 
  
  long counter=16000000;
  A = counter;
  B = A-1.0;
  Serial.print("Starting at a counter value of = "); Serial.println(counter); 
  Serial.print("A value is "); Serial.println(A); 
  Serial.print("B value is "); Serial.println(B); 
  Serial.print("A - B  is ");  Serial.println(A-B); 
   
  Serial.println();
  Serial.println("Continue to count up by 1.  Print the counter every 50,000 counts");
  Serial.println("Check to see that the differnce between A and B is always 1, error otherwise");
  Serial.println();
  while ((A-B) == 1)
  {
    if (counter%50000 == 0) Serial.println(counter);
    counter = counter+1;
    A=counter;
    B=counter-1;
  }
  
  Serial.print("\nUnexpected result: Counter = ");
  Serial.println(counter); 
  Serial.print("A value is "); Serial.println(A); 
  Serial.print("B value is "); Serial.println(B); 
  Serial.print("A - B  is ");  Serial.println(A-B); 
} // setup 

void loop() {} // spin forever
