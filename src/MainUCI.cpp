#include "engine/UCIconnect.h"

int main()
{
    Masks::initBitmasks();

    UCIconnection connection;

    connection.Loop();
    return 1;
}
