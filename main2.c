/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hskrzypi <hskrzypi@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 19:30:29 by hskrzypi          #+#    #+#             */
/*   Updated: 2024/10/31 19:30:47 by hskrzypi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	wait_for_processes(t_var *px_var, int pid1, int pid2)
{
	int	status;
	int	i;

	i = 0;
	while (i < 2)
	{
		if (pid1 > 0)
			waitpid(pid1, &status, 0);
		if (pid2 > 0)
		{
			waitpid(pid2, &status, 0);
			if (WIFEXITED(status))
				px_var->exitcode = WEXITSTATUS(status);
		}
		i++;
	}
}

void	create_pipe(int fd[2], t_var *px)
{
	if (pipe(fd) == -1)
	{
		clean_up(px);
		exit_file_error(px, "pipe creation error");
	}
}

int	pipex(t_var *px)
{
	int	pid1;
	int	pid2;
	int	fd[2];

	pid1 = -1;
	pid2 = -1;
	create_pipe(fd, px);
	if (!px->error_cmd1 && px->input_fd > -1)
		pid1 = fork_first_child(px, fd);
	if (px->cmd1)
		free_array(&px->cmd1);
	if (!px->error_cmd2 && px->output_fd > -1)
		pid2 = fork_second_child(px, fd);
	close(fd[0]);
	close(fd[1]);
	wait_for_processes(px, pid1, pid2);
	if (px->output_fd < 0)
		px->exitcode = 1;
	clean_up(px);
	return (px->exitcode);
}

int	main(int argc, char **argv, char **envp)
{
	t_var	px;

	if (argc != 5)
	{
		write(2, "Params should be: file1 cmd1 cmd2 file2\n", 40);
		exit(EXIT_FAILURE);
	}
	check_fd(argv, &px);
	init_variables(argv, envp, &px);
	pipex(&px);
	if (px.cmd1)
		free_array(&px.cmd1);
	if (px.cmd2)
		free_array(&px.cmd2);
	return (px.exitcode);
}
