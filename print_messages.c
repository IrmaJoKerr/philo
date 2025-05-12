/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_messages.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 20:19:42 by bleow             #+#    #+#             */
/*   Updated: 2025/05/13 01:08:05 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	print_error(const char *msg)
{
	printf("Error: %s\n", msg);
	return (1);
}

void	run_hermes(int id, long time, int n)
{
	if (id == FORK)
		printf("%ld \t%d has taken a fork\n", time, n + 1);
	else if (id == EAT)
		printf("%ld \t%d is eating\n", time, n + 1);
	else if (id == SLEEP)
		printf("%ld \t%d is sleeping\n", time, n + 1);
	else if (id == THINK)
		printf("%ld \t%d is thinking\n", time, n + 1);
	else if (id == DIED)
		printf("%ld \t%d died\n", time, n + 1);
	else if (id == END)
		printf("Simulation completed!\n");
}

void	print_status(int id, t_philo *philo)
{
	long			time;
	t_vars			*vars;
	int				should_print;
	struct timeval	start_time_val;

	vars = philo->shared_vars;
	if (!vars)
		return ;
	// Check if we should print (avoid printing after death except for DIED messages)
	pthread_mutex_lock(&vars->atropos);
	should_print = !vars->is_dead || id == DIED;
	pthread_mutex_unlock(&vars->atropos);
	if (!should_print)
		return ;
	// Convert start_time to timeval struct
	start_time_val.tv_sec = vars->start_time / 1000;
	start_time_val.tv_usec = (vars->start_time % 1000) * 1000;
	// Only lock hermes for the actual printing
	pthread_mutex_lock(&vars->hermes);
	// Use get_elapsed_time_ms for more precise timing
	time = get_elapsed_time_ms(&start_time_val);
	run_hermes(id, time, philo->id);
	pthread_mutex_unlock(&vars->hermes);
}
