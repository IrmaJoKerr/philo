/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   timing.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 05:31:28 by bleow             #+#    #+#             */
/*   Updated: 2025/05/29 23:24:39 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	better_usleep(int ms)
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
		usleep(100);
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

void	delay_start(int id, int philo_count, t_philo *philo)
{
	t_vars	*vars;
	int		base_delay;

	vars = philo->shared_vars;
	if (philo_count <= 1)
		return ;
	if (philo_count == 2)
	{
		if (id == 1)
			better_usleep(1);
		return ;
	}
	base_delay = (vars->time_to_eat * 4) / 10;
	if (philo_count % 2 != 0)
		base_delay = (base_delay * 4) / 3;
	if (base_delay < 20)
		base_delay = 20;
	if (base_delay > 200)
		base_delay = 200;
	if (id % 2 != 0)
	{
		better_usleep(base_delay);
	}
}
