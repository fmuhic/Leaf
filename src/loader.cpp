#include <fstream>
#include <sstream>

#include "loader.h"
#include "types.h"
#include "helpers.h"

using std::string;
using std::ifstream;
using std::stringstream;

string readFromFile(const char *path) {
    ifstream stream(path);
    stringstream buffer;
    buffer << stream.rdbuf();
    return buffer.str();
}
