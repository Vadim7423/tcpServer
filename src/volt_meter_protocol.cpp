#include "volt_meter_protocol.h"
#include "row_channels.h"
#include <sys/socket.h>
#include <iostream>
#include  <experimental/random>
#include "timer.h"
#include <set>

VoltMeterProtocol::VoltMeterProtocol(RowChannels& channels)
    : m_channels(channels)
{
    initFuncs();
    Timer::instance().registerFunc([this]() {
        statusUpdate();
    });
}

VoltMeterProtocol::~VoltMeterProtocol()
{
    Timer::instance().clear();
}

void VoltMeterProtocol::readRequest(int client_socet, const std::string &str)
{
    size_t pos = str.find(' ');
    std::string command = str.substr(0, pos);
    std::string params;

    if(pos != str.npos)
        params = str.substr(pos + 1);

    if(command.size() && command.back() == '\n')
        command.pop_back();

    if(m_funcs.count(command))
        (this->*m_funcs[command])(client_socet, params);
}

void VoltMeterProtocol::createFullData(int client_socet, const std::string& str)
{
    std::string result;
    result += "data " + std::to_string(m_channels.size());

    for(size_t i = 0; i < m_channels.size(); ++i) {
        result += ", ";
        result += std::to_string(static_cast<int>(m_channels.channel(i).status()));
        result += ", ";
        result += m_channels.channel(i).range().toString();
    }

    send(client_socet, result.c_str(), result.size(), 0);
}

void VoltMeterProtocol::initFuncs()
{
    m_funcs = {
        {"get_data", &VoltMeterProtocol::createFullData},
        {"get_status", &VoltMeterProtocol::getStatus},
        {"set_range", &VoltMeterProtocol::setRange},
        {"start_measure", &VoltMeterProtocol::startMeasure},
        {"stop_measure", &VoltMeterProtocol::stopMeasure},
        {"get_result", &VoltMeterProtocol::getResult}
    };
}

void VoltMeterProtocol::statusUpdate()
{
    static int count = 0;

    if(count > 5) {
        count = 0;
        for(int i = 0; i < m_channels.size(); ++i) {
            int val = std::experimental::randint(0, 3);
            auto status = static_cast<Channel::Status>(val);
            auto current_status = m_channels.channel(i).status();
            if(current_status != Channel::Status::measure_state &&
                status != Channel::Status::measure_state)
                m_channels.channel(i).setStatus(status);
        }
        return;
    }

    ++count;
}

void VoltMeterProtocol::getStatus(int client_socet, const std::string &str)
{
    std::string answer = "get_status ok ";
    auto params = getStrArray(str);

    bool flag = false;
    for(const auto& item : params) {
        if(flag)
            answer += ", ";
        std::string s = "channel";
        auto tmp_str = item.substr(s.size());
        int channel_id = std::stoi(tmp_str);
        int status = static_cast<int>(m_channels.channel(channel_id).status());
        answer += item + " status" + std::to_string(status);
        flag = true;
    }
    answer += '\n';

     send(client_socet, answer.c_str(), answer.size(), 0);
}

void VoltMeterProtocol::setRange(int client_socet, const std::string &str)
{
    std::string s = "channel";
    std::string r = "range";
    auto params = getStrArray(str);

    int channel_id = std::stoi(params[0].substr(s.size()));

    static std::set<Channel::Status> error_statusws = {
        Channel::Status::error_state,
        Channel::Status::busy_state
    };

    if(m_channels.size() <= channel_id ||
        error_statusws.count(m_channels.channel(channel_id).status()) ) {
        std::string command = "set_range fail " + params[0] + '\n';
        send(client_socet, command.c_str(), command.size(), 0);
        return;
    }

    int range_id = std::stoi(params[1].substr(r.size()));
    static std::vector<Range> ranges = {
        Range(0.0000001, 0.001),
        Range(0.001, 1),
        Range(1, 1000),
        Range(1000, 1000000)
    };

    m_channels.channel(channel_id).setRange(ranges.at(range_id));
    std::string command = "set_range ok " + params[0] + ", " + params[1] + '\n';
    send(client_socet, command.c_str(), command.size(), 0);
}

void VoltMeterProtocol::startMeasure(int client_socet, const std::string &str)
{
    auto params = getStrArray(str);
    std::string s = "channel";
    int channel_id = std::stoi(params[0].substr(s.size()));

    if(m_channels.channel(channel_id).status() != Channel::Status::idle_state) {
        std::string command = "start_measure fail " + params[0] + '\n';
        send(client_socet, command.c_str(), command.size(), 0);
        return;
    }

    m_measure_channels[params[0]] = Timer::instance().registerFunc([this, channel_id](){
        auto& channel = m_channels.channel(channel_id);
        int min = 1;
        int max = channel.range().max() / channel.range().min();
        float val = std::experimental::randint(min, max);
        channel.setValue(val * channel.range().min());
        std::cout << channel.value() << std::endl;
    });

    m_channels.channel(channel_id).setStatus(Channel::Status::measure_state);

    std::string command = "start_measure ok " + params[0] + '\n';
    send(client_socet, command.c_str(), command.size(), 0);
}

void VoltMeterProtocol::stopMeasure(int client_socet, const std::string &str)
{
    auto params = getStrArray(str);
    std::string s = "channel";
    int channel_id = std::stoi(params[0].substr(s.size()));

    if(!m_measure_channels.count(params[0])) {
        std::string command = "stop_measure fail " + params[0] + '\n';
        return;
    }

    m_channels.channel(channel_id).setStatus(Channel::Status::idle_state);
    Timer::instance().unsetFunc(m_measure_channels[params[0]]);
    std::string command = "stop_measure ok " + params[0] + '\n';
    send(client_socet, command.c_str(), command.size(), 0);
}

void VoltMeterProtocol::getResult(int client_socet, const std::string &str)
{
    auto params = getStrArray(str);
    std::string s = "channel";
    int channel_id = std::stoi(params[0].substr(s.size()));

    if(!m_measure_channels.count(params[0])) {
        std::string command = "get_result fail " + params[0] + '\n';
        return;
    }

    float res = m_channels.channel(channel_id).value();
    std::string command = "get_result ok " + params[0] + ", " + std::to_string(res) + '\n';
    send(client_socet, command.c_str(), command.size(), 0);
}

std::vector<std::string> VoltMeterProtocol::getStrArray(const std::string &str)
{
    std::vector<std::string> tokens;
    size_t pos = 0;
    size_t begin = 0;
    while(pos != str.npos) {
        pos = str.find(',', pos);
        tokens.push_back(str.substr(begin, pos));

        while(str[pos] == ' ' || str[pos] == ',') {
            ++pos;
        }

        begin = pos;
    }

    if(tokens.size() && tokens.back().back() == '\n') {
        tokens.back().pop_back();
    }

    return tokens;
}
