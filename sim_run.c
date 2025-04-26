/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_run.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:43:37 by bleow             #+#    #+#             */
/*   Updated: 2025/04/26 21:07:30 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

// void	handle_single_philosopher(t_philo *philo)
// {
// 	t_vars	*vars;

// 	vars = philo->shared_vars;
// 	pthread_mutex_lock(&vars->skeuos[philo->left_fork]);
// 	print_status(FORK, philo);
// 	usleep(vars->time_to_die * 1000);
// 	pthread_mutex_unlock(&vars->skeuos[philo->left_fork]);
// }
void	*handle_single_philosopher(t_philo *philo)
{
	t_vars	*vars;

	vars = philo->shared_vars;
	pthread_mutex_lock(&vars->skeuos[philo->left_fork]);
	print_status(FORK, philo);
	usleep(vars->time_to_die * 1000);
	pthread_mutex_unlock(&vars->skeuos[philo->left_fork]);
	return (NULL);
}

/**
 * Determines the order in which philosophers should acquire forks
 * Even philosophers: left fork first, right fork second
 * Odd philosophers: right fork first, left fork second
 * @param philo The philosopher
 * @param first_fork Pointer to store the first fork index
 * @param second_fork Pointer to store the second fork index
 */
void determine_fork_order(t_philo *philo, int *first_fork, int *second_fork)
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
    debug_print("Philo %d assigned forks: first=%d, second=%d", 
                philo->id + 1, *first_fork, *second_fork);
}

/**
 * Attempts to acquire both forks needed for a philosopher to eat
 * Uses asymmetric fork acquisition strategy to prevent deadlock
 * Implements safety margin to prevent starvation
 * @param philo The philosopher trying to acquire forks
 */
// void	grab_forks(t_philo *philo)
// {
// 	t_vars	*vars;
// 	int		first_fork;
// 	int		second_fork;
// 	long	next_meal;

// 	vars = philo->shared_vars;
// 	// if (vars->head_count == 1)
// 	// {
// 	// 	handle_single_philosopher(philo);
// 	// 	return ;
// 	// }
// 	determine_fork_order(philo, &first_fork, &second_fork);
// 	pthread_mutex_lock(&vars->skeuos[first_fork]);
// 	print_status(FORK, philo);
// 	if (run_atropos(philo))
// 	{
// 		pthread_mutex_unlock(&vars->skeuos[first_fork]);
// 		return ;
// 	}
// 	pthread_mutex_lock(&vars->atropos);
// 	next_meal = philo->next_meal_time;
// 	pthread_mutex_unlock(&vars->atropos);
// 	if (curr_time() > next_meal - 500)
// 	{
// 		pthread_mutex_unlock(&vars->skeuos[first_fork]);
// 		usleep(200);
// 		return ;
// 	}
// 	pthread_mutex_lock(&vars->skeuos[second_fork]);
// 	print_status(FORK, philo);
// }
void grab_forks(t_philo *philo)
{
    t_vars *vars;
    int first_fork;
    int second_fork;
    long next_meal;

    vars = philo->shared_vars;
    debug_print("Philo %d attempting to take forks at time %ld", 
                philo->id + 1, curr_time() - vars->start_time);

    determine_fork_order(philo, &first_fork, &second_fork);
    
    debug_print("Philo %d trying to acquire first fork %d", philo->id + 1, first_fork);
    pthread_mutex_lock(&vars->skeuos[first_fork]);
    debug_print("Philo %d acquired first fork %d at time %ld", 
                philo->id + 1, first_fork, curr_time() - vars->start_time);
    
    print_status(FORK, philo);
    
    if (run_atropos(philo))
    {
        debug_print("Philo %d releasing fork - simulation ended", philo->id + 1);
        pthread_mutex_unlock(&vars->skeuos[first_fork]);
        return;
    }
    
    pthread_mutex_lock(&vars->atropos);
    next_meal = philo->next_meal_time;
    pthread_mutex_unlock(&vars->atropos);
    
    long current = curr_time();
    debug_print("Philo %d safety check: current=%ld, next_meal=%ld, diff=%ld, threshold=%ld", 
                philo->id + 1, current - vars->start_time, 
                next_meal - vars->start_time,
                next_meal - current, 500);
                
    if (current > next_meal - 500)
    {
        debug_print("Philo %d releasing first fork - death too close (margin: %ld ms)", 
                    philo->id + 1, next_meal - current);
        pthread_mutex_unlock(&vars->skeuos[first_fork]);
        usleep(200);
        return;
    }
    
    debug_print("Philo %d trying to acquire second fork %d", philo->id + 1, second_fork);
    pthread_mutex_lock(&vars->skeuos[second_fork]);
    debug_print("Philo %d acquired both forks (%d, %d) at time %ld", 
                philo->id + 1, first_fork, second_fork, curr_time() - vars->start_time);
    print_status(FORK, philo);
}

// void	grab_forks(t_philo *philo)
// {
// 	t_vars	*vars;
// 	int		first_fork;
// 	int		second_fork;
// 	long	next_meal;

// 	vars = philo->shared_vars;
// 	if (vars->head_count == 1)
// 	{
// 		handle_single_philosopher(philo);
// 		return ;
// 	}
// 	if (philo->id % 2 == 0)
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
// 	if (run_atropos(philo))
// 	{
// 		pthread_mutex_unlock(&vars->skeuos[first_fork]);
// 		return ;
// 	}
// 	pthread_mutex_lock(&vars->atropos);
// 	next_meal = philo->next_meal_time;
// 	pthread_mutex_unlock(&vars->atropos);
// 	if (curr_time() > next_meal - 500)
// 	{
// 		pthread_mutex_unlock(&vars->skeuos[first_fork]);
// 		usleep(200);
// 		return ;
// 	}
// 	pthread_mutex_lock(&vars->skeuos[second_fork]);
// 	print_status(FORK, philo);
// }

//BACKUP BEFORE REFACTOR
// void	grab_forks(t_philo *philo)
// {
// 	t_vars	*vars;
// 	int		first_fork;
// 	int		second_fork;
// 	long	next_meal;

// 	vars = philo->shared_vars;
// 	if (vars->head_count == 1)
// 	{
// 		pthread_mutex_lock(&vars->skeuos[philo->left_fork]);
// 		print_status(FORK, philo);
// 		usleep(vars->time_to_die * 1000);
// 		pthread_mutex_unlock(&vars->skeuos[philo->left_fork]);
// 		return ;
// 	}
// 	if (philo->id % 2 == 0)
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
// 	if (run_atropos(philo))
// 	{
// 		pthread_mutex_unlock(&vars->skeuos[first_fork]);
// 		return ;
// 	}
// 	pthread_mutex_lock(&vars->atropos);
// 	next_meal = philo->next_meal_time;
// 	pthread_mutex_unlock(&vars->atropos);
// 	if (curr_time() > next_meal - 500)
// 	{
// 		pthread_mutex_unlock(&vars->skeuos[first_fork]);
// 		usleep(200);
// 		return ;
// 	}
// 	pthread_mutex_lock(&vars->skeuos[second_fork]);
// 	print_status(FORK, philo);
// }

// void	eat_start(t_philo *philo)
// {
// 	t_vars	*vars;

// 	vars = philo->shared_vars;
// 	pthread_mutex_lock(&vars->atropos);
// 	if (vars->is_dead)
// 	{
// 		pthread_mutex_unlock(&vars->atropos);
// 		pthread_mutex_unlock(&vars->skeuos[philo->left_fork]);
// 		pthread_mutex_unlock(&vars->skeuos[philo->right_fork]);
// 		return ;
// 	}
// 	pthread_mutex_unlock(&vars->atropos);
// 	print_status(EAT, philo);
// 	pthread_mutex_lock(&vars->atropos);
// 	philo->last_meal_time = curr_time();
// 	philo->next_meal_time = philo->last_meal_time + vars->time_to_die;
// 	pthread_mutex_unlock(&vars->atropos);
// 	pthread_mutex_lock(&vars->hestia);
// 	philo->meals_eaten++;
// 	pthread_mutex_unlock(&vars->hestia);
// 	usleep(vars->time_to_eat * 1000);
// 	if (philo->id % 2 == 0)
// 	{
// 		pthread_mutex_unlock(&vars->skeuos[philo->right_fork]);
// 		pthread_mutex_unlock(&vars->skeuos[philo->left_fork]);
// 	}
// 	else
// 	{
// 		pthread_mutex_unlock(&vars->skeuos[philo->left_fork]);
// 		pthread_mutex_unlock(&vars->skeuos[philo->right_fork]);
// 	}
// }

// void	release_forks(t_philo *philo)
// {
// 	t_vars	*vars;

// 	vars = philo->shared_vars;
// 	if (philo->id % 2 == 0)
// 	{
// 		pthread_mutex_unlock(&vars->skeuos[philo->right_fork]);
// 		pthread_mutex_unlock(&vars->skeuos[philo->left_fork]);
// 	}
// 	else
// 	{
// 		pthread_mutex_unlock(&vars->skeuos[philo->left_fork]);
// 		pthread_mutex_unlock(&vars->skeuos[philo->right_fork]);
// 	}
// }

// void	release_forks(t_philo *philo)
// {
// 	t_vars	*vars;
// 	int		first_fork;
// 	int		second_fork;

// 	vars = philo->shared_vars;
// 	determine_fork_order(philo, &first_fork, &second_fork);
// 	pthread_mutex_unlock(&vars->skeuos[second_fork]);
// 	pthread_mutex_unlock(&vars->skeuos[first_fork]);
// }
void release_forks(t_philo *philo)
{
    t_vars *vars;
    int first_fork;
    int second_fork;

    vars = philo->shared_vars;
    determine_fork_order(philo, &first_fork, &second_fork);
    
    debug_print("Philo %d releasing forks (second=%d, first=%d) at time %ld", 
                philo->id + 1, second_fork, first_fork, curr_time() - vars->start_time);
                
    pthread_mutex_unlock(&vars->skeuos[second_fork]);
    pthread_mutex_unlock(&vars->skeuos[first_fork]);
}

// void	eat_start(t_philo *philo)
// {
// 	t_vars	*vars;

// 	vars = philo->shared_vars;
// 	if (run_atropos(philo))
// 	{
// 		pthread_mutex_unlock(&vars->skeuos[philo->left_fork]);
// 		pthread_mutex_unlock(&vars->skeuos[philo->right_fork]);
// 		return ;
// 	}
// 	pthread_mutex_unlock(&vars->atropos);
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
void eat_start(t_philo *philo)
{
    t_vars *vars;
    long old_next_meal;

    vars = philo->shared_vars;
    if (run_atropos(philo))
    {
        debug_print("Philo %d aborting eat - simulation ended", philo->id + 1);
        pthread_mutex_unlock(&vars->skeuos[philo->left_fork]);
        pthread_mutex_unlock(&vars->skeuos[philo->right_fork]);
        return;
    }
    
    print_status(EAT, philo);
    
    pthread_mutex_lock(&vars->atropos);
    old_next_meal = philo->next_meal_time;
    philo->last_meal_time = curr_time();
    philo->next_meal_time = philo->last_meal_time + vars->time_to_die;
    
    debug_print("Philo %d updated meal times: last_meal=%ld, next_meal=%ld (was %ld), time_to_die=%d", 
                philo->id + 1, philo->last_meal_time - vars->start_time, 
                philo->next_meal_time - vars->start_time, 
                old_next_meal - vars->start_time, vars->time_to_die);
                
    pthread_mutex_unlock(&vars->atropos);
    
    pthread_mutex_lock(&vars->hestia);
    philo->meals_eaten++;
    debug_print("Philo %d has eaten %d meals", philo->id + 1, philo->meals_eaten);
    pthread_mutex_unlock(&vars->hestia);
    
    debug_print("Philo %d sleeping for %d ms during eating", philo->id + 1, vars->time_to_eat);
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


