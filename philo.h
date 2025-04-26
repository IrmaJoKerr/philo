/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:42:34 by bleow             #+#    #+#             */
/*   Updated: 2025/04/26 16:23:05 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <limits.h>
# include <pthread.h>
# include <sys/time.h>
# include <stdarg.h>  

# define DEBUG 1 

# define FORK 1
# define EAT 2
# define SLEEP 3
# define THINK 4
# define DIED 5
# define END 6

typedef struct s_philo	t_philo;

typedef struct s_vars
{
	int				num_philosophers;      // Renamed from n_philo
	int				time_to_die;          // Renamed from tt_die
	int				time_to_eat;          // Renamed from tt_eat
	int				time_to_sleep;        // Renamed from tt_sleep
	int				max_meals;           // Renamed from max_meal
	pthread_mutex_t	meal_check_mutex;     // Renamed from meal_check
	pthread_mutex_t	*fork_mutexes;        // Renamed from forks
	pthread_mutex_t	message_mutex;        // Renamed from msg
	pthread_mutex_t	death_mutex;          // Renamed from death
	int				is_done;              // Renamed from done
	int				is_dead;              // Renamed from die
	int				meals_completed;      // Renamed from meals_complete
	long			start_time;           // Renamed from tt_start
	pthread_t		*threads;             // Renamed from thread
	pthread_t		monitor_thread;       // Renamed from monitor
	t_philo			**philosophers;       // Renamed from philos
}	t_vars;

typedef struct s_philo
{
	int		id;                        // Renamed from n
	int		left_fork;                // Renamed from l_hnd
	int		right_fork;               // Renamed from r_hnd
	int		is_dead;                  // Renamed from die
	int		meals_eaten;              // Renamed from eaten
	long	next_meal_time;           // Renamed from next_meal
	long	last_meal_time;           // Renamed from last_meal
	t_vars	*shared_vars;             // Renamed from misc
}	t_philo;

void	debug_print(const char *format, ...);
int		print_error(const char *msg); // Renamed from ft_ext_msg
size_t	ft_strlen(const char *str);
int		ft_atoi(const char *str);
long	current_time(void); // Renamed from now
void	log_message(int id, long time, int n); // Time in milliseconds
void	print_philosopher_message(int id, t_philo *philo); // Renamed from ft_print_msg
int		validate_arguments(int ac, char **av); // New function
int		initialize_vars(char **av, t_vars *vars); // New function
int		parse_and_initialize(int ac, char **av, t_vars *vars); // Renamed from ft_parse
int		ft_check_done(t_vars *vars);
int		ft_check_num(int ac, char **av);
void	*ft_monitor(void *arg);
int		initialize_mutexes(t_vars *vars); // Renamed from ft_init_mutex
int		initialize_philosophers(t_vars *vars, t_philo ***philo); // Renamed from ft_init_philo
void	take_forks(t_philo *philo); // Renamed from ft_fork
void	start_eating(t_philo *philo); // Renamed from ft_eat
void	start_sleeping(t_philo *philo); // Renamed from ft_sleep
int		ft_mutex_death(t_philo *philo);
void	*dining_routine(void *arg); // Renamed from ft_dining
int		ft_thread(t_vars *vars, t_philo ***philo);
int		cleanup_resources(t_vars *vars, t_philo ***philo); // Renamed from ft_free

#endif