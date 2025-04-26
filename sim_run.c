/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_run.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:43:37 by bleow             #+#    #+#             */
/*   Updated: 2025/04/26 23:02:08 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*solo_philo_case(t_philo *philo)
{
	t_vars	*vars;

	vars = philo->shared_vars;
	pthread_mutex_lock(&vars->skeuos[philo->left_fork]);
	print_status(FORK, philo);
	usleep(vars->time_to_die * 1000);
	pthread_mutex_unlock(&vars->skeuos[philo->left_fork]);
	return (NULL);
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

void	grab_forks(t_philo *philo)
{
	int		first_fork;
	int		second_fork;
	long	next_meal;

	set_fork_order(philo, &first_fork, &second_fork);
	pthread_mutex_lock(&philo->shared_vars->skeuos[first_fork]);
	print_status(FORK, philo);
	if (run_atropos(philo))
	{
		pthread_mutex_unlock(&philo->shared_vars->skeuos[first_fork]);
		return ;
	}
	pthread_mutex_lock(&philo->shared_vars->atropos);
	next_meal = philo->next_meal_time;
	pthread_mutex_unlock(&philo->shared_vars->atropos);
	if (curr_time() > next_meal - 500)
	{
		pthread_mutex_unlock(&philo->shared_vars->skeuos[first_fork]);
		usleep(200);
		return ;
	}
	pthread_mutex_lock(&philo->shared_vars->skeuos[second_fork]);
	print_status(FORK, philo);
}

void	release_forks(t_philo *philo)
{
	t_vars	*vars;
	int		first_fork;
	int		second_fork;

	vars = philo->shared_vars;
	set_fork_order(philo, &first_fork, &second_fork);
	pthread_mutex_unlock(&vars->skeuos[second_fork]);
	pthread_mutex_unlock(&vars->skeuos[first_fork]);
}

void	eat_start(t_philo *philo)
{
	t_vars	*vars;

	vars = philo->shared_vars;
	if (run_atropos(philo))
	{
		pthread_mutex_unlock(&vars->skeuos[philo->left_fork]);
		pthread_mutex_unlock(&vars->skeuos[philo->right_fork]);
		return ;
	}
	print_status(EAT, philo);
	pthread_mutex_lock(&vars->atropos);
	philo->last_meal_time = curr_time();
	philo->next_meal_time = philo->last_meal_time + vars->time_to_die;
	pthread_mutex_unlock(&vars->atropos);
	pthread_mutex_lock(&vars->hestia);
	philo->meals_eaten++;
	pthread_mutex_unlock(&vars->hestia);
	usleep(vars->time_to_eat * 1000);
	release_forks(philo);
}

void	zzz_start(t_philo *philo)
{
	t_vars	*vars;

	vars = philo->shared_vars;
	pthread_mutex_lock(&vars->atropos);
	if (vars->is_dead)
	{
		pthread_mutex_unlock(&vars->atropos);
		return ;
	}
	pthread_mutex_unlock(&vars->atropos);
	print_status(SLEEP, philo);
	if (vars->time_to_die < vars->time_to_sleep)
		usleep(vars->time_to_die * 1000);
	else
		usleep(vars->time_to_sleep * 1000);
}


