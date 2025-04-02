#include <iostream>

#include "ModuleStreamCore.hpp"
#include "server.hpp"

int main( [[maybe_unused]] int argc, [[maybe_unused]] char* argv[] )
{
    std::unique_ptr<UniversalServerCore> core = std::make_unique<ModuleStreamCore>( 0, 154320 );
    Server server( "127.0.0.1", 34100, std::move( core ) );
    return server.run();
}
