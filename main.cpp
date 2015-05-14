#include "mbed.h"
#include "suli2.h"
#include "rpc_server.h"

//Serial pc(USBTX, USBRX);
extern void setup();
extern void loop();

int main()
{
    printf("start main \r\n");

    rpc_server_init();

    setup();

    for (;;)
    {
        rpc_server_loop();
        loop();
    }

    return 0;
}