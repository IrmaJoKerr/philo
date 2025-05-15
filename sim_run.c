/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_run.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:43:37 by bleow             #+#    #+#             */
/*   Updated: 2025/05/15 18:06:04 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*lonely_sophoi(t_philo *philo)
{
	t_vars	*vars;

	vars = philo->shared_vars;
	pthread_mutex_lock(&vars->skeuos[philo->left_fork]);
	print_status(FORK, philo);
	usleep(vars->time_to_die * 1000);
	pthread_mutex_unlock(&vars->skeuos[philo->left_fork]);
	return (NULL);
}

int	grab_forks(t_philo *philo)
{
	int		first_fork;
	int		second_fork;
	t_vars	*vars;

	vars = philo->shared_vars;
	set_fork_order(philo, &first_fork, &second_fork);
	pthread_mutex_lock(&vars->skeuos[first_fork]);
	print_status(FORK, philo);
	if (run_atropos(philo, 0, 0))
	{
		pthread_mutex_unlock(&vars->skeuos[first_fork]);
		return (0);
	}
	pthread_mutex_lock(&vars->skeuos[second_fork]);
	print_status(FORK, philo);
	return (1);
}

void	release_forks(t_philo *philo)
{
	t_vars	*vars;

	vars = philo->shared_vars;
	if (philo->left_fork < philo->right_fork)
	{
		pthread_mutex_unlock(&vars->skeuos[philo->left_fork]);
		pthread_mutex_unlock(&vars->skeuos[philo->right_fork]);
	}
	else
	{
		pthread_mutex_unlock(&vars->skeuos[philo->right_fork]);
		pthread_mutex_unlock(&vars->skeuos[philo->left_fork]);
	}
}

void	eat_start(t_philo *philo)
{
	t_vars	*vars;
	long	current_time;

	vars = philo->shared_vars;
	if (run_atropos(philo, 0, 0))
	{
		release_forks(philo);
		return ;
	}
	pthread_mutex_lock(&vars->atropos);
	current_time = curr_time();
	philo->last_meal_time = current_time;
	philo->next_meal_time = current_time + vars->time_to_die;
	pthread_mutex_unlock(&vars->atropos);
	print_status(EAT, philo);
	pthread_mutex_lock(&vars->hestia);
	philo->meals_eaten++;
	pthread_mutex_unlock(&vars->hestia);
	better_usleep(vars->time_to_eat);
	release_forks(philo);
	better_usleep(5);
}

void	zzz_start(t_philo *philo)
{
	t_vars	*vars;
	int		sleep_time;

	vars = philo->shared_vars;
	pthread_mutex_lock(&vars->atropos);
	if (vars->is_dead)
	{
		pthread_mutex_unlock(&vars->atropos);
		return ;
	}
	pthread_mutex_unlock(&vars->atropos);
	print_status(SLEEP, philo);
	sleep_time = vars->time_to_sleep;
	if (vars->time_to_die < vars->time_to_sleep)
	{
		sleep_time = vars->time_to_die;
	}
	better_usleep(sleep_time);
	run_atropos(philo, 0, 0);
}
