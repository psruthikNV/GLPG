#ifdef __linux__
#include <unistd.h>
#endif

namespace glpg {
    void pause()
    {
#ifdef __linux__
        sleep(5);
#elif defined _WIN32
        system("pause");
#endif
    }
}
