int nSmpl = 1, sample; // global variables

void setup()
{
  Serial.begin(9600);
  Serial.print("\nsmpl\tADC\n"); // column headers
}
void loop()
{
  sample = analogRead(A0);
  Serial.print(nSmpl); Serial.print('\t'); // tabbed output
  Serial.println(sample);
  ++nSmpl;
  delay(500);
}
