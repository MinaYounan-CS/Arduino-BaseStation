// This is a demo of the RBBB running as webserver with the Ether Card
// 2010-05-28 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php



//----------------------< Reading Using Serial Commuincation >-----------
String Pg_Dev_Change="";
String Pg_Dev_Nochange="";
# define trem '\n'
String  list[3];
int     count=0;
String  message;
int     commaPosition;  // the position of the next comma in the string
boolean reading=false;
//---------------------------------< Pins Status.. >---------------------
int pin_status[13]={0,0,0,0,0,0,0,0,0,0,0,0,0};
int alg_status[6] ={0,0,0,0,0,0};
//---------------------------------< Pins Code Num >---------------------
const int pc1=1,pc2=2,pc3=4,pc4=7,pc5=8,pc6=12,pc7=13;
const int ps1=3,ps2=5,ps3=6,ps4=9,ps5=10,ps6=11;
//---------------------------------< TempLM35, PhotoCell>----------------
int temp_pin=0,light_pin=1;
float tempc,tempf; 
int light_level;
boolean fan_on_temph=false,led_on_lighth=false;
int cur_fan_pin,cur_fan_tempv,cur_led_pin,cur_led_lightv;

//-----------------------------------------------< Setup >--------------
void setup () 
{
  Serial.begin(9600);
  pinMode(pc1,OUTPUT); pinMode(pc2,OUTPUT);
  pinMode(pc3,OUTPUT); pinMode(pc4,OUTPUT);
  pinMode(pc5,OUTPUT); pinMode(pc6,OUTPUT); pinMode(pc7,OUTPUT);
  
  pinMode(ps1,OUTPUT); pinMode(ps2,OUTPUT);
  pinMode(ps3,OUTPUT); pinMode(ps4,OUTPUT);
  pinMode(ps5,OUTPUT); pinMode(ps6,OUTPUT);
}
//-----------------------------------------------< Functions >----------
void ps_set_output(int pin_id,int value)
{  
  analogWrite(pin_id,value); 
  update_status(pin_id,value,"standard"); 
}
//----------------------------------------------------------------------

void pc_set_on_off(int pin_id,char op[])
{
  if(op=="on")
   { 
     analogWrite(pin_id,HIGH);
     update_status(pin_id,1,"standard"); 
   }
  else
   {
     analogWrite(pin_id,LOW);
     update_status(pin_id,0,"standard"); 
   }
}
//---------------------------------------------------------
void update_status(int pin,int val,char type[])
{
  if(type!="analog")//type means that it is an anlog type or not
  {
    if(pin_status[pin]!=val)
    {
      pin_status[pin]=val;
      Serial.print(pin);
      if(val!=0)
        Serial.println(": is on");
      else
        Serial.println(": is off");
    }
  }
  else
  {
    if(alg_status[pin]!=val)
    {
      alg_status[pin]=val;
      Serial.print(pin);
      Serial.print(": reading =");
      Serial.println(val);
    }
  }
}
//---------------------------------------------------------
int get_reading(int alg_pin)
{  return analogRead(alg_pin);  }
//---------------------------------------------------------
float get_temp_c(int value)
{   return (value*0.0049)*100;  }
//---------------------------------------------------------
float get_temp_f(int value)
{   return (value *0.882) + 32;  }
//---------------------------------------------------------
int get_light(int pin)
{  return (1023-analogRead(pin));  }

void loop () 
{
  
 list[0]="\0";list[1]="\0";list[2]="\0";count=0;
 while(Serial.available()>0)
 {
    reading=true;
    message=Serial.readStringUntil(trem);
    do
    {
      commaPosition = message.indexOf(',');
      if(commaPosition != -1)
      {
         list[count++]=message.substring(0,commaPosition);
          message = message.substring(commaPosition+1, message.length());
      }
      else
      {  // here after the last comma is found
         if(message.length() > 0)
          { // if there is text after the last comma, print it
           list[count++]=message;                        
          }
      }
     }while(commaPosition >=0);
 }// end of reading while loop

//----------------------< Check The Correct String Path >------------------------
 if(reading==true)
 {
   //---------< Print String Recieved >--------- 
   Serial.println("--------< Reading >------");
   for(int j=0;j<3;j++)
     {
       Serial.print(" R: ");
       Serial.println(list[j]);
     }
   Serial.println("-------------------------");
   //-------------------------------------------  
   if(list[0]=="temp_pin")
      {temp_pin=list[1].toInt();}//Serial.println("Temperature Sensor Pin was Set");}
   else if(list[0]=="light_pin")
      {light_pin=list[1].toInt();}//Serial.println("Light Sensor Pin was Set");}
   else if(list[0]=="set_on")
      pc_set_on_off(list[1].toInt(),"on") ;
   else if(list[0]=="set_off") 
      pc_set_on_off(list[1].toInt(),"off") ;
   else if(list[0]=="set_pin_value") 
      ps_set_output(list[1].toInt(),list[2].toInt()) ;
      
   else if(list[0]=="fan_on_temph")
      {
        fan_on_temph=true;
        cur_fan_pin=list[1].toInt();
        cur_fan_tempv=list[2].toInt();
      }
   else if(list[0]=="fan_off_temph")
      {fan_on_temph=false;cur_fan_pin=list[1].toInt();}  
   else if(list[0]=="led_on_lighth")
      {
        led_on_lighth=true;
        cur_led_pin=list[1].toInt();
        cur_led_lightv=list[2].toInt();
      }        
   else if(list[0]=="led_off_lighth")
      {led_on_lighth=false;cur_led_pin=list[1].toInt();}  
   else
      Serial.println("Wrong String");
   reading=false;  
 }//end if reading is true  
  
 //------------------------------< Continuous Processes >----------------
   if(fan_on_temph==true)
   {
     if(tempc>=float(cur_fan_tempv))
         {ps_set_output(cur_fan_pin,(100+cur_fan_tempv)) ;   Serial.println("Fan on");}
     else
         {ps_set_output(cur_fan_pin,0) ;  Serial.println("Fan off");}
   }
   if(led_on_lighth==true)
   {
      if(light_level>=cur_led_lightv)
        { ps_set_output(cur_led_pin,0) ; Serial.println("Led on");}
      else
        {ps_set_output(cur_led_pin,200) ; Serial.println("Led off");}
   }
   tempc=get_temp_c(get_reading(temp_pin));
   //Serial.print("Temp val=");Serial.println(tempc);
   tempf=get_temp_f(get_reading(temp_pin));
   light_level=get_light(light_pin);
}

