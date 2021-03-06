// Santos Change Machine
// Based on the Raynerd Change Machine ****
/* This code controls both a coin hopper which dispences 5 peso coins and a Philippine bill acceptor (must be new bills).
 */

//**************Variables ****************
volatile byte coinPulseCount=0;    // a counter to see how many times the pin has changed - which coin inserted
volatile byte hopperPulseCount = 0;  // a counter to she how many coins have been ejected 
volatile unsigned long pulseTime;  //this stores the time of the last pulse.
volatile unsigned long secondPulseTime;  //this stores the time of the last pulse.
byte newCoinInserted; // a place to put our last coin pulse count
byte coinValue = 0;      // number of pulses required to dispence each coin type.
byte onePeso = 0;       // count 5x 2 pences to dispence 10p
int amountInsertedSinceLastReset = 0;

//***********************************************************************************************
byte DELAY_BEFORE_COUNTING_PULSES_IN_MILLISECONDS =  1; //EDIT THIS VALUE TO CHANGE DELAY BETWEEN DETECTING BANK OF PULSES
int DELAY_BEFORE_EVALUATING_COIN_COUNT_IN_MILLISECONDS = 500; //EDIT THIS VALUE TO CHANGE DELAY BETWEEN DETECTING BANK OF PULSES
//***********************************************************************************************

//************Pins Used *******************
int hopperPin = 2;  // pin2 as optical count input 
int coinSelectorPin = 3;  // pin3 as optical count input
int relayPin = 7;  // pin7 output relay

void setup() 
{
  Serial.begin(9600);

  //**** pinModes *************
  pinMode(hopperPin, INPUT_PULLUP);       //hopper optical count is an input
  pinMode(coinSelectorPin, INPUT_PULLUP); //coin selector optical input
  pinMode (relayPin, OUTPUT);      //relay pin output
  digitalWrite(relayPin, LOW);   //turn off relay - active LOW.
  digitalWrite(hopperPin, HIGH); //use the internal pullup resistor on the hopper optical input  

  attachInterrupt(1, coinacceptor, FALLING); // CoinAcceptor - attach a PinChange Interrupt to our on the rising edge - link to coinacceptor function
  attachInterrupt(0, hopper, FALLING); // Hopper - attach a PinChange Interrupt to our pin on the falling edge - link to hopper function

}

void loop()
{  
  //CHECK NOW TO SEE WHICH COIN IS INSERTED 
  if (coinPulseCount >0 && millis()- pulseTime > DELAY_BEFORE_COUNTING_PULSES_IN_MILLISECONDS)    //if there is a coin count & the time between now and the last pulse is greater than 1/4 of a second - THE END OF BANK OF PULSES
  {
    secondPulseTime = millis();
    newCoinInserted += coinPulseCount;  //new variable to free up coinPulseCount on the interrupt.
    coinPulseCount = 0;                // clear pulse count ready for a new pulse on the interrupt.
  }

  printMessageWhileWaitingForCoins();

  if (millis() - secondPulseTime > DELAY_BEFORE_EVALUATING_COIN_COUNT_IN_MILLISECONDS){
    switch (newCoinInserted) {

    case 4:    
      amountInsertedSinceLastReset += 20;
      Serial.println("P20 inserted");
      printAmountInsertedSinceLastReset();
      newCoinInserted = 0;
      coinValue = 4;  
      dispence(); 
      break;

    case 10:
      amountInsertedSinceLastReset += 50;
      Serial.println("P50 inserted");
      printAmountInsertedSinceLastReset();
      newCoinInserted = 0;
      coinValue = 10;  
      dispence(); 
      break;

    case 20:    
      amountInsertedSinceLastReset += 100;
      Serial.println("P100 inserted");
      printAmountInsertedSinceLastReset();
      newCoinInserted = 0;
      coinValue = 20;  
      dispence(); 
      break;

    }



  }
}

//*****INTERUPT detecting pulses from the coin acceptor
void coinacceptor()      //Function called when coin enters coin acceptor
{
  coinPulseCount++;
  pulseTime = millis();   //store current time in pulseTime
}

//******INTERUPT detecting pulses from the hopper
void hopper()           //function called when a coin is ejected from the hopper
{
  hopperPulseCount++ ;
}

void dispence()
{
  digitalWrite(relayPin, HIGH);   //turn on relay - active LOW.
  delay(50);  
  hopperPulseCount =0;

  while (hopperPulseCount < coinValue)     
  {
    //do nothing and wait with the relay on dispencing coins until it hits the "coinValue"
  }

  delay(50);    //wait to ensure the coin has enough momentum to leave hopper but not long enough for another coins to dispence!
  digitalWrite(relayPin, LOW);   //turn off relay - active LOW.  

  //**************************************************************************************  
  delay(180);
  coinPulseCount = 0;   // ERROR IN SPIKES - hopper pulsing effects coin acceptor pulse line!
  //************************************************************************************** 

}

void printMessageWhileWaitingForCoins(){
  int INTERVAL_IN_MILLISECONDS = 500;

  if (newCoinInserted > 0 && (millis() % INTERVAL_IN_MILLISECONDS == 0)){
    Serial.println("Checking, checking, checking...");
  }
}

void printAmountInsertedSinceLastReset(){
  String stringOne =  String(amountInsertedSinceLastReset);      
  Serial.print("P");
  Serial.print(stringOne);
  Serial.println(" total money!!!");
}







