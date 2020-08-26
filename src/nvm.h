
#ifndef __NVM_H
#define __NVM_H

/*************************************************************************************************/

#include <inttypes.h>
#include <stdbool.h>

/*************************************************************************************************/

bool nvm_write(const uint8_t addr, uint8_t* data, const uint16_t data_length);
bool nvm_read(const uint8_t addr, uint8_t* data, const uint16_t data_length);
bool nvm_commit(void);

/*************************************************************************************************/

#endif /* __NVM_H */
