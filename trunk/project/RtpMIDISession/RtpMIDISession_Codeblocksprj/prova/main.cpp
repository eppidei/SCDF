#include <iostream>
#include "SessionStarter.hpp"
using namespace std;

int main()
{
    SessionStarter::Instance()->Start();
    return 0;
}
