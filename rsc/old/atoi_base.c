#include "../libft/libft.h"

int	char_to_numb(char c, char *base)
{
	int	i = 0;

	while (*base) {
		if (*base == c)
			return (i);
		++i;
		++base;
	}
	return (i);
}

int	char_in_base(char c, char *base)
{
	while (*base && *base != c) { /* base checked in is_valid_base/atoi start, can't be null here */
		++base;
	}
	return (*base != '\0');
}

int is_space(char c)
{
	return ((c >= '\t' && c <= '\r') || c == 32);
}

int	is_valid_base(char *str)
{
	int	i = 0;
	int	j = 0;
	int len = ft_strlen(str); /* strlen protect against NULL return 0 */

	if (len <= 1) {
		return (-1);
	}
	while (str[i]) {
		if (is_space(str[i]) || str[i] == '+' || str[i] == '-')
			return (-1);
		j = i + 1;
		while (str[j]) {
			if (str[i] == str[j])
				return (-1);
			++j;
		}
		++i;
	}
	return (len);
}

/**
 * @brief Convert a string to an integer
 * @param str string to convert
 * @param base base to use
 * @return str converted in integer decimal from base
*/
int	ft_atoi_base(char *str, char *base)
{
	int	i = 0;
	int	tmp = 0;
	int	sign = 1;
	int base_len = is_valid_base(base);

	if (!str || base_len == -1) {
		return (0);
	}
	while (is_space(str[i])) {
		i++;
	}
	while (str[i] == '+' || str[i] == '-') {
		/* ((c == 43) * 1) + ((c == 45) * -1) */
		sign *= ((str[i] == '+') + (str[i] == '-') * -1);
		++i;
	}
	while (char_in_base(str[i], base) == 1) {
		tmp *= base_len;
		tmp += char_to_numb(str[i], base);
		++i;
	}
	return (sign * tmp);
}

