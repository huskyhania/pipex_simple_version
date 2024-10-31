/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper_pipex.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hskrzypi <hskrzypi@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 21:03:04 by hskrzypi          #+#    #+#             */
/*   Updated: 2024/10/22 19:08:25 by hskrzypi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"
#include <stdio.h>// DELETE MEEEE

// Function redirect input to stdin and output to stdout
//closes fds, calls get cmd path and executes a command, cleans up if exec fails
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
	exit(127);
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
	if (px->output_fd < 0)
	{
		close(fd[0]);
		if (px->cmd_path)
		{
			free(px->cmd_path);
			px->cmd_path = NULL;
		}
		if (px->cmd2)
		{
			free_array(&px->cmd2);
			px->cmd2 = NULL;
		}
		px->exitcode = 1;
		exit (1);
	}
	execute_command(px, fd[0], px->output_fd, px->cmd2);
	if (px->cmd2)
		free_array(&px->cmd2);
	exit(px->exitcode);
}

// Function to wait for both child processes and set the exit code
void	wait_for_processes(t_var *px_var, int pid1, int pid2)
{
	int status;
	int completed = 0; // Count of completed processes

	// Loop until both processes have been reaped
	while (completed < 2)
	{
		int pid = waitpid(-1, &status, 0); // -1 waits for any child process

		if (pid == -1)
			break; // Error or no more children, exit loop

		if (WIFEXITED(status))
			px_var->exitcode = WEXITSTATUS(status); // Capture exit code

		// Increase completed count if the terminated process matches pid1 or pid2
		if (pid == pid1 || pid == pid2)
			completed++;
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
	int pid;

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

// Function opens a pipe, and with fork creates one child processes for commands
// after handling children processes closes file descriptors, waits for children to finish
// returns with exitcode set by handling files or commands
int	pipex(t_var *px, char **argv)
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
	clean_up(px);
	return (px->exitcode);
}
