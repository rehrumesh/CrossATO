/*
nRF24L01 to Arduino Uno
MISO -> 12
MOSI -> 11
SCK -> 13
CE -> 8
CSN -> 7
GND -> GND
VCC -> 3V3
*/


#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#define MAX_NODE_LIMIT 25
#define MOTHERMOTE_ID 1

static byte sensors[MAX_NODE_LIMIT]; 

struct init_request{
	byte sensornode_id;	// if sensornode_id > MAX_NODE_LIMIT it is a new node
};

struct init_response{
	byte mothermote_id;
	byte sensornode_id;
	int wakeup_delay;
};

void setup(){
	Serial.begin(9600);
	Mirf.spi = &MirfHardwareSpi;
	Mirf.init();
	Mirf.setRADDR((byte *) "cross");
	Mirf.channel = 50;
	//Mirf.payload = sizeof(compressed_packet);              
	Mirf.config();

}

void loop(){

	static init_request request;
	static init_response response;

	while(!Mirf.dataReady());

	if(Mirf.dataReady()){
		Mirf.getData((byte *) &request);
		if(request.sensornode_id > MAX_NODE_LIMIT){
			response.mothermote_id = MOTHERMOTE_ID;
			response.sensornode_id = generateSensorNodeId();
			
		}
	}

}

byte generateSensorNodeId(){
	return 50;
}