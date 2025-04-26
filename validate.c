/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:43:44 by bleow             #+#    #+#             */
/*   Updated: 2025/04/26 22:51:20 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	validate_args(int ac, char **av)
{
	if (ac < 5 || ac > 6)
		return (print_error("wrong number of arg"));
	if (digits_valid(ac, av))
		return (print_error("digit only"));
	if (!av[1] || ft_atoi(av[1]) < 1)
		return (print_error("number of philosopher"));
	if (!av[2] || ft_atoi(av[2]) < 60)
		return (print_error("time to die"));
	if (!av[3] || ft_atoi(av[3]) < 60)
		return (print_error("time to eat"));
	if (!av[4] || ft_atoi(av[4]) < 60)
		return (print_error("time to sleep"));
	if (av[5] && ft_atoi(av[5]) < 1)
		return (print_error("number to eat"));
	return (0);
}

size_t	ft_strlen(const char *str)
{
	const char	*s;

	s = str;
	while (*s)
		s++;
	return (s - str);
}

int	calc(long long res, int sign, const char *str, int i)
{
	while (str[i] >= '0' && str[i] <= '9')
	{
		if (res > (LLONG_MAX - (str[i] - '0')) / 10)
		{
			if (sign == 1)
				return (INT_MAX);
			else
				return (INT_MIN);
		}
		res = res * 10 + (str[i++] - '0');
	}
	return (res);
}

int	ft_atoi(const char *str)
{
	long long	res;
	int			sign;
	int			i;

	res = 0;
	sign = 1;
	i = 0;
	if (!str)
		return (0);
	while ((str[i] >= 9 && str[i] <= 13) || (str[i] == 32))
	{
		i++;
		if (i == (int)ft_strlen(str))
			return (0);
	}
	if (str[i] == '-' || str[i] == '+')
		sign = 1 - 2 * (str[i++] == '-');
	res = calc(res, sign, str, i);
	return ((int)(sign * res));
}
