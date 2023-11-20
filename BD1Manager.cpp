#include "BD1Manager.hpp"

/// @brief Constructor
BD1Manager::BD1Manager() noexcept {
    m_TextureFileNames.fill("");
}

/// @brief Destructor
BD1Manager::~BD1Manager() noexcept {
    m_BD1Data.clear();
}

/// @brief Convert MQO
/// @param out_MQOFormat Variable to receive MQO Format
std::optional<MQOFormat> BD1Manager::ConvertMQO() noexcept {
    if (m_BD1Data.empty()) {
        return std::nullopt;
    }
    
    // MQO
    MQOFormat mqo;

    // Header
    mqo.Header.Format = MQOHeaderFormatType::Text;
    mqo.Header.Version = 1.0F;

    // Material
    for (auto i = 0; i < MAX_BLOCK_TEXTURE; ++i) {
        MQOMaterial mat;
        
        mat.Name = (m_TextureFileNames[i] == "") ? "empty" : m_TextureFileNames[i];
        mat.ShaderType = MQOMaterialShaderType::Phong;
        mat.RGBA = { 1.0F, 1.0F, 1.0F, 1.0F };
        mat.Diffuse = 0.900F;
        mat.Ambient = 0.400F;
        mat.Emission = 0.100F;
        mat.Specular = 0.000F;
        mat.Power = 5.00F;
        mat.FileName = m_TextureFileNames[i];

        mqo.Materials.push_back(mat);
    }

    // Object
    mqo.Object.Name = "map";
    mqo.Object.Visibility = MQOObjectVisibility::Visible;
    mqo.Object.LockFlag = false;
    mqo.Object.ShadingFlag = true;
    mqo.Object.SmoothingAngle = 0.0F;
    mqo.Object.RGB = { 0.898F, 0.498F, 0.698F };
    mqo.Object.ColorType = MQOObjectColorType::Configuration;

    // Number of Block
    auto number_of_block = m_BD1Data.size();

    // Vertex
    for (auto i = 0; i < number_of_block; ++i) {
        for (auto j = 0; j < NUMBER_OF_VERTICES; ++j) {
            MQOObjectVertex vertex;

            vertex.X = -m_BD1Data[i].X[j];              // Caution: Negative
            vertex.Y = m_BD1Data[i].Y[j];
            vertex.Z = m_BD1Data[i].Z[j];

            mqo.Object.Vertexs.push_back(vertex);
        }
    }

    // Face
    for (auto i = 0; i < number_of_block; ++i) {
        for (auto j = 0; j < NUMBER_OF_FACES; ++j) {
            MQOObjectFace face;
            face.NumberOfVertices = 4;
            
            switch (j) {
                case 0: {
                    face.VertexIndices.push_back(3 + (i << 3));
                    face.VertexIndices.push_back(2 + (i << 3));
                    face.VertexIndices.push_back(1 + (i << 3));
                    face.VertexIndices.push_back(0 + (i << 3));
                } break;

                case 1: {
                    face.VertexIndices.push_back(4 + (i << 3));
                    face.VertexIndices.push_back(5 + (i << 3));
                    face.VertexIndices.push_back(6 + (i << 3));
                    face.VertexIndices.push_back(7 + (i << 3));
                } break;

                case 2: {
                    face.VertexIndices.push_back(0 + (i << 3));
                    face.VertexIndices.push_back(1 + (i << 3));
                    face.VertexIndices.push_back(5 + (i << 3));
                    face.VertexIndices.push_back(4 + (i << 3));
                } break;

                case 3: {
                    face.VertexIndices.push_back(1 + (i << 3));
                    face.VertexIndices.push_back(2 + (i << 3));
                    face.VertexIndices.push_back(6 + (i << 3));
                    face.VertexIndices.push_back(5 + (i << 3));
                } break;

                case 4: {
                    face.VertexIndices.push_back(2 + (i << 3));
                    face.VertexIndices.push_back(3 + (i << 3));
                    face.VertexIndices.push_back(7 + (i << 3));
                    face.VertexIndices.push_back(6 + (i << 3));
                } break;

                case 5: {
                    face.VertexIndices.push_back(3 + (i << 3));
                    face.VertexIndices.push_back(0 + (i << 3));
                    face.VertexIndices.push_back(4 + (i << 3));
                    face.VertexIndices.push_back(7 + (i << 3));
                } break;
            }

            face.MaterialIndex = m_BD1Data[i].Face[j].TextureId;

            for (auto k = 0; k < 4; ++k) {
                face.UV.push_back(m_BD1Data[i].Face[j].UV[k][0]);   // U
                face.UV.push_back(m_BD1Data[i].Face[j].UV[k][1]);   // V
            }

            // Add
            mqo.Object.Faces.push_back(face);
        }
    }

    return mqo;
}

/// @brief Load the BD1 File
/// @param filePath BD1 File Path
/// @return Successed(true), Failed(false)
[[nodiscard]] bool BD1Manager::LoadBD1(const std::string_view filePath) noexcept {
    if (filePath.empty())                                       { return false; }
    if (not std::filesystem::exists(filePath))                  { return false; }
    if (std::filesystem::path(filePath).extension() != ".bd1")  { return false; }

    // Open
    std::ifstream ifs(filePath.data(), std::ifstream::in | std::ifstream::binary);
    if (not ifs.is_open()) { return false; }

    // Texture File Name
    char cBuf[31]{ '\0' };
    for (auto i = 0; i < MAX_BLOCK_TEXTURE; ++i) {
        ifs.read((char*)cBuf, sizeof(cBuf));

        m_TextureFileNames[i] = cBuf;
    }

    // Number of Block
    int16_t number_of_block = 0;
    ifs.read((char*)&number_of_block, sizeof(number_of_block));

    // Block Data
    for (auto i = 0; i < number_of_block; ++i) {
        BD1Format block;
        float fBuf = 0.0F;
        int32_t iBuf = 0;

        // X Coords
        for (auto j = 0; j < NUMBER_OF_VERTICES; ++j) {
            ifs.read((char*)&fBuf, sizeof(fBuf));

            block.X[j] = fBuf;
        }
        
        // Y Coords
        for (auto j = 0; j < NUMBER_OF_VERTICES; ++j) {
            ifs.read((char*)&fBuf, sizeof(fBuf));

            block.Y[j] = fBuf;
        }
        
        // Z Coords
        for (auto j = 0; j < NUMBER_OF_VERTICES; ++j) {
            ifs.read((char*)&fBuf, sizeof(fBuf));

            block.Z[j] = fBuf;
        }

        // UV - U
        for (auto j = 0; j < NUMBER_OF_FACES; ++j) {
            for (auto k = 0; k < 4; ++k) {
                ifs.read((char*)&fBuf, sizeof(fBuf));

                block.Face[j].UV[k][0] = fBuf;
            }
        }

        // UV - V
        for (auto j = 0; j < NUMBER_OF_FACES; ++j) {
            for (auto k = 0; k < 4; ++k) {
                ifs.read((char*)&fBuf, sizeof(fBuf));

                block.Face[j].UV[k][1] = fBuf;
            }
        }

        // Texture's Id
        for (auto j = 0; j < NUMBER_OF_FACES; ++j) {
            ifs.read((char*)&iBuf, sizeof(iBuf));

            block.Face[j].TextureId = iBuf;
        }

        // Enable Flag        
        ifs.read((char*)&iBuf, sizeof(iBuf));

        block.EnableFlag = iBuf;

        // Add
        m_BD1Data.push_back(block);
    }

    ifs.close();

    return true;
}

/// @brief Save the BD1 File
/// @param filePath BD1 File Path
/// @return Successed(true), Failed(false)
[[nodiscard]] bool BD1Manager::SaveBD1(const std::string_view filePath) noexcept {
    if (filePath.empty())                                       { return false; }
    if (std::filesystem::path(filePath).extension() != ".bd1")  { return false; }
    if (m_BD1Data.empty())                                      { return false; }

    // Open
    std::ofstream ofs(filePath.data(), std::ofstream::out | std::ofstream::binary);
    if (not ofs.is_open()) { return false; }

    // Texture File Name
    char cBuf[30 + 1]{ '\0' };
    for (const auto& file_name : m_TextureFileNames) {
        std::snprintf(cBuf, 30, "%s", file_name.c_str());
        cBuf[30] = '\0';

        ofs.write((const char*)cBuf, 30 + 1);
    }

    // Number of Block
    auto number_of_block = m_BD1Data.size();
    ofs.write((const char*)&number_of_block, sizeof(uint16_t));

    // Block Data
    for (auto i = 0; i < number_of_block; ++i) {
        // X
        for (auto j = 0; j < NUMBER_OF_VERTICES; ++j) {
            ofs.write((const char*)&m_BD1Data[i].X[j], sizeof(m_BD1Data[i].X[j]));
        }
        // Y
        for (auto j = 0; j < NUMBER_OF_VERTICES; ++j) {
            ofs.write((const char*)&m_BD1Data[i].Y[j], sizeof(m_BD1Data[i].Y[j]));
        }
        // Z
        for (auto j = 0; j < NUMBER_OF_VERTICES; ++j) {
            ofs.write((const char*)&m_BD1Data[i].Z[j], sizeof(m_BD1Data[i].Z[j]));
        }

        // UV - U
        for (auto j = 0; j < NUMBER_OF_FACES; ++j) {
            for (auto k = 0; k < 4; ++k) {
                ofs.write((const char*)&m_BD1Data[i].Face[j].UV[k][0], sizeof(m_BD1Data[i].Face[j].UV[k][0]));
            }
        }

        // UV - V
        for (auto j = 0; j < NUMBER_OF_FACES; ++j) {
            for (auto k = 0; k < 4; ++k) {
                ofs.write((const char*)&m_BD1Data[i].Face[j].UV[k][1], sizeof(m_BD1Data[i].Face[j].UV[k][1]));
            }
        }

        // Texture's Id
        for (auto j = 0; j < NUMBER_OF_FACES; ++j) {
            ofs.write((const char*)&m_BD1Data[i].Face[j].TextureId, sizeof(m_BD1Data[i].Face[j].TextureId));
        }

        // Enable Flag
        int32_t enable_flag = m_BD1Data[i].EnableFlag;
        ofs.write((const char*)&enable_flag, sizeof(enable_flag));
    }

    ofs.close();

    return true;
}

/// @brief Export MQO
/// @param filePath MQO File Path
/// @return Successed(true), Failed(false)
[[nodiscard]] bool BD1Manager::ExportMQO(const std::string_view filePath) noexcept {
    if (filePath.empty())                                       { return false; }
    if (std::filesystem::path(filePath).extension() != ".mqo")  { return false; }

    // Get
    auto mqo = ConvertMQO();
    if (mqo == std::nullopt) {
        return false;
    }

    // Write
    std::ofstream ofs(filePath.data(), std::ofstream::out);
    if (not ofs.is_open()) { return false; }

    // Header
    ofs << mqo.value().Header.Signature << "\n";
    auto format_type = (mqo.value().Header.Format == MQOHeaderFormatType::Text) ? "Text" : "Compress";
    ofs << std::format("Format {} Ver {:.1f}\n", format_type, mqo.value().Header.Version).c_str();
    ofs << "\n";

    // Material
    ofs << std::format("Material {} {}\n", m_TextureFileNames.size(), "{");
    for (auto i = 0; i < mqo.value().Materials.size(); ++i) {
        ofs << "\t"
            << std::format("\"{}\"", mqo.value().Materials[i].Name).c_str()
            << std::format(" shader({:d})", int32_t(mqo.value().Materials[i].ShaderType)).c_str()
            << std::format(" col({:.3f} {:.3f} {:.3f} {:.3f})", mqo.value().Materials[i].RGBA[0], mqo.value().Materials[i].RGBA[1], mqo.value().Materials[i].RGBA[2], mqo.value().Materials[i].RGBA[3])
            << std::format(" dif({:.3f})", mqo.value().Materials[i].Diffuse).c_str()
            << std::format(" amb({:.3f})", mqo.value().Materials[i].Ambient).c_str()
            << std::format(" emi({:.3f})", mqo.value().Materials[i].Emission).c_str()
            << std::format(" spc({:.3f})", mqo.value().Materials[i].Specular).c_str()
            << std::format(" power({:.2f})", mqo.value().Materials[i].Power)
            << std::format(" tex(\"{}\")", m_TextureFileNames[i]).c_str()
            << "\n";
    }
    ofs << "}\n";
    ofs << "\n";

    // Object
    ofs << std::format("Object {} {}\n", mqo.value().Object.Name, "{").c_str();
    ofs << std::format("\tvisible {:d}\n", int32_t(mqo.value().Object.Visibility)).c_str();
    ofs << std::format("\tlocking {:d}\n", mqo.value().Object.LockFlag).c_str();
    ofs << std::format("\tshading {:d}\n", mqo.value().Object.ShadingFlag).c_str();
    ofs << std::format("\tfacet {:.1f}\n", mqo.value().Object.SmoothingAngle).c_str();
    ofs << std::format("\tcolor {:.3f} {:.3f} {:.3f}\n", mqo.value().Object.RGB[0], mqo.value().Object.RGB[1], mqo.value().Object.RGB[2]).c_str();
    ofs << std::format("\tcolor_type {:d}\n", int32_t(mqo.value().Object.ColorType)).c_str();

    // Vertex
    ofs << std::format("\tvertex {} {}\n", mqo.value().Object.Vertexs.size(), "{");
    for (auto i = 0; i < mqo.value().Object.Vertexs.size(); ++i) {
        ofs << std::format("\t\t{:.4f} {:.4f} {:.4f}\n", mqo.value().Object.Vertexs[i].X, mqo.value().Object.Vertexs[i].Y, mqo.value().Object.Vertexs[i].Z).c_str();
    }
    ofs << "\t}\n";

    // Face
    ofs << std::format("\tface {} {}\n", mqo.value().Object.Faces.size(), "{");
    for (auto i = 0; i < mqo.value().Object.Faces.size(); ++i) {
        ofs << std::format("\t\t{}", mqo.value().Object.Faces[i].NumberOfVertices).c_str()
            << std::format(" V({} {} {} {})", mqo.value().Object.Faces[i].VertexIndices[0], mqo.value().Object.Faces[i].VertexIndices[1], mqo.value().Object.Faces[i].VertexIndices[2], mqo.value().Object.Faces[i].VertexIndices[3]).c_str()
            << std::format(" M({})", mqo.value().Object.Faces[i].MaterialIndex).c_str()
            << std::format(" UV({:.5f} {:.5f} {:.5f} {:.5f} {:.5f} {:.5f} {:.5f} {:.5f})", mqo.value().Object.Faces[i].UV[0], mqo.value().Object.Faces[i].UV[1], mqo.value().Object.Faces[i].UV[2], mqo.value().Object.Faces[i].UV[3], mqo.value().Object.Faces[i].UV[4], mqo.value().Object.Faces[i].UV[5], mqo.value().Object.Faces[i].UV[6], mqo.value().Object.Faces[i].UV[7]).c_str()
            << "\n";
    }
    ofs << "\t}\n";
    ofs << "}\n";

    // Eof
    ofs << "Eof";

    ofs.close();

    return true;
}