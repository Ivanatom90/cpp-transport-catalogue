#include "serialization.h"


namespace ser {
    void Serialization(const std::string& file_path, const serialize::TransportCatalog& tc){
        const std::filesystem::path path(file_path);
        std::ofstream out_file(path,  std::ios::binary);
        tc.SerializePartialToOstream(&out_file);
    }

    serialize::TransportCatalog& DeSerialization(const std::string& file_path){
        serialize::TransportCatalog tc_out;
        std::ifstream file(file_path);
        tc_out.ParseFromIstream(&file);
        return tc_out;
    }
}
