#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "CL/cl2.hpp"

typedef int T;

const int n = 8;
const int DATA_SIZE = n;
const int WORK_GROUP_SIZE = 1;

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

	cl::Context context { defaultDevice };

	std::ifstream infile { "Source/BitonicSOrt.cl" };
	
	std::stringstream ss;
	ss << infile.rdbuf ();
	std::string kernelCode { ss.str () };

	//	Load kernel code
	cl::Program::Sources mainSource { kernelCode }; 
	cl::Program program { context, mainSource };

	if (program.build ({ defaultDevice }) != CL_SUCCESS) {
		std::cerr << "Error building program!" << std::endl;
		throw ERR_BUILD;
	}

	cl::CommandQueue queue { context, defaultDevice };
	cl::Kernel bitonicSort { program, "bitonicSort" };

	/* REAL GANGSTA CODE STARTS HERE */

	T data [n];
	T temp;
	for (int i = 0; i < n; ++i) {
		std::cin >> data[i];
	}

	cl::Buffer dataBuffer { context, CL_MEM_READ_WRITE, sizeof (int) * n };
	queue.enqueueWriteBuffer (dataBuffer, CL_TRUE, 0, sizeof (int) * n, data);

	bitonicSort.setArg (0, &data);
	
	//	LAUNCH THIS THING

	//!testing start
	int globalSize = DATA_SIZE >> 1, localSize = 1;
	int stagesSize = 0;
	for (int i = n; i > 1; i >>= 1) { ++stagesSize; }

	for (int stage = 0; stage < stagesSize; ++stage) {
    // stage of the algorithm
	bitonicSort.setArg (1, stage);
    // Every stage has stage + 1 passes
    for (int pass = 0; pass < stage + 1; ++pass) {
		bitonicSort.setArg (2, pass);
        //
        // Enqueue a kernel run call.
        // Each thread writes a sorted pair.
        // So, the number of threads (global) should be half the length of the input buffer.
        //
		queue.enqueueNDRangeKernel (bitonicSort, cl::NullRange, cl::NDRange (globalSize), cl::NDRange (localSize));
		queue.finish ();
        }
    }
	queue.enqueueMapBuffer (dataBuffer, true, CL_MAP_READ, 0, sizeof (T) * DATA_SIZE);
	//!testing end

	queue.enqueueNDRangeKernel (bitonicSort, cl::NullRange, cl::NDRange (globalSize), cl::NDRange (localSize));

	//	OK GO BACK
	queue.enqueueReadBuffer (dataBuffer, CL_TRUE, 0, sizeof (int) * n, data);
	queue.finish ();

	for (int i = 0; i < n; ++i) {
		std::cout << data[i] << ' ';
	}
	return 0;
}
