/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 19:39:38 by bleow             #+#    #+#             */
/*   Updated: 2025/05/12 09:51:15 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	clean_skeuos(t_vars *vars)
{
	int	i;
	int	mutex_count;

	if (!vars->skeuos)
		return ;
	mutex_count = vars->head_count;
	i = 0;
	while (i < mutex_count)
	{
		pthread_mutex_destroy(&vars->skeuos[i]);
		i++;
	}
	free(vars->skeuos);
}

int	run_katharsis(t_vars *vars, t_philo ***sophoi)
{
	int	i;

	clean_skeuos(vars);
	pthread_mutex_destroy(&vars->hermes);
	pthread_mutex_destroy(&vars->atropos);
	pthread_mutex_destroy(&vars->hestia);
	if (sophoi && *sophoi)
	{
		i = 0;
		while (i < vars->head_count)
		{
			if ((*sophoi)[i])
				free((*sophoi)[i]);
			i++;
		}
		free(*sophoi);
	}
	if (vars->clotho)
		free(vars->clotho);
	return (0);
}
