//sensor variables
int R1= 992; // Value of resistor for EC probe
int EC_Read = A0;
int ECPower = A1;
int Temp_pin = A5;
//LED variables
int redLED = A2;
int greenLED = A3;
int blueLED = A4;
//calculation variables
float Temp_C; // temperature in celcius
float Temp_F; // temperature in fahrenheit
float Temp1_Value = 0;
float Temp_Coef = 0.019; // temperature coefficient
/////////////////Change for calibration only///////////////
float Calibration_PPM =2500 ; //given ppm of a calibration solution
float K=1.89; //=change this constant once for calibration
float PPM_Con=1; //trial and error after k is found
/////////////////////////////////////////////////////////////////////////////////////
float CalibrationEC= (Calibration_PPM*2)/1000;
float Temperature;
float EC;
float EC_at_25;
int ppm;
float A_to_D= 0;
float Vin= 5;
float Vdrop= 0;
float R_Water;
float Value=0;

void setup()
{
  //sensor pins
  Serial.begin(9600);
  pinMode(EC_Read,INPUT);
  pinMode(ECPower,OUTPUT);
  pinMode(ECPower, HIGH);
  pinMode(Temp_pin, INPUT);
  //led pins
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);

  //////////////////////////////////////////////////////////////////////////////////////////
  //Calibrate (); // After calibration comment out this line
  //////////////////////////////////////////////////////////////////////////////////////////
}
void loop()
{
  GetEC(); //Calls GetEC()
  delay(6000); //Do not make this less than 6 sec (6000)
}
////////////////////////////////////////////////////////////////////////////////////
void GetEC()
{
  int val;
  double Temp;
  val=analogRead(Temp_pin);
  Temp = log(((10240000/val) - 10000));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp);
  Temp_C = Temp - 273.15; // Kelvin to Celsius
  Temp_F = (Temp_C * 9.0)/ 5.0 + 32.0; // Celsius to Fahrenheit
  Temp1_Value = Temp_C;
  Temperature = Temp_C;
  digitalWrite(ECPower,HIGH);
  A_to_D= analogRead(EC_Read);
  A_to_D= analogRead(EC_Read);
  digitalWrite(ECPower,LOW);
  Vdrop= (Vin*A_to_D) / 1024.0;
  R_Water = (Vdrop*R1) / (Vin-Vdrop);
  EC = 1000/ (R_Water*K);
  EC_at_25 = EC / (1+ Temp_Coef*(Temperature-25.0));
  ppm=(EC_at_25)*(PPM_Con*1000);
// Calculate voltage
  float voltage = A_to_D * (Vin / 1024.0);
  Serial.print("Voltage: ");
  Serial.print(voltage, 2); // Print voltage with 2 decimal places
  Serial.println("V");


  Serial.print(" EC: ");
  Serial.print(EC_at_25);
  Serial.print(" milliSiemens(mS/cm) ");
  Serial.print(ppm);
  Serial.print(" ppm ");
  Serial.print(Temperature);
  Serial.println(" *C ");
  showLED(ppm);
}
////////////////////////////////////////////////////////////////////////////////////
void Calibrate ()
{
  Serial.println("Calibration routine started");
  float Temperature_end=0;
  float Temperature_begin=0;
  int val;
  double Temp;
  val=analogRead(Temp_pin);


  Temp = log(((10240000/val) - 10000));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp);
  Temp_C = Temp - 273.15; // Kelvin to Celsius
  Temp_F = (Temp_C * 9.0)/ 5.0 + 32.0; // Celsius to Fahrenheit
  Temp1_Value = Temp_C;
  Temperature_begin=Temp_C;
  Serial.print(Temperature_begin);
  Value = 0;
  int i=1;
  while(i<=10){//totals a minute; if not enough time, change i<=10
    digitalWrite(ECPower,HIGH);
    A_to_D= analogRead(EC_Read);
    A_to_D= analogRead(EC_Read);
    digitalWrite(ECPower,LOW);
    Value=Value+A_to_D;
    i++;
    delay(6000);//wait 6 sec
  };
  A_to_D=(Value/10);
  val=analogRead(Temp_pin);
  Temp = log(((10240000/val) - 10000));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp);
  Temp_C = Temp - 273.15; // Kelvin to Celsius
  Temp_F = (Temp_C * 9.0)/ 5.0 + 32.0; // Celsius to Fahrenheit
  Temp1_Value = Temp_C;
  Temperature_end=Temp_C;
  EC =CalibrationEC*(1+(Temp_Coef*(Temperature_end-25.0)));
  Vdrop= (((Vin)*(A_to_D))/1024.0);
  R_Water=(Vdrop*R1)/(Vin-Vdrop);
  float K_cal= 1000/(R_Water*EC);
  Serial.print("Replace K in line 23 of code with K = ");
  Serial.println(K_cal);
  Serial.print("Temperature difference start to end were = ");
  Temp_C=Temperature_end-Temperature_begin;
  Serial.print(Temp_C);
  Serial.println("*C");
  Serial.println("Temperature difference start to end must be smaller than 0.15*C");
  Serial.println("");
  Calibrate ();
}
////////////////////////////////////////////////////////////////////////////////////
void showLED(int ppm){
int x;//higher bound
int y;//middle bound
int z;//lower bound

if (ppm > x){
  digitalWrite(redLED, HIGH);//turn red on
  digitalWrite(greenLED, LOW);//turn green off
  digitalWrite(blueLED, LOW);//turn blue off
}else if(ppm <x && ppm >y){
  digitalWrite(redLED, LOW);//turn red off
  digitalWrite(greenLED, HIGH);// turn green on
  digitalWrite(blueLED, LOW);//turn blue off
}else if (ppm < y && ppm > z){
  digitalWrite(redLED, LOW);//turn red off
  digitalWrite(greenLED, LOW);//turn green off
  digitalWrite(blueLED, HIGH);//turn blue on
}else {
  digitalWrite(redLED, LOW);//turn red off
  digitalWrite(greenLED, HIGH);//turn green on
  digitalWrite(blueLED, HIGH);//turn blue on
}
}

