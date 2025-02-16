#ifndef VOLT_METER_PROTOCOL_H
#define VOLT_METER_PROTOCOL_H

#include "protocol.h"
#include <vector>
#include <unordered_map>
#include <map>

class RowChannels;
class VoltMeterProtocol : public Protocol
{
public:
    VoltMeterProtocol(RowChannels& channels);
    ~VoltMeterProtocol();

    void readRequest(int client_socet, const std::string& str) override;

private:
    void createFullData(int client_socet, const std::string& str);
    void initFuncs();

    void statusUpdate();
    void getStatus(int client_socet, const std::string& str);

    void setRange(int client_socet, const std::string& str);
    void startMeasure(int client_socet, const std::string& str);
    void stopMeasure(int client_socet, const std::string& str);
    void getResult(int client_socet, const std::string& str);

    std::vector<std::string> getStrArray(const std::string &str);

    RowChannels& m_channels;
    std::map<std::string, int> m_measure_channels;
    std::unordered_map<std::string, void (VoltMeterProtocol::*) (int, const std::string&)> m_funcs;
};

#endif // VOLT_METER_PROTOCOL_H
