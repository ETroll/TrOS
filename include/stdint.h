//
// Standard typedefs
//

#ifndef INCLUDE_STDINT_H
#define INCLUDE_STDINT_H

typedef signed char         int8_t;
typedef unsigned char       uint8_t;
typedef short               int16_t;
typedef unsigned short      uint16_t;
typedef int                 int32_t;
typedef unsigned            uint32_t;
typedef long long           int64_t;
typedef unsigned long long  uint64_t;


#define INT8_MIN (-128)
#define INT16_MIN (-32768)
#define INT32_MIN (-2147483647 - 1)
#define INT64_MIN (-9223372036854775807LL - 1)

#define INT8_MAX 127
#define INT16_MAX 32767
#define INT32_MAX 2147483647
#define INT64_MAX 9223372036854775807LL

#define UINT8_MAX 0xFF /* 255U */
#define UINT16_MAX 0xFFFF /* 65535U */
#define UINT32_MAX 0xFFFFFFFF  /* 4294967295U */
#define UINT64_MAX 0xFFFFFFFFFFFFFFFFULL /* 18446744073709551615ULL */

#endif
