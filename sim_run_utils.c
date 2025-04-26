/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_run_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:43:30 by bleow             #+#    #+#             */
/*   Updated: 2025/04/26 14:49:00 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"


long	current_time(void)
{
	struct timeval	now;

	gettimeofday(&now, NULL);
	return ((now.tv_sec * 1000) + (now.tv_usec / 1000));
}

int	ft_check_num(int ac, char **av)
{
	int	i;
	int	j;

	i = 1;
	while (i < ac)
	{
		j = 0;
		while (av[i][j] != '\0')
		{
			if (av[i][j] < '0' || av[i][j] > '9')
				return (1);
			j++;
		}
		i++;
	}
	return (0);
}

int	ft_check_done(t_vars *vars)
{
	int	i;

	i = 0;
	while (i < vars->num_philosophers)
	{
		pthread_mutex_lock(&vars->death_mutex);
		if (vars->philosophers[i]->next_meal_time < current_time())
		{
			print_philosopher_message(DIED, vars->philosophers[i]);
			vars->is_dead = 1;
			pthread_mutex_unlock(&vars->death_mutex);
			return (1);
		}
		pthread_mutex_unlock(&vars->death_mutex);
		if (vars->max_meals != -1)
		{
			pthread_mutex_lock(&vars->meal_check_mutex);
			if (vars->philosophers[i]->meals_eaten < vars->max_meals)
				vars->is_done = 0;
			pthread_mutex_unlock(&vars->meal_check_mutex);
		}
		i++;
	}
	return (0);
}

void	*ft_monitor(void *arg)
{
	t_vars	*vars;

	vars = (t_vars *)arg;
	while (1)
	{
		vars->is_done = 1;
		if (ft_check_done(vars))
			return (NULL);
		if (vars->max_meals != -1 && vars->is_done)
		{
			pthread_mutex_lock(&vars->death_mutex);
			vars->is_dead = 1;
			pthread_mutex_unlock(&vars->death_mutex);
			printf("All philosophers have completed their meals\n");
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}

int	ft_mutex_death(t_philo *philo)
{
	t_vars	*vars;

	vars = philo->shared_vars;
	pthread_mutex_lock(&vars->death_mutex);
	if (vars->is_dead)
	{
		pthread_mutex_unlock(&vars->death_mutex);
		return (1);
	}
	pthread_mutex_unlock(&vars->death_mutex);
	return (0);
}
