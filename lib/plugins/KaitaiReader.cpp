//
// Created by Guy on 04/07/2021.
//

#include <kaitai/kaitaistream.h>
#include <plugins/api/KaitaiReader.h>

void *Kaitai::ReadRaw(const std::filesystem::path &path, std::function<void *(kaitai::kstream *)> ctor) {
    std::ifstream ifs(path.string().c_str(), std::ifstream::binary);
    kaitai::kstream ks(&ifs);
    return ctor(&ks);
}
