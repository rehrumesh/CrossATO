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
#define CCA_REGISTER 09

struct init_request{
	byte sensornode_id;	// if sensornode_id > MAX_NODE_LIMIT it is a new node
};

struct init_response{
	byte mothermote_id;
	byte sensornode_id;
	byte old_id;
	int wakeup_delay;
};

byte cca_reg_val;
static byte sensors[MAX_NODE_LIMIT]; 

void setup(){
	Serial.begin(9600);
	Mirf.spi = &MirfHardwareSpi;
	Mirf.init();
	Mirf.setRADDR((byte *) "cross");
	Mirf.channel = 50;
	Mirf.payload = sizeof(init_request);             
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
			response.old_id = request.sensornode_id;
			response.sensornode_id = generateSensorNodeId();
			if(response.sensornode_id >= 0){
				response.wakeup_delay = sensors[response.sensornode_id];
			}
			Mirf.payload = sizeof(response); 
			Mirf.setTADDR((byte *)"cross");
			Mirf.config();
			while(!isChannelClear());
                        Mirf.send((byte *) &response);
		}
	}

}

byte generateSensorNodeId(){
	for(int i=0; i<MAX_NODE_LIMIT; i++){
	    if(sensors[i] == 0){
	    	sensors[i] = (i+1)*100;
	    	return i;
	    }
	}
	return -1;
}

//CCA
bool isChannelClear(){
	//set receive mode
	while(Mirf.isSending());

	Mirf.readRegister(CCA_REGISTER, &cca_reg_val, sizeof(cca_reg_val););
	// true if clear
	return (cca_reg_val & 01) == 0;
}

