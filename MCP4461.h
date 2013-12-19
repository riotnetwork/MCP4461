 #ifndef MCP4561_H
    #define MCP4561_H
   //  #include <Wire.h>
	 #if ARDUINO >= 100
		#include "Arduino.h"
	#else
		#include "WProgram.h"
		#include "WConstants.h"
	#endif
	
	#define WIPER0_VOLATILE 0x00
	#define WIPER1_VOLATILE 0x01
	#define WIPER2_VOLATILE 0x06
	#define WIPER3_VOLATILE 0x07

	#define WIPER_0 (uint8_t)WIPER0_VOLATILE
	#define WIPER_1 (uint8_t)WIPER2_VOLATILE
	#define WIPER_2 (uint8_t)WIPER3_VOLATILE
	#define WIPER_3 (uint8_t)WIPER4_VOLATILE

	#define ALL_WIPERS 0xFF

	#define TCON0 0x04
	#define TCON1 0x0A

	#define WIPER0_EEPROM 0x02
	#define WIPER1_EEPROM 0x03
	#define WIPER2_EEPROM 0x08
	#define WIPER3_EEPROM 0x09
	
	#define MCP_4461_STATUS 0x05
	
	class MCP4461
	{
	public:
			MCP4461(uint8_t dev_address);
            ~MCP4461();
			// declare all our functions here
			// uint8_t output(uint8_t wiper, uint16_t setValue);
			uint16_t read(uint8_t mem_addr);	 // mem addr 0x00 - 0x0f ( 0-16 ), returns 9 bit value at that adress
			uint_8_t write(uint8_t mem_addr, uint16_t setValue); // mem_addr is 00-0F, setvalue is 0-257 returns 1if succesful
			uint_8_t openCircuit();
			uint_8_t enableOutput();
	private:
	uint8_t dev_ADDR;
	MCP_data pot_datapacket;
	};
	#endif
