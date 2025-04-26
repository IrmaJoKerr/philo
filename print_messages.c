/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_messages.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 20:19:42 by bleow             #+#    #+#             */
/*   Updated: 2025/04/27 01:14:53 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	print_error(const char *msg)
{
	fprintf(stderr, "Error: %s\n", msg);
	return (1);
}

// void	run_hermes(int id, long time, int n)
// {
// 	if (id == FORK)
// 		printf("%ld \t%d has taken a fork\n", time, n + 1);
// 	else if (id == EAT)
// 		printf("%ld \t%d is eating\n", time, n + 1);
// 	else if (id == SLEEP)
// 		printf("%ld \t%d is sleeping\n", time, n + 1);
// 	else if (id == THINK)
// 		printf("%ld \t%d is thinking\n", time, n + 1);
// 	else if (id == DIED)
// 		printf("%ld \t%d died\n", time, n + 1);
// 	else if (id == END)
// 		printf("Simulation completed!\n");
// }
void run_hermes(int id, long time, int n)
{
    if (id == FORK)
        fprintf(stdout, "%ld \t%d has taken a fork\n", time, n + 1);
    else if (id == EAT)
        fprintf(stdout, "%ld \t%d is eating\n", time, n + 1);
    else if (id == SLEEP)
        fprintf(stdout, "%ld \t%d is sleeping\n", time, n + 1);
    else if (id == THINK)
        fprintf(stdout, "%ld \t%d is thinking\n", time, n + 1);
    else if (id == DIED)
        fprintf(stdout, "%ld \t%d died\n", time, n + 1);
    else if (id == END)
        fprintf(stdout, "Simulation completed!\n");
    fflush(stdout);  // Ensure output is immediately visible
}

// void	print_status(int id, t_philo *philo)
// {
// 	long	time;

// 	if (!philo->shared_vars || philo->shared_vars->is_dead)
// 		return ;
// 	pthread_mutex_lock(&philo->shared_vars->hermes);
// 	time = curr_time() - philo->shared_vars->start_time;
// 	run_hermes(id, time, philo->id);
// 	if (id != DIED)
// 		pthread_mutex_unlock(&philo->shared_vars->hermes);
// }
void	print_status(int id, t_philo *philo)
{
    long	time;
    int		sim_is_dead;

    if (!philo->shared_vars)
        return ;
    pthread_mutex_lock(&philo->shared_vars->atropos);
    sim_is_dead = philo->shared_vars->is_dead;
    pthread_mutex_unlock(&philo->shared_vars->atropos);
    if (sim_is_dead && id != DIED)
        return ;
    pthread_mutex_lock(&philo->shared_vars->hermes);
    pthread_mutex_lock(&philo->shared_vars->atropos);
    sim_is_dead = philo->shared_vars->is_dead;
    pthread_mutex_unlock(&philo->shared_vars->atropos);
    if (!sim_is_dead || id == DIED)
    {
        time = curr_time() - philo->shared_vars->start_time;
        run_hermes(id, time, philo->id);
    }
    pthread_mutex_unlock(&philo->shared_vars->hermes);
}
