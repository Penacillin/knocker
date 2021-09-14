#include <iostream>

#include "drmprocessorclientimpl.h"

int main() {
    std::cout << "Hello World" << std::endl;
    DRMProcessorClientImpl client;
    const auto resp = client.sendHTTPRequest("http://example.org/");
    std::cout << resp << std::endl;
}
