cmake_minimum_required(VERSION 3.17)
project(Advance_progream_4)

set(CMAKE_CXX_STANDARD 11)
set(GCC_COVERAGE_COMPILE_FLAGS "-pthread -std=c++11")

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

add_executable(Advance_progream_4  MainTrain.cpp minCircle.h minCircle.cpp HybridAnomalyDetector.cpp HybridAnomalyDetector.h
        CLI.cpp CLI.h SimpleAnomalyDetector.h SimpleAnomalyDetector.cpp Server.cpp Server.h timeseries.cpp timeseries.h
        anomaly_detection_util.h anomaly_detection_util.cpp commands.h)