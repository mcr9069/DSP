#include <MsTimer2.h>

const int TSAMP_MSEC = 100;
volatile boolean sampleFlag = false;
int nSmpl = 1, sample;

//**********************************************************************
void setup()
{
  Serial.begin(9600);
  Serial.println("Enter 'g' to go .....");
  while (Serial.read() != 'g'); // spin until 'g' entry

  MsTimer2::set(TSAMP_MSEC, ISR_Sample); // Set sample msec, ISR name
  MsTimer2::start(); // start running the Timer 
}

//**********************************************************************
void loop()
{
  while (nSmpl == 257); // spin until 'g' entry
  while (sampleFlag == false); // spin until ISR trigger
  sampleFlag = false;          // disarm flag: enable next dwell
  
  sample = analogRead(A0);

  // Display results to console
  if (nSmpl == 1) Serial.print("smpl\tADC\n\r");
  Serial.print(nSmpl);  Serial.print('\t');
  Serial.print(sample); Serial.print('\r'); Serial.print('\n');

  ++nSmpl;
} // loop()

//**********************************************************************
void ISR_Sample()
{
  sampleFlag = true; 
}
