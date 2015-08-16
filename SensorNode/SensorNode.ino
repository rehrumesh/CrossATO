#include <SPI85.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpi85Driver.h>

#define MAX_NODE_ID 25

#define CCA_REGISTER 09


struct init_request{
	byte sensornode_id;	// if sensornode_id > MAX_NODE_ID it is a new node
};

struct init_response{
	byte mothermote_id;
	byte sensornode_id;
	int wakeup_delay;
};


static byte sensornode_id;
static init_request request;
bool isInitCompleted;
byte cca_reg_val;

void setup(){
	isInitCompleted = false;
	sensornode_id = random(MAX_NODE_ID) + MAX_NODE_ID;
	Mirf.cePin = PB4;
	Mirf.csnPin = PB3;

	Mirf.spi = &MirfHardwareSpi85;	  
	Mirf.init();
	Mirf.setRADDR((byte *) "cross");
	Mirf.payload = sizeof(request);
	Mirf.channel = 50;
	Mirf.config();
}

void loop(){
	if(!isInitCompleted){
		initReq();
	}else{

	}
}

void initReq(){

}

bool isChannelClear(){
	//set receive mode
	while(Mirf.isSending());

	Mirf.readRegister(CCA_REGISTER, &cca_reg_val, sizeof(cca_reg_val););
	// true if clear
	return (cca_reg_val & 01) == 0;
}

