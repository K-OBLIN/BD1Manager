#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

/// @brief MQO Header Format Type
enum class MQOHeaderFormatType {
    Text,
    Compress,
};

/// @brief MQO Material Shader Type
enum class MQOMaterialShaderType {
    Classic,
    Constant,
    Lambert,
    Phong,
    Blinn,
};

/// @brief MQO Object's Visibility
enum class MQOObjectVisibility {
    Invisible = 0,
    Visible = 15,
};

/// @brief MQO Object's Color Type
enum class MQOObjectColorType {
    Configuration = 0,
    ObjectUniqueColor = 1,
};

/// @brief MQO Format - Header
struct MQOHeader final {
    const std::string Signature = "Metasequoia Document";           //!< Signature
    MQOHeaderFormatType Format;                                     //!< Format Type
    float Version;                                                  //!< Version (1.0 or 1.1 or 1.2)
};

/// @brief MQO Format - Material
struct MQOMaterial final {
    std::string Name;                           //!< Name of the material
    MQOMaterialShaderType ShaderType;           //!< Shader Type
    std::array<float, 4> RGBA;                  //!< RGBA
    float Diffuse;                              //!< Diffuse
    float Ambient;                              //!< Ambient
    float Emission;                             //!< Emission
    float Specular;                             //!< Specular
    float Power;                                //!< Power
    std::string FileName;                       //!< Material's file name
};

/// @brief MQO Format - Object - Vertex
struct MQOObjectVertex final {
    float X;                                    //!< X Coord
    float Y;                                    //!< Y Coord
    float Z;                                    //!< Z Coord
};

/// @brief MQO Format - Object - Face
struct MQOObjectFace final {
    std::size_t NumberOfVertices;               //!< Number of vertices
    std::vector<std::size_t> VertexIndices;     //!< Vertex indices
    std::size_t MaterialIndex;                  //!< Material Index
    std::vector<float> UV;                      //!< UV Coords
};

/// @brief MQO Format - Object
struct MQOObject final {
    std::string Name;                           //!< Object's Name
    MQOObjectVisibility Visibility;             //!< Visibility
    bool LockFlag;                              //!< Lock
    bool ShadingFlag;                           //!< Shading
    float SmoothingAngle;                       //!< Smoothing Angle
    std::array<float, 3> RGB;                   //!< RGB
    MQOObjectColorType ColorType;               //!< Color Type
    std::vector<MQOObjectVertex> Vertexs;       //!< Vertexs
    std::vector<MQOObjectFace> Faces;           //!< Faces
};

/// @brief MQO Format
/// @note https://www.metaseq.net/en/format.html
struct MQOFormat final {
    MQOHeader Header;                           //!< Header
    std::vector<MQOMaterial> Materials;         //!< Materials
    MQOObject Object;                           //!< Object
};