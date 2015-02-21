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

#include "SampleRate.h"
#include "Ieee80211DataRate.h"

SampleRate::SampleRate(char currentOpMode) {
    opMode = currentOpMode;
    currentIdxBitrate = -1;

    //Initialize the map with rate statistics of each opMode
    int min_idx_rate = Ieee80211Descriptor::getMinIdx(opMode);
    int max_idx_rate = Ieee80211Descriptor::getMaxIdx(opMode);

    for(int idx_rate=min_idx_rate;idx_rate <= max_idx_rate;idx_rate++)
    {
        RateStats elem_rateStats;
        elem_rateStats.average_tx_time = 0;
        elem_rateStats.last_tx = 0;
        elem_rateStats.packets_acked = 0;
        elem_rateStats.retries = 0;
        elem_rateStats.perfect_tx_time = 0;
        elem_rateStats.successive_failures = 0;
        elem_rateStats.total_packets = 0;

        mapRateStats[idx_rate] = elem_rateStats;
    }

}

SampleRate::~SampleRate() {

}

/**
 * IF packetsMOD10=0 THEN
 *  res = random(less transmission time from the list and without 4 successive failures on the list)
 *
 * IF exist(bitrates without 4 successive failures)
 *  res = select the minimum transmission time
 *
 * ELSE
 *  res = select the minimum transmission time
 *  reset the successive failures
 *
 */
int SampleRate::getBitRate()
{
    int idxBitRate_result = -1;
    RateStats *bitrate_stats;

    //the first bitrate is the higher
    if(currentIdxBitrate == -1)
    {
        idxBitRate_result = Ieee80211Descriptor::getMaxIdx(opMode);
    }
    else
    {
        //Get the current bitrate stats
        bitrate_stats = &mapRateStats[currentIdxBitrate];
        if(bitrate_stats->packets_acked != 0 && bitrate_stats->packets_acked%10 == 0)
        {//Get the random bitrate from the less transmision time
            idxBitRate_result = selectBitRateMinTransmssionTime(true);
        }
        else//Get the bitrate with minimum transmission time
            idxBitRate_result = selectBitRateMinTransmssionTime(false);

        if(idxBitRate_result == -1)
        {//all bitrates with 4 successive failures
            idxBitRate_result = selectBitRateMinTransmssionTime(false);
        }

    }

    currentIdxBitrate = idxBitRate_result;
    return idxBitRate_result;
}

int SampleRate::selectBitRateMinTransmssionTime(bool random)
{
    int idxBitRate_result = -1;

    if(random)
    {
        int min_idx_rate = Ieee80211Descriptor::getMinIdx(opMode);
        int max_idx_rate = Ieee80211Descriptor::getMaxIdx(opMode);
        int ratesLessTime[max_idx_rate - min_idx_rate];

        RateStats *select_bitrate_stats;
        RateStats *current_bitrate_stats = &mapRateStats[currentIdxBitrate];

        int i = 0;//counter of less transmission time bitrates to select one randomly
        for(int idx_rate=min_idx_rate;idx_rate <= max_idx_rate;idx_rate++)
        {
            select_bitrate_stats = &mapRateStats[idx_rate];
            if(idx_rate != currentIdxBitrate &&
                    (select_bitrate_stats->average_tx_time < current_bitrate_stats->average_tx_time || select_bitrate_stats->average_tx_time == 0))
            {
               ratesLessTime[i++] = idx_rate;
            }
        }

        int random_index = rand()%i;
        idxBitRate_result = ratesLessTime[random_index];//random between the bitrates with less time

    }
    else
    {
        int min_idx_rate = Ieee80211Descriptor::getMinIdx(opMode);
        int max_idx_rate = Ieee80211Descriptor::getMaxIdx(opMode);
        RateStats *select_bitrate_stats;
        RateStats *current_bitrate_stats = &mapRateStats[currentIdxBitrate];

        for(int idx_rate=min_idx_rate;idx_rate <= max_idx_rate;idx_rate++)
        {
            select_bitrate_stats = &mapRateStats[idx_rate];
            if(idx_rate != currentIdxBitrate &&
                    (select_bitrate_stats->average_tx_time < current_bitrate_stats->average_tx_time || select_bitrate_stats->average_tx_time == 0))
            {
               idxBitRate_result = idx_rate;
            }

        }
        if(idxBitRate_result == -1)
        {//Follow with the same bitrate because has the minimum transmission time
            idxBitRate_result = currentIdxBitrate;
        }
    }

    return idxBitRate_result;
}
void SampleRate::reportDataOk(double transmissionTime)
{
    int idxBitRate;

    if(currentIdxBitrate == -1)//First case
    {
        idxBitRate = Ieee80211Descriptor::getMaxIdx(opMode);
    }
    else
        idxBitRate = currentIdxBitrate;


    RateStats *current_bitrate_stats = &mapRateStats[idxBitRate];
    current_bitrate_stats->packets_acked++;
    current_bitrate_stats->total_packets++;
    current_bitrate_stats->successive_failures = 0;

    if(current_bitrate_stats->retries > 0)
        //Average transmission time. Take a count the retries with transmission time.
        current_bitrate_stats->average_tx_time = (current_bitrate_stats->average_tx_time + (transmissionTime*current_bitrate_stats->retries)) / current_bitrate_stats->packets_acked;
    else
    {
        //Average transmission time
        current_bitrate_stats->average_tx_time = (current_bitrate_stats->average_tx_time + transmissionTime) / current_bitrate_stats->packets_acked;
    }
    current_bitrate_stats->retries = 0;

}
void SampleRate::reportDataFailed()
{//TODO: posiblemente haya que meter como parámetro el transmissiontime con los reintentos
    RateStats *current_bitrate_stats = &mapRateStats[currentIdxBitrate];
    current_bitrate_stats->successive_failures++;

}

void SampleRate::reportRecoveryFailure()
{//TODO: posiblemente haya que meter como parámetro el transmissiontime con los reintentos
    RateStats *current_bitrate_stats = &mapRateStats[currentIdxBitrate];

    current_bitrate_stats->retries++;
    current_bitrate_stats->successive_failures++;
}
