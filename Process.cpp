#include "Process.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

Process::Process(const std::vector<char*>& args, bool verbose) : 
    verbose(verbose), 
    m_name(args[0]),
    m_pid((pid_t)NULL),
    m_writepipe {-1,-1},
    m_readpipe {-1,-1},
    m_pwrite((FILE*)NULL),
    m_pread((FILE*)NULL)
{
    if(pipe(m_writepipe) < 0)
    {
	perror("pipe write");
	throw std::string("pipe write");
    }

    if(pipe(m_readpipe) < 0)
    {
	perror("pipe read");
	throw std::string("pipe read");
    }
    
    if ((m_pid = fork()) < 0)
    {
	perror("process fork");
	throw std::string("process pork");
    } 

    else if (m_pid == 0) //child process
    {
	if(close(PARENT_WRITE) < 0)
	{
		perror("child process close parent write");
		throw std::string("child process close parent write");
	}
	
	if(close(PARENT_READ) < 0)
	{
		perror("child process close parent read");
		throw std::string("child process close parent read");
	}

	if((dup2(CHILD_WRITE, 1)) < 0)
	{
		perror("child process dup write");
		throw std::string("child process dup write");
	}
	
	if((dup2(CHILD_READ, 0)) < 0)
	{
		perror("child process dup read");
		throw std::string("child process dup read");
	}

	if(close(CHILD_READ) < 0)
	{
		perror("child process close child read");
		throw std::string("child process close child read");
	}

	if(close(CHILD_WRITE) < 0)
	{
		perror("child process close child write");
		throw std::string("child process close child write");
	}

	execvp(args[0], const_cast<char**>(&args[0]));
	perror("Process execvp");
	throw std::string("Process execvp");
    } 
    
    else //parent process
    { 
	if (verbose)
	    std::cerr << "Process " << m_name << ": forked PID " << m_pid << std::endl;
	
	if(close(CHILD_READ) < 0)
	{
		perror("parent process close child read");
		throw std::string("parent process close child read");
	}

	if(close(CHILD_WRITE) < 0)
	{
		perror("parent process close child write");
		throw std::string("parent process close child write");
	}
	
	m_pread = fdopen(PARENT_READ, "r");
	m_pwrite = fdopen(PARENT_WRITE, "w");
     }
}

Process::~Process()
{
    if (verbose)
	std::cerr << "Process " << m_name << ": Entering ~Process()" << std::endl;
    
    fclose(m_pwrite);
    fclose(m_pread);
    kill(m_pid, SIGTERM);
    int status;
    pid_t pid = waitpid(m_pid, &status, 0);

    if (pid < 0)
	perror("~Process waitpid");

    if (verbose)
	std::cerr << "Process " << m_name << ": Leaving ~Process()" << std::endl;
}

void Process::write(const std::string& line)
{
	if(fputs(line.c_str(), m_pwrite) < 0);
	{
		perror("process fputs fail");
		throw std::string("process fputs fail");
	}

	if(fflush(m_pwrite) < 0)
	{
		perror("process fflush fail");
		throw std::string("process fflush fail");
	}
}

std::string Process::read()
{
	std::string line;
	char* strng = NULL;
	size_t num_bytes;

	getline(&strng, &num_bytes, m_pread);
	line = strng;

	return line;
}
