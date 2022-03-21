//Autor: Daniel Jose Centeno Gonzalez
//Version: 8.4v

//Precision farming greenhouse controller 

//Sensor de Humedad del suelo1 Pin A0
//Sensor de humedad del suelo2 Pin A2
//Fotoresistor Pin A1
//Sensor de DHT11 Pin 7
//Pin de comunicación (2,3) TX, RX
//Pin_PWM_5 BobinaA1
//Pin_PWM_6 BobinaA2
//Pin_PWM_10 BobinaB1
//Pin_PWM_11 BobinaB2

//*************************************************************************************************

//Pines del Motor
#define BobinaA1 5
#define BobinaA2 6
#define BobinaB1 10 //Inhabilitado
#define BobinaB2 11 //Inhabilitado

//*************************************************************************************************

//Libreria del Modulo Bluetooth HC06
#include <SoftwareSerial.h>
SoftwareSerial hc06(2,3); //TX, RX

//*************************************************************************************************

//Variables del sensor de humedad del suelo y fotoresistencia
#define sensor_humedad_pin1 A0 //Sensor de humedad de tierra en el pin A0
#define sensor_humedad_pin2 A2 //Sensor de humedad de tierra en el pin A2
#define fotoresistor_pin A1  //Fotoresistor en el pin A1
int Valor_Humedad_Tierra1 = 0, Valor_Humedad_Tierra2 = 0, Promedio_Humedad_Tierra = 0;  //Valor en la humedad del suelo Sensor1, Sensor2 y promedio
int fotoresistencia_value = 0; //Valor de fotoresistencia

//*************************************************************************************************

//Libreria del sensor DHT11
#include <DHT.h>

//Declarar variables
int DHTPin = 7;         //Sensor de temperatura en el pin 7 digital
DHT dht(DHTPin, DHT11); //Pasarle el pin y el tipo de DHT Y se inicializa la variable
float Temperatura = 0;  //Variable donde se almacenara el valor de la temperatura relativa
float Humedad = 0;      //Variable donde se almacenara el valor de la humedad relativa
float heat_index = 0;   //Variable donde se almacenara el valor del inidce de calor

//*************************************************************************************************

//Tiempo de regado 
boolean Bandera_Regar = false, Regando = false;
unsigned long Tiempo_inicial_Regar = 0, Tiempo_final_Regar = 0;
unsigned long Tiempo_retardo_Regar = 3600000; //Regar 3600000 = 1 hora
unsigned long diferencia_Regar = 0;
int Veces_Regadas=0;

//*************************************************************************************************

//Tiempo retardo DHT11;
unsigned long Tiempo_inicial_DHT11 = 0, Tiempo_final_DHT11 = 0;
unsigned long Tiempo_retardo_DHT11 = 2000; //retardo de 2 segundos
unsigned long diferencia_DHT11 = 0;

//*************************************************************************************************

//Tiempo de mandar datos
unsigned long Tiempo_inicial_hc06 = 0, Tiempo_final_hc06 = 0;
unsigned long Tiempo_retardo_hc06 = 10000; //Manda datos cada 10seg
unsigned long diferencia_hc06 = 0;

//*************************************************************************************************

//Variables para el array de datos 
float Datos_diario[48][5]; //Las columnas son de (T. Relativa, H.Relativa, I.Calor, H.Suelo, L.Ambiental, Veces regadas) Toma los valores cada hora por 2 dias
int cont_horas = 0, i, j;

//*************************************************************************************************

//Tiempo para guardar datos cada 1h;
unsigned long Tiempo_inicial_cadahora = 0, Tiempo_final_cadahora = 0;
unsigned long Tiempo_retardo_cadahora = 3600000; // tiempo de 1h (3600000)
unsigned long diferencia_cadahora = 0;

//Variable para mandar datos por bluethooth 
char valor;  //Variable para indicar que llega una orden

//Variables para el tiempo de giro del motor 

int retardo1 = 400;
int retardo2 = 300;

void setup() {

  //Rellena todas las celdas del array en 0
  for(i=0; i<48; i++){
    for(j=0; j<5; j++){

      Datos_diario[i][j] = 0;
    }
  }

  pinMode(BobinaA1, OUTPUT);
  pinMode(BobinaA2, OUTPUT);
  
  Serial.begin(9600);
  hc06.begin(9600); //Inicializar la instancia hc06
  dht.begin(); //Inicializar la instancia dht
  delay(2000);
  
}

//*************************************************

void loop() {

   ControlState(); //Control del tiempo de ejecucion de cada proceso
   Sensor_Humedad_Tierra(); //Funcion de la humedad del suelo y regado
   Mandar_Datos_array();    //Funcion del envio de datos
}

void ControlState(){

  //Tiempo de respuesta del DHT11 cada 2s
  diferencia_DHT11 = Tiempo_inicial_DHT11 - Tiempo_final_DHT11;
  Tiempo_inicial_DHT11 = millis();

  if(diferencia_DHT11 >= Tiempo_retardo_DHT11){
    
      Tiempo_final_DHT11 = Tiempo_inicial_DHT11;   
          
      Temp_Hume_Index();
   }

   //Tiempo para guardar cada hora
  diferencia_cadahora = Tiempo_inicial_cadahora - Tiempo_final_cadahora;
  Tiempo_inicial_cadahora = millis();

  if(diferencia_cadahora >= Tiempo_retardo_cadahora){ 

     Tiempo_final_cadahora = Tiempo_inicial_cadahora;
     Guardar_datos();
  }

  diferencia_Regar = Tiempo_inicial_Regar - Tiempo_final_Regar;
  Tiempo_inicial_Regar = millis();
   
}

void Temp_Hume_Index(){

  //Capturar valores de la humedad Temperatura e indice de calor
  Humedad = dht.readHumidity();        //Guardar valor de la humedad relativa del sensor DHT11
  Temperatura = dht.readTemperature(); //Guardar valor de la temperatura relativa en grados celsius
  
  if(isnan(Humedad) || isnan(Temperatura) ){ //Preguntar si el sensor tiene esos valores
    Serial.println("Error en la lectura del sensor");
    return;
  }
  
  heat_index = dht.computeHeatIndex(Temperatura, Humedad, false); //Guardar el valor del indice de calor
}

void Sensor_Humedad_Tierra(){

  //Lectura y mapeo de los sensores de humedad del suelo y fotoresistencia ademas del promedio de la humedad del suelo
  Valor_Humedad_Tierra1 = map(analogRead(sensor_humedad_pin1), 0, 1023, 100, 0);  
  Valor_Humedad_Tierra2 = map(analogRead(sensor_humedad_pin2), 0, 1023, 100, 0);
  Promedio_Humedad_Tierra = ((Valor_Humedad_Tierra1 + Valor_Humedad_Tierra2)/2);
  fotoresistencia_value = map(analogRead(fotoresistor_pin), 1023, 0, 100, 0);

  //Sección de regado 
  if(Bandera_Regar == true){
    goto Ir_regar; 
  }
  
  /*Se debe de cumplir ciertas condiciones para que la bomba de agua se encienda
   * y se riegue:
   * El sistema de riego se activa:
   * Si la humedad en el suelo es igual o inferior a 50%
   * Si la luminosidad es inferior a 30%
   * Si la temperatura es inferior a 32%
   */
   
  if(Promedio_Humedad_Tierra<=50 && fotoresistencia_value<30 && Temperatura<32){

    Bandera_Regar = true;  //Activar bandera para el sistema de riego
    Regando = true;
  }

Ir_regar:
  Regar();
  
}

void Regar(){

  //Abrir valvula
  if(Regando == true && Bandera_Regar == true){

    //Secuencia de riego
    AbrirGrifo(); 

    //Poner regando en false para que solo entre una vez
    Regando = false;
    Veces_Regadas++;
  }

  //Tiempo regando 

  if(diferencia_Regar >= Tiempo_retardo_Regar && Bandera_Regar == true){
    Tiempo_final_Regar = Tiempo_inicial_Regar;
    Regando = true;
    Bandera_Regar = false;
    //Cerrar valvula
    CerrarGrifo();
  }
}

void Guardar_datos(){

     //Secuencia del guardado de datos en el array
     if (cont_horas >= 48){

        //Subir una pocision toda la tabla
        for(i=0; i<47; i++){
          for(j=0; j<5; j++){

             Datos_diario[i][j] = Datos_diario[i+1][j];
          }
        }

        //Lenar la ultima fila desplazada
        for(j=0; j<5; j++){
    
           if(j==0){
              Datos_diario[47][j] = Temperatura;
           }
           else if(j==1){
              Datos_diario[47][j] = Humedad;
           }
           else if(j==2){
              Datos_diario[47][j] = heat_index;
           }
           else if(j==3){
              Datos_diario[47][j] = Promedio_Humedad_Tierra;
           }
           else if(j==4){
              Datos_diario[47][j] = fotoresistencia_value;
           }
         }
     }
     else{

         cont_horas++;
         
         for(j=0; j<5; j++){
    
           if(j==0){
              Datos_diario[cont_horas-1][j] = Temperatura;
           }
           else if(j==1){
              Datos_diario[cont_horas-1][j] = Humedad;
           }
           else if(j==2){
              Datos_diario[cont_horas-1][j] = heat_index;
           }
           else if(j==3){
              Datos_diario[cont_horas-1][j] = Promedio_Humedad_Tierra;
           }
           else if(j==4){
              Datos_diario[cont_horas-1][j] = fotoresistencia_value;
           }
         }      
     } 
}

void Mandar_Datos_array(){

  //Secuencia del envio de datos por el modulo bluetooth HC-06
  
  if (hc06.available())  //Si el puerto serie (Bluetooth) está disponible
  {
    valor = hc06.read();

    if (valor == 'D' || valor == 'd')   //Si el dato que llega es una d o D
    {

        //Se mandan todos los datos
        
        //Mandar todos los datos de temperatura de las ultimas 48h
        hc06.println("Temperatura Relativa");
        for(i=0; i<48; i++){
          hc06.print(String(i+1)+"h : "); hc06.println(Datos_diario[i][0]);
        }
        hc06.println(" ");

        //Mandar todos los datos de Humedad de las ultimas 48h
        hc06.println("Humedad relativa");
        for(i=0; i<48; i++){
          hc06.print(String(i+1)+"h : "); hc06.println(Datos_diario[i][1]);
        }
        hc06.println(" ");

        //Mandar todos los datos del indice de calor de las ultimas 48h
        hc06.println("Indice de calor");
        for(i=0; i<48; i++){
          hc06.print(String(i+1)+"h : "); hc06.println(Datos_diario[i][2]);
        }
        hc06.println(" ");

        //Mandar el promedio de la humedad del suelo de las ultimas 48h
        hc06.println("Promedio de la humedad del suelo");
        for(i=0; i<48; i++){
          hc06.print(String(i+1)+"h : "); hc06.println(Datos_diario[i][3]);
        }
        hc06.println(" ");
        
        //Mandar todos los datos de la luz ambiental de las ultimas 48h
        hc06.println("Luz ambiental");
        for(i=0; i<48; i++){
          hc06.print(String(i+1)+"h: "); hc06.println(Datos_diario[i][4]);
        }
        hc06.println(" ");
        
        //Mandar la cantidad de veces que se a regado
        hc06.print("Veces regadas: "); hc06.println(Veces_Regadas);
        hc06.println(" ");
      } 
    }

    //Secuencia del envio de datos si se detecta una conexion usb serial

    if(Serial.available()){ //Si el puerto seria esta disponible
      
      diferencia_hc06 = Tiempo_inicial_hc06 - Tiempo_final_hc06;
      Tiempo_inicial_hc06 = millis();
    
      if(Tiempo_inicial_hc06 - Tiempo_final_hc06 >= Tiempo_retardo_hc06){ 
    
        Tiempo_final_hc06 = Tiempo_inicial_hc06;
    
        //Se mandan todos los datos
        
        //Mandar todos los datos de temperatura de las ultimas 48h
        Serial.println("Temperatura Relativa");
        for(i=0; i<48; i++){
          Serial.print(String(i+1)+"h : "); Serial.println(Datos_diario[i][0]);
        }
        Serial.println(" ");

        //Mandar todos los datos de Humedad de las ultimas 48h
        Serial.println("Humedad relativa");
        for(i=0; i<48; i++){
          Serial.print(String(i+1)+"h : "); Serial.println(Datos_diario[i][1]);
        }
        Serial.println(" ");

        //Mandar todos los datos del indice de calor de las ultimas 48h
        Serial.println("Indice de calor");
        for(i=0; i<48; i++){
          Serial.print(String(i+1)+"h : "); Serial.println(Datos_diario[i][2]);
        }
        Serial.println(" ");

        //Mandar todos los datos de la humedad del suelo de las ultimas 48h
        Serial.println("Promedio de la humedad del suelo");
        for(i=0; i<48; i++){
          Serial.print(String(i+1)+"h : "); Serial.println(Datos_diario[i][3]);
        }
        Serial.println(" ");
        
        //Mandar todos los datos de la luz ambiental de las ultimas 48h
        Serial.println("Luz ambiental");
        for(i=0; i<48; i++){
          Serial.print(String(i+1)+"h: "); Serial.println(Datos_diario[i][4]);
        }
        Serial.println(" "); 

        //Mandar la cantidad de veces que se a regado
        Serial.print("Veces regadas: "); hc06.println(Veces_Regadas);
        Serial.println(" "); 
       }
  }
}

void AbrirGrifo(){

  //Preparar el motor
  delay(20);
  //Mover el motor para abrir el grifo
  digitalWrite(BobinaA1, HIGH);
  digitalWrite(BobinaA2, LOW);
  delay(retardo1); //Tiempo para hacer un giro de 90°

  //APAGAR TODO 
  digitalWrite(BobinaA1, LOW);
  digitalWrite(BobinaA2, LOW);
  
}

void CerrarGrifo(){
  
  delay(20);
  //Mover el motor para cerrar el grifo
  digitalWrite(BobinaA1, LOW);
  digitalWrite(BobinaA2, HIGH);
  delay(retardo2); //Tiempo para hacer un giro de 90°

  //APAGAR TODO 
  digitalWrite(BobinaA1, LOW);
  digitalWrite(BobinaA2, LOW);
   
}
