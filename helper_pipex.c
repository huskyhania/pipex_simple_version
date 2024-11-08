/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper_pipex.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hskrzypi <hskrzypi@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 21:03:04 by hskrzypi          #+#    #+#             */
/*   Updated: 2024/11/02 13:47:44 by hskrzypi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

// Function redirect input to stdin and output to stdout
// closes fds, calls get cmd path and executes a command, cleans up if exec fails
void	execute_command(t_var *px, int input_fd, int output_fd, char **cmd)
{
	dup2(input_fd, STDIN_FILENO);
	dup2(output_fd, STDOUT_FILENO);
	close(input_fd);
	close(output_fd);
	px->cmd_path = get_command_path(cmd[0], px);
	if (px->cmd_path)
		execve(px->cmd_path, cmd, px->envp);
	free(px->cmd_path);
	free_array(&cmd);
	clean_up(px);
	exit(px->exitcode);
}

// Function to execute command for the first child, 
// closes unused read end of the pipe, passes input, 
// output and command to execute command
void	handle_first_child(t_var *px, int fd[2])
{
	close(fd[0]);
	execute_command(px, px->input_fd, fd[1], px->cmd1);
	if (px->cmd1)
		free_array(&px->cmd1);
	exit(px->exitcode);
}

// Function to execute command for the second child, 
// closes unused write end of the pipe
// passes input, output and command to execute command
void	handle_second_child(t_var *px, int fd[2])
{
	close(fd[1]);
	execute_command(px, fd[0], px->output_fd, px->cmd2);
	if (px->cmd2)
		free_array(&px->cmd2);
	exit(127);
}

int	fork_first_child(t_var *px, int fd[2])
{
	int	pid;

	pid = fork();
	if (pid < 0)
	{
		clean_up(px);
		exit_file_error(px, "fork error");
	}
	if (pid == 0)
		handle_first_child(px, fd);
	return (pid);
}

int	fork_second_child(t_var *px, int fd[2])
{
	int	pid;

	pid = fork();
	if (pid < 0)
	{
		clean_up(px);
		exit_file_error(px, "fork 2 error");
	}
	if (pid == 0)
		handle_second_child(px, fd);
	return (pid);
}
