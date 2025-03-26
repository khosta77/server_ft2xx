#include <iostream>

#include "ModuleFT232RL.hpp"

int main()
{
    ModuleFT232RL module_;
    std::cout << module_ << std::endl;
    std::vector<uint32_t> frame = { 1, 2, 3, 4, 5, 6, 7, 8 };
    module_.writeData<uint32_t>( frame );
    //std::vector<uint32_t> retFrame( frame.size(), 0 );
    //module_.readData32( retFrame );
    auto retFrame = module_.read<uint32_t>(10000);
    for( const auto it : retFrame )
        std::cout << it << ' ';
    std::cout << std::endl;
    return 0;
}
