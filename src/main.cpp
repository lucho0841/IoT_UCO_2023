#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <String.h>
#include <ArduinoJson.h>
#include <TimeLib.h>

const char *ssid = "SSID";
const char *password = "PASSWORD";

const char *mqttServer = "XXX.com";
const int mqttPort = 00000;
const char *mqttUser = "XXXX";
const char *mqttPassword = "XXXX";
const char *inputTopicSub = "input";
const char *aliveTopicPub = "alive";
const char *StatusRequestTopicPub = "StatusRequest";
const char *JsonStatusTopicPub = "JsonStatus";
const char *OutputTopicPub = "Output";

const char *host = "HostDeLaApi";

const char *daysOfWeek[] = {"Domingo", "Lunes", "Martes", "Miércoles", "Jueves", "Viernes", "Sábado"};
const char *month[] = {"Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio",
                            "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"};

WiFiClient espClient;
PubSubClient clientMQTT(espClient);

String formatDateTime(String datetime) // 2023-03-09T10:16:43.457711-05:00
{
  tmElements_t tm;
  tm.Year = datetime.substring(1, 4).toInt(); // Año 2123
  tm.Month = datetime.substring(5, 7).toInt();
  tm.Day = datetime.substring(8, 10).toInt();
  tm.Hour = datetime.substring(11, 13).toInt();
  tm.Minute = datetime.substring(14, 16).toInt();
  tm.Second = datetime.substring(17, 19).toInt();

  // Construir una estructura time_t con la fecha y hora de tm
  time_t t = makeTime(tm);
  int dayOfWeek = weekday(t);                      // obtiene el número del día de la semana
  String nameOfDay = daysOfWeek[dayOfWeek - 1]; // obtiene el nombre del día de la semana
  String monthName = String(month[tm.Month - 1]);

  return nameOfDay + ", " + datetime.substring(8, 10) + " de " + monthName +
         " de " + datetime.substring(0, 4) + " -- " + datetime.substring(11, 13) + ":" + datetime.substring(14, 16);
}

/********* Setup wifi ***************************
   setup wifi connect to wifi with the constants
   defined up
   while does not connect print a "."
   if connect then print the local ip
************************************************/
void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(F("."));
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

String readResponse(WiFiClient client)
{
  // Espera a que el servidor responda
  while (client.connected() && !client.available())
  {
    delay(1);
  }

  String response = "";

  while (client.available())
  {
    char c = client.read();
    response += c;
    delay(1);
  }

  return response;
}

void getTimeZone(String timeZone)
{
  // Creación del objeto WiFiClient para hacer la petición HTTP
  WiFiClient client;

  // Conexión al servidor remoto
  Serial.println("Conectando al servidor remoto...");
  if (!client.connect(host, 80))
  {
    Serial.println("Error al conectarse al servidor.");
    return;
  }

  // Envío de la petición HTTP
  Serial.println("Enviando petición HTTP...");
  client.println("GET /api/timezone/" + timeZone + " HTTP/1.1");
  client.println("Host: worldtimeapi.org");
  client.println("Connection: close");
  client.println();

  // Leer la respuesta completa del servidor
  String response = readResponse(client);

  client.stop();

  Serial.println("Contenido de response:");
  Serial.println(response);
  Serial.println();

  // Lectura de la respuesta HTTP
  Serial.println("Leyendo respuesta HTTP...");
  String httpStatus = response.substring(0, response.indexOf("\n"));
  Serial.println(httpStatus);
  Serial.println();
  httpStatus = httpStatus.substring(httpStatus.lastIndexOf(" "));
  clientMQTT.publish(StatusRequestTopicPub, httpStatus.c_str());

  if (httpStatus.indexOf("OK") != -1)
  {
    int position = response.indexOf("\r\n\r\n") + 4;
    String headers = response.substring(0, position);
    String body = response.substring(position);

    Serial.println("Contenido de headers:");
    Serial.println(headers);

    Serial.println("Contenido de body:");
    Serial.println(body);

    clientMQTT.publish(JsonStatusTopicPub, "Trama procesada: ");

    // Obtener el cuerpo del mensaje como un objeto JSON
    DynamicJsonDocument doc(1024); // Tamaño del objeto JSON en bytes
    deserializeJson(doc, body);

    // Obtener el valor del atributo "datetime"
    String datetime = doc["datetime"];

    // Imprimir el valor obtenido
    Serial.println(datetime);
    clientMQTT.publish(OutputTopicPub, formatDateTime(datetime).c_str());
  }

  Serial.println("_______________________________________________");
  Serial.println();
}

/********* MQTT Callback ***************************
   here is defined the logic to execute after
   a messages arrives in the desired
   topic, for this example the variable:
   TopicSub
************************************************/
void callback(char *topic, byte *payload, unsigned int length)
{
  // Notify about message arrived
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  // Print the message received
  Serial.print("Message:");
  String timeZone = "";
  for (int i = 0; i < length; i++)
  {
    timeZone += (char)payload[i];
  }

  // Prints with visualization purposes
  Serial.println(timeZone);
  Serial.println("-----------------------");

  getTimeZone(timeZone);
}

void setup()
{

  // Start Serial Communication
  Serial.begin(115200);

  // Connect to WiFi
  setup_wifi();

  // Connect to MQTT Broker
  clientMQTT.setServer(mqttServer, mqttPort);
  clientMQTT.setCallback(callback);

  // MQTT Connection Validation
  while (!clientMQTT.connected())
  {
    Serial.println("Connecting to MQTT...");

    if (clientMQTT.connect("ESP8266Client", mqttUser, mqttPassword))
    {

      Serial.println("connected");
    }
    else
    {

      Serial.print("failed with state ");
      Serial.print(clientMQTT.state());
      delay(2000);
    }
  }

  // Publish to desired topic and subscribe for messages
  clientMQTT.subscribe(inputTopicSub);
  clientMQTT.publish(aliveTopicPub, "Hello from ESP8266");
}

void loop()
{
  // MQTT clientMQTT loop
  clientMQTT.loop();
}
