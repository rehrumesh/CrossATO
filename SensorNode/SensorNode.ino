// Arduino --------------------------------------------
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
//-----------------------------------------------------

// // ATtiny85-----------------------------------------
// #include <SPI85.h>
// #include <Mirf.h>
// #include <nRF24L01.h>
// #include <MirfHardwareSpi85Driver.h>
// //--------------------------------------------------

#define MAX_NODE_ID 25
#define SENSOR_ID 31
#define ASSIGNED_MOTHERMOTE_ID 1
//----------------
//packet_type = 0   =>  mothermote broadcast
//packet_type = 1   => 	new node request
//packet_type = 2   =>  new node reply
//packet_type = 3   =>  data
//----------------

struct packet_struct{
	byte id;	// mothermote id range = 1-20,  sensor node id range = 30+
	byte packet_type;
	int data;
};

struct decodedData{
	int cycleTime;
	int nodes;
};

static packet_struct packet;
static packet_struct broadcast_packet;

void setup(){
	//Arduino------------------------------
	Mirf.spi = &MirfHardwareSpi;
	Serial.begin(9600);
	//

	// //ATtiny85-----------------------------
	// Mirf.cePin = PB4;
	// Mirf.csnPin = PB3;
	// Mirf.spi = &MirfHardwareSpi85;		
	// //-------------------------------------
	
	Mirf.init();	
	Mirf.payload = sizeof(packet_struct);
	Mirf.channel = 90;
	Mirf.config();	

}

void loop(){
	unsinged long rtt_delay = 0;
	Mirf.setRADDR((byte *) "cross");
	while(Mirf.dataReady());
	if(Mirf.dataReady()){
		Mirf.getData((byte *) &broadcast_packet);
		if(broadcast_packet.packet_type == 0 && broadcast_packet.id == ASSIGNED_MOTHERMOTE_ID){

		}
	}
}

// 1: clear
// 0: not clear
boolean isChannelClear(){
	byte carrier_detect_reg_value = (byte) 0;        
        //RX mode
        Mirf.ceHi();
        Mirf.readRegister(CD, &carrier_detect_reg_value, sizeof(carrier_detect_reg_value));

        //Serial.print("carrier_detect_reg_value = ");
        //Serial.println(carrier_detect_reg_value, BIN);
        return carrier_detect_reg_value == 0;
}


int beacondataEncoder(int cycleTime, int numberOfNodes){
	int ans;
	ans = cycleTime<<5;
	return ans + numberOfNodes;	
}

void beacondataDecoder(int data, struct decodedData *temp){
	int a = data % B100000;
	int b = (data - a)>>5;
        temp->nodes = a;
        temp->cycleTime = b;
}