#include <SPI.h> 
#include <Ethernet.h> 
#include "DHT.h"


#define DHTPIN 2 // номер пина, к которому подсоединен датчик
DHT dht(DHTPIN, DHT11);

// введите ниже MAC-адрес и IP-адрес вашего контроллера; 
// IP-адрес будет зависеть от вашей локальной сети: 
byte mac[] = { 
0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; 
IPAddress ip(192,168,1,111); 

// инициализируем библиотеку Ethernet Server, указывая нужный порт 
// (по умолчанию порт для HTTP – это «80»): 
EthernetServer server(80); 

// задаем контакт и начальное состояние для реле: 
String relay1State = "Off"; 
String relay2State = "Off"; 
const int relay1 = 8; 
const int relay2 = 6; 

String relay3State = "Off"; 
const int relay3 = 3; 




// задаем переменные для клиента: 
char linebuf[80]; 
int charcount=0; 
int knopkaRelay1=0; 
int knopkaRelay2=0;
int x=1;




void setup() { 

float h = dht.readHumidity();
float t = dht.readTemperature();
pinMode(knopkaRelay1, INPUT); 
pinMode(knopkaRelay2, INPUT); 




// подготавливаем реле-модуль: 
pinMode(relay1, OUTPUT); 
digitalWrite(relay1, HIGH); 
pinMode(relay2, OUTPUT); 
digitalWrite(relay2, HIGH); 
pinMode(relay3, OUTPUT); 
digitalWrite(relay3, HIGH);

// открываем последовательную коммуникацию на скорости 9600 бод: 
Serial.begin(9600); 

// запускаем Ethernet-коммуникацию и сервер: 
Ethernet.begin(mac, ip); 
server.begin(); 
Serial.print("server is at "); // "сервер на " 
Serial.println(Ethernet.localIP()); 
} 

// Показываем веб-страницу с кнопкой «вкл/выкл» для реле: 
void dashboardPage(EthernetClient &client) { 
client.println("<!DOCTYPE HTML><html><head>"); 
client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><body>"); 
client.println("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"); 
client.println("<h3>Arduino Web Server - <a href=\"/\">Обновить</a></h3>"); 

client.println("<style> p1 { background: #D9FFAD; color: green; padding: 2px; }</style> "); 
client.println("<style> p2 { background: #f97f7f; color: reed; padding: 2px; }</style> "); 
client.println("<center>"); 

// генерируем кнопку для управления реле1: 
client.println("<h4>Состояние реле 1:"); 
// если реле выключено, показываем кнопку «вкл»: 
if(relay1State == "Off"){ 
client.println("<p2> " + relay1State + "</p2> </h4>"); 
client.println("<a href=\"/1n\"><button>ВКЛ</button></a>"); 
} 
// если реле включено, показываем кнопку «выкл»: 
else if(relay1State == "On"){ 
client.println(" <p1> " + relay1State + "</p1> </h4>"); 
client.println("<a href=\"/1f\"><button>ВЫКЛ</button></a>"); 
} 



client.println("<h4>Состояние реле 2:"); 
// если реле выключено, показываем кнопку «вкл»: 
if(relay2State == "Off"){ 
client.println("<p2> " + relay2State + "</p2> </h4>"); 
client.println("<a href=\"/2n\"><button>ВКЛ</button></a>"); 
} 
// если реле включено, показываем кнопку «выкл»: 
else if(relay2State == "On"){ 
client.println("<p1> " + relay2State + "</p1> </h4>"); 
client.println("<a href=\"/2f\"><button>ВЫКЛ</button></a>"); 
} 


float h = dht.readHumidity();
float t = dht.readTemperature();
client.println("<br><br>"); 
client.println(" Температура:");
client.println(t); 

client.println("<br><br>");
client.println(" Влажность:");
client.println(h); 

client.println("<br><br>"); 

client.println("</center></body></html>"); 
} 


void loop() { 




// работа с сервером 
// прослушиваем входящих клиентов: 
EthernetClient client = server.available(); 
if (client) { 
Serial.println("new client"); // "новый клиент" 
memset(linebuf,0,sizeof(linebuf)); 
charcount=0; 
// HTTP-запрос заканчивается пустой строкой: 
boolean currentLineIsBlank = true; 
while (client.connected()) { 
if (client.available()) { 
char c = client.read(); 
// считываем HTTP-запрос, символ за символом: 
linebuf[charcount]=c; 
if (charcount<sizeof(linebuf)-1) charcount++; 
// если вы дошли до конца строки (т.е. если получили 
// символ новой строки), это значит, 
// что HTTP-запрос завершен, и вы можете отправить ответ: 
if (c == '\n' && currentLineIsBlank) { 
dashboardPage(client); 
break; 
} 


if (c == '\n') { 

if (strstr(linebuf,"GET /1f") > 0){ 
digitalWrite(relay1, HIGH); 
relay1State = "Off"; 
Serial.print("Состояние реле1: "); 
Serial.print(relay1State); 
Serial.print(" "); 
} 
else if (strstr(linebuf,"GET /1n") > 0){ 
digitalWrite(relay1, LOW); 
relay1State = "On"; 
Serial.print("Состояние реле1: "); 
Serial.print(relay1State); 
Serial.print(" "); 
} 

else if (strstr(linebuf,"GET /2f") > 0){ 
digitalWrite(relay2, HIGH);
relay2State = "Off"; 
Serial.print("Состояние реле2: "); 
Serial.print(relay2State); 
Serial.print(" "); 
} 
else if (strstr(linebuf,"GET /2n") > 0){ 
digitalWrite(relay2, LOW); 
relay2State = "On"; 
Serial.print("Состояние реле2: "); 
Serial.print(relay2State); 
Serial.print(" "); 
} 






// если получили символ новой строки... 
currentLineIsBlank = true; 
memset(linebuf,0,sizeof(linebuf)); 
charcount=0; 
} 
else if (c != '\r') { 
// если получили какой-то другой символ... 
currentLineIsBlank = false; 
} 
} 
} 

// даем веб-браузеру время на получение данных: 
delay(1); 
// закрываем соединение: 
client.stop(); 
Serial.println("client disonnected"); // "Клиент отключен" 
} 











// работа без сервера 
// кнопка 1 реле 1 
if(digitalRead(9)==HIGH&&knopkaRelay1==0)//если кнопка нажата и перемення "knopkaRelay1" равна 0 , то 
{ 
Serial.print("кнопка:"); 
delay(250);//защита от дребезга 
if (relay1State == "Off") 
{ 
Serial.print("ВКЛ "); 
digitalWrite(relay1, LOW); 
relay1State = "On"; 
} 
else 
{ 
Serial.print("ВЫКЛ "); 
digitalWrite(relay1, HIGH); 
relay1State = "Off"; 
} 
knopkaRelay1++;//пишем 1 в переменную кноjпка 
Serial.print("Состояние: "); 
Serial.print(relay1State); 
Serial.print(" "); 
} 
if(digitalRead(9)==LOW&&knopkaRelay1==1)//если кнопка НЕ нажата и перемення "knopkaRelay1" равна 1 , то 
{ 
knopkaRelay1=0;//обнуляем переменную "knopkaRelay1" 
} 








// кнопка 2 реле 2 
if(digitalRead(7)==HIGH&&knopkaRelay2==0)//если кнопка нажата и перемення "knopkaRelay1" равна 0 , то 
{ 
Serial.print("кнопка:"); 
delay(250);//защита от дребезга 
if (relay2State == "Off") 
{ 
Serial.print("ВКЛ "); 
digitalWrite(relay2, LOW); 
relay2State = "On"; 
} 
else 
{ 
Serial.print("ВЫКЛ "); 
digitalWrite(relay2, HIGH); 
relay2State = "Off"; 
} 
knopkaRelay2++;//пишем 1 в переменную кноjпка 
Serial.print("Состояние: "); 
Serial.print(relay2State); 
Serial.print(" "); 
} 
if(digitalRead(7)==LOW&&knopkaRelay2==1)//если кнопка НЕ нажата и перемення "knopkaRelay2" равна 1 , то 
{ 
knopkaRelay2=0;//обнуляем переменную "knopkaRelay2" 
} 





float h = dht.readHumidity();
float t = dht.readTemperature();


if(t>=34)//если кнопка нажата и перемення "knopkaRelay1" равна 0 , то 
{ 
Serial.print("ВКЛ "); 
digitalWrite(relay3, LOW); 
relay3State = "On"; 
} 
if (t<34) 
{ 
Serial.print("ВЫКЛ "); 
digitalWrite(relay3, HIGH); 
relay3State = "Off"; 
}



}
