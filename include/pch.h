//
// Created by Guy on 04/06/2021.
//

#ifndef GLMODELVIEWER_PCH_H
#define GLMODELVIEWER_PCH_H


#ifdef NDEBUG
#define CHECK(x) x
#else
#define CHECK(x) if(x != vk::Result::eSuccess) throw std::runtime_error(#x);
#endif

#ifndef MAX_FRAMES_IN_FLIGHT
#define MAX_FRAMES_IN_FLIGHT 3
#endif // MAX_FRAMES_IN_FLIGHT

#define CAST_STR_WSTR(str) std::wstring((wchar_t*) str.data(), str.size() / 2)
#define CAST_WSTR_STR(wstr) std::string((char*) wstr.data(), wstr.size() * 2)



#endif//GLMODELVIEWER_PCH_H
