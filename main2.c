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

int     main(int argc, char **argv, char **envp)
{
	t_var   px;

	if (argc != 5)
	{
		write(2, "Params should be: file1 cmd1 cmd2 file2\n", 40);
		exit(EXIT_FAILURE);
	}
	check_fd(argv, &px);
	init_variables(argv, envp, &px);
	pipex(&px, argv);
	if (px.cmd1)
		free_array(&px.cmd1);
	if (px.cmd2)
		free_array(&px.cmd2);
	return (px.exitcode);
}

