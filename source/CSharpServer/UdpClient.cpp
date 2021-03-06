#include "stdafx.h"

#include "UdpClient.h"

namespace CSharpServer {

    void UdpClientEx::onConnected()
    {
        root->InternalOnConnected();
    }

    void UdpClientEx::onDisconnected()
    {
        root->InternalOnDisconnected();
    }

    void UdpClientEx::onJoinedMulticastGroup(const std::string& address)
    {
        root->InternalOnJoinedMulticastGroup(marshal_as<String^>(address));
    }

    void UdpClientEx::onLeftMulticastGroup(const std::string& address)
    {
        root->InternalOnLeftMulticastGroup(marshal_as<String^>(address));
    }

    void UdpClientEx::onReceived(const asio::ip::udp::endpoint& endpoint, const void* buffer, size_t size)
    {
        array<Byte>^ bytes = gcnew array<Byte>((int)size);
        pin_ptr<Byte> ptr = &bytes[bytes->GetLowerBound(0)];
        memcpy(ptr, buffer, size);
        root->_receive_endpoint->_endpoint.Assign((asio::ip::udp::endpoint*)&endpoint);
        root->InternalOnReceived(root->_receive_endpoint, bytes, size);
        root->_receive_endpoint->_endpoint.Assign(nullptr);
    }

    void UdpClientEx::onSent(const asio::ip::udp::endpoint& endpoint, size_t sent)
    {
        root->_send_endpoint->_endpoint.Assign((asio::ip::udp::endpoint*)&endpoint);
        root->InternalOnSent(root->_send_endpoint, sent);
        root->_send_endpoint->_endpoint.Assign(nullptr);
    }

    void UdpClientEx::onError(int error, const std::string& category, const std::string& message)
    {
        String^ cat = marshal_as<String^>(category);
        String^ msg = marshal_as<String^>(message);
        root->InternalOnError(errno, cat, msg);
    }

    UdpClient::UdpClient(CSharpServer::Service^ service, String^ address, int port) :
        _service(service),
        _client(new std::shared_ptr<UdpClientEx>(std::make_shared<UdpClientEx>(service->_service.Value, marshal_as<std::string>(address), port))),
        _receive_endpoint(gcnew UdpEndpoint()),
        _send_endpoint(gcnew UdpEndpoint())
    {
        _client->get()->root = this;
    }

    UdpClient::UdpClient(CSharpServer::Service^ service, String^ address, String^ scheme) :
        _service(service),
        _client(new std::shared_ptr<UdpClientEx>(std::make_shared<UdpClientEx>(service->_service.Value, marshal_as<std::string>(address), marshal_as<std::string>(scheme)))),
        _receive_endpoint(gcnew UdpEndpoint()),
        _send_endpoint(gcnew UdpEndpoint())
    {
        _client->get()->root = this;
    }

    UdpClient::UdpClient(CSharpServer::Service^ service, UdpEndpoint^ endpoint) :
        _service(service),
        _client(new std::shared_ptr<UdpClientEx>(std::make_shared<UdpClientEx>(service->_service.Value, endpoint->_endpoint.Value))),
        _receive_endpoint(gcnew UdpEndpoint()),
        _send_endpoint(gcnew UdpEndpoint())
    {
        _client->get()->root = this;
    }

}
