
// include the modules for lcd, servo motor and IR remote
#include <LiquidCrystal.h>
#include<Servo.h>
#include<IRremote.h>
unsigned int code1 = 16582903; //code recieved from button 1
unsigned int code2 = 16615543;//code recieved from button 2
unsigned int code3 = 16580863; //code recieved from 'power button'
int RECV_PIN = 5;
IRrecv irrecv(RECV_PIN);
decode_results results;

Servo servo_rice; //declaring servo motors
Servo servo_wheat;

float weight_rice;
float weight_wheat;
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, A1, A2, A3, A4);
//initialising lcd with the pins connected to arduino

//details of the card of the customer
struct card_detail{
  char numname[5];
  char goods[14];
  int statusrice;//1 if buying rice is allowed and 0 if not
  int statuswheat;
};


card_detail card_registered[10];
void card_registered_database()
{
  card_registered[0]={{'A','0','0','1','1'},{'r','i','c','e','-','1',':','w','h','e','a','t','-','1'},0,0};
  card_registered[1]={{'A','0','0','1','2'},{'r','i','c','e','-','1',':','w','h','e','a','t','-','0'},0,1};
  card_registered[2]={{'A','0','0','1','3'},{'r','i','c','e','-','0',':','w','h','e','a','t','-','1'},1,0};
  card_registered[3]={{'A','0','0','1','4'},{'r','i','c','e','-','1',':','w','h','e','a','t','-','1'},0,0};
  card_registered[4]={{'A','0','0','1','5'},{'r','i','c','e','-','1',':','w','h','e','a','t','-','0'},0,1};
  card_registered[5]={{'A','0','0','1','6'},{'r','i','c','e','-','1',':','w','h','e','a','t','-','1'},0,0};
  card_registered[6]={{'A','0','0','1','7'},{'r','i','c','e','-','0',':','w','h','e','a','t','-','1'},1,0};
  card_registered[7]={{'A','0','0','1','8'},{'r','i','c','e','-','1',':','w','h','e','a','t','-','1'},0,0};
  card_registered[8]={{'A','0','0','1','9'},{'r','i','c','e','-','0',':','w','h','e','a','t','-','1'},1,0};
  card_registered[9]={{'A','0','0','2','0'},{'r','i','c','e','-','1',':','w','h','e','a','t','-','0'},0,1};}

//To get the card input and check if it is present in the registered data
int find(char numname_0,char numname_1,char numname_2,char numname_3,char numname_4)
{
  int status=-1;
  for(int i=0;i<10;i++)
  {
    if(numname_0==card_registered[i].numname[0] &&numname_1==card_registered[i].numname[1]&&numname_2==card_registered[i].numname[2]&&numname_3==card_registered[i].numname[3]&&numname_4==card_registered[i].numname[4]) 
    {//comparing all the characters of the input with data
      status=i;//the index of the card in database
      return status;
      break;
    }
  }
  return status;
}
void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn(); //enabling the IR remote
  servo_wheat.attach(3); //connect the servo motor of wheat outlet controller to pin 3
  servo_rice.attach(6);//connect the servo motor of rice outlet controller to pin 6
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);//switching on lcd
  card_registered_database();
}
   
void loop() {
  int status=-1;
  servo_rice.write(90); //initially close the outlets by giving 90deg angle
  servo_wheat.write(90); //initially close the outlets 
  //lcd.println("Enter CardNum");
  if(Serial.available()>0) //get the input from user
  {
    char numname_0=Serial.read();
    delay(500);
    char numname_1=Serial.read();
    delay(500);
    char numname_2=Serial.read();
    delay(500);
    char numname_3=Serial.read();
    delay(500);
    char numname_4=Serial.read();
    Serial.print("You have typed ");
    Serial.print(numname_0);
    Serial.print(numname_1);
    Serial.print(numname_2);
    Serial.print(numname_3);
    Serial.print(numname_4);    
   status=find(numname_0,numname_1,numname_2,numname_3,numname_4);
    
    if(status>=0)//means the customer is registered and he can proceed
    {
      for(int j=0;j<15;j++)
      {
        lcd.setCursor(j,0);
      	lcd.print(card_registered[status].goods[j]);//displays the goods available
        Serial.print(card_registered[status].goods[j]);
      }
      
      lcd.setCursor(0,1);
      unsigned int choice;
      lcd.print("choose 1 or 2");
      delay(5000);
      decode_results results;//gets the input from IR remote-1 for rice and 2 for wheat
      if (irrecv.decode(&results))
 		{
        choice = results.value;//get the choice pressed by customer
        lcd.clear();
        Serial.println(choice);
        if (choice==code1){//choice for RICE
        Serial.println("Choice:");
 		Serial.print(1);
        Serial.println("--rice");
         lcd.print("Choice:");
 		lcd.print(1);
        lcd.print("--rice");
          
        lcd.setCursor(0,1);
 		irrecv.resume();
          //if the status is 1 then the rice is already bought and hence he cant buy
          if (card_registered[status].statusrice==1){
            Serial.println("RICE ALREADY TAKEN");
            lcd.clear();
          lcd.print("RICE ALREADY TAKEN");}
          else if(card_registered[status].statusrice==0){
            servo_rice.write(180);//since status is 0, the rice outlet opens
            float load  = analogRead(A0);//rice starts pouring on load sensor
            //sensor reads the weight
            //lcd.print("Weight:");
             while(load<10.00){
     		load = analogRead(A0);
   			servo_rice.write(180);
             //it stays open till weight is less than 1kg=10N
 			}
              servo_rice.write(90);//after reaching 1kg it closes
            lcd.clear();
            lcd.setCursor(0,0);
  			lcd.print("RICE : 1Kg");
            card_registered[status].statusrice = 1;
          }    
 		}
        //same functionality for rice slot
        if (choice==code2){
        Serial.print("Choice:");
 		Serial.print(2);
        Serial.println("--wheat");
        lcd.print("Choice:");
 		lcd.print(1);
        lcd.print("--wheat");
 		irrecv.resume();
          if (card_registered[status].statuswheat==1){
            Serial.println("WHEAT ALREADY TAKEN");
          lcd.clear();
          lcd.print("WHEAT ALREADY TAKEN");}
          else if(card_registered[status].statuswheat==0){
            servo_wheat.write(180);
            float load1  = analogRead(A5);
             while(load1<10.0){   //10N is 1kg of mass
     		load1 = analogRead(A5);
   			servo_wheat.write(180);
 			}
              servo_wheat.write(90);
            lcd.setCursor(0,1);
  			lcd.print("WHEAT : 1Kg");
            card_registered[status].statuswheat = 1;
          }    
 		}   
      }}
    else
    {//if the card number did not match the customer cannot buy
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("CARD NOT REGISTERED");}
    
  
}
}
 
