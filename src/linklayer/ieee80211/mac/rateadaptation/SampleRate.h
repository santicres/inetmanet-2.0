//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef SAMPLERATE_H_
#define SAMPLERATE_H_
#include <map>
#include <iostream>

struct RateStats
{
    unsigned average_tx_time;
    int successive_failures;
    int retries;
    int total_packets;
    int packets_acked;
    unsigned perfect_tx_time; /* transmit time for 0 retries */
    int last_tx;
};

class SampleRate {
private:
    std::map<int,RateStats> mapRateStats; //map(bitrate,RateStats)
    char opMode;
    int currentIdxBitrate;

    int selBitRateMinTransTime();
public:
    SampleRate(char currentOpMode);
    virtual ~SampleRate();

    /**
     * Check the best bitrate that meet the algorithm conditions.
     * Res: idx_bitrate
     */
    int getBitRate();

    /**
     * Look for the bitrate with minimum transmission time on the list. The candidates have not 4 successive failures.
     * If random = true then get randomly from the bitrates with less average transmission time.
     * If random = false then get the bitrate with minimum average transmission time
     * bitrate = -1 if all bitrates with successive failures equal 4
     */
    int selectBitRateMinTransmssionTime(bool random);
        /**
     * Reset the variables of algorithm
     */
    void reset();

    /**
     * Packet has been sent with success.
     * Reset the variables.
     */
    void reportDataOk(double transmissionTime);

    /**
     * Report of packet data failed
     */
    void reportDataFailed();

    /**
     * Report of recovery failure
     */
    void reportRecoveryFailure();


    std::string toString();


};

#endif /* SAMPLERATE_H_ */
