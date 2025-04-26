/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:42:34 by bleow             #+#    #+#             */
/*   Updated: 2025/04/26 20:58:54 by bleow            ###   ########.fr       */
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



/*
Since this is a philosophers program I wanted to be creative with names.
I decided to use names from Greek mythology.
-hestia: Goddess of the hearth and food. Lock and checks if ate already.
-hermes: God of messengers. Locks and prints messages.
-clotho: Goddess of spinning life threads. For making new threads.
-lachesis : Goddess of measuring and weaving life threads. Manages the
 threads.
-atropos: Goddess of determining death. Locks and checks if dead.
-argus: All-seeing Giant. Keeps an eye on each of the philosophers.
-cerberus: Guardian hound of Hades. Mutexes are locks and cerberus 
 guards against trespassing.
-sophoi: Greek word for "wise men" or philosophers. Philosopher thread.
-skeuos: Greek word for "utensil". For forks/fork mutexes.
-akademia: Greek word for "school/place of learning".
-katharsis: Greek word for "cleansing/purging". For full cleanup.
*/
typedef struct s_vars
{
	int				head_count;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				max_meals;
	pthread_mutex_t	hestia;
	pthread_mutex_t	*skeuos;
	pthread_mutex_t	hermes;
	pthread_mutex_t	atropos;
	int				is_done;
	int				is_dead;
	// int				meals_completed;      // Renamed from meals_complete
	long			start_time;
	pthread_t		*clotho;
	pthread_t		argus;
	t_philo			**sophoi;
}	t_vars;

typedef struct s_philo
{
	int		id;
	int		left_fork;
	int		right_fork;
	int		is_dead;
	int		meals_eaten;
	long	next_meal_time;
	long	last_meal_time;
	t_vars	*shared_vars;
}	t_philo;

void	clean_skeuos(t_vars *vars);
int		run_katharsis(t_vars *vars, t_philo ***philo);

void	debug_print(const char *format, ...); //DEBUG
int		init_vars(char **av, t_vars *vars);
int		chk_args_and_init(int ac, char **av, t_vars *vars);
int		init_cerberus(t_vars *vars);
int		init_akademia(t_vars *vars, t_philo ***philo);

int		run_lachesis(t_vars *vars, t_philo ***philo);
void	*run_sim(void *arg);

int		print_error(const char *msg);
void	run_hermes(int id, long time, int n);
void	print_status(int id, t_philo *philo);

long	curr_time(void);
int		chk_ate_or_dead(t_vars *vars);
int		digits_valid(int ac, char **av);
void	*run_argus(void *arg);
// void	handle_single_philosopher(t_philo *philo);
void	*handle_single_philosopher(t_philo *philo);
void	determine_fork_order(t_philo *philo, int *first_fork, int *second_fork);
void	grab_forks(t_philo *philo);
void	release_forks(t_philo *philo);
void	eat_start(t_philo *philo);
void	zzz_start(t_philo *philo);
int		run_atropos(t_philo *philo);

int		validate_arguments(int ac, char **av);
size_t	ft_strlen(const char *str);
int		calc(long long res, int sign, const char *str, int i);
int		ft_atoi(const char *str);

#endif