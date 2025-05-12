/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_run_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:43:30 by bleow             #+#    #+#             */
/*   Updated: 2025/05/13 01:14:37 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	precise_usleep(int ms)
{
	struct timeval	start;
	struct timeval	current;
	long			elapsed;

	gettimeofday(&start, 0);
	while (1)
	{
		gettimeofday(&current, 0);
		elapsed = (current.tv_sec - start.tv_sec) * 1000;
		elapsed += (current.tv_usec - start.tv_usec) / 1000;
		if (elapsed >= ms)
			break ;
		usleep(100); // sleep 100 microseconds to reduce CPU
	}
}

long	get_elapsed_time_ms(struct timeval *start)
{
	struct timeval	current;
	long			elapsed;

	gettimeofday(&current, 0);
	elapsed = (current.tv_sec - start->tv_sec) * 1000;
	elapsed += (current.tv_usec - start->tv_usec) / 1000;
	return (elapsed);
}

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

void	stagger_start(int id, int philo_count, t_philo *philo)
{
	t_vars	*vars;
	int		base_delay;

	vars = philo->shared_vars;
	if (philo_count <= 1)
		return ;
	// Calculate adaptive staggering delay based on eat time
	base_delay = (vars->time_to_eat * 4) / 10;  // 40% of time_to_eat
	// Add small adjustment for odd philosopher counts
	if (philo_count % 2 != 0)
		base_delay = (base_delay * 4) / 3;  // Add 25% more delay
	// Ensure minimum and maximum reasonable delays
	if (base_delay < 20)
		base_delay = 20;  // Minimum 20ms
	if (base_delay > 200)
		base_delay = 200;  // Maximum 200ms
	// Apply staggering to odd philosophers
	if (id % 2 != 0)
	{
		precise_usleep(base_delay);
		debug_print("Philo %d delayed start (odd ID staggering - %dms)", 
				   philo->id + 1, base_delay);
	}
	else
	{
		debug_print("Philo %d immediate start (even ID staggering)", 
				   philo->id + 1);
	}
}

int chk_ate_or_dead(t_vars *vars)
{
	int		i;
	long	curr_timestamp;

	i = 0;
	curr_timestamp = curr_time();
	// Set is_done to true initially (assuming all have finished)
	pthread_mutex_lock(&vars->hestia);
	vars->is_done = 1;
	pthread_mutex_unlock(&vars->hestia);
	while (i < vars->head_count)
	{
		// Check for death with the check_death flag set to 1
		if (run_atropos(vars->sophoi[i], curr_timestamp, 1))
			return (1);
		// Check meal completion
		if (vars->max_meals != -1)
		{
			pthread_mutex_lock(&vars->hestia);
			if (vars->sophoi[i]->meals_eaten < vars->max_meals)
				vars->is_done = 0;  // Directly modify is_done
			pthread_mutex_unlock(&vars->hestia);
		}
		i++;
	}
	return (0);
}

int	run_atropos(t_philo *philo, long curr_timestamp, int check_death)
{
	t_vars	*vars;
	int		is_dead;

	vars = philo->shared_vars;
	pthread_mutex_lock(&vars->atropos);
	// Death detection (when check_death is true)
	if (check_death && !vars->is_dead && philo->next_meal_time < curr_timestamp)
	{
		// Mark as dead
		vars->is_dead = 1;
		vars->died_philo_id = philo->id;
		is_dead = 1;
		// Unlock mutex before printing
		pthread_mutex_unlock(&vars->atropos);
		// Announce death
		print_status(DIED, philo);
		#ifdef DEBUG
		debug_print("Death detected and announced by Philo %d", philo->id + 1);
		#endif
		return (is_dead);
	}
	// Regular death status check
	is_dead = vars->is_dead;
	pthread_mutex_unlock(&vars->atropos);
	#ifdef DEBUG
	if (is_dead)
		debug_print("Death detected by Philo %d", philo->id + 1);
	#endif
	return (is_dead);
}

void	*run_argus(void *arg)
{
	t_vars	*vars;
	long	monitor_interval_ms;

	vars = (t_vars *)arg;
	precise_usleep(5);
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
			monitor_interval_ms = 1;
		else
			monitor_interval_ms = 3;
		precise_usleep(monitor_interval_ms);
	}
	return (NULL);
}
