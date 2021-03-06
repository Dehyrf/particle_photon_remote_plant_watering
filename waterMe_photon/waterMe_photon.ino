// -----------------------------------
// Controlling plant watering over the Internet
// -----------------------------------

//Initilize global variables
#define ONE_SYNC_MILLIS (60 * 60 * 1000) //Define leeway for clock variation (is currently allowing 1 hour variation)
unsigned long lastSync = millis();  //Timer from last time sync used later for getting realtime data
int ledPin = D7;    //Pin for the led to indicate when relay is enabled
int relayPin = D6;  //Pin turning pump control relay on/off
int startPin = D5;  //Will start/stop the watering if connected/disconnected for a hardware failsafe
bool enabled = false;//to hold startPin value
int maxTime = (10 * 1000);   //Max time in seconds to run the pump per sitting
int minDelay = (8 * 60 * 60 * 1000);    //Minimum amount of time in hours between watering sessions (to prevent water spillage) 
int autoDelay = (48 * 60 * 60 * 1000);  //period in hours between automated watering
int canWater = 1;   //Tells user if they can water more today (1:yes, 0:no)
int lastTime = 0;   //Time since last watering session (milliseconds)
int totalWatered = 0;   //Times watered total;
int lastAutoTime = 0;   //Time since last automated watering session
int temp = 0;   //Generic variable used for various tasks and previously for debugging

// Register our Particle functions we will use that will allow for online control and setup io pins
void setup()
{
   //Start pin configuration
   pinMode(ledPin, OUTPUT);
   pinMode(relayPin, OUTPUT);
   pinMode(startPin, INPUT);
   digitalWrite(relayPin, HIGH); //Ensure relay pin is low/off at the start
   
   if (Particle.connected() == false) { //Connect particle to cloud if it is not already
        Particle.connect();
   }   
   
   Particle.variable("canWater", &canWater, INT);   //Declare variable to return watering status
   Particle.variable("total", &totalWatered, INT);  //Shows total times watered since initialized
   Particle.function("waterMe", remoteWater);       //Declare Particle function for the relay
   
   remoteWater("water");    //Run first water cycle
}

void loop()
{
    enabled = digitalRead(startPin);    //Read hardware enabling pin data
    
    if (enabled && (millis() - lastTime) > minDelay)   //Make sure time delay between watering sessions is being respected
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
        temp = remoteWater("water");
    }
    else if (canWater == 0 && (millis() - lastAutoTime) > autoDelay)
        lastAutoTime = millis();
        
    if ((millis() - lastSync) > ONE_SYNC_MILLIS) { //Request time synchronization from the Particle Cloud
        Particle.syncTime();
        lastSync = millis();
    }
}

int remoteWater(String in) {    //Water and reset time since last watering cycle
    enabled = digitalRead(startPin);    //Read hardware enabling pin data
    
    if (enabled && canWater == 1) {
        lastTime = millis();
        digitalWrite(relayPin, LOW);
        digitalWrite(ledPin, HIGH);
        delay(maxTime);
        digitalWrite(relayPin, HIGH);
        digitalWrite(ledPin, LOW);
        totalWatered++;
        return 1;
    }
    else
        return 0;
}
