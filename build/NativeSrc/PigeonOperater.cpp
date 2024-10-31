
#include "PigeonOperater.h"
#include "tjConfig.h"

PigeonOperater::PigeonOperater()
{
    net = new NetController();
    activity = NULL;
}

PigeonOperater::~PigeonOperater()
{
    if (net != NULL)
    {
        DestroyActive();
        delete net;
        net = NULL;
    }
}

void PigeonOperater::CreateActive()
{
    if (activity == NULL)
    {
        activity = net->createClient();
        ((RPCClient*)activity)->setListener(new UnityClientListener(activity));
    }
}

void PigeonOperater::DestroyActive()
{
    if (activity != NULL)
    {
        close();
        net->destroyActivity(activity);
        activity = NULL;
    }
}

bool PigeonOperater::connect(const char* host, in_port_t port, long timeout)
{
    bool result = ((RPCClient*)activity)->connect(host, port, timeout);
    return result;
}

bool PigeonOperater::repair(bool force)
{
    bool result = ((RPCClient*)activity)->repair(force);
    return result;
}

void PigeonOperater::close()
{
    ((RPCClient*)activity)->close();
}

int PigeonOperater::request(int method,
                          char* params,
                          size_t datalen,
                          RequestCallback callback,
                          uint16_t extraRTO)
{   
    int result = ((RPCClient*)activity)->request(
             method, (void*)params, datalen,
             [method, callback](int req, int err, const std::string& resp,float timestamp) {
                 pigeonPerformInLuaThread([=]() {
                    if (callback != NULL)
                    {
                        std::string temp = resp;
                        callback(method, req, err, temp.data(), temp.size(), timestamp);
                    }
                 });
             }, extraRTO);

    return result;
}

int PigeonOperater::rerequest(int reqId)
{
    int result = ((RPCClient*)activity)->request(reqId);
    return result;
}

int PigeonOperater::notify(int event, const void* data, size_t datalen)
{
    int result = ((RPCClient*)activity)->notify(event, data, datalen);
    return result;
}

int PigeonOperater::respond(int request, int err, const void* data, size_t datalen)
{
    int result = ((RPCClient*)activity)->respond(request, err, data, datalen);
    return result;
}

int PigeonOperater::isvalid()
{
    if (!activity) {
        return false;
    } 
    return true;
}

void PigeonOperater::setHeartbeatInterval(long millisecond)
{
    ((RPCClient*)activity)->setHeartbeatInterval(millisecond);
}

long PigeonOperater::getHeartbeatInterval()
{
    long value = ((RPCClient*)activity)->getHeartbeatInterval();
    return value;
}

void PigeonOperater::setHeartbeatTimeout(long millisecond)
{
    ((RPCClient*)activity)->setHeartbeatTimeout(millisecond);
}

void PigeonOperater::setLoopInterval(int loopInterval)
{
    ((RPCClient*)activity)->setloopInterval(loopInterval);
}

void PigeonOperater::setReqRTOBounds(int lbound, int ubound)
{
    ((RPCClient*)activity)->setReqRTOBounds(lbound, ubound);
}

void PigeonOperater::setReqTimeout(int milliseconds)
{
    ((RPCClient*)activity)->setReqTimeout(milliseconds);
}

int PigeonOperater::getReqTimeout()
{
    int value = ((RPCClient*)activity)->getReqTimeout();
    return value;
}

long PigeonOperater::getMaxReqDelay()
{
    long value = ((RPCClient*)activity)->getMaxReqDelay();
    return value;
}

long PigeonOperater::getAvgReqDelay()
{
    long value = ((RPCClient*)activity)->getAvgReqDelay();
    return value;
}

long PigeonOperater::remoteTimestamp(long* elb, long* eub)
{
    long value = ((RPCClient*)activity)->remoteTimestamp(elb, eub);
    return value;
}

long PigeonOperater::remoteTimeMillis(long* elb, long* eub)
{
    int64_t value = ((RPCClient*)activity)->remoteTimeMillis(elb, eub);
    return value;
}

double PigeonOperater::remoteTimescale(double* lb, double* ub)
{
    double value = ((RPCClient*)activity)->remoteTimescale(lb, ub);
    return value;
}

void PigeonOperater::timeCorrectorReset()
{
    ((RPCClient*)activity)->timeCorrectorReset();
}

string PigeonOperater::getCryptoConfig()
{
    std::string config = ((RPCClient*)activity)->getCryptoConfig();

    return config;
}

int PigeonOperater::getCryptoType()
{
    int type = ((RPCClient*)activity)->getCryptoType();
    return type;
}

void PigeonOperater::setListenerCallback(NetErrorCallback errorCallback, NetOnConnectedCallback onConnectCallback, NetOnNotifiedCallback onNotifiedCallback)
{
    UnityClientListener* listner = (UnityClientListener*)(((RPCClient*)activity)->getListener());
    if (listner != NULL)
    {
        listner->setListenerCallback(errorCallback, onConnectCallback, onNotifiedCallback);
    }
}

void PigeonOperater::enableRecvPushBuffer()
{
    ((RPCClient*)activity)->enableRecvPushBuffer();
}

void PigeonOperater::resetRecvPushBuffer(int recvPushId)
{
    ((RPCClient*)activity)->resetRecvPushBuffer(recvPushId);
}

void PigeonOperater::setComprType(int type)
{
    ((RPCClient*)activity)->setComprType((RPCTransport::CompressionType)type);
}

