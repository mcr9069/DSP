#include <MsTimer2.h>

const int TSAMP_MSEC = 100;
volatile boolean sampleFlag = false;
int nSmpl = 1, sample;
const int NUM_SAMPLES = 256;

//**********************************************************************
  
void setup()
{
  Serial.begin(9600);
  
  // This line tells MATLAB that the Arduino is ready to accept data
  Serial.println("%Arduino Ready");
  
  //Serial.println("Enter 'g' to go .....");

  // Wait until MATLAB sends a 'g' to start sending data
  while (Serial.read() != 'g'); // spin until 'g' entry

  MsTimer2::set(TSAMP_MSEC, ISR_Sample); // Set sample msec, ISR name
  MsTimer2::start(); // start running the Timer 
  }


//**********************************************************************
void loop()
{
  while (sampleFlag == false); // spin until ISR trigger
  sampleFlag = false;          // disarm flag: enable next dwell
  
  sample = analogRead(A0);

  // Display results to console
  if (nSmpl == 1) Serial.print("\nsmpl\tADC\n");
  Serial.print(nSmpl);  Serial.print('\t');
  Serial.print(sample); Serial.print('\n');

  if (++nSmpl > NUM_SAMPLES) MsTimer2::stop();    
    
} // loop()

//**********************************************************************
void ISR_Sample()
{
  sampleFlag = true; 
}
