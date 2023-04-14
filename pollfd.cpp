
#include <vector>
#include <poll.h>


using namespace std;

class PollFd: vector<pollfd> {

    add();
    remove();

    poll(int timeout)
    {
        poll(data(), {}, time);
    }
};