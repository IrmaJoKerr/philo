/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_run.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:43:37 by bleow             #+#    #+#             */
/*   Updated: 2025/04/26 14:48:06 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	take_forks(t_philo *philo) // Renamed from ft_fork
{
	t_vars	*vars;

	vars = philo->shared_vars;
	if (vars->num_philosophers == 1)
	{
		pthread_mutex_lock(&vars->fork_mutexes[philo->left_fork]);
		log_message(FORK, 0, 0); // Updated time calculation
		usleep(vars->time_to_die * 1000);
		pthread_mutex_unlock(&vars->fork_mutexes[philo->left_fork]);
		return ;
	}
	pthread_mutex_lock(&vars->fork_mutexes[philo->left_fork]);
	print_philosopher_message(FORK, philo);
	pthread_mutex_lock(&vars->fork_mutexes[philo->right_fork]);
	print_philosopher_message(FORK, philo);
}

void	start_eating(t_philo *philo) // Renamed from ft_eat
{
	t_vars	*vars;

	vars = philo->shared_vars;
	pthread_mutex_lock(&vars->death_mutex);
	if (vars->is_dead)
	{
		pthread_mutex_unlock(&vars->death_mutex);
		pthread_mutex_unlock(&vars->fork_mutexes[philo->left_fork]);
		pthread_mutex_unlock(&vars->fork_mutexes[philo->right_fork]);
		return ;
	}
	pthread_mutex_unlock(&vars->death_mutex);
	print_philosopher_message(EAT, philo);
	pthread_mutex_lock(&vars->death_mutex);
	philo->last_meal_time = current_time();
	philo->next_meal_time = philo->last_meal_time + vars->time_to_die;
	pthread_mutex_unlock(&vars->death_mutex);
	pthread_mutex_lock(&vars->meal_check_mutex);
	philo->meals_eaten++;
	pthread_mutex_unlock(&vars->meal_check_mutex);
	usleep(vars->time_to_eat * 1000);
	pthread_mutex_unlock(&vars->fork_mutexes[philo->left_fork]);
	pthread_mutex_unlock(&vars->fork_mutexes[philo->right_fork]);
}

void	start_sleeping(t_philo *philo) // Renamed from ft_sleep
{
	t_vars	*vars;

	vars = philo->shared_vars;
	pthread_mutex_lock(&vars->death_mutex);
	if (vars->is_dead)
	{
		pthread_mutex_unlock(&vars->death_mutex);
		return ;
	}
	pthread_mutex_unlock(&vars->death_mutex);
	print_philosopher_message(SLEEP, philo);
	if (vars->time_to_die < vars->time_to_sleep)
		usleep(vars->time_to_die * 1000);
	else
		usleep(vars->time_to_sleep * 1000);
}

void	*dining_routine(void *arg) // Renamed from ft_dining
{
	t_philo	*philo;
	t_vars	*vars;

	philo = (t_philo *)arg;
	vars = philo->shared_vars;
	if (philo->id % 2 == 0)
		usleep(vars->time_to_eat / 2 * 1000);
	while (!ft_mutex_death(philo))
	{
		take_forks(philo);
		start_eating(philo); // Updated reference
		pthread_mutex_lock(&vars->meal_check_mutex);
		if (vars->max_meals != -1 && philo->meals_eaten >= vars->max_meals)
		{
			pthread_mutex_unlock(&vars->meal_check_mutex);
			break ;
		}
		pthread_mutex_unlock(&vars->meal_check_mutex);
		start_sleeping(philo); // Updated reference
		print_philosopher_message(THINK, philo);
	}
	return (NULL);
}
