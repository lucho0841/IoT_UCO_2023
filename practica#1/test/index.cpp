#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const char* ssid = "SSID";
const char* password = "Password";

ESP8266WebServer server(80);

int ledConection = 5;
bool ledState = LOW;

void handleRoot() {
  String html = "<html><body><h1>Control de LED</h1>";
  html += "<div class=\"container\">";
  html += "<button id=\"controller\" class=\"off\" onclick=\"generarCaracteres(true)\">Off</button>";
  html += "</div>";
  html += "<script>";
  html += "function sendRequest(checked) {";
  html += "var xhttp = new XMLHttpRequest();";
  html += "xhttp.open('POST', '/', true);";
  html += "xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');";
  html += "xhttp.send('command=' + (checked ? 'on' : 'off'));";
  html += "}";
  html += "</script>";
  html += "<style>";
  html += "body {";
  html += "background: #1E1E1E;";
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
  html += "<script>";
  html += "function urlMethod(valor){";
  html += "var boton = document.getElementById('controller');";
  html += "if (boton.classList.contains('off')){";
  html += "boton.classList.remove('off');";
  html += "boton.classList.add('on');";
  html += "boton.innerHTML = 'On';";
  html += "} else {";
  html += "boton.classList.remove('on')";
  html += "boton.classList.add('off');";
  html += "boton.innerHTML = 'Off';";
  html += "}";
  html += "var xhttp = new XMLHttpRequest();";
  html += "xhttp.open('POST', '/', true);";
  html += "xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');";
  html += "xhttp.send('command=' + (valor ? 'on' : 'off'));";
  html += "}";
  html += "</script>";
  html += "</html>";
  server.send(200, "text/html", html);
}

void handlePost() {
  if(server.hasArg("command")) {
    String command = server.arg("command");
    if(command == "on") {
      ledState = HIGH;
    } else if(command == "off") {
      ledState = LOW;
    }
    digitalWrite(ledConection, ledState);
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  pinMode(ledConection, OUTPUT);
  digitalWrite(ledConection, ledState);

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