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
 *
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/risch114/bachelorarbeit
 */

#ifndef HHUOS_GRAVITYCOMPONENT_H
#define HHUOS_GRAVITYCOMPONENT_H

#include "Component.h"

namespace Util {
namespace Game {
namespace D2 {
class Entity;
}  // namespace D2
}  // namespace Game
}  // namespace Util

namespace Util::Game::D2 {

class GravityComponent : public Component {

public:
    /**
    * Constructor.
    */
    explicit GravityComponent(Entity &entity, double mass = 2, double stopFactorX = 0.15, double gravityValue = -1.25);

    /**
     * Copy Constructor.
     */
    GravityComponent(const GravityComponent &other) = delete;

    /**
     * Assignment operator.
     */
    GravityComponent &operator=(const GravityComponent &other) = delete;

    /**
     * Destructor.
     */
    ~GravityComponent() = default;

protected:

    void update(double delta) override;

private:

    double mass;
    double stopFactorX;
    double gravityValue;
};

}

#endif
