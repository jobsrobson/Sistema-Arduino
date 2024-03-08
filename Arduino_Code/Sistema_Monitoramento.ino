#include <dht.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SD.h>
#include <Wire.h>
#include <RTClib.h>

#define LCD_Backlight 6

/* Pinos */
const int DHTpin = A1;          // Umidade do ar
const int SNA = A2;             // Nível da água
const int Potenciometro = A3;   // Potenciômetro para Brilho do LCD
const int DS18B20_PIN = 2;      // Temperatura da Água
const int chipSelect = 10;      // Data Logger

/* Definições */
dht DHT;
LiquidCrystal_I2C lcd(0x27, 20, 4);
const char* fileName = "data.csv";
OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);
RTC_DS1307 rtc;
File dataFile;
bool sdCardPresent = true; // Variável para rastrear a presença do cartão SD

/* Controle do horário de gravações - NOVO! */
unsigned long previousDataTime = 0;
unsigned long previousLCDTime = 0;
const unsigned long dataInterval = 5UL * 60UL * 1000UL;  // 5 minutos em milissegundos
const unsigned long lcdInterval = 15UL * 1000UL;   // 15 segundos


/* SETUP */
void setup() {
  Wire.begin();
  Serial.begin(9600);
  rtc.begin();
  oneWire.begin(DS18B20_PIN);
  sensors.begin();

  /* INICIALIZAÇÃO DO DISPLAY LCD */
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Aguarde...");
  int potValue = analogRead(Potenciometro);
  int brilhoLCD = map(potValue, 0, 1023, 0, 255);                  // Lê o potenciômetro e transforma em valor para Brilho do LCD
  analogWrite(6, brilhoLCD);                                       // Ajusta o Brilho do LCD

  Serial.println("Aguarde inicialização.");

  /* VERIFICAÇÃO DO CARTÃO SD */
  if (!SD.begin(chipSelect)) {
    lcd.clear();
    lcd.print("Impossivel iniciar");
    lcd.setCursor(0,1);
    lcd.print("o Cartao SD.");
    lcd.setCursor(0,3);
    lcd.print("Reinicie p/ cont.");
    Serial.println("Falha ao iniciar o Cartão SD.");
    while (true) {
    }
  }

  dataFile = SD.open(fileName, FILE_WRITE);

  /* VERIFICAÇÃO DO ARQUIVO DE DADOS*/
  if (!dataFile) {
    lcd.clear();
    lcd.print("Falha ao gravar o");
    lcd.setCursor(0,1);
    lcd.print("arq. de dados.");
    lcd.setCursor(0,3);
    lcd.print("Reinicie p/ cont.");
    sdCardPresent = false;
    while (true) {
    }
  }

  /* ESCRITA DO CABEÇALHO DO ARQUIVO DE DADOS*/
  if (dataFile.size() == 0) {
    dataFile.println("data;horario;temp_ambiente;temp_agua;umid_ar");
  }

  /* AJUSTE e VERIFICAÇÃO DA DATA E HORA DO RTC 
  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  } 
  */

  lcd.clear();
  lcd.print("Pronto para iniciar.");

}


/* LOOP */
void loop() {

  unsigned long currentMillis = millis();

  /* LEITURA DO ESTADO DO POTENCIÔMETRO E AJUSTE DO BRILHO DO LCD */
  int potValue = analogRead(Potenciometro);
  int brilhoLCD = map(potValue, 0, 1023, 0, 255); // Lê o potenciômetro e transforma em valor para Brilho do LCD
  analogWrite(6, brilhoLCD); // Ajusta o Brilho do LCD

  /* VERIFICAÇÃO CONTÍNUA DO ESTADO DA GRAVAÇÃO */
  if (!SD.begin(chipSelect)) {
    sdCardPresent = false; // Define a variável para falso se o cartão SD não estiver presente
  } else {
    sdCardPresent = true; // Define a variável como verdadeira se o cartão SD estiver presente e funcional
  }

  if (!sdCardPresent) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Cartao SD removido!");
    lcd.setCursor(0,2);
    lcd.print("Ins. SD e reinicie");
    lcd.setCursor(0,3);
    lcd.print("para continuar.");
    while (true) {
    }
  }


  /* LEITURA DA TEMPERATURA DA ÁGUA E DO AMBIENTE - DS18B20 */
  sensors.requestTemperatures();
  float tempAgua = sensors.getTempCByIndex(1);
  float tempAmbiente = sensors.getTempCByIndex(0); // Leitura do segundo DS18B20

  /* VERIFICAÇÃO DE LEITURA DOS SENSORES DE TEMPERATURA */
  if (tempAmbiente == -127.00 || tempAgua == -127.00) {
    lcd.clear();
    lcd.print("Falha na leitura");
    lcd.setCursor(0,1);
    lcd.print("dos sensores temp.");
    while (true) {
    }
  }

  /* LEITURA DA UMIDADE DO AR */
  DHT.read11(DHTpin);
  float umidAr = DHT.humidity;

  /* LEITURA DA DATA E HORA */
  DateTime now = rtc.now();
  char timeBuffer[9]; // Cria um buffer char para armazenar o formato da hora. HH:MM:SS + o caractere nulo 
  snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d", now.hour(), now.minute(), now.second()); // Formata a hora e a data no buffer

  // Cálculo do horário da próxima gravação
  unsigned long timeToNextRecord = (dataInterval - (currentMillis - previousDataTime)) / 1000;
  int nextRecordHour = (now.hour() + (timeToNextRecord / 3600)) % 24;
  int nextRecordMinute = (now.minute() + (timeToNextRecord / 60) % 60) % 60;
  String nextRecordHourStr = (nextRecordHour < 10) ? "0" + String(nextRecordHour) : String(nextRecordHour);
  String nextRecordMinuteStr = (nextRecordMinute < 10) ? "0" + String(nextRecordMinute) : String(nextRecordMinute);

  /* GRAVAÇÃO DOS DADOS NO ARQUIVO */
  if (currentMillis - previousDataTime >= dataInterval) {
    dataFile.print(getFormattedDateTime(now));
    dataFile.print(";");
    dataFile.print(tempAmbiente);
    dataFile.print(";");
    dataFile.print(tempAgua);
    dataFile.print(";");
    dataFile.println(umidAr);
    dataFile.flush();
    previousDataTime = currentMillis;
  }

  /* EXIBIÇÃO DAS INFORMAÇÕES NO LCD */
  if (currentMillis - previousLCDTime >= lcdInterval) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ambiente:    " + String(tempAmbiente) + " C");
    lcd.setCursor(0, 1);
    lcd.print("Agua:        " + String(tempAgua) + " C");
    lcd.setCursor(0, 2);
    lcd.print("Umidade:     " + String(umidAr) + " %");
    lcd.setCursor(0, 3);
    lcd.print("Prox. Grav:    " +  nextRecordHourStr + ":" + nextRecordMinuteStr);
    previousLCDTime = currentMillis;
  }

}


/* FUNÇÕES EXTRAS */
float getTemperature(int pin) {
  int raw = analogRead(pin);
  return (raw * 500.0) / 1023.0;
}

/* FUNÇÃO PARA FORMATAR DATA E HORA */
String getFormattedDateTime(DateTime dateTime) {
  String formattedDateTime = "";

  formattedDateTime += String(dateTime.day()) + "/";
  formattedDateTime += String(dateTime.month()) + "/";
  formattedDateTime += String(dateTime.year()) + ";";
  formattedDateTime += String(dateTime.hour()) + ":";
  formattedDateTime += String(dateTime.minute()) + ":";
  formattedDateTime += String(dateTime.second());

  return formattedDateTime;
}