/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_run.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:43:37 by bleow             #+#    #+#             */
/*   Updated: 2025/05/13 01:14:09 by bleow            ###   ########.fr       */
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

int	grab_forks(t_philo *philo)
{
	int		first_fork;
	int		second_fork;
	t_vars	*vars;

	vars = philo->shared_vars;
	// Use the set_fork_order function to determine order
	set_fork_order(philo, &first_fork, &second_fork);
	debug_print("Philo %d attempting to grab forks. First: %d, Second: %d",
		philo->id + 1, first_fork, second_fork);
	// Try to get first fork
	pthread_mutex_lock(&vars->skeuos[first_fork]);
	debug_print("Philo %d acquired fork %d", philo->id + 1, first_fork);
	print_status(FORK, philo);
	// Check if sim ended before grabbing second fork
	// if (run_atropos(philo))
	if (run_atropos(philo, 0, 0))
	{
		pthread_mutex_unlock(&vars->skeuos[first_fork]);
		return (0);
	}
	// Try to get second fork immediately - no delay
	pthread_mutex_lock(&vars->skeuos[second_fork]);
	debug_print("Philo %d acquired fork %d", philo->id + 1, second_fork);
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

// void	eat_start(t_philo *philo)
// {
// 	t_vars	*vars;

// 	vars = philo->shared_vars;
// 	if (run_atropos(philo))
// 	{
// 		release_forks(philo);
// 		return ;
// 	}
// 	print_status(EAT, philo);
// 	pthread_mutex_lock(&vars->atropos);
// 	philo->last_meal_time = curr_time();
// 	// philo->next_meal_time = philo->last_meal_time + vars->time_to_die;
// 	philo->next_meal_time = philo->last_meal_time + vars->time_to_eat + vars->time_to_die;
// 	pthread_mutex_unlock(&vars->atropos);
// 	pthread_mutex_lock(&vars->hestia);
// 	philo->meals_eaten++;
// 	pthread_mutex_unlock(&vars->hestia);
// 	// Add 200μs safety margin for system overhead
// 	usleep((vars->time_to_eat * 1000) + 100);
// 	release_forks(philo);
// }
void	eat_start(t_philo *philo)
{
	t_vars	*vars;
	long	current_time;

	vars = philo->shared_vars;
	// Check for death before eating
	if (run_atropos(philo, 0, 0))
	{
		release_forks(philo);
		return ;
	}
	// Update meal times - critical section BEFORE printing
	pthread_mutex_lock(&vars->atropos);
	current_time = curr_time();
	philo->last_meal_time = current_time;
	// Correct calculation - philosophers have exactly time_to_die ms from meal start
	philo->next_meal_time = current_time + vars->time_to_die;
	pthread_mutex_unlock(&vars->atropos);
	print_status(EAT, philo);
	// Update meal count - critical section
	pthread_mutex_lock(&vars->hestia);
	philo->meals_eaten++;
	pthread_mutex_unlock(&vars->hestia);
	// Eat for the specified time using precise sleep
	precise_usleep(vars->time_to_eat);
	// Return forks
	release_forks(philo);
	// Small delay after releasing forks
	// This gives other philosophers a chance to grab them
	precise_usleep(5);// 5ms delay
}

void	zzz_start(t_philo *philo)
{
	t_vars	*vars;
	int		sleep_duration;

	vars = philo->shared_vars;
	pthread_mutex_lock(&vars->atropos);
	if (vars->is_dead)
	{
		pthread_mutex_unlock(&vars->atropos);
		return ;
	}
	pthread_mutex_unlock(&vars->atropos);
	print_status(SLEEP, philo);
	// Determine sleep duration
	sleep_duration = vars->time_to_sleep;
	if (vars->time_to_die < vars->time_to_sleep)
		sleep_duration = vars->time_to_die;
	// Use precise_usleep for exact timing
	precise_usleep(sleep_duration);
	// Check if simulation ended during sleep
	run_atropos(philo, 0, 0);
}
