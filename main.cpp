#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "CL/cl.hpp"

const int n = 1024;

enum Errors {
	ERR_BUILD
};

int main () {

	std::vector <cl::Platform> platforms {};
	cl::Platform::get (&platforms);
	std::cerr << "Platforms size: " << platforms.size () << std::endl;
	cl::Platform defaultPlatform { platforms[1] };

	std::vector <cl::Device> devices {};
	defaultPlatform.getDevices (CL_DEVICE_TYPE_ALL, &devices); // Getting the list of all devices
	std::cerr << "Devices size: " << devices.size () << std::endl;
	cl::Device defaultDevice { devices[0] };

	std::cout << "Name: " << devices[0].getInfo <CL_DEVICE_NAME> () << std::endl;
	std::cout << "Threads: " << devices[0].getInfo <CL_DEVICE_MAX_WORK_GROUP_SIZE> () << std::endl;

	cl::Context mainContext { defaultDevice };

	std::ifstream infile { "ADD_OPERATOR.cl" };	//	ADD operator for vectors
	
	std::stringstream ss;
	ss << infile.rdbuf ();
	std::string kernelCode { ss.str () };

	//	Load kernel code
	cl::Program::Sources mainSource { 1, std::make_pair (kernelCode.c_str (), kernelCode.size ()) }; 
	cl::Program program { mainContext, mainSource };

	if (program.build ({ defaultDevice }) != CL_SUCCESS) {
		std::cerr << "Error building program!" << std::endl;
		return ERR_BUILD;
	}

	cl::CommandQueue mainQueue { mainContext, defaultDevice };
	cl::Kernel functionAdd { program, "functionAdd" };

	/* REAL GANGSTA CODE STARTS HERE */

	int A[n], B[n], C[n];
	for (int i = 0; i < n; ++i) {
		A[i] = i;
		B[i] = i;
		C[i] = 777;
	}

	cl::Buffer bufferA { mainContext, CL_MEM_READ_ONLY, sizeof (int) * n };
	mainQueue.enqueueWriteBuffer (bufferA, CL_TRUE, 0, sizeof (int) * n, A);

	cl::Buffer bufferB { mainContext, CL_MEM_READ_ONLY, sizeof (int) * n };
	mainQueue.enqueueWriteBuffer (bufferB, CL_TRUE, 0, sizeof (int) * n, B);

	cl::Buffer bufferC { mainContext, CL_MEM_WRITE_ONLY, sizeof (int) * n };

	functionAdd.setArg (0, bufferA);
	functionAdd.setArg (1, bufferB);
	functionAdd.setArg (2, bufferC);
	
	//	LAUNCH THIS THING
	mainQueue.enqueueNDRangeKernel (functionAdd, cl::NullRange, cl::NDRange (n), cl::NDRange (1));

	//	OK GO BACK
	mainQueue.enqueueReadBuffer (bufferC, CL_TRUE, 0, sizeof (int) * n, C);
	mainQueue.finish ();

	for (int i = 0; i < n; ++i) {
		std::cout << "C[" << i << "] = " << C[i] << std::endl;
	}

	return 0;
}
