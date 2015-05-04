#include "mbed.h"
#include "suli2.h"
#include "rpc_server.h"

//Serial pc(USBTX, USBRX);

int main()
{
    printf("start main \r\n");

    rpc_server_init();

    for (;;)
    {
        rpc_server_loop();
    }

    return 0;
}