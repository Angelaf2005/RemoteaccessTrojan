#include <iostream>
#include <vector>


std::string executeCommand(const std::string& cmd) {
    // Function to execute commands in the system
    std::string output;

    // open pipe in binary mode
    FILE* pipe = _popen(cmd.c_str(), "rb");

    if (!pipe) {
        // popen did not work, return empty string
        return {};
    }

    constexpr size_t BUF_SZ = 4096;
    std::vector<char> buf(BUF_SZ);

    // Read output command by blocks
    size_t n;
    while ((n = fread(buf.data(), 1, BUF_SZ, pipe)) >0) {
        output.append(buf.data(), buf.data() + n);
    }

    _pclose(pipe);
    return output;
}