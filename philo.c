/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:40:28 by bleow             #+#    #+#             */
/*   Updated: 2025/04/26 14:50:17 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"


int	validate_arguments(int ac, char **av)
{
	if (ac < 5 || ac > 6)
		return (print_error("wrong number of arg"));
	if (ft_check_num(ac, av))
		return (print_error("digit only"));
	if (!av[1] || ft_atoi(av[1]) < 1)
		return (print_error("number of philosopher"));
	if (!av[2] || ft_atoi(av[2]) < 60)
		return (print_error("time to die"));
	if (!av[3] || ft_atoi(av[3]) < 60)
		return (print_error("time to eat"));
	if (!av[4] || ft_atoi(av[4]) < 60)
		return (print_error("time to sleep"));
	if (av[5] && ft_atoi(av[5]) < 1)
		return (print_error("number to eat"));
	return (0);
}

int	initialize_vars(char **av, t_vars *vars)
{
	vars->num_philosophers = ft_atoi(av[1]);
	vars->time_to_die = ft_atoi(av[2]);
	vars->time_to_eat = ft_atoi(av[3]);
	vars->time_to_sleep = ft_atoi(av[4]);
	vars->max_meals = -1;
	if (av[5] != NULL)
		vars->max_meals = ft_atoi(av[5]);
	return (0);
}

int	parse_and_initialize(int ac, char **av, t_vars *vars)
{
	if (validate_arguments(ac, av))
		return (1);
	return (initialize_vars(av, vars));
}

int	initialize_mutexes(t_vars *vars)
{
	int	i;

	vars->fork_mutexes = malloc(sizeof(pthread_mutex_t) * vars->num_philosophers);
	if (!vars->fork_mutexes)
		return (1);
	i = 0;
	while (i < vars->num_philosophers)
		if (pthread_mutex_init(&vars->fork_mutexes[i++], NULL))
			return (1);
	vars->start_time = current_time();
	if (pthread_mutex_init(&vars->message_mutex, NULL) ||
		pthread_mutex_init(&vars->death_mutex, NULL) ||
		pthread_mutex_init(&vars->meal_check_mutex, NULL))
		return (1);
	return (0);
}

int	initialize_philosophers(t_vars *vars, t_philo ***philosophers)
{
	int	i;

	i = 0;
	*philosophers = malloc(sizeof(t_philo *) * vars->num_philosophers);
	if (!*philosophers)
		return (1);
	vars->philosophers = *philosophers;
	while (i < vars->num_philosophers)
	{
		(*philosophers)[i] = malloc(sizeof(t_philo));
		if (!(*philosophers)[i])
			return (1);
		(*philosophers)[i]->id = i;
		(*philosophers)[i]->left_fork = i;
		(*philosophers)[i]->right_fork = (i + 1) % vars->num_philosophers;
		(*philosophers)[i]->is_dead = 0;
		(*philosophers)[i]->meals_eaten = 0;
		(*philosophers)[i]->shared_vars = vars;
		(*philosophers)[i]->last_meal_time = current_time();
		(*philosophers)[i]->next_meal_time = (*philosophers)[i]->last_meal_time + vars->time_to_die;
		i++;
	}
	return (0);
}

int	ft_thread(t_vars *vars, t_philo ***philosophers)
{
	int	i;

	i = 0;
	if (pthread_create(&vars->monitor_thread, NULL, ft_monitor, vars))
		return (1);
	vars->threads = malloc(sizeof(pthread_t) * vars->num_philosophers);
	if (!vars->threads)
		return (1);
	while (i < vars->num_philosophers)
	{
		if (pthread_create(&vars->threads[i], NULL, dining_routine, (*philosophers)[i]))
			return (1);
		i++;
	}
	i = 0;
	while (i < vars->num_philosophers)
		pthread_join(vars->threads[i++], NULL);
	pthread_join(vars->monitor_thread, NULL);
	return (0);
}

int	main(int ac, char **av)
{
	t_vars	vars;
	t_philo	**philo;

	vars = (t_vars){0};
	if (parse_and_initialize(ac, av, &vars))
		return (0);
	if (initialize_mutexes(&vars))
	{
		cleanup_resources(&vars, NULL);
		return (0);
	}
	if (initialize_philosophers(&vars, &philo))
	{
		cleanup_resources(&vars, NULL);
		return (0);
	}
	ft_thread(&vars, &philo);
	cleanup_resources(&vars, &philo);
	return (0);
}
