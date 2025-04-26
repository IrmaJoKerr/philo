/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_run.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:43:37 by bleow             #+#    #+#             */
/*   Updated: 2025/04/26 16:34:31 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

// void	take_forks(t_philo *philo) // Renamed from ft_fork
// {
// 	t_vars	*vars;

// 	vars = philo->shared_vars;
// 	if (vars->num_philosophers == 1)
// 	{
// 		pthread_mutex_lock(&vars->fork_mutexes[philo->left_fork]);
// 		log_message(FORK, 0, 0); // Updated time calculation
// 		usleep(vars->time_to_die * 1000);
// 		pthread_mutex_unlock(&vars->fork_mutexes[philo->left_fork]);
// 		return ;
// 	}
// 	pthread_mutex_lock(&vars->fork_mutexes[philo->left_fork]);
// 	print_philosopher_message(FORK, philo);
// 	pthread_mutex_lock(&vars->fork_mutexes[philo->right_fork]);
// 	print_philosopher_message(FORK, philo);
// }
void take_forks(t_philo *philo)
{
    t_vars *vars;
    int first_fork, second_fork;

    vars = philo->shared_vars;
    debug_print("Philo %d attempting to take forks at time %ld", 
        philo->id + 1, current_time() - vars->start_time);
    if (vars->num_philosophers == 1)
    {
        // Single philosopher case unchanged
        pthread_mutex_lock(&vars->fork_mutexes[philo->left_fork]);
        print_philosopher_message(FORK, philo);  // Use proper timestamp and ID
        usleep(vars->time_to_die * 1000);
        pthread_mutex_unlock(&vars->fork_mutexes[philo->left_fork]);
        return;
    }

    // Determine fork acquisition order based on philosopher ID
    if (philo->id % 2 == 0)
    {
        first_fork = philo->left_fork;
        second_fork = philo->right_fork;
    }
    else
    {
        first_fork = philo->right_fork;
        second_fork = philo->left_fork;
    }

    // Lock first fork
    pthread_mutex_lock(&vars->fork_mutexes[first_fork]);
    print_philosopher_message(FORK, philo);
    debug_print("Philo %d acquired first fork %d at time %ld", 
        philo->id + 1, first_fork, current_time() - vars->start_time);

    // Check if simulation ended after first fork acquisition
    if (ft_mutex_death(philo))
    {
        pthread_mutex_unlock(&vars->fork_mutexes[first_fork]);
        return;
    }
    
    // NEW: Check if death is imminent before taking second fork
    pthread_mutex_lock(&vars->death_mutex);
    long next_meal = philo->next_meal_time;
    pthread_mutex_unlock(&vars->death_mutex);
    
    if (current_time() > next_meal - 200) // 200ms safety margin
    {
        debug_print("Philo %d releasing first fork - death too close (margin: %ld ms)", 
            philo->id + 1, next_meal - current_time());
        pthread_mutex_unlock(&vars->fork_mutexes[first_fork]);
        usleep(1000); // Small delay before retrying
        return; // Return to main loop to try again
    }

    // Try to take second fork
    pthread_mutex_lock(&vars->fork_mutexes[second_fork]);
    print_philosopher_message(FORK, philo);
    debug_print("Philo %d acquired both forks (%d, %d) at time %ld", 
            philo->id + 1, first_fork, second_fork, current_time() - vars->start_time);
}

// void	start_eating(t_philo *philo) // Renamed from ft_eat
// {
// 	t_vars	*vars;

// 	vars = philo->shared_vars;
// 	pthread_mutex_lock(&vars->death_mutex);
// 	if (vars->is_dead)
// 	{
// 		pthread_mutex_unlock(&vars->death_mutex);
// 		pthread_mutex_unlock(&vars->fork_mutexes[philo->left_fork]);
// 		pthread_mutex_unlock(&vars->fork_mutexes[philo->right_fork]);
// 		return ;
// 	}
// 	pthread_mutex_unlock(&vars->death_mutex);
// 	print_philosopher_message(EAT, philo);
// 	pthread_mutex_lock(&vars->death_mutex);
// 	philo->last_meal_time = current_time();
// 	philo->next_meal_time = philo->last_meal_time + vars->time_to_die;
// 	pthread_mutex_unlock(&vars->death_mutex);
// 	pthread_mutex_lock(&vars->meal_check_mutex);
// 	philo->meals_eaten++;
// 	pthread_mutex_unlock(&vars->meal_check_mutex);
// 	usleep(vars->time_to_eat * 1000);
// 	pthread_mutex_unlock(&vars->fork_mutexes[philo->left_fork]);
// 	pthread_mutex_unlock(&vars->fork_mutexes[philo->right_fork]);
// }
void	start_eating(t_philo *philo)
{
	t_vars	*vars;

	vars = philo->shared_vars;
	debug_print("Philo %d starting to eat at time %ld", 
		philo->id + 1, current_time() - vars->start_time);
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
	debug_print("Philo %d updated meal time: last=%ld, next=%ld (now=%ld)", 
                philo->id + 1, philo->last_meal_time - vars->start_time, 
                philo->next_meal_time - vars->start_time, 
                current_time() - vars->start_time);
	pthread_mutex_unlock(&vars->death_mutex);
	pthread_mutex_lock(&vars->meal_check_mutex);
	philo->meals_eaten++;
	pthread_mutex_unlock(&vars->meal_check_mutex);
	usleep(vars->time_to_eat * 1000);
	if (philo->id % 2 == 0)
	{
		debug_print("Philo %d releasing forks after eating at time %ld", 
			philo->id + 1, current_time() - vars->start_time);
		pthread_mutex_unlock(&vars->fork_mutexes[philo->right_fork]);
		pthread_mutex_unlock(&vars->fork_mutexes[philo->left_fork]);
	}
	else
	{
		debug_print("Philo %d releasing forks after eating at time %ld", 
			philo->id + 1, current_time() - vars->start_time);
		pthread_mutex_unlock(&vars->fork_mutexes[philo->left_fork]);
		pthread_mutex_unlock(&vars->fork_mutexes[philo->right_fork]);
	}
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

/*
Pre refactor
*/
// void	*dining_routine(void *arg) // Renamed from ft_dining
// {
// 	t_philo	*philo;
// 	t_vars	*vars;

// 	philo = (t_philo *)arg;
// 	vars = philo->shared_vars;
// 	if (philo->id % 2 == 0)
// 		usleep(vars->time_to_eat / 2 * 1000);
// 	while (!ft_mutex_death(philo))
// 	{
// 		take_forks(philo);
// 		start_eating(philo); // Updated reference
// 		pthread_mutex_lock(&vars->meal_check_mutex);
// 		if (vars->max_meals != -1 && philo->meals_eaten >= vars->max_meals)
// 		{
// 			pthread_mutex_unlock(&vars->meal_check_mutex);
// 			break ;
// 		}
// 		pthread_mutex_unlock(&vars->meal_check_mutex);
// 		start_sleeping(philo); // Updated reference
// 		print_philosopher_message(THINK, philo);
// 	}
// 	return (NULL);
// }


void	*dining_routine(void *arg) // Renamed from ft_dining
{
	t_philo	*philo;
	t_vars	*vars;

	philo = (t_philo *)arg;
	vars = philo->shared_vars;
	debug_print("Philo %d started at time %ld", 
		philo->id + 1, current_time() - vars->start_time);
	if (philo->id % 2 == 0)
	{
		debug_print("Philo %d (even) delaying start by %d ms", 
                    philo->id + 1, vars->time_to_eat / 2);
		usleep(vars->time_to_eat / 2 * 1000);
	}
	while (!ft_mutex_death(philo))
	{
		debug_print("Philo %d loop iteration at time %ld", 
			philo->id + 1, current_time() - vars->start_time);
		take_forks(philo);
		start_eating(philo); // Updated reference
		pthread_mutex_lock(&vars->meal_check_mutex);
		if (vars->max_meals != -1 && philo->meals_eaten >= vars->max_meals)
		{
			pthread_mutex_unlock(&vars->meal_check_mutex);
			break ;
		}
		pthread_mutex_unlock(&vars->meal_check_mutex);
		// Before sleeping
        debug_print("Philo %d beginning sleep at time %ld", 
			philo->id + 1, current_time() - vars->start_time);
		start_sleeping(philo); // Updated reference
		print_philosopher_message(THINK, philo);
		// After thinking message
        debug_print("Philo %d beginning thinking at time %ld", 
			philo->id + 1, current_time() - vars->start_time);
	}
	debug_print("Philo %d exiting at time %ld", 
		philo->id + 1, current_time() - vars->start_time);
	return (NULL);
}
