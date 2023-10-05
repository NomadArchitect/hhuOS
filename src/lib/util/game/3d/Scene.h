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
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#ifndef HHUOS_SCENE_3D_H
#define HHUOS_SCENE_3D_H

#include "lib/util/game/Scene.h"

namespace Util {
namespace Game {
class Graphics;
}  // namespace Game
}  // namespace Util

namespace Util::Game::D3 {

class Scene : public Util::Game::Scene {

public:
    /**
     * Default Constructor.
     */
    Scene() = default;

    /**
     * Copy Constructor.
     */
    Scene(const Scene &other) = delete;

    /**
     * Assignment operator.
     */
    Scene &operator=(const Scene &other) = delete;

    /**
     * Destructor.
     */
    ~Scene() override = default;

    void initialize(Graphics &graphics) override;

    void updateEntities(double delta) override;

    void checkCollisions() override;
};

}

#endif
