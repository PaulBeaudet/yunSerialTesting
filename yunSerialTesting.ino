// yunSerialTesting.ino -- Gain terminal access to Yun's linux system
void setup() 
{
  Serial.begin(115200); // open serial connection via USB-Serial
  Serial1.begin(250000);// open serial connection to Linux 
  bootCheck(true);      // true denotes output <---------COMMENT IN THIS
  //delay(90000);       // wait for linux to boot <---------OR THIS
  Serial1.write((uint8_t *)"\xff\0\0\x05XXXXX\x7f\xf9", 11); //shutdown bridge
  Serial1.println();//send a new line character to enter shutdown garbage
  delay(2);// wait for the buffer to fill with garbage
  while(Serial1.available()){Serial1.read();} // read out shutdown garbage 
  Serial1.println("ls"); // list current contents of current directory
}// Serial Monitor access to Yun (ash) Terminal achieved

void loop() 
{ // send and recieve tty
  if(Serial.available()){Serial1.write(Serial.read());} 
  //"outgoing" serial buffer have bytes? write them to Open WRT
  if(Serial1.available()){Serial.write(Serial1.read());}
  //"incoming" serial buffer have bytes? write them to Serial Monitor
}

//******************** Other examples ***********************
boolean bootCheck(boolean startUpOutput)//pass true for verbose output
{
  boolean booting = false;//assume complete boot
  timer(17800);           //set timer for max distance between boot outputs
  // have recorded +16 sec between some outputs: important if reset midboot
  while(!timer(0))        //before the timer is up
  {
    while(Serial1.available())
    {
      bootHandler(startUpOutput);
      booting = true;    //buffer filled before user interaction was possible
    }
  }                      // timer returns true when finished exiting loop
  if (booting)
  {
    timer(50000);        //give enough time to finish booting
    while(!timer(0))     //before time is finished     
    {
      while(Serial1.available()){bootHandler(startUpOutput);}
    }                    //handle rest of output
  }
  return booting;        //in case of conditions looking for boot
}

void bootHandler(boolean startUpOutput)//pass true for verbose output
{ //mirror boot process to the serial monitor if true argument is passed
  if(startUpOutput){Serial.write(Serial1.read());} 
  else{Serial1.read();}//empty buffer with empty reads
}

boolean timer(uint32_t durration)
{//used for checking an setting timer
  static uint32_t ptimer[2] = { };// create timer to modify
  if(durration)
  {
    ptimer[1]=durration; //set durration
    ptimer[0]=millis();  // note the time set
  }
  else if(millis() - ptimer[0] > ptimer[1]){return true;}
  // if the durration has elapsed return true
  return false;
} 
