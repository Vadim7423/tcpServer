#include "server.h"
#include "src/row_channels.h"
#include "src/volt_meter_protocol.h"
#include "src/timer.h"

const int PORT = 12345;
constexpr int N = 2;

int main() {
    Range default_range;
    RowChannels channels(N, default_range);
    VoltMeterProtocol protocol(channels);
    Server server(PORT, protocol);
    server.start();
    Timer::instance().stop();

    while(!Timer::instance().isFinished()) {}
    return 0;
}
