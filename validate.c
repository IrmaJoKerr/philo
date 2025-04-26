/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:43:44 by bleow             #+#    #+#             */
/*   Updated: 2025/04/26 16:05:11 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	print_error(const char *msg) // Renamed from ft_ext_msg
{
	fprintf(stderr, "Error: %s\n", msg);
	return (1);
}

void	log_message(int id, long time, int n) // Renamed from ft_msg
{
	if (id == FORK)
		printf("%ld \t%d has taken a fork\n", time, n + 1);
	else if (id == EAT)
		printf("%ld \t%d is eating\n", time, n + 1);
	else if (id == SLEEP)
		printf("%ld \t%d is sleeping\n", time, n + 1);
	else if (id == THINK)
		printf("%ld \t%d is thinking\n", time, n + 1);
	else if (id == DIED)
		printf("%ld \t%d died\n", time, n + 1);
	else if (id == END)
		printf("Simulation completed!\n");
}

void	print_philosopher_message(int id, t_philo *philo) // Renamed from ft_print_msg
{
	long	time;

	if (!philo->shared_vars || philo->shared_vars->is_dead)
		return ;
	pthread_mutex_lock(&philo->shared_vars->message_mutex);
	time = current_time() - philo->shared_vars->start_time; // Updated to use current_time()
	log_message(id, time, philo->id); // Updated reference
	if (id != DIED)
		pthread_mutex_unlock(&philo->shared_vars->message_mutex);
}

size_t	ft_strlen(const char *str)
{
	const char	*s;

	s = str;
	while (*s)
		s++;
	return (s - str);
}

static int	calc(long long res, int sign, const char *str, int i)
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

/*
Cleans up fork mutexes.
Handles partial allocations in failure.
*/
static void	clean_fork_mutexes(t_vars *vars)
{
	int	i;
	int	mutex_count;

	if (!vars->fork_mutexes)
		return ;
	mutex_count = vars->num_philosophers;
	i = 0;
	while (i < mutex_count)
	{
		pthread_mutex_destroy(&vars->fork_mutexes[i]);
		i++;
	}
	free(vars->fork_mutexes);
}

/*
Clean up all resources used by the simulation
*/
int	cleanup_resources(t_vars *vars, t_philo ***philosophers)
{
	int	i;

	clean_fork_mutexes(vars);
	pthread_mutex_destroy(&vars->message_mutex);
	pthread_mutex_destroy(&vars->death_mutex);
	pthread_mutex_destroy(&vars->meal_check_mutex);
	if (philosophers && *philosophers)
	{
		i = 0;
		while (i < vars->num_philosophers)
		{
			if ((*philosophers)[i])
				free((*philosophers)[i]);
			i++;
		}
		free(*philosophers);
	}
	if (vars->threads)
		free(vars->threads);
	return (0);
}
