

//extern void Start(HWND hwnd);

#include <Logging.h>
#include <iostream>
#include <startup.h>

int main(int _, char** argv) {
    Logging::Init();
    Logging::Get() << "Starting" << std::endl;
    Start(nullptr);
    return 0;
}

