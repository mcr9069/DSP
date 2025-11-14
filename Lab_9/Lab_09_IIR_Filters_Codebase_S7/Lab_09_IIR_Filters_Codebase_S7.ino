// OPEN NEW ARDUINO SKETCH.
// CLICK IN THIS TEXT BOX. USE Ctrl-A, Ctrl-C TO COPY ALL.
// CLICK IN SKETCH. USE Ctrl-A, CTRL-V TO PASTE. SAVE. RUN.

//  Modified 3/9/2020 by Mark Thompson to recursively operate over the stages
//      of the SOS filter.  Comments added

// Exercise_3_IIR_Filters_2195

// Variable Declarations

// TSAMP_MSEC -- Sample time for the sample interrupt
// DATA_LEN -- Number of samples of data to collect or compute

const int TSAMP_MSEC = 10;
const unsigned int DATA_LEN = 3600; // 512 1200;

//  xv -- Input samples to the filter
//  yv -- Output samples from the filter
//  tick -- Sample counter
//  statRest -- Flag to reset the resettable stats computations
//  stat_t --  Structure to hold the output stats computations
//  mean -- running mean of the statistics
//  var -- running variance of the statistics
//  stdev -- running standard devation of the statistics
//  statsX -- Holds the stats for X
//  statsY -- Holds the stats for Y

static char buffer[100];
static size_t bufLen;

float xv, yv, stdX, stdY;
int tick = 0;
bool statsReset;
struct stats_t
{
  int tick = 1;
  float mean, var, stdev;
} statsX, statsY;

//  execUsec -- holds the accumulated execution time value
unsigned long execUsec;

//*********************************************************************
void setup()
{
  Serial.begin(115200); delay(20);
  
 //Handshake with MATLAB 
  Serial.println(F("%Arduino Ready"));
  while (Serial.read() != 'g'); // spin   
}

//*********************************************************************
void loop()
{

  //  Select the input to the system.  Impulse function or from the test
  //  vector generator.

  // Create an input from the test vector generator
  xv = testVector();

  // Compute the output of the filter using the cascaded SOS sections
  yv = IIR_SOS(xv); // second order systems cascade  


  //  Set the statsReset flag if the tick counter is at multiples of 100
  statsReset = (tick%100 == 0);

  //  Compute the latest output of the running stats for the output of the filters.
  //  Pass the entire set of output values, the latest stats structure and the reset flag
  getStats(yv, statsY, statsReset); stdY = statsY.stdev;


 //  Post it up on the console
  displayData();
  delay(20); // clear comm buffer: 11 char/msec @ 115200 baud

  //  If we've collected enough samples, just hang out
  ++tick;
  while (tick >= DATA_LEN);
} // loop()

//*******************************************************************************
float IIR_SOS(float xv)
{ 
  //  ***  Copy variable declarations from MATLAB generator to here  ****

  // Filter specific variable declarations
  const int numStages = 5;
  static float G[numStages];
  static float b[numStages][3];
  static float a[numStages][3];

  // CHEBY bandpass, order 5, R= 1.0, [12 25] BPM
  G[0] = 0.0442008;
  b[0][0] = 1.0000000; b[0][1] = -0.0002347; b[0][2]= -0.9992038;
  a[0][0] = 1.0000000; a[0][1] =  -1.9290684; a[0][2] =  0.9612907;
  G[1] = 0.0442008;
  b[1][0] = 1.0000000; b[1][1] = 2.0008350; b[1][2]= 1.0008352;
  a[1][0] = 1.0000000; a[1][1] =  -1.9109777; a[1][2] =  0.9617828;
  G[2] = 0.0442008;
  b[2][0] = 1.0000000; b[2][1] = 1.9996805; b[2][2]= 0.9996808;
  a[2][0] = 1.0000000; a[2][1] =  -1.9549606; a[2][2] =  0.9755173;
  G[3] = 0.0442008;
  b[3][0] = 1.0000000; b[3][1] = -2.0004549; b[3][2]= 1.0004550;
  a[3][0] = 1.0000000; a[3][1] =  -1.9166497; a[3][2] =  0.9838062;
  G[4] = 0.0442008;
  b[4][0] = 1.0000000; b[4][1] = -1.9998259; b[4][2]= 0.9998260;
  a[4][0] = 1.0000000; a[4][1] =  -1.9762576; a[4][2] =  0.9920692;

  //  *** Stop copying MATLAB variable declarations here
  
  int stage;
  int i;
  static float xM0[numStages] = {0.0}, xM1[numStages] = {0.0}, xM2[numStages] = {0.0};
  static float yM0[numStages] = {0.0}, yM1[numStages] = {0.0}, yM2[numStages] = {0.0};
  
  float yv = 0.0;
  unsigned long startTime;

  //  Initialize the timer for each point computed
  startTime = micros();


  //  Iterate over each second order stage.  For each stage shift the input data
  //  buffer ( x[kk] ) by one and the output data buffer by one ( y[k] ).  Then bring in 
  //  a new sample xv into the buffer;
  //
  //  Then execute the recusive filter on the buffer
  //
  //  y[k] = -a[2]*y[k-2] + -a[1]*y[k-1] + g*b[0]*x[k] + b[1]*x[k-1] + b[2]*x[k-2] 
  //
  //  Pass the output from this stage to the next stage by setting the input
  //  variable to the next stage x to the output of the current stage y
  //  
  //  Repeat this for each second order stage of the filter

  
  for (i =0; i<numStages; i++)
    {
      yM2[i] = yM1[i]; yM1[i] = yM0[i];  xM2[i] = xM1[i]; xM1[i] = xM0[i], xM0[i] = G[i]*xv;
      yv = -a[i][2]*yM2[i] - a[i][1]*yM1[i] + b[i][2]*xM2[i] + b[i][1]*xM1[i] + b[i][0]*xM0[i];
      yM0[i] = yv;
      xv = yv;
    }

  execUsec += micros()-startTime;
  
  return yv;
}
//*******************************************************************
void getStats(float xv, stats_t &s, bool reset)
{
  float oldMean, oldVar;
  
  if (reset == true)
  {
    s.stdev = sqrt(s.var/s.tick);
    s.tick = 1;
    s.mean = xv;
    s.var = 0.0;  
  }
  else
  {
    oldMean = s.mean;
    s.mean = oldMean + (xv - oldMean)/(s.tick+1);
    oldVar = s.var; 
    s.var = oldVar + (xv - oldMean)*(xv - s.mean);      
  }
  s.tick++;  
}
//*************************************************************
float testVector(void)
{
  // Variable rate sinusoidal input
  // Specify segment frequencies in bpm.
  // Test each frequency for nominally 60 seconds.
  // Adjust segment intervals for nearest integer cycle count.
    
  const int NUM_BAND = 6;
  const float CAL_FBPM = 10.0, CAL_AMP = 2.0; 
  
  const float FBPM[NUM_BAND] = {5, 10, 15, 20, 30, 70}; // LPF test
  static float bandAmp[NUM_BAND] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

  //  Determine the number of samples (around 600 ) that will give you an even number
  //  of full cycles of the sinewave.  This is done to avoid a large discontinuity 
  //  between bands.  This forces the sinewave in each band to end near a value of zer
  
  static int bandTick = int(int(FBPM[0]+0.5)*(600/FBPM[0]));
  static int simTick = 0, band = 0;
  static float Fc = FBPM[0]/600, cycleAmp = bandAmp[0];

  //for (int i = 0; i < NUM_BAND; i++) bandAmp[i] = CAL_AMP*(CAL_FBPM/FBPM[i]);  

  //  Check to see if the simulation tick has exceeded the number of tick in each band.
  //  If it has then switch to the next frequency (band) again computing how many
  //  ticks to go through to end up at the end of a cycle.
  
  if ((simTick >= bandTick) && (FBPM[band] > 0.0))
  {

    //  The simTick got to the end of the band cycle.  Go to the next frequency
    simTick = 0;
    band++;
    Fc = FBPM[band]/600.0;
    cycleAmp = bandAmp[band];
    bandTick = int(int(FBPM[band]+0.5)*(600/FBPM[band]));
  }
 
  float degC = 0.0; // DC offset
  degC += cycleAmp*sin(TWO_PI*Fc*simTick++);  
  //degC += 1.0*(tick/100.0); // drift: degC / 10sec
  //degC += 0.1*((random(0,101)-50.0)/29.0); // stdev scaled from 1.0
  return degC;
}

//*********************************************************************
void displayData(){
  bufLen = 0;

  switch (tick) {
    case 0:  Serial.write("n\t\txv\t\tyv\n"); break;
    default: break;
  }

  bufLen += sprintf(buffer, "%d\t", tick);
  dtostrf(xv, 10, 4, buffer + bufLen); bufLen += 10;
  buffer[bufLen++] = '\t';
  dtostrf(yv, 10, 4, buffer + bufLen); bufLen += 10;
  buffer[bufLen++] = '\n';
  Serial.write(buffer, bufLen);
  memset(buffer, 0, bufLen);
}