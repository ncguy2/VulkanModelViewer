//
// Created by Guy on 12/07/2021.
//

#ifndef GLMODELVIEWER_CONFIGREGISTRY_H
#define GLMODELVIEWER_CONFIGREGISTRY_H

#include <map>
#include <string>

#include <core/Events.h>

enum ConfigValueType {
    eULong,
    eUInt,
    eUShort,
    eUByte,
    eLong,
    eInt,
    eShort,
    eByte,
    eBool,
    eFloat,
    eString
};

struct ConfigValue {
    ConfigValueType type;
    union {
        uint64_t ulong{};
        uint32_t uint;
        uint16_t ushort;
        uint8_t ubyte;
        int64_t slong;
        int32_t sint;
        int16_t sshort;
        int8_t sbyte;
        bool boolean;
        float floating;
        const char *string;
    };

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"
#pragma ide diagnostic ignored "google-explicit-constructor"
    explicit ConfigValue() : ulong(0), type(eULong) {}

    explicit ConfigValue(uint64_t val) : ulong(val), type(eULong) {}
    explicit ConfigValue(uint32_t val) : uint(val), type(eUInt) {}
    explicit ConfigValue(uint16_t val) : ushort(val), type(eUShort) {}
    explicit ConfigValue(uint8_t val) : ubyte(val), type(eUByte) {}
    explicit ConfigValue(int64_t val) : slong(val), type(eLong) {}
    explicit ConfigValue(int32_t val) : sint(val), type(eInt) {}
    explicit ConfigValue(int16_t val) : sshort(val), type(eShort) {}
    explicit ConfigValue(int8_t val) : sbyte(val), type(eByte) {}
    explicit ConfigValue(bool val) : boolean(val), type(eBool) {}
    explicit ConfigValue(float val) : floating(val), type(eFloat) {}
    explicit ConfigValue(const char *str) : string(str), type(eString) {}
#pragma clang diagnostic pop

    template <typename T>
    bool EqualValue(T t) {
        return EqualValue(ConfigValue(t));
    }
    bool EqualValue(ConfigValue cval) {
        if(cval.type != type)
            return false;

        switch (type) {
            case eULong:
                return cval.ulong == ulong;
            case eUInt:
                return cval.uint == uint;
            case eUShort:
                return cval.ushort == ushort;
            case eUByte:
                return cval.ubyte == ubyte;
            case eLong:
                return cval.slong == slong;
            case eInt:
                return cval.sint == sint;
            case eShort:
                return cval.sshort == sshort;
            case eByte:
                return cval.sbyte == sbyte;
            case eBool:
                return cval.boolean == boolean;
            case eFloat:
                return cval.floating == floating;
            case eString:
                return cval.string == string;
        }

        return false;
    }
};

class ConfigRegistry {
public:
    typedef Delegate<std::string> ConfigSetEvent;
    ConfigSetEvent configSet;

    template<typename T>
    ConfigValue Get(std::string key, T defaultValue) {
        if (Has(key))
            return registryMap[key];
        return Set(key, defaultValue);
    }

    template<typename T>
    ConfigValue Set(std::string key, T value) {

        if (Has(key)) {
            if (registryMap[key].EqualValue(value))
                return registryMap[key];
        }

        registryMap[key] = ConfigValue(value);
        configSet(key);
        return registryMap[key];
    }

    bool Has(const std::string &key);

protected:
    std::map<std::string, ConfigValue> registryMap;
};

#endif//GLMODELVIEWER_CONFIGREGISTRY_H
