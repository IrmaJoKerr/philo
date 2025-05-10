/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_run.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:43:37 by bleow             #+#    #+#             */
/*   Updated: 2025/05/10 17:38:14 by bleow            ###   ########.fr       */
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

// int	grab_forks(t_philo *philo)
// {
// 	int		first_fork;
// 	int		second_fork;
// 	t_vars	*vars;

// 	vars = philo->shared_vars;
// 	if (philo->left_fork < philo->right_fork)
// 	{
// 		first_fork = philo->left_fork;
// 		second_fork = philo->right_fork;
// 	}
// 	else
// 	{
// 		first_fork = philo->right_fork;
// 		second_fork = philo->left_fork;
// 	}
// 	pthread_mutex_lock(&vars->skeuos[first_fork]);
// 	print_status(FORK, philo);
// 	usleep(100); // Add small yield to reduce contention
// 	pthread_mutex_lock(&vars->skeuos[second_fork]);
// 	print_status(FORK, philo);
// 	return (1);
// }
int	grab_forks(t_philo *philo)
{
	int		first_fork;
	int		second_fork;
	t_vars	*vars;

	vars = philo->shared_vars;
	if (philo->left_fork < philo->right_fork)
	{
		first_fork = philo->left_fork;
		second_fork = philo->right_fork;
	}
	else
	{
		first_fork = philo->right_fork;
		second_fork = philo->left_fork;
	}
	debug_print("Philo %d attempting to grab forks. First: %d, Second: %d",
		philo->id + 1, first_fork, second_fork);
	pthread_mutex_lock(&vars->skeuos[first_fork]);
	debug_print("Philo %d acquired fork %d", philo->id + 1, first_fork);
	print_status(FORK, philo);
	usleep(100); // Add small yield to reduce contention
	pthread_mutex_lock(&vars->skeuos[second_fork]);
	debug_print("Philo %d acquired fork %d", philo->id + 1, second_fork);
	print_status(FORK, philo);
	return (1);
}

// void	release_forks(t_philo *philo)
// {
// 	t_vars	*vars;

// 	vars = philo->shared_vars;
// 	if (philo->left_fork < philo->right_fork)
// 	{
// 		pthread_mutex_unlock(&vars->skeuos[philo->left_fork]);
// 		pthread_mutex_unlock(&vars->skeuos[philo->right_fork]);
// 	}
// 	else
// 	{
// 		pthread_mutex_unlock(&vars->skeuos[philo->right_fork]);
// 		pthread_mutex_unlock(&vars->skeuos[philo->left_fork]);
// 	}
// 	if (DEBUG)
// 	{
// 		pthread_mutex_lock(&vars->hermes);
// 		fprintf(stderr, "Philosopher %d released both forks\n", philo->id + 1);
// 		pthread_mutex_unlock(&vars->hermes);
// 	}
// }
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
	debug_print("Philo %d released forks %d and %d",
		philo->id + 1, philo->left_fork, philo->right_fork);
	if (DEBUG)
	{
		pthread_mutex_lock(&vars->hermes);
		fprintf(stderr, "Philosopher %d released both forks\n", philo->id + 1);
		pthread_mutex_unlock(&vars->hermes);
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
// 	philo->next_meal_time = philo->last_meal_time + vars->time_to_die;
// 	pthread_mutex_unlock(&vars->atropos);
// 	pthread_mutex_lock(&vars->hestia);
// 	philo->meals_eaten++;
// 	pthread_mutex_unlock(&vars->hestia);
// 	usleep(vars->time_to_eat * 1000);
// 	release_forks(philo);
// }
void	eat_start(t_philo *philo)
{
	t_vars	*vars;

	vars = philo->shared_vars;
	if (run_atropos(philo))
	{
		release_forks(philo);
		return ;
	}
	print_status(EAT, philo);
	pthread_mutex_lock(&vars->atropos);
	philo->last_meal_time = curr_time();
	// philo->next_meal_time = philo->last_meal_time + vars->time_to_die;
	philo->next_meal_time = philo->last_meal_time + vars->time_to_eat + vars->time_to_die;
	pthread_mutex_unlock(&vars->atropos);
	pthread_mutex_lock(&vars->hestia);
	philo->meals_eaten++;
	pthread_mutex_unlock(&vars->hestia);
	// Add 200μs safety margin for system overhead
	usleep((vars->time_to_eat * 1000) + 100);
	release_forks(philo);
}

// void	zzz_start(t_philo *philo)
// {
// 	t_vars	*vars;

// 	vars = philo->shared_vars;
// 	pthread_mutex_lock(&vars->atropos);
// 	if (vars->is_dead)
// 	{
// 		pthread_mutex_unlock(&vars->atropos);
// 		return ;
// 	}
// 	pthread_mutex_unlock(&vars->atropos);
// 	print_status(SLEEP, philo);
// 	if (vars->time_to_die < vars->time_to_sleep)
// 		usleep(vars->time_to_die * 1000);
// 	else
// 		usleep(vars->time_to_sleep * 1000);
// }
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
	// Add safety margin for sleep
	if (vars->time_to_die < vars->time_to_sleep)
		usleep(vars->time_to_die * 1000);
	else
		usleep((vars->time_to_sleep * 1000) + 100); // Added safety margin
}
