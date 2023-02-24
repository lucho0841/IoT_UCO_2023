#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const char* ssid = "SSID";
const char* password = "Password";

ESP8266WebServer server(80);

int ledConection1 = D4;
int ledConection2 = D5;
bool ledState = LOW;
bool ledState2 = LOW;

void handleRoot() {
  String html = "<html><body><h1>Control Leds</h1>";
  html += "<div class=\"container\">";
  html += "<button id=\"controller\" class=\"off\" onclick=\"urlMethod(1)\">LED1</button>";
  html += "<button id=\"controller\" class=\"off\" onclick=\"urlMethod(2)\">LED2</button>";
  html += "</div>";
  html += "<script>";
  html += "function urlMethod(led){";
  html += "var boton = document.getElementById('controller');";
  html += "var checked1 = false;";
  html += "var checked2 = false;";
  html += "if (boton.classList.contains('off')){";
  html += "boton.classList.remove('off');";
  html += "boton.classList.add('on');";
  html += "led == 1 ? checked1 = true : checked2 = true;";
  html += "} else {";
  html += "boton.classList.remove('on');";
  html += "boton.classList.add('off');";
  html += "led == 1 ? checked1 = true : checked2 = true;";
  html += "}";
  html += "var xhttp = new XMLHttpRequest();";
  html += "xhttp.open('POST', '/', true);";
  html += "xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');";
  html += "if (led == 1){";
  html += "xhttp.send('command=' + (checked1 ? 'on' + led : 'off' + led));";
  html += "} else {";
  html += "xhttp.send('command=' + (checked2 ? 'on' + led : 'off' + led));";
  html += "}";
  html += "}";
  html += "</script>";
  html += "<style>";
  html += "body {";
  html += "background: #1E1E1E;";
  html += "color: #fff;";
  html += "}";
  html += ".container {";
  html += "display: flex;";
  html += "justify-content: center;";
  html += "}";
  html += "button{";
  html += "font-family: 'Roboto', sans-serif;";
  html += "margin-right: 20px;";
  html += "background: black;";
  html += "cursor: pointer;";
  html += "border: none;";
  html += "padding: 12px 26px;";
  html += "color: #fff;";
  html += "font-size: 16px;";
  html += "font-weight: bold;";
  html += "position: relative;";
  html += "border-radius: 12px;";
  html += "}";
  html += "button.on::before{";
  html += "content: '';";
  html += "position: absolute;";
  html += "top: 0;";
  html += "left: 0;";
  html += "z-index: -1;";
  html += "width: 100%;";
  html += "height: 100%;";
  html += "background: linear-gradient(45deg, red, blue, deeppink, green, red, blue, deeppink, green );";
  html += "background-size: 800%;";
  html += "border-radius: 10px;";
  html += "filter: blur(8px);";
  html += "animation: glowing 20s linear infinite;";
  html += "}";
  html += "button.off::before{";
  html += "content: '';";
  html += "position: absolute;";
  html += "top: 0;";
  html += "left: 0;";
  html += "z-index: -1;";
  html += "width: 100%;";
  html += "height: 100%;";
  html += "background: linear-gradient(45deg, grey, white, black,  grey, white, black );";
  html += "background-size: 800%;";
  html += "border-radius: 10px;";
  html += "filter: blur(8px);";
  html += "animation: glowing 20s linear infinite;";
  html += "}";
  html += "@keyframes glowing{";
  html += "0% {";
  html += "background-position: 0 0;";
  html += "50% {";
  html += "background-position: 400% 0;";
  html += "}";
  html += "100% {";
  html += "background-position: 0 0;";
  html += "}";
  html += "}";
  html += "</style>";
  html += "</body>";
  html += "</html>";
  server.send(200, "text/html", html);
}

void handlePost() {
  if(server.hasArg("command")) {
    String command = server.arg("command");
    Serial.println(command);
    if(command == "on1") {
      ledState = HIGH;
    } else if(command == "off1") {
      ledState = LOW;
    } else if(command == "on2") {
      ledState2 = HIGH;
    } else if(command == "off2") {
      ledState2 = LOW;
    }
    digitalWrite(ledConection1, ledState);
    digitalWrite(ledConection2, ledState2);
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  pinMode(ledConection1, OUTPUT);
  pinMode(ledConection2, OUTPUT);
  digitalWrite(ledConection1, ledState);
  digitalWrite(ledConection2, ledState2);

  Serial.begin(9200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conexión a WiFi establecida");

  server.on("/", HTTP_GET, handleRoot);
  server.on("/", HTTP_POST, handlePost);

  server.begin();
  Serial.println("Iniciando Servidor...");

  Serial.print("Dirección IP del servidor: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  server.handleClient();
}