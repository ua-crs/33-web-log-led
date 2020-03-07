/*
 *  Project 33-web-log-led - main.cpp
 *      Pagina de web para comandar el led luego del login
 */

#include <Arduino.h>

#ifdef ESP32

#include <WiFi.h>
#include <WebServer.h>

#define ServerObject    WebServer

#else

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define ServerObject    ESP8266WebServer
#endif

#include "wifi_ruts.h"

ServerObject server(80);    // Create a webserver object that listens for HTTP request on port 80 

/*
 *  Constants defined in platformio.ini
 *      VALID_USER
 *      VALID_PASS
 *      LED 
 *      MY_PASS
 *      MY_SSID
 *      SERIAL_BAUD
 */

static void
handleRoot(void)
{
    Serial.println(__FUNCTION__);
    // When URI / is requested, send a web page with a button to toggle the LED
    server.send(200, "text/html",
                "<form action=\"/login\" method=\"POST\"><input type=\"text\" name=\"username\" "
                "placeholder=\"Username\"></br><input type=\"password\" name=\"password\" placeholder=\"Password\"></br><input "
                "type=\"submit\" value=\"Login\"></form>");
}

static void
handleLogin(void)
{
    // If a POST request is made to URI /login
    Serial.println(__FUNCTION__);

    // POST request has username and password data ?
    if (!server.hasArg("username") || !server.hasArg("password")
        || server.arg("username") == NULL || server.arg("password") == NULL)
        server.send(400, "text/plain", "400: Invalid Request");
    else if (server.arg("username") == VALID_USER && server.arg("password") == VALID_PASS)
        server.send(200, "text/html", "<form action=\"/LED\" method=\"POST\"><input type=\"submit\" value=\"Toggle LED\"></form>");
    else
        server.send(401, "text/plain", "401: Unauthorized");
}

static void
handleLED(void)
{
    // If a POST request is made to URI /LED, then toggle LED state
    Serial.println(__FUNCTION__);
    Serial.println(!digitalRead(LED));
    delay(300);

    server.send(200, "text/html", "<form action=\"/LED\" method=\"POST\"><input type=\"submit\" value=\"Toggle LED\"></form>");
    delay(300);

    digitalWrite(LED, !digitalRead(LED)); // Change the state of the LED

    Serial.println(!digitalRead(LED));
    delay(300);

    server.sendHeader("Location",
                      "/LED"); // Add a header to respond with a new location for the browser to go to the home page again
}

static void
handleNotFound(void)
{
//  Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
    server.send(404, "text/plain", "404: Not found");
}

void
setup(void)
{
    Serial.begin(SERIAL_BAUD);
    delay(10);
    Serial.println('\n');

    pinMode(LED, OUTPUT);

    connect_wifi(MY_SSID, MY_PASS);

    server.on("/", HTTP_GET, handleRoot);
    server.on("/login", HTTP_POST, handleLogin);
    server.on("/LED", HTTP_POST, handleLED);
    server.onNotFound(handleNotFound); // When a client requests an unknown URI

    server.begin(); // Start the server
    Serial.println("HTTP server started");
}

void
loop(void)
{
    server.handleClient(); // Listen for HTTP requests from clients
}
