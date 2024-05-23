#include <iostream>
#include <string>
#include <chrono>

#include <cmath>

#include "ProfilingInstrumentor.h"

void PrintFunction()
{
    PROFILE_FUNCTION();

    for (int i = 0; i < 1000; i++)
    {
        std::cout << "Hello World #" << i << std::endl;
    }
    
}

void PrintFunction(int value)
{
    PROFILE_FUNCTION();

    for (int i = 0; i < 1000; i++)
    {
        std::cout << "Hello World #" << (i + value) << std::endl;
    }
    
}

void Function2()
{
    PROFILE_FUNCTION();

    for (int i = 0; i < 1000; i++)
    {
        std::cout << "Hello World #" << sqrt(i) << std::endl;
    }
    
}

void RunBenchmarks()
{
    PROFILE_FUNCTION();
    std::cout << "Running Benchmarks...\n";      
    std::thread a([]() {PrintFunction(); });
    std::thread b([]() {PrintFunction(3); });
    std::thread c([]() {Function2(); });

    a.join();
    b.join();
    c.join();
}

int main()
{
    ProfilingInstrumentor::Instrumentor::Get().BeginSession("Profile");

    PrintFunction();
    RunBenchmarks();
    Function2();

    ProfilingInstrumentor::Instrumentor::Get().EndSession();
}