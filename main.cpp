#include <iostream>
#include <string>
#include <chrono>

#include <cmath>

#include "ProfilingInstrumentor.h"

void printFunction()
{
    PROFILE_FUNCTION();

    for (int i = 0; i < 1000; i++)
    {
        std::cout << "Hello World #" << i << std::endl;
    }
    
}

void printFunction(int value)
{
    PROFILE_FUNCTION();

    for (int i = 0; i < 1000; i++)
    {
        std::cout << "Hello World #" << (i + value) << std::endl;
    }
    
}

void function2()
{
    PROFILE_FUNCTION();

    for (int i = 0; i < 1000; i++)
    {
        std::cout << "Hello World #" << sqrt(i) << std::endl;
    }
    
}

void runBenchmarks()
{
    PROFILE_FUNCTION();
    std::cout << "Running Benchmarks...\n";      
    std::thread a([]() {printFunction(); });
    std::thread b([]() {printFunction(3); });
    std::thread c([]() {function2(); });

    a.join();
    b.join();
    c.join();
}

void manualProfiling()
{
    ProfilingInstrumentor::Timer timer("ManualFunction1");
    for (int i = 0; i < 1000; i++)
    {
        std::cout << "Hello World #" << sqrt(i) << std::endl;
    }
    timer.stop();

    ProfilingInstrumentor::Timer timer2("ManualFunction2");
    for (int i = 0; i < 1500; i++)
    {
        std::cout << "Hello World #" << sqrt(i) << std::endl;
    }
    timer2.stop();
}

int main()
{
    ProfilingInstrumentor::Instrumentor::getInstance().beginSession("Profile");

    printFunction();
    runBenchmarks();
    function2();
    manualProfiling();

    ProfilingInstrumentor::Instrumentor::getInstance().endSession();
}