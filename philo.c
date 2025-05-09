/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huidris <huidris@student.42kl.edu.my>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:40:28 by bleow             #+#    #+#             */
/*   Updated: 2025/05/10 04:09:14 by huidris          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	run_lachesis(t_vars *vars, t_philo ***sophoi)
{
	int	i;

	i = 0;
	if (pthread_create(&vars->argus, NULL, run_argus, vars))
		return (1);
	vars->clotho = malloc(sizeof(pthread_t) * vars->head_count);
	if (!vars->clotho)
		return (1);
	while (i < vars->head_count)
	{
		if (pthread_create(&vars->clotho[i], NULL, run_sim, (*sophoi)[i]))
			return (1);
		i++;
	}
	i = 0;
	while (i < vars->head_count)
		pthread_join(vars->clotho[i++], NULL);
	pthread_join(vars->argus, NULL);
	return (0);
}

void	*run_sim(void *arg)
{
	t_philo	*philo;
	t_vars	*vars;

	philo = (t_philo *)arg;
	vars = philo->shared_vars;
	if (vars->head_count == 1)
		return (solo_philo_case(philo), NULL);
	usleep((philo->id) * 20 * 1000);
	while (!run_atropos(philo))
	{
		if (grab_forks(philo))
		{
			eat_start(philo);
			pthread_mutex_lock(&vars->hestia);
			if (vars->max_meals != -1 && philo->meals_eaten >= vars->max_meals)
			{
				pthread_mutex_unlock(&vars->hestia);
				break ;
			}
			pthread_mutex_unlock(&vars->hestia);
			zzz_start(philo);
			print_status(THINK, philo);
		}
		else
			usleep(((philo->id * 37) % 10 + 1) * 500);
	}
	return (NULL);
}

int	main(int ac, char **av)
{
	t_vars	vars;
	t_philo	**philo;

	vars = (t_vars){0};
	if (chk_args_and_init(ac, av, &vars))
		return (0);
	if (init_cerberus(&vars))
	{
		run_katharsis(&vars, NULL);
		return (0);
	}
	if (init_akademia(&vars, &philo))
	{
		run_katharsis(&vars, NULL);
		return (0);
	}
	run_lachesis(&vars, &philo);
	run_katharsis(&vars, &philo);
	return (0);
}
