int ledPin = 13;    //Pin for the led to indicate when relay is enabled
int relayPin = 12;  //Pin turning pump control relay on/off
int startPin = 11;  //Will start/stop the watering if connected/disconnected for a hardware failsafe
bool enabled = false;//to hold startPin value
int maxTime = (10 * 1000);   //Max time in seconds to run the pump per sitting
int minDelay = (8 * 60 * 60 * 1000);    //Minimum amount of time in hours between watering sessions (to prevent water spillage) 
int autoDelay = (48 * 60 * 60 * 1000);  //period in hours between automated watering
int canWater = 1;   //Tells user if they can water more today (1:yes, 0:no)
int totalWatered = 0;   //Times watered total;
int lastAutoTime = 0;   //Time since last automated watering session
int temp = 0;   //Generic variable used for various tasks and previously for debugging

void setup()
{
   //Start pin configuration
   pinMode(ledPin, OUTPUT);
   pinMode(relayPin, OUTPUT);
   pinMode(startPin, INPUT);
   digitalWrite(relayPin, HIGH); //Ensure relay pin is low/off at the start
  
   Serial.begin(9600);	//Start serial for reading total times watered or debug data
 
   water();    //Run first water cycle
}

void loop()
{
    enabled = digitalRead(startPin);    //Read hardware enabling pin data
   
    if (enabled)   //Make sure time delay between watering sessions is being respected
        canWater = 1;
    else
        canWater = 0;
        
    if (canWater == 1 && temp > 7500) { //indicate watering status with the led
        temp = 0;
        digitalWrite(ledPin, HIGH);
        delay(75);
        digitalWrite(ledPin, LOW);
    }
    else if (canWater == 1)
        temp++;
        
    if (canWater == 1 && (millis() - lastAutoTime) > autoDelay) {   //Run auto water or delay another auto period
        lastAutoTime = millis();
        water();
    }
}

int water() {    //Water and reset time since last watering cycle
    enabled = digitalRead(startPin);    //Read hardware enabling pin data
    
    if (enabled && canWater == 1) {
        digitalWrite(relayPin, LOW);
        digitalWrite(ledPin, HIGH);
        delay(maxTime);
        digitalWrite(relayPin, HIGH);
        digitalWrite(ledPin, LOW);
        totalWatered++;
	Serial.print(totalWatered);
    }
}
