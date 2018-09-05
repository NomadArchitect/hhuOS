#include <kernel/Kernel.h>
#include "SoundBlaster.h"

Logger &SoundBlaster::log = Logger::get("SOUNDBLASTER");

bool SoundBlaster::checkPort(uint16_t baseAddress) {
    SoundBlaster tmp(baseAddress);

    return tmp.reset();
}

uint16_t SoundBlaster::getBasePort() {
    for(uint16_t i = 0x210; i < 0x290; i += 0x10) {
        if(checkPort(i)) {
            return i;
        }
    }

    return 0;
}

bool SoundBlaster::isAvailable() {
    return getBasePort() != 0;
}

SoundBlaster::SoundBlaster(uint16_t baseAddress) :
        resetPort(static_cast<uint16_t>(baseAddress + 0x06)),
        readDataPort(static_cast<uint16_t>(baseAddress + 0x0a)),
        writeDataPort(static_cast<uint16_t>(baseAddress + 0x0c)),
        readBufferStatusPort(static_cast<uint16_t>(baseAddress + 0x0e)),
        timeService(Kernel::getService<TimeService>()) {

}

SoundBlaster::SoundBlaster() : baseAddress(getBasePort()),
        resetPort(static_cast<uint16_t>(baseAddress + 0x06)),
        readDataPort(static_cast<uint16_t>(baseAddress + 0x0a)),
        writeDataPort(static_cast<uint16_t>(baseAddress + 0x0c)),
        readBufferStatusPort(static_cast<uint16_t>(baseAddress + 0x0e)),
        timeService(Kernel::getService<TimeService>()) {
    log.info("Found base port at address 0x%04x", baseAddress);

    // Reset card
    log.info("Resetting card...");
    bool ret = reset();
    log.info(ret ? "Successfully resetted card" : "Unable to reset card");

    // Get DSP Version
    writeToDSP(0xe1);
    majorVersion = readFromDSP();
    minorVersion = readFromDSP();

    log.info("Major version: 0x%02x, Minor version: 0x%02x", majorVersion, minorVersion);
}

uint8_t SoundBlaster::readFromDSP() {
    while((readBufferStatusPort.inb() & 0x80) != 0x80);

    return readDataPort.inb();
}

void SoundBlaster::writeToDSP(uint8_t value) {
    while((readBufferStatusPort.inb() & 0x80) == 0x80);

    writeDataPort.outb(value);
}

void SoundBlaster::writeToDAC(uint8_t value) {
    writeToDSP(0x10);
    writeToDSP(value);
}

bool SoundBlaster::reset() {
    // Issue reset command
    resetPort.outb(0x01);
    timeService->msleep(5);
    resetPort.outb(0x00);

    bool timeout = true;
    uint32_t timeoutTime = timeService->getSystemTime() + RESET_TIMEOUT;

    // Wait for read buffer to become ready
    do {
        uint8_t status = readBufferStatusPort.inb();

        if((status & 0x80) == 0x80) {
            timeout = false;
            break;
        }
    } while(timeService->getSystemTime() < timeoutTime);

    if(timeout) {
        return false;
    }

    timeoutTime = timeService->getSystemTime() + RESET_TIMEOUT;

    // Wait ready code (represented by 0xaa) to appear in the read buffer
    do {
        uint8_t status = readDataPort.inb();

        if(status == 0xaa) {
            return true;
        }
    } while(timeService->getSystemTime() < timeoutTime);

    return false;
}

String SoundBlaster::getVendorName() {
    return VENDOR_NAME;
}

String SoundBlaster::getDeviceName() {
    if(majorVersion < 0x02) {
        return SOUND_BLASTER_1;
    }

    if(majorVersion < 0x03) {
        return SOUND_BLASTER_2;
    }

    if(majorVersion < 0x04) {
        return SOUND_BLASTER_PRO;
    }

    if(majorVersion == 0x04 && minorVersion < 0x0c) {
        return SOUND_BLASTER_16;
    }

    return SOUND_BLASTER_AWE32;
}

void SoundBlaster::setup() {

}

void SoundBlaster::turnSpeakerOn() {
    writeToDSP(0xd1);
}

void SoundBlaster::turnSpeakerOff() {
    writeToDSP(0xd3);
}

void SoundBlaster::playSample(uint8_t sample) {
    writeToDAC(sample);
}

void SoundBlaster::playSample(uint16_t sample) {
    playSample(static_cast<uint8_t>((sample / 65535.0) * 255));
}
