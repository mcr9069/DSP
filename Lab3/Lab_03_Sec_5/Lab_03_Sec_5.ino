// OPEN NEW ARDUINO SKETCH.
// CLICK IN THIS TEXT BOX. CTRL-A, CTRL-C.
// CLICK IN SKETCH. CTRL-A, CTRL-V.

// Lab 3 starter: Cook stats

const int TOTAL_SAMPLES = 600; // simulated samples
int numSamples = 1;

// Declare the variables that are computed in the calculateStats function
float sample, runningMean = 0.0, runningStdev = 0.0;

//**********************************************************************
void setup()
{
  Serial.begin(9600);
 
// This line tells MATLAB that the Arduino is ready to accept data
  Serial.println("%Arduino Ready");  

// Wait until MATLAB sends a 'g' to start sending data
while (Serial.read() != 'g'); // spin until 'g' entry

}

//**********************************************************************
void loop()
{ 
  sample = simSample();
  
  // Call the statistics calculation function
  calculateStats(sample);
  
  // Display the statistics
  displayStatsData();
  
  // Run TOTAL_SAMPLES iterations then halt
  if (++numSamples > TOTAL_SAMPLES) while (true);
  
} // loop()

//**********************************************************************
float simSample(void)
{
  // Simulate sensor for stats calculation development
  float simSmpl, simAmp = 1.0, simT = 60;
  
  //simAmp = ((numSamples > 180) && (numSamples < 300)) ? 0.125 : 1.0; // burst amplitude
  simT = ((numSamples > 180) && (numSamples < 300)) ? 30.0 : 60.0; // burst frequency
  simSmpl = 180.0 + simAmp*sin((numSamples/simT)*TWO_PI); // fixed amplitude, frequency
  
  return simSmpl; 
}
//**********************************************************************
void calculateStats(float xi )
{
  // Calculate running statistics per Cook pseudo code.
  static int tick = 1;
  static float oldMean = 0.0, oldRunningSumVar = 0.0;
  float runningSumVar;
  
  if (tick == 1) {
    runningMean = xi;
    runningStdev = 0;
    oldMean = runningMean;
    oldRunningSumVar = 0;
  } else {
    runningMean = oldMean + (xi - oldMean) / tick;
    runningSumVar = oldRunningSumVar + (xi - oldMean) * (xi - runningMean);
    runningStdev = sqrt(runningSumVar / (tick - 1));

    oldMean = runningMean;
    oldRunningSumVar = runningSumVar;
  }
  
  tick++;
}
//**********************************************************************
void displayStatsData(void)
{
  // Display results to console
  if (numSamples == 1)
  {
    Serial.print("\nn\tsmpl\trunningMean\tstdev\n");
  }
  Serial.print(numSamples);  Serial.print('\t');
  Serial.print(sample); Serial.print('\t');
  Serial.print(runningMean);   Serial.print('\t');
  Serial.print(runningStdev);  Serial.print('\n');  
}
