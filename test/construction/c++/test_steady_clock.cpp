#include "stdafx.h"
#include <catch.hpp>
#include <chrono>

//
// steady_clock is high_resolution_clock on windows 
// system_clock uses long long tick
// use system clock to implement simple_timer
// change current timer to steady_timer 
//
