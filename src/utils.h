
#ifndef __UTILS_H
#define __UTILS_H

#ifdef __cplusplus
    extern "C" {
#endif

/*************************************************************************************************/

#include <inttypes.h>
#include <stddef.h>

/*************************************************************************************************/

// Functions Return Codes
#define RC_UTILS_OK 0
#define RC_UTILS_BAD -1
#define RC_UTILS_INVALID_INPUT -2

/*************************************************************************************************/

uint32_t cstr_count_words(const char* str_in, const size_t str_in_len);
int8_t cstr_string_to_u8(char* str_in, size_t str_in_len, uint8_t* value_out, uint8_t base);
int8_t cstr_string_to_u16(char* str_in, size_t str_in_len, uint16_t* value_out, uint8_t base);
int8_t cstr_string_to_u32(char* str_in, size_t str_in_len, uint32_t* value_out, uint8_t base);
int8_t cstr_string_to_u64(char* str_in, size_t str_in_len, uint64_t* value_out, uint8_t base);
int8_t cstr_string_to_i8(char* str_in, size_t str_in_len, int8_t* value_out, uint8_t base);
int8_t cstr_string_to_i16(char* str_in, size_t str_in_len, int16_t* value_out, uint8_t base);
int8_t cstr_string_to_i32(char* str_in, size_t str_in_len, int32_t* value_out, uint8_t base);
int8_t cstr_string_to_i64(char* str_in, size_t str_in_len, int64_t* value_out, uint8_t base);

/*************************************************************************************************/

#ifdef __cplusplus
    }
#endif

#endif /* __UTILS_H */
