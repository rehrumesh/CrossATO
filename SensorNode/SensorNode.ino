#include <SPI85.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpi85Driver.h>

#define MAX_NODE_ID 25

struct init_request{
	byte sensornode_id;	// if sensornode_id > MAX_NODE_ID it is a new node
};

struct init_response{
	byte mothermote_id;
	byte sensornode_id;
	int wakeup_delay;
};


static byte sensornode_id;


void setup(){
	sensornode_id = random(MAX_NODE_ID) + MAX_NODE_ID;
	Mirf.cePin = PB4;
	Mirf.csnPin = PB3;

	Mirf.spi = &MirfHardwareSpi85;	  
	Mirf.init();
	Mirf.setRADDR((byte *) "cross");
	Mirf.channel = 50;
	Mirf.config();
}

void loop(){

}


