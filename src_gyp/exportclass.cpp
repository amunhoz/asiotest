#include "exportclass.h"

using namespace Napi;
using namespace std::placeholders;

// Initialize native add-on
Napi::Object Init (Napi::Env env, Napi::Object exports) {
    LibNodeTransport::Init(env, exports);    
    return exports;
}
// Register and initialize native add-on
//NODE_API_MODULE(libnngcom, Init)
NODE_API_MODULE(addon, Init)