// Variables

const int sensorPin = ;
const int buzzerPin = ;

int breathCount = 0;
int baseline = 0;
bool rising = false;

const int windowTime = 15000; // 15-second window
unsigned long windowStart = 0;

void setup() {
  // put your setup code here, to run once:
  configureArduino();
  Serial.begin(115200);delay(5);
  pinMode(buzzerPin, OUTPUT);

   //Handshake with MATLAB 
  Serial.println(F("%Arduino Ready"));
  while (Serial.read() != 'g'); // spin

  MsTimer2::set(TSAMP_MSEC, ISR_Sample); // Set sample msec, ISR name
  MsTimer2::start(); // start running the Timer  

  // Collect data code
  // 
  // Ex
  long sum = 0;
  for (int i = 0; i < 100; i++) {
    sum += analogRead(sensorPin);
    delay(5);
  }
  baseline = sum / 100;

  windowStart = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  int sensorValue = analogRead(sensorPin);
  
  // Detect peaks
  if (sensorValue > baseline + 30 && !rising) {
    rising = true;
    breathCount++;
  }
  if (sensorValue < baseline + 10) {
    rising = false;
  }
  // Every 15 seconds compute BPM
  if (millis() - windowStart >= windowTime) {
    float bpm = breathCount * 4.0; // 4 × 15-seconds = 60s

    Serial.print("Breathing Rate = ");
    Serial.print(bpm);
    Serial.println(" BPM");

    checkBPM(bpm);

    // Reset window
    breathCount = 0;
    windowStart = millis();
  }
  delay(20);
}

void checkBPM(float bpm) {
  if (bpm < 12) {
    Serial.println("⚠ LOW BREATHING RATE WARNING");
    tone(buzzerPin, 600);
    delay(1000);
    noTone(buzzerPin);
  }
  else if (bpm > 40) {
    Serial.println("⚠ FAST BREATHING WARNING");
    tone(buzzerPin, 1000);
    delay(1000);
    noTone(buzzerPin);
  }
  else {
    Serial.println("Breathing Normal");
  }
}

// Draft
int AlarmCheck( float stdLF, float stdMF, float stdHF)
{
//  Your alarm check logic code will go here.
int alarmCode = 0;

    // --- Thresholds (TUNE THESE to your data) ---
    const float LOW_BREATH_THR = 0.20;    // very small motion - low breathing (random numbers ** Need testing)
    const float HIGH_BREATH_THR = 1.20;   // large oscillation - fast breathing (random numbers ** Need testing)
    const float SENSOR_FAIL_THR = 2.50;   // very high = noise or disconnection (random numbers ** Need testing)

    // --- Sensor failure check ---
    if (stdHF > SENSOR_FAIL_THR || stdMF > SENSOR_FAIL_THR)
    {
        return 3;  // sensor not functional
    }

    // --- Low breathing detection ---
    if (stdLF < LOW_BREATH_THR)
    {
        return 1;
    }

    // --- High (fast) breathing detection ---
    if (stdHF > HIGH_BREATH_THR)
    {
        return 2;
    }

    // --- Normal breathing ---
    return 0;
  
//return alarmCode;

}  // end AlarmCheck

void setAlarm(int aCode, boolean isToneEn)
{

// Your alarm code goes here
// Case 0 Functional
// stop tone
// Case 1 Low Breathing
// sound alarm
// Case 2 High Breathing
// sound different alarm
// Case 3 Non-functional 
// sound alarm
  if (!isToneEn)
    {
        noTone(SPKR);
        return;
    }

    switch (aCode)
    {
        case 0: // Normal breathing
            noTone(SPKR);
            break;

        case 1: // Low breathing rate
            tone(SPKR, 500);   // slow alert beep (values need testing)
            break;

        case 2: // High breathing rate
            tone(SPKR, 1200);  // fast alert beep (values need testing)
            break;

        case 3: // Sensor failure
            tone(SPKR, 300);   // low warning tone (values need testing)
            break;
    }
    
} // setBreathRateAlarm()
