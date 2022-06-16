#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <array>
#include <complex>
#include <string>

int main(int argc, char* argv[])
{
	srand(time(0));
	MPI_Init(&argc, &argv);

	int process;
	int rank;
	const int width = 6000;
	const int height = 6000;
	const int iterations = 256;

	auto red = new int[width][height];
	auto green = new int[width][height];
	auto blue = new int[width][height];

	MPI_Comm_size(MPI_COMM_WORLD, &process);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int startWidth = rank * (width/process);
	int endWidth = rank * (width/process) + (width/process);

	//std::cout << "Process size: " << process << " | My rank is: " << rank << "\n";

	// Actually proccess madlebrot

	std::cout << "Generating fractal...\n";
	for (int h = 0; h < height; h++)
	{
		//std::cout << "Percent Complete: " << (float)h/(float)height * 100 <<'\n';
		for (int w = startWidth; w < endWidth; w++)
		{
			float xOfFractal = (0.47 - (-2.0)) * (w - 0) / (width - 0) + (-2.0);
			float yOfFractal = (1.12 - (-1.12)) * (h - 0) / (height - 0) + (-1.12);
			std::complex<double> c(xOfFractal, yOfFractal);
			std::complex<double> z(0, 0);

			int n = 0;
			while (abs(z) < 2.0 && n < iterations)
			{
				z = (z * z) + c;
				n++;
			}

			float v = (float)n;
			red[w][h] = (0.5f * sin(0.1f * v) + 0.5f)* 255;
			green[w][h] = (0.5f * sin(0.1f * v + 2.094f) + 0.5f)* 255;
			blue[w][h] = (0.5f * sin(0.1f * v + 4.188f) + 0.5f)* 255;
		}
	}



	// Fill image
	std::ofstream picture;
	std::string file = "Fractal";
	file += std::to_string(rank);
	file+=".ppm";
	std::cout << file << '\n';
	picture.open(file, std::ios::out | std::ios::trunc);

	if (picture.is_open())
	{
		std::cout << "Writing to image...\n";
		picture << "P3 " << endWidth-startWidth << " " << height << " 255\n";
		for (int h = 0; h < height; h++)
		{
			//std::cout << "Percent Complete: " << (float)h/(float)height * 100 <<'\n';
			for (int w = startWidth; w < endWidth; w++)
			{
				picture << " " << red[w][h] <<
				" " << green[w][h] <<
				" " << blue[w][h];
			}
			picture << '\n';
		}
		picture << '\n';
	}
	else
	{
		std::cout << "Could not open Fractal" << rank << ".ppm";
	}
	picture.close();
	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0)
	{
		std::cout << "All done!\n";
		std::vector<std::ifstream> files;
		picture.open("FinalFractal.ppm", std::ios::out | std::ios::trunc);
		picture << "P3 " << width << " " << height << " 255\n";
		std::string line;
		for (int i = 0; i < process; i++)
		{
			files.emplace_back();

			file = "";
			file += "Fractal";
			file += std::to_string(i);
			file+=".ppm";
			files[i].open(file, std::ios::in);
			std::getline(files[i], line);
			// take width of each image and do it row by row for each rank
		}
		
		for (int h = 0; h < height; h++)
		{
			for (int i = 0; i < process; i++)
			{
				//picture << "255 255 255";
				std::getline(files[i], line);
				picture << line << " ";
			}
		}
		picture << "\n";
	}

	MPI_Finalize();
	return 0;
}

