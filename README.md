This is a simple version of the Pipex project from 42 curriculum
It recreates the basic functionalities of bash shell - execution of 2 commands and redirecting input from one file to another with the use of a pipe
In order to give it a try ```make``` an executable and run it with four parameters: ```./pipex infile cmd1 cmd2 outfile```
It will mimic the behaviour of bash command given as follows: ```< infile cmd1 | cmd2 > outfile```