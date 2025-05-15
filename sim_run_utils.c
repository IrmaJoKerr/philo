/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_run_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:43:30 by bleow             #+#    #+#             */
/*   Updated: 2025/05/13 06:00:45 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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

void	set_fork_order(t_philo *philo, int *first_fork, int *second_fork)
{
	if (philo->id % 2 == 0)
	{
		*first_fork = philo->left_fork;
		*second_fork = philo->right_fork;
	}
	else
	{
		*first_fork = philo->right_fork;
		*second_fork = philo->left_fork;
	}
}

int	chk_ate_or_dead(t_vars *vars)
{
	int		i;
	long	curr_timestamp;

	i = 0;
	curr_timestamp = curr_time();
	pthread_mutex_lock(&vars->hestia);
	vars->is_done = 1;
	pthread_mutex_unlock(&vars->hestia);
	while (i < vars->head_count)
	{
		if (run_atropos(vars->sophoi[i], curr_timestamp, 1))
		{
			return (1);
		}
		if (vars->max_meals != -1)
		{
			pthread_mutex_lock(&vars->hestia);
			if (vars->sophoi[i]->meals_eaten < vars->max_meals)
				vars->is_done = 0;
			pthread_mutex_unlock(&vars->hestia);
		}
		i++;
	}
	return (0);
}

int	run_atropos(t_philo *philo, long curr_timestamp, int check_mode)
{
	t_vars	*vars;
	int		is_dead;

	vars = philo->shared_vars;
	pthread_mutex_lock(&vars->atropos);
	if (check_mode && !vars->is_dead && philo->next_meal_time < curr_timestamp)
	{
		vars->is_dead = 1;
		is_dead = 1;
		pthread_mutex_unlock(&vars->atropos);
		print_status(DIED, philo);
		return (is_dead);
	}
	is_dead = vars->is_dead;
	pthread_mutex_unlock(&vars->atropos);
	return (is_dead);
}

void	*run_argus(void *arg)
{
	t_vars	*vars;
	long	poll_rate_ms;

	vars = (t_vars *)arg;
	better_usleep(5);
	while (1)
	{
		vars->is_done = 1;
		if (chk_ate_or_dead(vars))
			return (NULL);
		if (vars->max_meals != -1 && vars->is_done)
		{
			pthread_mutex_lock(&vars->atropos);
			vars->is_dead = 1;
			pthread_mutex_unlock(&vars->atropos);
			printf("All philosophers have completed their meals\n");
			return (NULL);
		}
		if (vars->head_count <= 5 && vars->time_to_die < 800)
			poll_rate_ms = 1;
		else
			poll_rate_ms = 3;
		better_usleep(poll_rate_ms);
	}
	return (NULL);
}
