#ifndef HEADER_HANDLE_ENDIAN_H
#define HEADER_HANDLE_ENDIAN_H

#include "../libft/libft.h"

/* Litlle endian 1234 */
#define L_ENDIAN 0

/* Big endian 4321 */
#define B_ENDIAN 1

/**
 *  @brief Simple detect endian function: https://www.embeddedsoft.	net/simple-code-to-find-endianness-in-c
 *	@return (L_ENDIAN, or B_ENDIAN)
*/
FT_INLINE int detect_local_endian()
{
	int i = 1;
	/* just check value of lower byte */
	return ((*((char *)&i) == 1) ? L_ENDIAN : B_ENDIAN);
}

/* From <byteswap.h> */
/* Swap bytes in 16-bit value.  */
# define SWAP_BYTE_16(x)					\
  ((__uint16_t) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)))

/* Swap bytes in 32-bit value.  */
# define SWAP_BYTE_32(x)					\
  ((((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >> 8)	\
   | (((x) & 0x0000ff00u) << 8) | (((x) & 0x000000ffu) << 24))

/* Swap bytes in 64-bit value.  */
# define SWAP_BYTE_64(x)			\
  ((((x) & 0xff00000000000000ull) >> 56)	\
   | (((x) & 0x00ff000000000000ull) >> 40)	\
   | (((x) & 0x0000ff0000000000ull) >> 24)	\
   | (((x) & 0x000000ff00000000ull) >> 8)	\
   | (((x) & 0x00000000ff000000ull) << 8)	\
   | (((x) & 0x0000000000ff0000ull) << 24)	\
   | (((x) & 0x000000000000ff00ull) << 40)	\
   | (((x) & 0x00000000000000ffull) << 56))


/* if size == 2 swap 16 bits, else: if size == 4 swap 32 bit, else swap 64*/
# define REVERSE_ENDIAN(data, size)	size == 2 ? SWAP_BYTE_16(data) : ((size == 4 ? SWAP_BYTE_32(data) : SWAP_BYTE_64(data)))

/* if local endian == little endian, return data, else call reverse endian */
# define CONVERT_LITTLE_ENDIAN(data, size)	(detect_local_endian() == L_ENDIAN ? data : REVERSE_ENDIAN(data, size))

/*	if endian = 0, here endian is local endian - data endian 
	|| size data == 1, don't revese, else call reverse endian 
*/
# define READ_DATA(data, endian)	(endian == 0 || sizeof(data) == 1) ? data : REVERSE_ENDIAN(data, sizeof(data))

#endif /* HEADER_HANDLE_ENDIAN_H */
