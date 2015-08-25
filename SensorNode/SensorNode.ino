#include <SPI85.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpi85Driver.h>

#define MAX_NODE_ID 25

byte CCA_REG = 9;


struct init_request{
	byte sensornode_id;	// if sensornode_id > MAX_NODE_ID it is a new node
};

struct init_response{
	byte mothermote_id;
	byte sensornode_id;
	byte old_id;
	int wakeup_delay;
};

struct data_packet{
	byte sensornode_id;
	int data;
};


static byte sensornode_id;
static init_request init_req;
static init_response init_res;
static int wakeup_delay;

bool isInitCompleted;
byte cca_reg_val;

void setup(){
	isInitCompleted = false;
	sensornode_id = random(MAX_NODE_ID) + MAX_NODE_ID;
	wakeup_delay = 100;	//default sleep time


	Mirf.cePin = PB4;
	Mirf.csnPin = PB3;

	Mirf.spi = &MirfHardwareSpi85;	  
	Mirf.init();
	Mirf.setTADDR((byte *) "cross");
	Mirf.payload = sizeof(init_req);
	Mirf.channel = 50;
	Mirf.config();
}

void loop(){
	if(!isInitCompleted){
		initReq();
	}else{
		Mirf.setTADDR((byte *) "cross");
		Mirf.payload = sizeof(data_packet);

		data_packet dPacket;
		dPacket.sensornode_id = sensornode_id;
		dPacket.data = 3456;

		Mirf.send((byte *) &dPacket);
		while(Mirf.isSending());
	}

	delay(wakeup_delay);
}

void initReq(){
	init_req.sensornode_id = sensornode_id;
	Mirf.send((byte *) &init_req);
	while(Mirf.isSending()){}

	Mirf.setRADDR((byte *) "cross");
	Mirf.payload = sizeof(init_response);
	
	do{
	    while(!Mirf.dataReady());
	    Mirf.getData((byte *) &init_res);
	} while (init_res.old_id != sensornode_id);

	sensornode_id = init_res.sensornode_id;
	wakeup_delay = init_res.wakeup_delay;
	isInitCompleted = true;
}

bool isChannelClear(){
	//set receive mode
	while(Mirf.isSending());

	Mirf.readRegister(CCA_REG, &cca_reg_val, sizeof(cca_reg_val));
	// true if clear
	return (cca_reg_val & 01) == 0;
}

