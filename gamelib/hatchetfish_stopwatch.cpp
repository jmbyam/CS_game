// SSPHH/Fluxions/Unicornfish/Viperfish/Hatchetfish/Sunfish/Damselfish/GLUT Extensions
// Copyright (C) 2017-2019 Jonathan Metzgar
// All rights reserved.
//
// This program is free software : you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.If not, see <https://www.gnu.org/licenses/>.
//
// For any other type of licensing, please contact me at jmetzgar@outlook.com
#include "pch.h"
#include <hatchetfish_stopwatch.hpp>

namespace Hf {
    StopWatch::StopWatch() { start_timepoint = std::chrono::system_clock::now(); }

    StopWatch::~StopWatch() {}

    void StopWatch::start() { start_timepoint = std::chrono::system_clock::now(); }

    void StopWatch::stop() { end_timepoint = std::chrono::system_clock::now(); }

    double StopWatch::getSecondsElapsed() {
        auto diff = end_timepoint - start_timepoint;
        return std::chrono::duration<double>(diff).count();
    }

    double StopWatch::getMillisecondsElapsed() {
        auto diff = end_timepoint - start_timepoint;
        return std::chrono::duration<double, std::milli>(diff).count();
    }
}
