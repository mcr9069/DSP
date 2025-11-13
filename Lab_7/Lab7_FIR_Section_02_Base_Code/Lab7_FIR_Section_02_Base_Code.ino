// OPEN A NEW SKETCH WINDOW IN ARDUINO
// CLICK IN THIS BOX, CTL-A, CTL-C (Copy code)
// CLICK IN SKETCH, CTL-A, CTL-V (Paste code into sketch)

// file: Lab7_FIR_Section_02_Base_Code.ino

// created:  Clark Hochgraf/David Orlicki October 17, 2017
// modified: David Orlicki October 26, 2018
// modified: David Orlicki October 25, 2019
// modified: Mark Thompson 3/21/2021 -- Added some comments, Added MATLAB handshake

//-----------------------------------------------------------------
// Create the sync based FIR filters indicated below.
//   Specify fractional corner frequency Fc.
//   Specify max kernel index M. (kernel lenght M+1, 0..M).
//   Use the companion sketch to create specified FIR kernels.
// Show the impulse response for each case.
// Show the FFT based frequency response for each case.
// Show the 10% - 90% transition BW(s) for each case.
// Use testVector() function to measure amplitude gain(s)
//   at frequencies +/- 5 BPM from each cut frequency.

// Filter specifications
// BPF: low 15 bpm, high = 50 bpm, transition BW = 10 bpm
// BSF: low 15 bpm, high = 50 bpm, transition BW = 10 bpm

#include <avr/wdt.h>

#define HFXPT 4096
#define MFILT 101

#define TSAMP_MSEC 5
#define DATA_LEN   1200
#define DATA_FXPT  100

const float INV_HFXPT = 1.0/HFXPT;

static int lpArr[MFILT];
static int hpArr[MFILT];

int tick, xv, yv, yBPL, yBPH, yBSL, yBSH;
float sample;
unsigned long execUsec;

static char buffer[50];
static size_t bufLen;

static int hBPLPF[] = {2, 1, 0, -1, -2, -3, -3, -2, 0, 2, 5, 6, 6, 4,
	0, -5, -10, -13, -12, -8, 0, 10, 18, 23, 22, 14, 0, -17,
	-32, -41, -39, -24, 0, 29, 56, 70, 67, 43, 0, -53, -103, -135,
	-133, -89, 0, 128, 278, 431, 563, 652, 683, 652, 563, 431, 278, 128,
	0, -89, -133, -135, -103, -53, 0, 43, 67, 70, 56, 29, 0, -24,
	-39, -41, -32, -17, 0, 14, 22, 23, 18, 10, 0, -8, -12, -13,
	-10, -5, 0, 4, 6, 6, 5, 2, 0, -2, -3, -3, -2, -1,
	0, 1, 2
};
static int hBPHPF[] = {-2, -2, -2, -2, -2, -2, -2, -2, -1, -1, 0, 1, 2, 4,
	5, 7, 9, 11, 13, 15, 17, 19, 20, 21, 21, 20, 18, 15,
	11, 6, 0, -8, -16, -26, -38, -50, -63, -76, -90, -104, -119, -133,
	-146, -158, -170, -180, -188, -195, -200, -203, 3892, -203, -200, -195, -188, -180,
	-170, -158, -146, -133, -119, -104, -90, -76, -63, -50, -38, -26, -16, -8,
	0, 6, 11, 15, 18, 20, 21, 21, 20, 19, 17, 15, 13, 11,
	9, 7, 5, 4, 2, 1, 0, -1, -1, -2, -2, -2, -2, -2,
	-2, -2, -2
};

static int hBSLPF[] = {2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, -1, -2, -4,
	-5, -7, -9, -11, -13, -15, -17, -19, -20, -21, -21, -20, -18, -15,
	-11, -6, 0, 8, 16, 26, 38, 50, 63, 76, 90, 104, 119, 133,
	146, 158, 170, 180, 188, 195, 200, 203, 204, 203, 200, 195, 188, 180,
	170, 158, 146, 133, 119, 104, 90, 76, 63, 50, 38, 26, 16, 8,
	0, -6, -11, -15, -18, -20, -21, -21, -20, -19, -17, -15, -13, -11,
	-9, -7, -5, -4, -2, -1, 0, 1, 1, 2, 2, 2, 2, 2,
	2, 2, 2
};

static int hBSHPF[] = {-2, -1, 0, 1, 2, 3, 3, 2, 0, -2, -5, -6, -6, -4,
	0, 5, 10, 13, 12, 8, 0, -10, -18, -23, -22, -14, 0, 17,
	32, 41, 39, 24, 0, -29, -56, -70, -67, -43, 0, 53, 103, 135,
	133, 89, 0, -128, -278, -431, -563, -652, 3413, -652, -563, -431, -278, -128,
	0, 89, 133, 135, 103, 53, 0, -43, -67, -70, -56, -29, 0, 24,
	39, 41, 32, 17, 0, -14, -22, -23, -18, -10, 0, 8, 12, 13,
	10, 5, 0, -4, -6, -6, -5, -2, 0, 2, 3, 3, 2, 1,
	0, -1, -2
};

//*******************************************************************
void setup()
{
  memset(buffer, 0, 50);

  memset(lpArr, 0, MFILT);
  memset(hpArr, 0, MFILT);

  Serial.begin(115200); delay(20);
  // Serial.setTimeout(50);
 
  // Handshake with MATLAB      
  Serial.println(F("%Arduino Ready"));
  while (Serial.read() != 'g'); // spin
}

//*******************************************************************
void loop()
{
//  sample = (tick == 0) ? 1000.0 : 0;
  sample = testVector();
  xv = int(DATA_FXPT*sample+0.5); // rounded, fixed point
  
  execUsec = micros();
  
  // Combine the LPF and HPF appropriately to create a BPF 
  // and then a BSF.  See diagrams for assistance

  // yv = xv;
  // yv = FIR(xv, hBSLPF);

  // BP
  yv = FIR(xv, hBPLPF, lpArr);
  yv = FIR(yv, hBPHPF, hpArr);

  // BS
  // yv = FIR(xv, hBSLPF, lpArr) + FIR(xv, hBSHPF, hpArr);

  // yv = FIR(FIR(xv, hBPLPF), hBPHPF); // BPF
  
  execUsec = micros()-execUsec;

  displayData(tick,xv,yv);

  switch (tick) {
  case DATA_LEN:
    Serial.print(F("\ncalculation msec/sample = "));
    Serial.println(0.001*execUsec,3);
    // Serial.println(execUsec);
    wdt_enable(WDTO_15MS); 
    for (;;); // freeze process (spin forever)
    break;
  default:
    ++tick;
    return;
  }
} // loop()

//*******************************************************************
int FIR_STUB(int x) {return(x);} // timing measurement stub

//*******************************************************************
int FIR(int &x, int h[], int xvArr[])
//
// FIR_A is a generic function that implements convolution of the input value x
// with the impulse response that is declared below.  The student will
// compute the impulse response in MATLAB using the FIR_Designer tool
// and use the C header that is created to replace the code below
//
{
  long hv, accum = 0;

  // Right shift old xv values. Install new x in xv[0];
  for (int i = (MFILT-1); i > 0; i--) xvArr[i] = xvArr[i-1]; xvArr[0] = x;

  // h[]*x[] overlap multiply-accumumlate
  for (int i = 0; i < MFILT; i++)
  {
    hv = h[i]; // create 32 bit space
    accum += hv*xvArr[MFILT-1-i];
  }
  return (accum*INV_HFXPT);
}

//*******************************************************************
void displayData(int n, int x, int y)
{
  switch (n) {
  case 0:
    Serial.write("\nn\tx\ty\n");
    break;
  default:
    bufLen = sprintf(buffer, "%d\t%d\t%d\n", n, x, y);
    Serial.write(buffer, bufLen);
    memset(buffer, 0, bufLen);
    break;
  }
}

//*******************************************************************
float testVector(void)
{
  // Variable rate periodic input
  // Specify segment amplitude, bpm rate, interval seconds.
  // Intervals trimmed for nearest cycle ending zero crossing.

  const float AMP1 = 2.0, BPM1 = 5.0, TSEC1 = 24.0; 
  const float AMP2 = 2.0, BPM2 = 30.0, TSEC2 = 24.0; 
  const float AMP3 = 2.0, BPM3 = 70.0, TSEC3 = 24.0;
    
  static int simTick, xt1, xt2, breathsPerInterval;
  static float cycleAmp, fracFreq, fracFreq1, fracFreq2, fracFreq3;
  float secPerBreath;
  
  if (tick == 0) // map full test amplitudes, frequencies and durations
  {
    fracFreq1 = BPM1/600; 
    secPerBreath = 60.0/BPM1;
    breathsPerInterval = int(TSEC1/secPerBreath+0.5)+1;
    xt1 = 10*breathsPerInterval*secPerBreath;
    
    fracFreq2 = BPM2/600;
    secPerBreath = 60.0/BPM2;
    breathsPerInterval = int(TSEC2/secPerBreath+0.5)+1;
    xt2 = xt1 + 10*breathsPerInterval*secPerBreath;
    
    fracFreq3 = BPM3/600; 
    secPerBreath = 60.0/BPM3;

    simTick = 0; cycleAmp = AMP1; fracFreq = fracFreq1; // interval 1
  }
  else if (tick == xt1) // transition to interval 2
  {
    simTick = 0; cycleAmp = AMP2; fracFreq = fracFreq2;
  }
  else if (tick == xt2) // transition to interval 3
  {
    simTick = 0; cycleAmp = AMP3; fracFreq = fracFreq3;
  }
  
  // Run breathing simulation parameterized by interval
  float degC = 5.0; // DC offset
  degC += 0.0*tick/100.0; // drift
  degC += cycleAmp*sin((fracFreq*simTick++)*TWO_PI);  
  degC += 0.0*((random(0,11)/10.0-0.5)/0.29); 
  return degC;
}
