#include "Engine.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: NonEuclidean [env]"
                  << "where env is a json file containing a virtual env" << std::endl;
        return 1;
    }
    else
    {
        // Run the main engine
        Engine engine(argv[1]);
        return engine.Run();
    }
}
