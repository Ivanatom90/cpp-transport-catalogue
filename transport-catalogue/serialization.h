#pragma once
#include <iostream>
#include <transport_catalogue.pb.h>
#include <filesystem>

#include <fstream>
#include <iostream>

namespace ser {
    void Serialization(const std::string& file_path, const serialize::TransportCatalog& tc);
    serialize::TransportCatalog& DeSerialization(const std::string& file_path);
}
