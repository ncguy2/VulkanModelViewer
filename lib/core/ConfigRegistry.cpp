//
// Created by Guy on 12/07/2021.
//

#include <core/ConfigRegistry.h>

bool ConfigRegistry::Has(const std::string& key) {
    return registryMap.contains(key);
}
