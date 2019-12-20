/* 
Connect Vcc and Gnd of sensor to arduino, and the 
signal line to arduino digital pin 2.
*/

byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;

// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 1.25;

volatile byte pulseCount;  

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
unsigned int  mLperMinute;
unsigned long oldTime;
unsigned long oldTime2;

void setup()
{
  // Initialize a serial connection for reporting values to the host
  Serial.begin(9600);
 
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, LOW);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  mLperMinute       = 0;
  oldTime           = 0;
  oldTime2          = 0;
  // The Hall-effect sensor is connected to pin 2 which uses interrupt 1.
  // Configured to trigger on a RISING state change (transition from LOW
  // state to HIGH state)
  attachInterrupt(sensorInterrupt, pulseCounter, RISING);
}
void loop()
{
   if((millis() - oldTime) > 1000)    // Only process counters once per second
  { 
    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(sensorInterrupt);
        
    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    
    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    oldTime = millis();
    
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;
    
    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
    mLperMinute += flowMilliLitres;  
    unsigned int frac;
    
    // Print the flow rate for this second in litres / minute
//    Serial.print("Flow rate: ");
//    Serial.print(int(flowRate));  // Print the integer part of the variable
//    Serial.print("L/min");
//    Serial.print("\t");       // Print tab space
    // Print the cumulative total of litres flowed since starting
//    Serial.print("Output Liquid Quantity: ");        
//    Serial.print(totalMilliLitres);
//    Serial.println("mL"); 
    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;
    
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorInterrupt, pulseCounter, RISING);
  }
     if((millis() - oldTime2) > 60000) {
    //Serial.print("its been 5 seconds, total flow in the past minute: ");
    Serial.print(mLperMinute);
    mLperMinute = 0;
    oldTime2 = millis();
   }
}
/*
Insterrupt Service Routine
 */
void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}
