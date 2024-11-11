/****************************************************************************
 *
 *   @copyright Copyright (c) 2024 Dave Hylands     <dhylands@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the MIT License version as described in the
 *   LICENSE file in the root of this repository.
 *
 ****************************************************************************/
/**
 *   @file   LittleFsPacketHandler.cpp
 *
 *   @brief  Packet handle for LittleFs packets.
 *
 ****************************************************************************/

#include "LittleFS.h"
#include "LittleFsPacketHandler.h"
#include "Packer.h"
#include "Unpacker.h"

char const* LittleFsPacketHandler::as_str(Packet::Command::Type cmd) const {
    switch (cmd) {
        case Command::FORMAT:
            return "FORMAT";
        case Command::INFO:
            return "INFO";
        case Command::LIST:
            return "LIST";
        case Command::MKDIR:
            return "MKDIR";
        case Command::REMOVE:
            return "REMOVE";
        case Command::RENAME:
            return "RENAME";
        case Command::COPY:
            return "COPY";
        case Command::READ:
            return "READ";
        case Command::WRITE:
            return "WRITE";
        case Command::APPEND:
            return "APPEND";
        case Command::RMDIR:
            return "RMDIR";
    }
    return "???";
}

bool LittleFsPacketHandler::handlePacket(Packet const& cmd, Packet* rsp) {
    switch (cmd.getCommand()) {
        case Command::COPY: {
            this->handleCopy(cmd, rsp);
            return true;
        }
        case Command::FORMAT: {
            this->handleFormat(cmd, rsp);
            return true;
        }
        case Command::INFO: {
            this->handleInfo(cmd, rsp);
            return true;
        }
        case Command::LIST: {
            this->handleList(cmd, rsp);
            return true;
        }
        case Command::MKDIR: {
            this->handleMkDir(cmd, rsp);
            return true;
        }
        case Command::REMOVE: {
            this->handleRemove(cmd, rsp);
            return true;
        }
        case Command::RENAME: {
            this->handleRename(cmd, rsp);
            return true;
        }
        case Command::READ: {
            this->handleRead(cmd, rsp);
            return true;
        }
        case Command::WRITE: {
            this->handleWriteAppend(FILE_WRITE, cmd, rsp);
            return true;
        }
        case Command::APPEND: {
            this->handleWriteAppend(FILE_APPEND, cmd, rsp);
            return true;
        }
        case Command::RMDIR: {
            this->handleRmDir(cmd, rsp);
            return true;
        }
    }
    return false;
}

void LittleFsPacketHandler::handleCopy(Packet const& cmd, Packet* rsp) {
    rsp->setCommand(Command::COPY);
}

void LittleFsPacketHandler::handleFormat(Packet const& cmd, Packet* rsp) {
    // Command: No Data
    // Response:
    //      u8 - Error Code
    rsp->setCommand(Command::FORMAT);

    if (LittleFS.format()) {
        rsp->appendByte(to_underlying(Error::NONE));
    } else {
        rsp->appendByte(to_underlying(Error::FORMAT_FAILED));
    }
}

void LittleFsPacketHandler::handleInfo(Packet const& cmd, Packet* rsp) {
    // Command: No Data
    // Response:
    //      u32 - totalBytes
    //      u32 - usedBytes

    rsp->setCommand(Command::INFO);

    InfoResponse info = {};
    info.totalBytes = LittleFS.totalBytes();
    info.usedBytes = LittleFS.usedBytes();

    rsp->appendData(sizeof(info), &info);
}

void LittleFsPacketHandler::handleList(Packet const& cmd, Packet* rsp) {
    // Command:
    //      u16 - index
    //      str - dirname
    // Response:
    //  Variable number of entries
    //      u16 - index
    //      u8  - flags
    //      u32 - filesize
    //      u32 - timestamp
    //      str - filename
    Unpacker unpacker(cmd);
    uint16_t index;
    char const* dirName;

    unpacker.unpack(&index);
    unpacker.unpack(&dirName);

    uint16_t fileNum = 0;
    File dir = LittleFS.open(dirName);
    File file = dir.openNextFile();
    while (fileNum < index) {
        fileNum++;
        file = dir.openNextFile();
    }

    rsp->setCommand(Command::LIST);
    while (file) {
        Flags flags;
        if (file.isDirectory()) {
            flags.set(Flags::DIR);
        }
        uint32_t fileSize = file.size();
        uint32_t timestamp = file.getLastWrite();
        char const* filename = file.name();

        uint32_t entrySize = sizeof(fileNum) + sizeof(flags) + sizeof(fileSize) +
                             sizeof(timestamp) + strlen(filename) + 1;
        if (entrySize > rsp->getSpaceRemaining()) {
            break;
        }

        rsp->append(fileNum);
        rsp->append(flags);
        rsp->append(fileSize);
        rsp->append(timestamp);
        rsp->append(filename);
        fileNum++;
        file = dir.openNextFile();
    }
}

void LittleFsPacketHandler::handleMkDir(Packet const& cmd, Packet* rsp) {
    // Command:
    //      str - dirname
    // Response:
    //      u8 - error code.
    rsp->setCommand(Command::MKDIR);
    Unpacker unpacker(cmd);
    char const* dirName;
    unpacker.unpack(&dirName);

    if (LittleFS.mkdir(dirName)) {
        rsp->appendByte(to_underlying(Error::NONE));
    } else {
        rsp->appendByte(to_underlying(Error::MKDIR_FAILED));
    }
}

void LittleFsPacketHandler::handleRemove(Packet const& cmd, Packet* rsp) {
    // Command:
    //      str - filename
    // Response:
    //      u8 - error code.
    rsp->setCommand(Command::REMOVE);
    Unpacker unpacker(cmd);
    char const* fileName;
    unpacker.unpack(&fileName);

    if (LittleFS.remove(fileName)) {
        rsp->appendByte(to_underlying(Error::NONE));
    } else {
        rsp->appendByte(to_underlying(Error::REMOVE_FAILED));
    }
}

void LittleFsPacketHandler::handleRename(Packet const& cmd, Packet* rsp) {
    rsp->setCommand(Command::RENAME);
}

void LittleFsPacketHandler::handleRead(Packet const& cmd, Packet* rsp) {
    Unpacker unpacker(cmd);
    char const* filename;
    uint32_t offset;
    uint32_t length;

    unpacker.unpack(&filename);
    unpacker.unpack(&offset);
    unpacker.unpack(&length);

    rsp->setCommand(Command::READ);
    uint8_t* errPtr = rsp->getWriteData();
    rsp->append(to_underlying(Error::NONE));
    rsp->append(offset);
    uint32_t* lenPtr = reinterpret_cast<uint32_t*>(rsp->getWriteData());
    rsp->append(length);

    if (length > rsp->getSpaceRemaining()) {
        *errPtr = to_underlying(Error::READ_FAILED);
        *lenPtr = 0;
        return;
    }

    File file = LittleFS.open(filename, FILE_READ);
    if (!file) {
        *errPtr = to_underlying(Error::UNABLE_TO_OPEN_FILE);
        return;
    }
    if (!file.seek(offset)) {
        *errPtr = to_underlying(Error::SEEK_FAILED);
        return;
    }

    *lenPtr = file.read(rsp->getWriteData(0), length);
    (void)rsp->getWriteData(*lenPtr);

    file.close();
    *errPtr = to_underlying(Error::NONE);
}

void LittleFsPacketHandler::handleRmDir(Packet const& cmd, Packet* rsp) {
    // Command:
    //      str - dirname
    // Response:
    //      u8 - error code.
    rsp->setCommand(Command::RMDIR);
    Unpacker unpacker(cmd);
    char const* dirName;
    unpacker.unpack(&dirName);

    if (LittleFS.rmdir(dirName)) {
        rsp->appendByte(to_underlying(Error::NONE));
    } else {
        rsp->appendByte(to_underlying(Error::RMDIR_FAILED));
    }
}

void LittleFsPacketHandler::handleWriteAppend(char const* mode, Packet const& cmd, Packet* rsp) {
    // Command:
    //      str - filename
    //      u32 - data length
    //      bytes - data
    // Response:
    //      u8 - Error code
    Unpacker unpacker(cmd);
    char const* filename;
    uint32_t length;
    uint8_t const* data;

    unpacker.unpack(&filename);
    unpacker.unpack(&length);
    unpacker.unpack(length, &data);

    rsp->setCommand(cmd.getCommand());

    File file = LittleFS.open(filename, mode);
    if (!file) {
        rsp->appendByte(to_underlying(Error::UNABLE_TO_OPEN_FILE));
        return;
    }
    if (file.write(data, length) != length) {
        rsp->appendByte(to_underlying(Error::WRITE_FAILED));
        return;
    }
    file.close();
    rsp->appendByte(to_underlying(Error::NONE));
}
