#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <memory>
#include "Process.hpp"

int main(int argc, char *argv[])
{
    using namespace std;
    std::vector<char*> args;

    for(int nn=1; nn<argc; ++nn)
    {
	std::cerr << "Arg: " << argv[nn] << std::endl;
	args.push_back(argv[nn]);
    }
    args.push_back( NULL );

    
    string line;
    string output;

    {
	Process myproc(args,true);
	//sleep(5);
	/* write/read test */
	/*
	for(int n=0; n<5; ++n) {
	    stringstream ss(line);
	    ss << "1+" << n << endl;
	    line = ss.str();
	    cerr << "calling write with line=" << line << "END" << endl;
	    myproc.write(line);
	    output = myproc.read();
	    cerr << "output from process: " << output << "END" << endl;
	}
	*/
	sleep(5);
	cerr << "Process object destroyed" << endl;
    }
    cerr << "Program exiting in 5 seconds.  Confirm child process has already been cleaned up." << endl;
    sleep(5);
    return(EXIT_SUCCESS);
}
