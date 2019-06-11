/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#ifndef HHUOS_BMPVIEW_H
#define HHUOS_BMPVIEW_H

#include "lib/file/bmp/Bmp.h"
#include "Command.h"

/**
 * Implementation of Command.
 * Shows a BMP-file.
 *
 * -h, --help: Show help message
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class BmpView : public Command, Kernel::Receiver {

private:

    Bmp *bmp = nullptr;

    uint8_t initScalingDividor = 1;

    uint8_t scalingFactor = 1;
    uint8_t scalingDividor = 1;

    bool isRunning = true;

private:

    void scaleUp();

    void scaleDown();

    void drawBitmap();

public:
    /**
     * Default-constructor.
     */
    BmpView() = delete;

    /**
     * Copy-constructor.
     */
    BmpView(const BmpView &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit BmpView(Shell &shell);

    /**
     * Destructor.
     */
    ~BmpView() override = default;

    /**
     * Overriding function from Command.
     */
    void execute(Util::Array<String> &args) override;

    /**
     * Overriding function from Command.
     */
    const String getHelpText() override;

    /**
     * Overriding function from Receiver.
     */
    void onEvent(const Kernel::Event &event) override;
};

#endif
