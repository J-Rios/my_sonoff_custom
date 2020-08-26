
#include "utils.h"

/*************************************************************************************************/

// Count the number of words inside a string
uint32_t cstr_count_words(const char* str_in, const size_t str_in_len)
{
    uint32_t n = 1;

    // Check if string is empty
    if(str_in[0] == '\0')
        return 0;

    // Check for character occurrences
    for(size_t i = 1; i < str_in_len; i++)
    {
        // Check if end of string detected
        if(str_in[i] == '\0')
            break;

        // Check if pattern "X Y", "X\rY" or "X\nY" does not meet
        if((str_in[i] != ' ') && (str_in[i] != '\r') && (str_in[i] != '\n'))
            continue;
        if((str_in[i-1] == ' ') || (str_in[i-1] == '\r') || (str_in[i-1] == '\n'))
            continue;
        if((str_in[i+1] == ' ') || (str_in[i+1] == '\r') || (str_in[i+1] == '\n'))
            continue;
        if(str_in[i+1] == '\0')
            continue;

        // Pattern detected, increase word count
        n = n + 1;
    }

    return n;
}

// Convert string into unsigned 8 bytes value
int8_t cstr_string_to_u8(char* str_in, size_t str_in_len, uint8_t* value_out, uint8_t base)
{
    uint8_t rc = 0;
    uint64_t tmp = (uint64_t)(*value_out);

    rc = cstr_string_to_u64(str_in, str_in_len, &tmp, base);
    *value_out = (uint8_t)(tmp);

    return rc;
}

// Convert string into unsigned 16 bytes value
int8_t cstr_string_to_u16(char* str_in, size_t str_in_len, uint16_t* value_out, uint8_t base)
{
    uint8_t rc = 0;
    uint64_t tmp = (uint64_t)(*value_out);

    rc = cstr_string_to_u64(str_in, str_in_len, &tmp, base);
    *value_out = (uint16_t)(tmp);

    return rc;
}

// Convert string into unsigned 32 bytes value
int8_t cstr_string_to_u32(char* str_in, size_t str_in_len, uint32_t* value_out, uint8_t base)
{
    uint8_t rc = 0;
    uint64_t tmp = (uint64_t)(*value_out);

    rc = cstr_string_to_u64(str_in, str_in_len, &tmp, base);
    *value_out = (uint32_t)(tmp);

    return rc;
}

// Convert string into unsigned 64 bytes value
int8_t cstr_string_to_u64(char* str_in, size_t str_in_len, uint64_t* value_out, uint8_t base)
{
    char* ptr = str_in;
    uint8_t digit;

    *value_out = 0;

    // Check and ignore + symbol
    if(ptr[0] == '+')
    {
        if(str_in_len < 2)
            return RC_UTILS_INVALID_INPUT;
        ptr = ptr + 1;
        str_in_len = str_in_len - 1;
    }

    // Check for hexadecimal "0x" bytes and go through it
    if(base == 16)
    {
        if((ptr[1] == 'x') || (ptr[1] == 'X'))
        {
            if(str_in_len < 3)
                return RC_UTILS_INVALID_INPUT;
            ptr = ptr + 2;
            str_in_len = str_in_len - 2;
        }
    }

    // Convert each byte of the string
    for(uint16_t i = 0; i < str_in_len; i++)
    {
        if(base == 10)
        {
            if(ptr[i] >= '0' && ptr[i] <= '9')
                digit = ptr[i] - '0';
            else
                return RC_UTILS_INVALID_INPUT;
        }
        else if(base == 16)
        {
            if(ptr[i] >= '0' && ptr[i] <= '9')
                digit = ptr[i] - '0';
            else if (base == 16 && ptr[i] >= 'a' && ptr[i] <= 'f')
                digit = ptr[i] - 'a' + 10;
            else if (base == 16 && ptr[i] >= 'A' && ptr[i] <= 'F')
                digit = ptr[i] - 'A' + 10;
            else
                return RC_UTILS_INVALID_INPUT;
        }
        else
            return RC_UTILS_INVALID_INPUT;

        *value_out = ((*value_out)*base) + digit;
    }

    return RC_UTILS_OK;
}

// Convert string into signed 8 bytes value
int8_t cstr_string_to_i8(char* str_in, size_t str_in_len, int8_t* value_out, uint8_t base)
{
    uint8_t rc = 0;
    int64_t tmp = (int64_t)(*value_out);

    rc = cstr_string_to_i64(str_in, str_in_len, &tmp, base);
    *value_out = (int8_t)(tmp);

    return rc;
}

// Convert string into signed 16 bytes value
int8_t cstr_string_to_i16(char* str_in, size_t str_in_len, int16_t* value_out, uint8_t base)
{
    uint8_t rc = 0;
    int64_t tmp = (int64_t)(*value_out);

    rc = cstr_string_to_i64(str_in, str_in_len, &tmp, base);
    *value_out = (int16_t)(tmp);

    return rc;
}

// Convert string into signed 32 bytes value
int8_t cstr_string_to_i32(char* str_in, size_t str_in_len, int32_t* value_out, uint8_t base)
{
    uint8_t rc = 0;
    int64_t tmp = (int64_t)(*value_out);

    rc = cstr_string_to_i64(str_in, str_in_len, &tmp, base);
    *value_out = (int32_t)(tmp);

    return rc;
}

// Convert string into signed 64 bytes value
int8_t cstr_string_to_i64(char* str_in, size_t str_in_len, int64_t* value_out, uint8_t base)
{
    char* ptr = str_in;
    uint8_t digit;
    uint8_t negative_number = 0;

    *value_out = 0;

    // Check for + and - symbols
    if((ptr[0] == '+') || (ptr[0] == '-'))
    {
        if(str_in_len < 2)
            return RC_UTILS_INVALID_INPUT;

        if(ptr[0] == '-')
            negative_number = 1;

        ptr = ptr + 1;
        str_in_len = str_in_len - 1;
    }

    // Check for hexadecimal "0x" bytes and go through it
    if(base == 16)
    {
        if((ptr[1] == 'x') || (ptr[1] == 'X'))
        {
            if(str_in_len < 3)
                return RC_UTILS_INVALID_INPUT;
            ptr = ptr + 2;
            str_in_len = str_in_len - 2;
        }
    }

    // Convert each byte of the string
    for(uint16_t i = 0; i < str_in_len; i++)
    {
        if(base == 10)
        {
            if(ptr[i] >= '0' && ptr[i] <= '9')
                digit = ptr[i] - '0';
            else
                return RC_UTILS_INVALID_INPUT;
        }
        else if(base == 16)
        {
            if(ptr[i] >= '0' && ptr[i] <= '9')
                digit = ptr[i] - '0';
            else if (base == 16 && ptr[i] >= 'a' && ptr[i] <= 'f')
                digit = ptr[i] - 'a' + 10;
            else if (base == 16 && ptr[i] >= 'A' && ptr[i] <= 'F')
                digit = ptr[i] - 'A' + 10;
            else
                return RC_UTILS_INVALID_INPUT;
        }
        else
            return RC_UTILS_INVALID_INPUT;

        *value_out = ((*value_out)*base) + digit;
    }

    // Turn into negative if needs
    if(negative_number)
        *value_out = (*value_out)*(-1);

    return RC_UTILS_OK;
}

/*************************************************************************************************/
