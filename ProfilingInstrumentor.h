//
// Basic instrumentation profiler by Cherno
// Check readme
#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <thread>

// Function macro
#define PROFILING 1 // Enable or disable profiling
#if PROFILING
// Whrite function execution time with custom name
#define PROFILE_SCOPE(name) ProfilingInstrumentor::Timer timer##__LINE__(name);
// Whrite function execution time with _function_ name
#define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCTION__)
#else
#define PROFILE_FUNCTION()
#endif

namespace ProfilingInstrumentor
{
    /**
     *  @brief  Struct of execution time to store in file.
     */
    struct ProfileResult
    {
        std::string Name;
        long long Start, End;
        uint32_t ThreadID;
    };

    /**
     *  @brief  Struct to distinguish session
     */
    struct Session
    {
        std::string Name;
    };

    /**
     *  @brief  Main class to write profiling results
     */
    class Instrumentor
    {
    private:
        Session *m_CurrentSession;
        std::ofstream m_OutputStream;
        int m_ProfileCount;

    public:
        Instrumentor()
            : m_CurrentSession(nullptr), m_ProfileCount(0)
        {
        }

        /**
         *  @brief Initialize profiling session
         *  @param  name  The name of the session.
         *  @param  filepath  filepath to results file
         */
        void BeginSession(const std::string &name, const std::string &filepath = "results.json")
        {
            m_OutputStream.open(filepath);
            WriteHeader();
            m_CurrentSession = new Session{name};
        }

        /**
         *  @brief End session, write closing footer into file and close output stream
         */
        void EndSession()
        {
            WriteFooter();
            m_OutputStream.close();
            delete m_CurrentSession;
            m_CurrentSession = nullptr;
            m_ProfileCount = 0;
        }

        /**
         *  @brief Write ProfileResult into stream
         */
        void WriteProfile(const ProfileResult &result)
        {
            if (m_ProfileCount++ > 0)
                m_OutputStream << ",";

            std::string name = result.Name;
            std::replace(name.begin(), name.end(), '"', '\'');

            m_OutputStream << "{";
            m_OutputStream << "\"cat\":\"function\",";
            m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
            m_OutputStream << "\"name\":\"" << name << "\",";
            m_OutputStream << "\"ph\":\"X\",";
            m_OutputStream << "\"pid\":0,";
            m_OutputStream << "\"tid\":" << result.ThreadID << ",";
            m_OutputStream << "\"ts\":" << result.Start;
            m_OutputStream << "}";

            m_OutputStream.flush();
        }

        /**
         *  @brief Write profiling header that should be inside every json file in order to be interpreted by chrome
         */
        void WriteHeader()
        {
            m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
            m_OutputStream.flush();
        }

        /**
         *  @brief Write closing footer into file
         */
        void WriteFooter()
        {
            m_OutputStream << "]}";
            m_OutputStream.flush();
        }

        /**
         *  @brief Get singleton
         */
        static Instrumentor &Get()
        {
            static Instrumentor instance;
            return instance;
        }
    };

    /**
     *  @brief Class to start, store and stop execution time
     */
    class Timer
    {
    public:
        /**
         *  @brief Constructor. Also starts the timer
         */
        Timer(const char *name)
            : m_Name(name), m_Stopped(false)
        {
            m_StartTimepoint = std::chrono::high_resolution_clock::now();
        }

        /**
         *  @brief Destructor. Also stops the timer
         */
        ~Timer()
        {
            if (!m_Stopped)
                Stop();
        }

        /**
         *  @brief Stop the timer
         */
        void Stop()
        {
            auto endTimepoint = std::chrono::high_resolution_clock::now();

            long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
            long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

            uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
            Instrumentor::Get().WriteProfile({m_Name, start, end, threadID});

            m_Stopped = true;
        }

    private:
        const char *m_Name;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
        bool m_Stopped;
    };
}