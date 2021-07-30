//
// Created by Guy on 04/06/2021.
//

#ifndef GLMODELVIEWER_PCH_H
#define GLMODELVIEWER_PCH_H


#ifdef NDEBUG
#define CHECK(x) x
#else
#define CHECK(x)                                                                                \
    {                                                                                           \
        vk::Result res = x;                                                                     \
        if (res != vk::Result::eSuccess)                                                        \
            throw std::runtime_error(vk::to_string(res) + ": " + #x);                           \
    }
#endif

#ifndef MAX_FRAMES_IN_FLIGHT
#define MAX_FRAMES_IN_FLIGHT 3
#endif // MAX_FRAMES_IN_FLIGHT

#define CAST_STR_WSTR(str) std::wstring((wchar_t*) str.data(), str.size() / 2)
#define CAST_WSTR_STR(wstr) std::string((char*) wstr.data(), wstr.size() * 2)


#define GET_VK_HANDLE(obj) reinterpret_cast<uint64_t&>(obj)

#endif//GLMODELVIEWER_PCH_H
