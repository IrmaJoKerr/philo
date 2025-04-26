/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_run_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:43:30 by bleow             #+#    #+#             */
/*   Updated: 2025/04/26 22:30:03 by bleow            ###   ########.fr       */
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

// int	chk_ate_or_dead(t_vars *vars)
// {
// 	int		i;
// 	long	curr_timestamp;

// 	i = 0;
// 	curr_timestamp = curr_time();
// 	while (i < vars->head_count)
// 	{
// 		pthread_mutex_lock(&vars->atropos);
// 		if (vars->sophoi[i]->next_meal_time < curr_timestamp)
// 		{
// 			print_status(DIED, vars->sophoi[i]);
// 			vars->is_dead = 1;
// 			pthread_mutex_unlock(&vars->atropos);
// 			return (1);
// 		}
// 		pthread_mutex_unlock(&vars->atropos);
// 		if (vars->max_meals != -1)
// 		{
// 			pthread_mutex_lock(&vars->hestia);
// 			if (vars->sophoi[i]->meals_eaten < vars->max_meals)
// 				vars->is_done = 0;
// 			pthread_mutex_unlock(&vars->hestia);
// 		}
// 		i++;
// 	}
// 	return (0);
// }
// Add to chk_ate_or_dead for death debugging
int chk_ate_or_dead(t_vars *vars)
{
    int i;
    long curr_timestamp;

    i = 0;
    curr_timestamp = curr_time();
    debug_print("Monitor checking philosophers at time %ld", 
                curr_timestamp - vars->start_time);
                
    while (i < vars->head_count)
    {
        pthread_mutex_lock(&vars->atropos);
        long next_meal = vars->sophoi[i]->next_meal_time;
        pthread_mutex_unlock(&vars->atropos);
        
        debug_print("Philo %d next_meal=%ld, now=%ld, diff=%ld", 
                    i + 1, next_meal - vars->start_time,
                    curr_timestamp - vars->start_time,
                    next_meal - curr_timestamp);
                    
        pthread_mutex_lock(&vars->atropos);
        if (vars->sophoi[i]->next_meal_time < curr_timestamp)
        {
            debug_print("!!! Philo %d DIED: next_meal=%ld, now=%ld, diff=%ld", 
                        i + 1, vars->sophoi[i]->next_meal_time - vars->start_time,
                        curr_timestamp - vars->start_time,
                        vars->sophoi[i]->next_meal_time - curr_timestamp);
            print_status(DIED, vars->sophoi[i]);
            vars->is_dead = 1;
            pthread_mutex_unlock(&vars->atropos);
            return (1);
        }
        pthread_mutex_unlock(&vars->atropos);
        
        if (vars->max_meals != -1)
        {
            pthread_mutex_lock(&vars->hestia);
            int meals_eaten = vars->sophoi[i]->meals_eaten;
            debug_print("Philo %d has eaten %d/%d meals", 
                        i + 1, meals_eaten, vars->max_meals);
            if (meals_eaten < vars->max_meals)
                vars->is_done = 0;
            debug_print("After checking philosopher %d: is_done=%d, meals_eaten=%d", 
                        i+1, vars->is_done, vars->sophoi[i]->meals_eaten);
            pthread_mutex_unlock(&vars->hestia);
        }
        
        i++;
    }
    return (0);
}

// void	*run_argus(void *arg)
// {
// 	t_vars	*vars;

// 	vars = (t_vars *)arg;
// 	while (1)
// 	{
// 		vars->is_done = 1;
// 		if (chk_ate_or_dead(vars))
// 			return (NULL);
// 		if (vars->max_meals != -1 && vars->is_done)
// 		{
// 			pthread_mutex_lock(&vars->atropos);
// 			vars->is_dead = 1;
// 			pthread_mutex_unlock(&vars->atropos);
// 			printf("All philosophers have finished eating meals\n");
// 			return (NULL);
// 		}
// 		usleep(1000);
// 	}
// 	return (NULL);
// }
void *run_argus(void *arg)
{
    t_vars *vars;

    vars = (t_vars *)arg;
    // Add a delay to allow philosophers to start eating
    usleep(100000);  // 100ms initial delay
    
    while (1)
    {
        vars->is_done = 1;
        debug_print("Before meal check: is_done=%d, max_meals=%d", vars->is_done, vars->max_meals);
        
        if (chk_ate_or_dead(vars))
            return (NULL);
        
        if (vars->max_meals != -1 && vars->is_done)
        {
            // Check if philosophers have actually eaten using a recursive helper function
            if (check_any_philosopher_has_eaten(vars, 0))
            {
                debug_print("All philosophers have eaten at least %d meals", vars->max_meals);
                pthread_mutex_lock(&vars->atropos);
                vars->is_dead = 1;
                pthread_mutex_unlock(&vars->atropos);
                printf("All philosophers have finished eating meals\n");
                return (NULL);
            }
            else
            {
                debug_print("Meal completion check pending - no one has eaten yet");
            }
        }
        usleep(1000);
    }
    return (NULL);
}

// Helper function to check if any philosopher has eaten, using recursion instead of a loop
int check_any_philosopher_has_eaten(t_vars *vars, int index)
{
    // Base case: we've checked all philosophers and found none that have eaten
    if (index >= vars->head_count)
        return (0);
        
    pthread_mutex_lock(&vars->hestia);
    int has_eaten = vars->sophoi[index]->meals_eaten > 0;
    pthread_mutex_unlock(&vars->hestia);
    
    // If this philosopher has eaten, return true
    if (has_eaten)
        return (1);
        
    // Otherwise, check the next philosopher
    return check_any_philosopher_has_eaten(vars, index + 1);
}

// int	run_atropos(t_philo *philo)
// {
// 	t_vars	*vars;

// 	vars = philo->shared_vars;
// 	pthread_mutex_lock(&vars->atropos);
// 	if (vars->is_dead)
// 	{
// 		pthread_mutex_unlock(&vars->atropos);
// 		return (1);
// 	}
// 	pthread_mutex_unlock(&vars->atropos);
// 	return (0);
// }
int run_atropos(t_philo *philo)
{
    t_vars *vars;
    int is_dead;

    vars = philo->shared_vars;
    pthread_mutex_lock(&vars->atropos);
    is_dead = vars->is_dead;
    if (is_dead)
    {
        debug_print("Death detected by Philo %d", philo->id + 1);
    }
    pthread_mutex_unlock(&vars->atropos);
    return is_dead;
}
