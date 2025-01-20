#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <iomanip>

#define BUFFER_SIZE 16

std::string print_buf(const char *b, int number);

int main(){
	std::cout	<< "\n------------------------------------------"
			<< "\n Test program for character driver drvchr " 
			<< "\n------------------------------------------" << std::endl;

	const char* path = "/dev/devchr";

	/*Write to file*/
	std::ofstream ofile;
	ofile.open(path, std::ios::binary);

	if(!ofile.is_open()){
		std::cerr << "Fail open " << path << ":" << strerror(errno) << std::endl;
		return errno;
	}

	char bw[BUFFER_SIZE];
	const char *pbw = &bw[0];

	std::srand(std::time(nullptr));
	for(int i = 0; i < sizeof(bw); i++){
		bw[i] = std::rand()%256;
	}

	std::cout << "WR: " << print_buf(pbw, sizeof(bw)) << std::endl;

	ofile.write(pbw, sizeof(bw));
	ofile.close();

	/*Read from file*/
	std::ifstream ifile;
	ifile.open (path, std::ios::binary);

	if(!ifile.is_open()){
		std::cerr << "Fail open " << path << ":" << strerror(errno) << std::endl;
		return errno;
	}

	//std::cout << "File " << path << " is open" << std::endl;

	char br[BUFFER_SIZE];
	const char *pbr = &br[0];

	ifile.read(br, sizeof(br));

	std::cout << "RD: " << print_buf(pbr, sizeof(br)) << std::endl;

	ifile.close();

	return 0;
}

std::string print_buf(const char* b, int number){
	std::stringstream ss;
	for(int i = 0; i < number; i++){
		ss	<< std::hex
			<< std::uppercase
			<< std::setfill('0')
			<< std::setw(2)
			<< (0xFF & *(b+i)) << " ";
	}
	return ss.str();
}
