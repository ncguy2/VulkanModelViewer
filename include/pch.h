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

#endif//GLMODELVIEWER_PCH_H
