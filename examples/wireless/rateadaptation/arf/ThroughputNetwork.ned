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
import inet.nodes.inet.AdhocHost;
import inet.nodes.wireless.AccessPoint;
import inet.world.radio.ChannelControl;


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

module ThroughputServer
{
    parameters:
        @display("i=device/wifilaptop");
        @node();
    gates:
        input radioIn @directIn;

    submodules:
        notificationBoard: NotificationBoard {
            parameters:
                @display("p=60,70");
        }
        sink: Sink {
            parameters:
                @display("p=210,68,col");
        }
        ieee80211Nic: Ieee80211Nic {
            @display("p=141,113");
            mgmtType = "Ieee80211MgmtSTASimplified";
        }
    connections allowunconnected:
        sink.in++ <-- ieee80211Nic.upperLayerOut;
        radioIn --> ieee80211Nic.radioIn;
}

network ThroughputNetwork
{
    parameters:
        int numCli;
        //        double playgroundSizeX;
        //        double playgroundSizeY;
        @display("b=297,203");
    submodules:
        cliHost[numCli]: ThroughputClient {
            parameters:
                @display("r=,,#707070");
        }
        srvHost: ThroughputServer {
            parameters:
                @display("p=350,350;r=,,#707070");
        }
        channelControl: ChannelControl {
            @display("p=52,44");
        }
        accessPoint: AccessPoint {
            parameters:
            	wlan[*].mgmtType = "Ieee80211MgmtAPSimplified";
            	@display("p=230,154");
        }
}
