/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_DEMOSPRITEFACTORY_H
#define HHUOS_DEMOSPRITEFACTORY_H

#include "lib/util/math/Random.h"

class DemoSprite;

class DemoSpriteFactory {

public:
    /**
     * Default Constructor.
     */
    DemoSpriteFactory() = default;

    /**
     * Copy Constructor.
     */
    DemoSpriteFactory(const DemoSpriteFactory &other) = delete;

    /**
     * Assignment operator.
     */
    DemoSpriteFactory &operator=(const DemoSpriteFactory &other) = delete;

    /**
     * Destructor.
     */
    ~DemoSpriteFactory() = default;

    DemoSprite* createSprite();

private:

    Util::Math::Random random = Util::Math::Random();
};

#endif
