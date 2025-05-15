/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialise.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:42:07 by bleow             #+#    #+#             */
/*   Updated: 2025/05/13 05:58:34 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	init_vars(char **av, t_vars *vars)
{
	vars->head_count = ft_atoi(av[1]);
	vars->time_to_die = ft_atoi(av[2]);
	vars->time_to_eat = ft_atoi(av[3]);
	vars->time_to_sleep = ft_atoi(av[4]);
	vars->max_meals = -1;
	vars->is_done = 0;
	vars->is_dead = 0;
	if (av[5] != NULL)
		vars->max_meals = ft_atoi(av[5]);
	return (0);
}

int	chk_args_and_init(int ac, char **av, t_vars *vars)
{
	if (validate_args(ac, av))
		return (1);
	return (init_vars(av, vars));
}

int	init_cerberus(t_vars *vars)
{
	int	i;

	vars->skeuos = malloc(sizeof(pthread_mutex_t) * vars->head_count);
	if (!vars->skeuos)
		return (1);
	i = 0;
	while (i < vars->head_count)
		if (pthread_mutex_init(&vars->skeuos[i++], NULL))
			return (1);
	vars->start_time = curr_time();
	if (pthread_mutex_init(&vars->hermes, NULL)
		|| pthread_mutex_init(&vars->atropos, NULL)
		|| pthread_mutex_init(&vars->hestia, NULL))
		return (1);
	return (0);
}

int	init_akademia(t_vars *vars, t_philo ***sophoi)
{
	int	i;

	i = 0;
	*sophoi = malloc(sizeof(t_philo *) * vars->head_count);
	if (!*sophoi)
		return (1);
	vars->sophoi = *sophoi;
	while (i < vars->head_count)
	{
		(*sophoi)[i] = malloc(sizeof(t_philo));
		if (!(*sophoi)[i])
			return (1);
		(*sophoi)[i]->id = i;
		(*sophoi)[i]->left_fork = i;
		(*sophoi)[i]->right_fork = (i + 1) % vars->head_count;
		(*sophoi)[i]->is_dead = 0;
		(*sophoi)[i]->meals_eaten = 0;
		(*sophoi)[i]->shared_vars = vars;
		(*sophoi)[i]->last_meal_time = curr_time();
		(*sophoi)[i]->next_meal_time = (*sophoi)[i]->last_meal_time
			+ vars->time_to_die;
		i++;
	}
	return (0);
}
