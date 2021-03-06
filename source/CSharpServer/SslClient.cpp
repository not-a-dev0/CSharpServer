#include "stdafx.h"

#include "SslClient.h"

namespace CSharpServer {

    void SslClientEx::onConnected()
    {
        root->InternalOnConnected();
    }

    void SslClientEx::onHandshaked()
    {
        root->InternalOnHandshaked();
    }

    void SslClientEx::onDisconnected()
    {
        root->InternalOnDisconnected();
    }

    void SslClientEx::onReceived(const void* buffer, size_t size)
    {
        array<Byte>^ bytes = gcnew array<Byte>((int)size);
        pin_ptr<Byte> ptr = &bytes[bytes->GetLowerBound(0)];
        memcpy(ptr, buffer, size);
        root->InternalOnReceived(bytes, size);
    }

    void SslClientEx::onSent(size_t sent, size_t pending)
    {
        root->InternalOnSent(sent, pending);
    }

    void SslClientEx::onEmpty()
    {
        root->InternalOnEmpty();
    }

    void SslClientEx::onError(int error, const std::string& category, const std::string& message)
    {
        String^ cat = marshal_as<String^>(category);
        String^ msg = marshal_as<String^>(message);
        root->InternalOnError(errno, cat, msg);
    }

    SslClient::SslClient(CSharpServer::Service^ service, SslContext^ context, String^ address, int port) :
        _service(service),
        _context(context),
        _client(new std::shared_ptr<SslClientEx>(std::make_shared<SslClientEx>(service->_service.Value, context->_context.Value, marshal_as<std::string>(address), port)))
    {
        _client->get()->root = this;
    }

    SslClient::SslClient(CSharpServer::Service^ service, SslContext^ context, String^ address, String^ scheme) :
        _service(service),
        _context(context),
        _client(new std::shared_ptr<SslClientEx>(std::make_shared<SslClientEx>(service->_service.Value, context->_context.Value, marshal_as<std::string>(address), marshal_as<std::string>(scheme))))
    {
        _client->get()->root = this;
    }

    SslClient::SslClient(CSharpServer::Service^ service, SslContext^ context, TcpEndpoint^ endpoint) :
        _service(service),
        _context(context),
        _client(new std::shared_ptr<SslClientEx>(std::make_shared<SslClientEx>(service->_service.Value, context->_context.Value, endpoint->_endpoint.Value)))
    {
        _client->get()->root = this;
    }

}
