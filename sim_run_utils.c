/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_run_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:43:30 by bleow             #+#    #+#             */
/*   Updated: 2025/04/26 23:01:40 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	curr_time(void)
{
	struct timeval	now;

	gettimeofday(&now, NULL);
	return ((now.tv_sec * 1000) + (now.tv_usec / 1000));
}

int	digits_valid(int ac, char **av)
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

int	chk_ate_or_dead(t_vars *vars)
{
	int		i;
	long	curr_timestamp;
	int		meals_eaten;

	i = 0;
	curr_timestamp = curr_time();
	while (i < vars->head_count)
	{
		pthread_mutex_lock(&vars->atropos);
		if (vars->sophoi[i]->next_meal_time < curr_timestamp)
		{
			print_status(DIED, vars->sophoi[i]);
			vars->is_dead = 1;
			pthread_mutex_unlock(&vars->atropos);
			return (1);
		}
		pthread_mutex_unlock(&vars->atropos);
		if (vars->max_meals != -1)
		{
			pthread_mutex_lock(&vars->hestia);
			meals_eaten = vars->sophoi[i]->meals_eaten;
			if (meals_eaten < vars->max_meals)
				vars->is_done = 0;
			pthread_mutex_unlock(&vars->hestia);
		}
		i++;
	}
	return (0);
}

void	*run_argus(void *arg)
{
	t_vars	*vars;

	vars = (t_vars *)arg;
	usleep(100000);
	while (1)
	{
		vars->is_done = 1;
		if (chk_ate_or_dead(vars))
			return (NULL);
		if (vars->max_meals != -1 && vars->is_done)
		{
			if (philo_eaten_chk(vars, 0))
			{
				pthread_mutex_lock(&vars->atropos);
				vars->is_dead = 1;
				pthread_mutex_unlock(&vars->atropos);
				printf("All philosophers have finished eating meals\n");
				return (NULL);
			}
		}
		usleep(1000);
	}
	return (NULL);
}

int	philo_eaten_chk(t_vars *vars, int index)
{
	int	has_eaten;

	if (index >= vars->head_count)
		return (0);
	pthread_mutex_lock(&vars->hestia);
	has_eaten = vars->sophoi[index]->meals_eaten > 0;
	pthread_mutex_unlock(&vars->hestia);
	if (has_eaten)
		return (1);
	return (philo_eaten_chk(vars, index + 1));
}

int	run_atropos(t_philo *philo)
{
	t_vars	*vars;
	int		is_dead;

	vars = philo->shared_vars;
	pthread_mutex_lock(&vars->atropos);
	is_dead = vars->is_dead;
	if (is_dead)
	{
		debug_print("Death detected by Philo %d", philo->id + 1);
	}
	pthread_mutex_unlock(&vars->atropos);
	return (is_dead);
}
