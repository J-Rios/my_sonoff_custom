
#include "nvm.h"
#include "config.h"

#include <Arduino.h>
#include <EEPROM.h>

/*************************************************************************************************/

// Write any number of bytes data
bool nvm_write(const uint8_t addr, uint8_t* data, const uint16_t data_length)
{
    // Check if number of bytes to write fits in NVM region
    if(addr + data_length >= NVM_SIZE_TO_USE - 1)
    {
        Serial.println("Warning: Trying to write data to out region of NVM.");
        Serial.println("Ignoring write.");
        return false;
    }

    // Write the data
    //Serial.println("Writing data to NVM:");
    for(uint8_t i = 0; i < data_length; i++)
    {
        //Serial.printf("[0x%04X]: 0x%02X\n", addr + i, data[i]);
        EEPROM.write(addr + i, data[i]);
    }
    //Serial.println("");

    return nvm_commit();
}

// Read any number of bytes data
bool nvm_read(const uint8_t addr, uint8_t* data, const uint16_t data_length)
{
    // Check if number of bytes to read is inside NVM region
    if(addr + data_length >= NVM_SIZE_TO_USE - 1)
    {
        Serial.println("Warning: Trying to read data from out region of NVM.");
        Serial.println("Ignoring read.");
        return false;
    }

    // Read the data
    //Serial.println("Reading data from NVM:");
    for(uint8_t i = 0; i < data_length; i++)
    {
        data[i] = EEPROM.read(addr + i);
        //Serial.printf("[0x%04X]: 0x%02X\n", addr + i, data[i]);
    }
    //Serial.println("");

    return true;
}

// Commit the written data
bool nvm_commit(void)
{
    if(EEPROM.commit())
    {
        Serial.println("Data successfully stored in NVM.");
        return true;
    }
    else
    {
        Serial.println("Error: Data failed to be stored in NVM.");
        return false;
    }
}

/*************************************************************************************************/
