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

byte CCA_REG = 9;

//----------------
//packet_type = 1   => 	init_request
//packet_type = 2   =>  data_packet
//----------------

struct packet_struct{
	byte packet_type;
	byte sensornode_id;
	byte old_id;
	byte mothermote_id;
	int wakeup_delay;
	int data;
};

// struct init_request{
// 	byte packet_type;
// 	byte sensornode_id;	// if sensornode_id > MAX_NODE_LIMIT it is a new node
// };

// struct init_response{
// 	byte mothermote_id;
// 	byte sensornode_id;
// 	byte old_id;
// 	int wakeup_delay;
// };

// struct data_packet{
// 	byte packet_type;
// 	byte sensornode_id;
// 	int data;
// };

byte cca_reg_val;
static byte sensors[MAX_NODE_LIMIT]; 

// static init_request request;
// static init_response response;
// static data_packet data_pack;


void setup(){
	Serial.begin(9600);
	Mirf.spi = &MirfHardwareSpi;
	Mirf.init();
	Mirf.setRADDR((byte *) "cross");
	Mirf.channel = 50;
	Mirf.payload = sizeof(packet_struct);             
	Mirf.config();

}

void loop(){
	static packet_struct packet;
	static packet_struct response;
	while(!Mirf.dataReady());

	if(Mirf.dataReady()){
		// use a implementation specific byte to determine diference between control packet and a data packet
		Serial.println("Packet received");
		//byte packet_type = stripPacket();
		Mirf.getData((byte *) &packet);
		Serial.println("Packet type : " + packet.packet_type);
		//Mirf.getData((byte *) &request);
		if(packet.packet_type == 1){
			if(packet.sensornode_id > MAX_NODE_LIMIT){
				response.mothermote_id = MOTHERMOTE_ID;
				response.old_id = packet.sensornode_id;
				response.sensornode_id = generateSensorNodeId();
				if(response.sensornode_id >= 0){
					response.wakeup_delay = sensors[response.sensornode_id];
				}
				
				Mirf.setTADDR((byte *)"cross");
				while(!isChannelClear());
	                        Mirf.send((byte *) &response);
			}
		}else if(packet.packet_type = 2){
			Serial.println("Received from : "+ packet.sensornode_id);
			Serial.println("Data : " + packet.data);
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

	Mirf.readRegister(CCA_REG, &cca_reg_val, sizeof(cca_reg_val));
	// true if clear
	return (cca_reg_val & 01) == 0;
}

// byte stripPacket(){
// 	data_packet tmpPck;
// 	Mirf.payload = sizeof(data_packet);
// 	Mirf.config();
// 	while(!Mirf.dataReady());
// 	Mirf.getData((byte *) &tmpPck);
// 	if(tmpPck.packet_type == 1){
// 		request.packet_type = tmpPck.packet_type;
// 		request.sensornode_id = tmpPck.sensornode_id;
// 		return 1;				
// 	}else if(tmpPck.packet_type == 2){
// 		data_pack.packet_type = 2;
// 		data_pack.sensornode_id = tmpPck.sensornode_id;
// 		data_pack.data = tmpPck.data;
// 		return 2;
// 	}
// 	return -1;
// }