
#include "ACARS.h"
#include "Ieee80211DataRate.h"

using namespace std;

ACARS::ACARS(char opMode)
{
    this->opMode = opMode;
    current_SNR = 0;
    //Initialize the snr map
    int min_idx_rate = Ieee80211Descriptor::getMinIdx(opMode);
    int max_idx_rate = Ieee80211Descriptor::getMaxIdx(opMode);

    int idx_rate = min_idx_rate;
    while (idx_rate <= max_idx_rate)
    {
        mapLastSNR[idx_rate] = 0.0;
        idx_rate++;
    }
}


/**
 * Select the best bitrate using ACARS (Adapted Context-aware rate selection) algorithm.
 * POST: res = indexBest_rate in Ieee80211DataDescriptor
 */
double ACARS::getBitRate(int rateIndex, double snr)
{
    double idx_BestBitrate;
    double l_snr;

    this->current_SNR = snr;
    //insert the snr in the map with the index of the bitrate
    mapLastSNR.at(rateIndex) = snr;

    //Loop on the bitrates to check the snr
    int min_idx_rate = Ieee80211Descriptor::getMinIdx(opMode);
    int max_idx_rate = Ieee80211Descriptor::getMaxIdx(opMode);
    int idx_rate = min_idx_rate;

    bool endSearch = false;
    while (idx_rate <= max_idx_rate and !endSearch)
    {
        l_snr = mapLastSNR[idx_rate];
        if (l_snr == 0 || l_snr > snr)
        {
            idx_BestBitrate = idx_rate;
            endSearch = true;
        }
        if(!endSearch)
            idx_rate++;
    }//end while

    if(idx_BestBitrate != this->idxBitrate_currentSNR)
    {
        this->idxBitrate_currentSNR = idx_BestBitrate;
    }

    return idx_BestBitrate;
}


ACARS::~ACARS() {
}

