//
// Copyright (C) 2005 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//


package inet.examples.wireless.rateadaptation.arf;

import inet.mobility.single.RandomWPMobility;
import inet.applications.ethernet.EtherAppCli;
import inet.base.NotificationBoard;
import inet.base.Sink;
import inet.examples.wireless.throughput.WirelessAPWithSink;
import inet.linklayer.ieee80211.Ieee80211Nic;
import inet.mobility.contract.IMobility;
import inet.mobility.single.LinearMobility;
import inet.nodes.inet.WirelessHost;
import inet.nodes.wireless.AccessPoint;
import inet.world.radio.ChannelControl;
import inet.networklayer.autorouting.ipv4.IPv4NetworkConfigurator;

module ThroughputClient
{
    parameters:
        @display("i=device/wifilaptop;bgb=209,199");
        @node();
        string mobilityType = default("LinearMobility"); // how the node moves around. see: inet.mobility
    gates:
        input radioIn @directIn;

    submodules:
        notificationBoard: NotificationBoard {
            parameters:
                @display("p=52,32");
        }
        cli: EtherAppCli {
            parameters:
                registerSAP = false;
                @display("b=40,24;p=180,60,col");
        }
        mobility: <mobilityType> like IMobility {
            //randomWPMobility: RandomWPMobility {
            @display("p=52,134");
            //speed = uniform(16, 28);//50kmh - 100kmh
            //speed = 2mps;
        }
        wlan: Ieee80211Nic {
            @display("p=126,132");
            mgmtType = "Ieee80211MgmtSTASimplified";

        }
    connections allowunconnected:
        cli.out --> wlan.upperLayerIn;
        wlan.radioIn <-- radioIn;
}

network ThroughputNetwork
{
    parameters:
        int numCli;
        //        double playgroundSizeX;
        //        double playgroundSizeY;
        @display("b=297,203");
    submodules:
        channelControl: ChannelControl {
            @display("p=52,44");
        }
        wirelessMobilityHost[numCli]: WirelessHost {
            @display("p=307,54");
        }
        configurator: IPv4NetworkConfigurator {
            config = xml("<config><interface hosts='*' address='145.236.x.x' netmask='255.255.0.0'/></config>");
            @display("p=140,50");
        }
        accessPoint: AccessPoint {
            @display("p=178,96");
        }
}

