/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:40:28 by bleow             #+#    #+#             */
/*   Updated: 2025/05/13 01:16:45 by bleow            ###   ########.fr       */
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
	stagger_start(philo->id, vars->head_count, philo);
	while (!run_atropos(philo, 0, 0))
	{
		if (grab_forks(philo))
		{
			eat_start(philo);
			if (should_exit_after_meals(philo))
				break ;
			zzz_start(philo);
			print_status(THINK, philo);
		}
		// else
		// 	calculate_and_wait_retry_delay(philo);
	}
	return (NULL);
}

int	should_exit_after_meals(t_philo *philo)
{
	t_vars	*vars;
	int		should_exit;

	vars = philo->shared_vars;
	should_exit = 0;
	pthread_mutex_lock(&vars->hestia);
	if (vars->max_meals != -1 && philo->meals_eaten >= vars->max_meals)
		should_exit = 1;
	pthread_mutex_unlock(&vars->hestia);
	return (should_exit);
}

// void	calculate_and_wait_retry_delay(t_philo *philo)
// {
// 	t_vars	*vars;
// 	long	time_left;
// 	int		retry_delay_ms;

// 	vars = philo->shared_vars;
// 	pthread_mutex_lock(&vars->atropos);
// 	time_left = philo->next_meal_time - curr_time();
// 	pthread_mutex_unlock(&vars->atropos);
// 	// Simple adaptive retry strategy
// 	if (time_left < 50)
// 		retry_delay_ms = 1;  // Very urgent - 1ms
// 	else if (time_left < 100)
// 		retry_delay_ms = 3;  // Somewhat urgent - 3ms
// 	else
// 	{
// 		// Normal delay: use ID to differentiate even/odd philosophers
// 		if (philo->id % 2 == 0)
// 			retry_delay_ms = 7;  // Even IDs get 7ms
// 		else
// 			retry_delay_ms = 5;  // Odd IDs get 5ms
// 	}
// 	precise_usleep(retry_delay_ms);
// }

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
