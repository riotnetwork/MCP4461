#include "Arduino.h"
#include "MCP4461.h"
 #include <Wire.h>

/*
 http://ww1.microchip.com/downloads/en/DeviceDoc/22265a.pdf
ADDR is 0101 110x ( for the MCP 4461, 0101 aaax for ones with more than 1 adress pin)
 

 x is the R/!W bit set by the wire library
 
 COMMAND char :  AAAA CC DD
 AAAA        : Adress in memory to do things
 CC     : What to do at AAAA ( R,W,Inc,Decr )
 00      : Write
 01      : increment (NV only)
 10      : Decrement (NV only)
 11      : Read
 DD  : D9 and D8 ( MSB of the value we are sending)
 
 
 Datachar     : DDDDDDDD ( 0-255 : 00-FF ), this gets used with D8 to give a full range of 00-257 : 0x00-0x100
 8 bit data ( 8 LSB of the 10 bit data, 7 or 8 bit devices will only use the lower 7 or 8 bits )
 memory map
 Data memory is only 8 bits wide
 
 ADDR    |FUNCTION    |CMDs
 00      |VOL WP 0    |R,W,Inc,Decr 
 01      |VOL WP 1    |R,W,Inc,Decr 
 02      |NV WP 0     |R,W,HVInc,HVDecr 
 03      |NV WP 1     |R,W,HVInc,HVDecr 
 04      |VOL TCON    |R,W
 05      |STATUS      |R
 06 - 0E |EEPROM      |R,W
 0F      |EEPROM      |R,W,HV_WPD,HV_RPD
 
 
 to send/recieve something:
 IIC addr + RW : 0101 110x
	
 COMMAND       : AAAA CCDD
 DATA          : DDDD DDDD * only read and write commands have data 
 */
  //<<constructor>> 
  MCP4461::MCP4461(char dev_address) // define the device adress for this instance
  {
  dev_ADDR= dev_address;
  };
  //<<destructor>>
  MCP4461::~MCP4461(){
  Wire.end()
  };

 /*
 set the output of a wiper
 */
uint8_t setWiper(uint8_t wiper, uint16_t setValue)
{
	if (wiper == WIPER0 ||wiper == WIPER1 || wiper == WIPER1||wiper == WIPER1) // sanity check
	{
		return write(wiper, setValue) ;
	}
	else
	{
		return 0;
	}
}
/*
a wrapper for the read, specific to the wipers
*/
uint16_t getWiper(uint8_t wiper, uint16_t setValue)
{
	return read(wiper);
}

 
uint8_t MCP4461::write(uint8_t mem_addr, uint16_t setValue) // mem_addr is 00-0F, setvalue is 0-257
{
//if you set the volatile output register, the same value ispu tto the non-volatile register
	if(setValue <0)
	{
	setValue =0;
	}
	if(setValue >257)
	{
	setValue =257;
	}
	if(mem_addr <0x00 || mem_addr >0x0F )
	{
	return 0;
	}

  byte cmd_byte = 0x00,data_byte = 0x00;   
  cmd_byte = ((mem_addr <<4) &B11110000) | (((setValue&0x01FF)>>8)&B00000011);  //  top 4 is now address   2 command   2 data (D9,D8)
  data_byte = lowByte(setValue);          // is now D7-D0
  Wire.beginTransmission(dev_ADDR);       // transmit to device this has the read byte ( LSB) set as 1
                                          // device address                 0101110x
  Wire.write(cmd_byte);                   // sends command byte             AAAACCDD
  Wire.write(data_byte);                  // sends potentiometer value byte DDDDDDDD  (D7-D0)
  Wire.endTransmission();                 // stop transmitting
  Wire.flush();
  delay(10);							  // give unit time to apply the value to non volatile register
  uint_16_t set_reading = read(mem_addr);
  if (set_reading == setValue)
  {
  return 1; // it has accepted our setting ( EEPROM reflects what we set it to )
  }
  return 0;
}

uint16_t MCP4461::read(uint8_t mem_addr)// mem addr 0x00 - 0x0f ( 0-16 )
{
	if(mem_addr <0x00 || mem_addr >0x0F )
	{
	return 0x0FFF; // return something that is out of the expected bounds to signify an error
	}
   byte cmd_byte =0x0F, highbyte,lowbyte;
   cmd_byte = (mem_addr<<4) | B00001100 ; 
   
  Wire.beginTransmission(dev_ADDR);
  Wire.write(cmd_byte); 
  Wire.endTransmission(); 
  Wire.requestFrom(dev_ADDR, 2); 
  Wire.endTransmission();                 // stop transmitting
  if(Wire.available())
  {
  highbyte =  Wire.read();  //high byte
  lowbyte =   Wire.read();  //low byte
  Wire.flush();
  }
  uint16_t returnValue =0;
  returnValue = (((uint16_t)highbyte<<8)|lowbyte) & 0x01FF;
  return returnValue;
}

/*
Disconnect all the wipers from the resistor ladder
TODO : add a parameter so we can select which wiper to disconnect
look at the structures of TCON 0 and 1 to know what bits to clear
*/
uint8_t MCP4461::openCircuit()
{
	boolean success;
	success = write(TCON0,0x0088);
	success &= write(TCON1,0x0088);
	return success;
//returns 1 if the operation was succesful, 0 otherwise
}
/*
connect the all wipers from the resistor ladder
TODO : add a parameter so we can select which wiper to disconnect
look at the structures of TCON 0 and 1 to know what bits to set
*/
uint8_t MCP4461::enableOutput()// connect all outputs
{
	boolean success;
	success = write(TCON0,0x00FF);
	success &= write(TCON1,0x00FF);
	return success;
//returns 1 if the operation was succesful, 0 otherwise
}
