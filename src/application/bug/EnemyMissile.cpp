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

#include "EnemyMissile.h"

#include "lib/util/game/GameManager.h"
#include "lib/util/game/Game.h"
#include "lib/util/game/entity/component/LinearMovementComponent.h"
#include "lib/util/game/entity/event/TranslationEvent.h"
#include "lib/util/game/entity/event/CollisionEvent.h"
#include "PlayerMissile.h"
#include "application/bug/Ship.h"
#include "lib/util/game/Graphics2D.h"
#include "lib/util/game/Scene.h"
#include "lib/util/game/entity/collider/Collider.h"
#include "lib/util/game/entity/collider/RectangleCollider.h"
#include "lib/util/math/Vector2D.h"
#include "application/bug/Explosive.h"
#include "lib/util/game/entity/Entity.h"

class Bug;

EnemyMissile::EnemyMissile(const Util::Math::Vector2D &position, Bug &bug) : Explosive(TAG, position, Util::Game::RectangleCollider(position, Util::Game::Collider::STATIC, SIZE_X, SIZE_Y)), bug(bug) {
    addComponent(new Util::Game::LinearMovementComponent(*this));
}

void EnemyMissile::initialize() {
    Explosive::initialize();
    sprite = Util::Game::Sprite("/initrd/bug/enemy_missile.bmp", SIZE_X, SIZE_Y);
}

void EnemyMissile::onUpdate(double delta) {
    Explosive::onUpdate(delta);

    if (hasExploded()) {
        Util::Game::GameManager::getCurrentScene().removeObject(this);
    }
}

void EnemyMissile::onTranslationEvent(Util::Game::TranslationEvent &event) {
    if (event.getTargetPosition().getY() < -1.0) {
        Util::Game::GameManager::getGame().getCurrentScene().removeObject(this);
    }

    if (isExploding()) {
        event.cancel();
    }
}

void EnemyMissile::onCollisionEvent(Util::Game::CollisionEvent &event) {
    if (isExploding()) {
        return;
    }

    auto tag = event.getCollidedWidth().getTag();
    if (tag == PlayerMissile::TAG || tag == Ship::TAG) {
        explode();
    }
}

void EnemyMissile::draw(Util::Game::Graphics2D &graphics) {
    if (isExploding()) {
        Explosive::draw(graphics);
    } else {
        graphics.drawImage(getPosition(), sprite.getImage());
    }
}
