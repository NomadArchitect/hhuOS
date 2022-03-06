/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include <cstdint>
#include "lib/util/system/System.h"
#include "lib/util/time/Date.h"

int32_t main(int32_t argc, char *argv[]) {
    auto date = Util::Time::getCurrentDate();
    Util::System::out << Util::Memory::String::format("%u-%02u-%02u %02u:%02u:%02u",
                                                      date.getYear(), date.getMonth(), date.getDayOfMonth(),
                                                      date.getHours(), date.getMinutes(), date.getSeconds()) << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
    return 0;
}