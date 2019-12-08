/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 *  Copyright (c) 2007,2008, 2009 INRIA, UDcast
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Mohamed Amine Ismail <amine.ismail@sophia.inria.fr>
 *                              <amine.ismail@udcast.com>
 */

#include <fstream>
#include "ns3/log.h"
#include "ns3/abort.h"
#include "ns3/config.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"
#include "ns3/inet-socket-address.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/udp-client-server-helper.h"
#include "ns3/udp-echo-helper.h"
#include "ns3/simple-net-device.h"
#include "ns3/simple-channel.h"
#include "ns3/test.h"
#include "ns3/simulator.h"

using namespace ns3;

/**
 * \ingroup applications
 * \defgroup applications-test applications module tests
 */

/**
 * \ingroup applications-test
 * \ingroup tests
 *
 * Test that all the UDP packets generated by an UdpClient application are
 * correctly received by an UdpServer application
 */
class UdpClientServerTestCase : public TestCase
{
public:
  UdpClientServerTestCase ();
  virtual ~UdpClientServerTestCase ();

private:
  virtual void DoRun (void);

};

UdpClientServerTestCase::UdpClientServerTestCase ()
  : TestCase ("Test that all the udp packets generated by an udpClient application are correctly received by an udpServer application")
{
}

UdpClientServerTestCase::~UdpClientServerTestCase ()
{
}

void UdpClientServerTestCase::DoRun (void)
{
  NodeContainer n;
  n.Create (2);

  InternetStackHelper internet;
  internet.Install (n);

  // link the two nodes
  Ptr<SimpleNetDevice> txDev = CreateObject<SimpleNetDevice> ();
  Ptr<SimpleNetDevice> rxDev = CreateObject<SimpleNetDevice> ();
  n.Get (0)->AddDevice (txDev);
  n.Get (1)->AddDevice (rxDev);
  Ptr<SimpleChannel> channel1 = CreateObject<SimpleChannel> ();
  rxDev->SetChannel (channel1);
  txDev->SetChannel (channel1);
  NetDeviceContainer d;
  d.Add (txDev);
  d.Add (rxDev);

  Ipv4AddressHelper ipv4;

  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i = ipv4.Assign (d);

  uint16_t port = 4000;
  UdpServerHelper server (port);
  ApplicationContainer apps = server.Install (n.Get (1));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (10.0));

  uint32_t MaxPacketSize = 1024;
  Time interPacketInterval = Seconds (1.);
  uint32_t maxPacketCount = 10;
  UdpClientHelper client (i.GetAddress (1), port);
  client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  apps = client.Install (n.Get (0));
  apps.Start (Seconds (2.0));
  apps.Stop (Seconds (10.0));

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (server.GetServer ()->GetLost (), 0, "Packets were lost !");
  NS_TEST_ASSERT_MSG_EQ (server.GetServer ()->GetReceived (), 8, "Did not receive expected number of packets !");
}

/**
 * Test that all the udp packets generated by an udpTraceClient application are
 * correctly received by an udpServer application
 */

class UdpTraceClientServerTestCase : public TestCase
{
public:
  UdpTraceClientServerTestCase ();
  virtual ~UdpTraceClientServerTestCase ();

private:
  virtual void DoRun (void);

};

UdpTraceClientServerTestCase::UdpTraceClientServerTestCase ()
  : TestCase ("Test that all the udp packets generated by an udpTraceClient application are correctly received by an udpServer application")
{
}

UdpTraceClientServerTestCase::~UdpTraceClientServerTestCase ()
{
}

void UdpTraceClientServerTestCase::DoRun (void)
{
  NodeContainer n;
  n.Create (2);

  InternetStackHelper internet;
  internet.Install (n);

  // link the two nodes
  Ptr<SimpleNetDevice> txDev = CreateObject<SimpleNetDevice> ();
  Ptr<SimpleNetDevice> rxDev = CreateObject<SimpleNetDevice> ();
  n.Get (0)->AddDevice (txDev);
  n.Get (1)->AddDevice (rxDev);
  Ptr<SimpleChannel> channel1 = CreateObject<SimpleChannel> ();
  rxDev->SetChannel (channel1);
  txDev->SetChannel (channel1);
  NetDeviceContainer d;
  d.Add (txDev);
  d.Add (rxDev);

  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i = ipv4.Assign (d);

  uint16_t port = 4000;
  UdpServerHelper server (port);
  ApplicationContainer apps = server.Install (n.Get (1));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (10.0));

  uint32_t MaxPacketSize = 1400 - 28; // ip/udp header
  UdpTraceClientHelper client (i.GetAddress (1), port,"");
  client.SetAttribute ("MaxPacketSize", UintegerValue (MaxPacketSize));
  apps = client.Install (n.Get (0));
  apps.Start (Seconds (2.0));
  apps.Stop (Seconds (10.0));

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (server.GetServer ()->GetLost (), 0, "Packets were lost !");
  NS_TEST_ASSERT_MSG_EQ (server.GetServer ()->GetReceived (), 247, "Did not receive expected number of packets !");
}


/**
 * Test that all the PacketLossCounter class checks loss correctly in different cases
 */

class PacketLossCounterTestCase : public TestCase
{
public:
  PacketLossCounterTestCase ();
  virtual ~PacketLossCounterTestCase ();

private:
  virtual void DoRun (void);

};

PacketLossCounterTestCase::PacketLossCounterTestCase ()
  : TestCase ("Test that all the PacketLossCounter class checks loss correctly in different cases")
{
}

PacketLossCounterTestCase::~PacketLossCounterTestCase ()
{
}

void PacketLossCounterTestCase::DoRun (void)
{
  PacketLossCounter lossCounter (32);
  lossCounter.NotifyReceived (32); //out of order
  for (uint32_t i = 0; i < 64; i++)
    {
      lossCounter.NotifyReceived (i);
    }

  NS_TEST_ASSERT_MSG_EQ (lossCounter.GetLost (), 0, "Check that 0 packets are lost");

  for (uint32_t i = 65; i < 128; i++) // drop (1) seqNum 64
    {
      lossCounter.NotifyReceived (i);
    }
  NS_TEST_ASSERT_MSG_EQ (lossCounter.GetLost (), 1, "Check that 1 packet is lost");

  for (uint32_t i = 134; i < 200; i++) // drop seqNum 128,129,130,131,132,133
    {
      lossCounter.NotifyReceived (i);
    }
  NS_TEST_ASSERT_MSG_EQ (lossCounter.GetLost (), 7, "Check that 7 (6+1) packets are lost");

  // reordering without loss
  lossCounter.NotifyReceived (205);
  lossCounter.NotifyReceived (206);
  lossCounter.NotifyReceived (207);
  lossCounter.NotifyReceived (200);
  lossCounter.NotifyReceived (201);
  lossCounter.NotifyReceived (202);
  lossCounter.NotifyReceived (203);
  lossCounter.NotifyReceived (204);
  for (uint32_t i = 205; i < 250; i++)
    {
      lossCounter.NotifyReceived (i);
    }
  NS_TEST_ASSERT_MSG_EQ (lossCounter.GetLost (), 7, "Check that 7 (6+1) packets are lost even when reordering happens");

  // reordering with loss
  lossCounter.NotifyReceived (255);
  // drop (2) seqNum 250, 251
  lossCounter.NotifyReceived (252);
  lossCounter.NotifyReceived (253);
  lossCounter.NotifyReceived (254);
  for (uint32_t i = 256; i < 300; i++)
    {
      lossCounter.NotifyReceived (i);
    }
  NS_TEST_ASSERT_MSG_EQ (lossCounter.GetLost (), 9, "Check that 9 (6+1+2) packet are lost");
}

/**
 * Test fix for \bugid{1378}
 */

class UdpEchoClientSetFillTestCase : public TestCase
{
public:
  UdpEchoClientSetFillTestCase ();
  virtual ~UdpEchoClientSetFillTestCase ();

private:
  virtual void DoRun (void);

};

UdpEchoClientSetFillTestCase::UdpEchoClientSetFillTestCase ()
  : TestCase ("Test that the UdpEchoClient::SetFill class sets packet size (bug 1378)")
{
}

UdpEchoClientSetFillTestCase::~UdpEchoClientSetFillTestCase ()
{
}

void UdpEchoClientSetFillTestCase::DoRun (void)
{
  NodeContainer nodes;
  nodes.Create (2);

  InternetStackHelper internet;
  internet.Install (nodes);

  Ptr<SimpleNetDevice> txDev = CreateObject<SimpleNetDevice> ();
  Ptr<SimpleNetDevice> rxDev = CreateObject<SimpleNetDevice> ();
  nodes.Get (0)->AddDevice (txDev);
  nodes.Get (1)->AddDevice (rxDev);
  Ptr<SimpleChannel> channel1 = CreateObject<SimpleChannel> ();
  rxDev->SetChannel (channel1);
  txDev->SetChannel (channel1);
  NetDeviceContainer d;
  d.Add (txDev);
  d.Add (rxDev);

  Ipv4AddressHelper ipv4;

  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = ipv4.Assign (d);

  uint16_t port = 5000;
  UdpEchoServerHelper echoServer (port);
  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));
  UdpEchoClientHelper echoClient (interfaces.GetAddress (1), port);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));

  uint8_t arry[64];
  uint8_t i;
  for (i = 0; i < 64; i++)
    {
      arry[i] = i;
    }
  echoClient.SetFill (clientApps.Get (0), &(arry[0]), (uint32_t)64, (uint32_t)64);

  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  Simulator::Run ();
  Simulator::Destroy ();
}



/**
 * \ingroup applications-test
 * \ingroup tests
 *
 * \brief UDP client and server TestSuite
 */
class UdpClientServerTestSuite : public TestSuite
{
public:
  UdpClientServerTestSuite ();
};

UdpClientServerTestSuite::UdpClientServerTestSuite ()
  : TestSuite ("udp-client-server", UNIT)
{
  AddTestCase (new UdpTraceClientServerTestCase, TestCase::QUICK);
  AddTestCase (new UdpClientServerTestCase, TestCase::QUICK);
  AddTestCase (new PacketLossCounterTestCase, TestCase::QUICK);
  AddTestCase (new UdpEchoClientSetFillTestCase, TestCase::QUICK);
}

static UdpClientServerTestSuite udpClientServerTestSuite; //!< Static variable for test initialization