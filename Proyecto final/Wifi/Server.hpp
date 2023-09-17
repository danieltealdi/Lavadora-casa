AsyncWebServer server(80);

void handleFormText(AsyncWebServerRequest *request)
{
  String MiText = request->arg("selector");

  Serial.print("MiText:\t");
  Serial.println(MiText);

  //String response = "<a href='/'> Go Back </a>";
  //request->send(200, "text/html", response);

  if (MiText=="Lavar"){
    digitalWrite( 2, LOW );
    digitalWrite( 0, LOW );
    Serial.println("ON");
    
    }
  else{
    digitalWrite( 2, HIGH );
    digitalWrite( 0, HIGH );
    Serial.println("OFF");
    
    }

  request->redirect("/");

}

void InitServer()
{
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.on("/SetText", HTTP_POST, handleFormText);

  server.onNotFound([](AsyncWebServerRequest * request) {
    request->send(400, "text/plain", "Not found");
  });

  server.begin();
  Serial.println("HTTP server started");
}
