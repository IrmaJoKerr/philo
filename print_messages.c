/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_messages.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huidris <huidris@student.42kl.edu.my>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 20:19:42 by bleow             #+#    #+#             */
/*   Updated: 2025/05/10 04:04:09 by huidris          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	print_error(const char *msg)
{
	fprintf(stderr, "Error: %s\n", msg);
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
	long	time;
	t_vars	*vars;

	vars = philo->shared_vars;
	if (!vars)
		return ;
	pthread_mutex_lock(&vars->hermes);
	pthread_mutex_lock(&vars->atropos);
	if (vars->is_dead && id != DIED)
	{
		pthread_mutex_unlock(&vars->atropos);
		pthread_mutex_unlock(&vars->hermes);
		return ;
	}
	pthread_mutex_unlock(&vars->atropos);
	time = curr_time() - vars->start_time;
	run_hermes(id, time, philo->id);
	pthread_mutex_unlock(&vars->hermes);
}
