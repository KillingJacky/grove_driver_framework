#include "mbed.h"
#include "suli2.h"
#include "rpc_server.h"

Serial pc(USBTX, USBRX);

int main()
{
    pc.printf("adsfasdfasdfas");

    rpc_server_init();

    for (;;)
    {
        rpc_server_loop();
        pc.printf("a");
        suli_delay_ms(1);
    }

    return 0;
}