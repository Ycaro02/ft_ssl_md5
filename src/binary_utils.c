#include "../libft/libft.h"

/**
 * @brief Convert a char to a binary string
 * @param c char to convert
 * @return allocated char * c converted to binary
*/
char *char_to_binary(char c)
{
	char	*binary = malloc(8);
	int		i = 7;
	
	if (!binary) {
		return (NULL);
	}
	while (i >= 0) {
		binary[i] = c % 2 + '0';
		c /= 2;
		i--;
	}
	return (binary);
}
