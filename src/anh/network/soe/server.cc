/*
 This file is part of SWGANH. For more information, visit http://swganh.com
 
 Copyright (c) 2006 - 2011 The SWG:ANH Team

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "anh/network/soe/server.h"

#include <boost/pool/pool_alloc.hpp>
#include <glog/logging.h>

#include "anh/byte_buffer.h"
#include "anh/event_dispatcher.h"

#include "anh/network/soe/packet.h"
#include "anh/network/soe/session.h"
#include "anh/network/soe/filters/receive_packet_filter.h"
#include "anh/network/soe/filters/crc_in_filter.h"
#include "anh/network/soe/filters/decryption_filter.h"
#include "anh/network/soe/filters/decompression_filter.h"
#include "anh/network/soe/filters/soe_protocol_filter.h"
#include "anh/network/soe/filters/outgoing_start_filter.h"
#include "anh/network/soe/filters/compression_filter.h"
#include "anh/network/soe/filters/crc_out_filter.h"
#include "anh/network/soe/filters/encryption_filter.h"
#include "anh/network/soe/filters/send_packet_filter.h"
#include "anh/network/soe/filters/security_filter.h"

using namespace anh;
using namespace network::soe;
using namespace filters;
using namespace std;
using namespace tbb;
using boost::asio::ip::udp;

Server::Server(boost::asio::io_service& io_service, EventDispatcher* event_dispatcher, MessageHandler message_handler)
    : io_service_(io_service)
    , socket_(io_service)
    , event_dispatcher_(event_dispatcher)
    , crc_seed_(0xDEADBABE)
    , active_(io_service)
    , message_handler_(message_handler)
    , max_receive_size_(496)
{}

Server::~Server(void)
{	
}

void Server::Start(uint16_t port)
{
    incoming_filter_ = 
        make_filter<void, shared_ptr<Packet>>(filter::serial_in_order, ReceivePacketFilter(incoming_messages_)) &
        make_filter<shared_ptr<Packet>, shared_ptr<Packet>>(filter::parallel, SecurityFilter(max_receive_size_)) &
        make_filter<shared_ptr<Packet>, shared_ptr<Packet>>(filter::parallel, CrcInFilter()) &
        make_filter<shared_ptr<Packet>, shared_ptr<Packet>>(filter::parallel, DecryptionFilter()) &
        make_filter<shared_ptr<Packet>, shared_ptr<Packet>>(filter::parallel, DecompressionFilter()) &
        make_filter<shared_ptr<Packet>, void>(filter::serial_in_order, SoeProtocolFilter());
        
    outgoing_filter_ = 
        make_filter<void, shared_ptr<Packet>>(filter::serial_in_order, OutgoingStartFilter(outgoing_messages_)) &
        make_filter<shared_ptr<Packet>, shared_ptr<Packet>>(filter::parallel, CompressionFilter()) &
        make_filter<shared_ptr<Packet>, shared_ptr<Packet>>(filter::parallel, EncryptionFilter()) &
        make_filter<shared_ptr<Packet>, shared_ptr<Packet>>(filter::parallel, CrcOutFilter()) &
        make_filter<shared_ptr<Packet>, void>(filter::serial_in_order, SendPacketFilter(this));
    
    socket_.open(udp::v4());
    socket_.bind(udp::endpoint(udp::v4(), port));
    
    AsyncReceive();

    active_.AsyncRepeated(boost::posix_time::microseconds(1), [this] () {
        parallel_pipeline(1000, incoming_filter_);
        parallel_pipeline(1000, outgoing_filter_);

        session_manager_.Update();
    });
}

void Server::Shutdown(void) {
    socket_.close();
}
    
void Server::SendMessage(shared_ptr<Session> session, const shared_ptr<ByteBuffer>& message) {    
    outgoing_messages_.push(make_shared<Packet>(session, message));
}

void Server::SendTo(const boost::asio::ip::udp::endpoint& endpoint, const shared_ptr<ByteBuffer>& buffer) {
    socket_.async_send_to(boost::asio::buffer(buffer->data(), buffer->size()), 
        endpoint, 
        [this, buffer](const boost::system::error_code& error, std::size_t bytes_transferred)
    {
        if (bytes_transferred == 0) {
            DLOG(WARNING) << "Sent 0 bytes";
        }

        bytes_sent_ += bytes_transferred;
    });
}

void Server::HandleMessage(shared_ptr<Packet> packet) {    
    message_handler_(packet);
}

void Server::AsyncReceive() {
    socket_.async_receive_from(boost::asio::buffer(&recv_buffer_[0], recv_buffer_.size()), current_remote_endpoint_,
        [this] (const boost::system::error_code& error, std::size_t bytes_transferred) {
            if(bytes_transferred > 2 || !error || error == boost::asio::error::message_size)
            {
                bytes_recv_ += bytes_transferred;
                OnSocketRecv_(current_remote_endpoint_, std::make_shared<ByteBuffer>((const unsigned char*)recv_buffer_.data(), bytes_transferred));
            }

            AsyncReceive();
    });
}
void Server::OnSocketRecv_(boost::asio::ip::udp::endpoint remote_endpoint, const std::shared_ptr<anh::ByteBuffer>& message) {
    // Query the SessionManager for the Session.
    std::shared_ptr<Session> session = session_manager_.GetSession(remote_endpoint);

    // If the Session doesnt exist, check for a Session Requesst.
    if(session == nullptr) {
        session = make_shared<Session>(remote_endpoint, this);
    }
        
    incoming_messages_.push(make_shared<Packet>(session, message));
}

bool Server::AddSession(std::shared_ptr<Session> session) {
    if (session_manager_.AddSession(session)) {
        event_dispatcher_->Dispatch(make_shared<ValueEvent<shared_ptr<Session>>>("NetworkSessionAdded", session));
        return true;
    }

    return false;
}

bool Server::RemoveSession(std::shared_ptr<Session> session) {
    if (session_manager_.RemoveSession(session)) {
        event_dispatcher_->Dispatch(make_shared<ValueEvent<shared_ptr<Session>>>("NetworkSessionRemoved", session));
        return true;
    }

    return false;
}

std::shared_ptr<Session> Server::GetSession(boost::asio::ip::udp::endpoint& endpoint) {
    return session_manager_.GetSession(endpoint);
}

boost::asio::ip::udp::socket* Server::socket() {
    return &socket_;
}

uint32_t Server::max_receive_size() {
    return max_receive_size_;
}

shared_ptr<ByteBuffer> Server::AllocateBuffer() {    
    auto allocated_buffer = allocate_shared<ByteBuffer, boost::pool_allocator<ByteBuffer>>(
        boost::pool_allocator<ByteBuffer>());

    return allocated_buffer;
}
