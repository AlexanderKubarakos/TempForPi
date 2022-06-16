#include <mpi.h>
#include <iostream>
#include <vector>

void findPrimes(int start, int end, std::vector<int>& primes);

int main(int argc, char* argv[])
{
	std::cout << "Hello World!\n";
	MPI_Init(&argc, &argv);

	int process;
	int rank;
	const int range = 1000000;

	MPI_Comm_size(MPI_COMM_WORLD, &process);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	//std::cout << "Process size: " << process << " | My rank is: " << rank << "\n";

	int rangeStart = rank * (range/process);
	int rangeEnd = rank * (range/process) + (range/process);

	std::vector<int> primes;
	findPrimes(rangeStart, rangeEnd, primes);

	for (int prime : primes)
		std::cout << prime << '\n';

	MPI_Finalize();
	return 0;
}

void findPrimes(int start, int end, std::vector<int>& primes)
{
	for (int currentNumber = start; currentNumber <= end; currentNumber++)
	{
		bool notPrime = false;
		if (currentNumber % 2 == 0)
		{
			continue;
		}

		for (int i = 3; i < currentNumber/2 + 1 && !notPrime; i++)
		{
			if (currentNumber % i == 0)
				notPrime = true;
		}

		if (!notPrime)
		{
			primes.push_back(currentNumber);
		}
	}
}