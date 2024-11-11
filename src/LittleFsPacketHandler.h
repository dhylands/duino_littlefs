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
 *   @file   LittleFsPacketHandler.h
 *
 *   @brief  Packet handle for LittleFs packets.
 *
 ****************************************************************************/

#pragma once

#include <cinttypes>

#include "PacketHandler.h"

//! Packet handler for dealing with core commands.
class LittleFsPacketHandler : public IPacketHandler {
 public:
    //! Commands accepted by the Core packet handler.
    struct Command : public Packet::Command {
        static constexpr Type FORMAT = 0x40;  //!< Format a file system.
        static constexpr Type INFO = 0x41;    //!< Return info about a file system.
        static constexpr Type LIST = 0x42;    //!< List files in a directory
        static constexpr Type MKDIR = 0x43;   //!< Create a new directory.
        static constexpr Type REMOVE = 0x44;  //!< Remove a file or directory.
        static constexpr Type RENAME = 0x45;  //!< Rename a file or directory.
        static constexpr Type COPY = 0x46;    //!< Copy a file
        static constexpr Type READ = 0x47;    //!< Read data from a file.
        static constexpr Type WRITE = 0x48;   //!< Write data to a file.
        static constexpr Type APPEND = 0x49;  //!< Append data to a file.
        static constexpr Type RMDIR = 0x4a;   //!< Remove a directory.
    };

    enum class Error : uint8_t {
        NONE = 0,
        UNABLE_TO_OPEN_FILE = 1,
        WRITE_FAILED = 2,
        READ_FAILED = 3,
        SEEK_FAILED = 4,
        FORMAT_FAILED = 5,
        MKDIR_FAILED = 6,
        RMDIR_FAILED = 7,
        REMOVE_FAILED = 8,
    };

    struct Flags : public Bits<uint8_t> {
        static constexpr Type DIR = 0x01;
    };

    //! Response returned by INFO command.
    struct InfoResponse {
        uint32_t totalBytes;  //!< Total number of bytes in the file system.
        uint32_t usedBytes;   //!< Number of used bytes in the file system.
    };

    bool handlePacket(Packet const& cmd, Packet* rsp) override;

    char const* as_str(Packet::Command::Type cmd) const override;

    //! Handles the COPY command
    void handleCopy(
        Packet const& cmd,  //!< [in] Ping packet.
        Packet* rsp         //!< [mod] Place to store ping response.
    );

    //! Handles the FORMAT command
    void handleFormat(
        Packet const& cmd,  //!< [in] Ping packet.
        Packet* rsp         //!< [mod] Place to store ping response.
    );

    //! Handles the INFO command
    void handleInfo(
        Packet const& cmd,  //!< [in] Ping packet.
        Packet* rsp         //!< [mod] Place to store ping response.
    );

    //! Handles the LIST command
    void handleList(
        Packet const& cmd,  //!< [in] Ping packet.
        Packet* rsp         //!< [mod] Place to store ping response.
    );

    //! Handles the MKDIR command
    void handleMkDir(
        Packet const& cmd,  //!< [in] Ping packet.
        Packet* rsp         //!< [mod] Place to store ping response.
    );

    //! Handles the REMOVE command
    void handleRemove(
        Packet const& cmd,  //!< [in] Ping packet.
        Packet* rsp         //!< [mod] Place to store ping response.
    );

    //! Handles the RENAME command
    void handleRename(
        Packet const& cmd,  //!< [in] Ping packet.
        Packet* rsp         //!< [mod] Place to store ping response.
    );

    //! Handles the READ command
    void handleRead(
        Packet const& cmd,  //!< [in] Ping packet.
        Packet* rsp         //!< [mod] Place to store ping response.
    );

    //! Handles the RMDIR command
    void handleRmDir(
        Packet const& cmd,  //!< [in] Ping packet.
        Packet* rsp         //!< [mod] Place to store ping response.
    );

    //! Handles the Write command
    void handleWriteAppend(
        char const* mode,   //!< [in] FILE_WRITE or FILE_APPEND
        Packet const& cmd,  //!< [in] Ping packet.
        Packet* rsp         //!< [mod] Place to store ping response.
    );
};
