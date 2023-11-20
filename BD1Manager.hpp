#pragma once

// Check C++ Version
static_assert(__cplusplus >= 202002L, "Required C++20!");

#include <array>
#include <cstddef>
#include <cstdint>
#include <format>
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "MQOFormat.hpp"

constexpr auto NUMBER_OF_VERTICES   = 8;
constexpr auto NUMBER_OF_FACES      = 6;
// constexpr auto MAX_BLOCK            = 160;
constexpr auto MAX_BLOCK_TEXTURE    = 10;

/// @brief BD1's Face Info
struct BD1Face final {
    float UV[4][2];                                 //!< UV Coords
    std::size_t TextureId;                          //!< Texture's Id
};

/// @brief BD1 Format
struct BD1Format final {
    std::array<float, NUMBER_OF_VERTICES> X;        //!< X Coords
    std::array<float, NUMBER_OF_VERTICES> Y;        //!< Y Coords
    std::array<float, NUMBER_OF_VERTICES> Z;        //!< Z Coords
    std::array<BD1Face, NUMBER_OF_FACES> Face;      //!< Faces
    bool EnableFlag;                                //!< Enable Flag
};

/// @brief BD1's Manager class
class BD1Manager final {
private:
    std::array<std::string, MAX_BLOCK_TEXTURE> m_TextureFileNames;          //!< Array of texture file names
    std::vector<BD1Format> m_BD1Data;                                       //!< BD1's Block Data

    std::optional<MQOFormat> ConvertMQO() noexcept;

public:
    BD1Manager() noexcept;
    ~BD1Manager() noexcept;

    [[nodiscard]] bool LoadBD1(const std::string_view filePath) noexcept;
    [[nodiscard]] bool SaveBD1(const std::string_view filePath) noexcept;
    [[nodiscard]] bool ExportMQO(const std::string_view filePath) noexcept;

    const std::string& GetTextureFileName(std::size_t textureId) const noexcept;
    void GetBlockData(std::size_t blockId, BD1Format* out_BlockData) const noexcept;
};