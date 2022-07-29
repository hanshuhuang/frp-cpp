#ifndef FRP_ID_GENERATOR_H
#define FRP_ID_GENERATOR_H

#include <string>
#include <atomic>

namespace FRP
{

class IDGenerator {
public:
    static std::string GetClientID();
    static std::string GetConnID();
private:
    static atomic<int> id;
}

} // namespace FRP


#endif