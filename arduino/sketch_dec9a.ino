#include <Ethernet.h>
#include <SPI.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 2, 113);       // IP del Arduino
IPAddress server(192, 168, 2, 111);  // IP del servidor
EthernetClient client;

// Configuración para el relé
#define RELAY_PIN 7   // Pin digital para controlar el relé

// Pines del Wiegand
#define D0_PIN 2
#define D1_PIN 3

volatile unsigned long cardCode = 0;  // Variable para almacenar el código de la tarjeta
volatile int bitCount = 0;            // Contador de bits recibidos

void setup() {
  Serial.begin(9600);
    Serial.println("Iniciando Ethernet...");
    Ethernet.begin(mac, ip);
    Serial.println("Conexión Ethernet lista.");
    
    // Configuración del lector de tarjeta
    pinMode(D0_PIN, INPUT);  // Configurar pin D0 como entrada
    pinMode(D1_PIN, INPUT);  // Configurar pin D1 como entrada

    attachInterrupt(digitalPinToInterrupt(D0_PIN), readD0, FALLING);  // Interrupción en D0
    attachInterrupt(digitalPinToInterrupt(D1_PIN), readD1, FALLING);  // Interrupción en D1

    // Configuración del relé
    pinMode(RELAY_PIN, OUTPUT);  // Configurar pin del relé como salida
    digitalWrite(RELAY_PIN, LOW);  // Asegurarse de que el relé esté apagado al principio

    Serial.println("Esperando tarjeta...");
}

void loop() {
    // Comprobar si se ha leído una tarjeta completa
    if (bitCount >= 26) {  // Normalmente, el protocolo Wiegand usa 26 bits o más
        // Mostrar el código en hexadecimal
        Serial.print("Codigo hexadecimal: 0x");
        Serial.println(cardCode, HEX);

        // Convertir a decimal con el cálculo solicitado
        long decimalNumber = (cardCode - 1) / 2 + 1;
        Serial.print("Numero decimal: ");
        Serial.println(decimalNumber);  // Mostrar en decimal

        // Enviar solicitud GET con el número decimal de la tarjeta
        enviarPeticionGET(decimalNumber);

        // Reiniciar los datos para la siguiente tarjeta
        resetWiegand();
    }
}

// Función para leer el bit D0 (0)
void readD0() {
    cardCode = (cardCode << 1);  // Desplazar ID a la izquierda y agregar un 0
    bitCount++;                 // Incrementar contador de bits
}

// Función para leer el bit D1 (1)
void readD1() {
    cardCode = (cardCode << 1) | 1;  // Desplazar ID a la izquierda y agregar un 1
    bitCount++;                      // Incrementar contador de bits
}

// Resetear los datos para una nueva tarjeta
void resetWiegand() {
    cardCode = 0;
    bitCount = 0;
}

// Función para enviar una solicitud GET con el ID de la tarjeta
void enviarPeticionGET(long id) {
    Serial.println("Intentando conectar al servidor...");
    if (client.connect(server, 80)) {
        Serial.println("Conexión establecida con el servidor.");
        
        // Crear y enviar la solicitud GET
        client.print("GET /consulta.php?codigo=");
        client.print(id);  // Incluir el ID de la tarjeta en la solicitud
        client.print(" HTTP/1.1\r\n");
        client.print("Host: 192.168.2.111\r\n");
        client.print("Connection: close\r\n");
        client.print("\r\n");

        // Esperar y leer la respuesta
        unsigned long timeout = millis() + 5000;  // Esperar hasta 5 segundos
        while (!client.available() && millis() < timeout) {
            // Esperar datos
        }

        if (client.available()) {
            Serial.println("Respuesta del servidor: ");
            String respuesta = "";
            while (client.available()) {
                char c = client.read();
                respuesta += c;  // Leer la respuesta completa
            }
            Serial.println(respuesta);  // Imprimir la respuesta completa

            // Validar la respuesta
            if (respuesta.indexOf("Valido") >= 0) {
                Serial.println("La tarjeta es válida.");
                digitalWrite(RELAY_PIN, HIGH);  // Encender el relé
                delay(2000);  // Mantenerlo encendido por 2 segundos
                digitalWrite(RELAY_PIN, LOW);   // Apagar el relé
            } else if (respuesta.indexOf("Invalido") >= 0) {
                Serial.println("La tarjeta es inválida.");
            } else {
                Serial.println("Respuesta desconocida.");
            }
        } else {
            Serial.println("No se recibió respuesta del servidor.");
        }

        client.stop();  // Cerrar la conexión
        Serial.println("\nConexión cerrada.");
    } else {
        Serial.println("Conexión fallida.");
    }
}
