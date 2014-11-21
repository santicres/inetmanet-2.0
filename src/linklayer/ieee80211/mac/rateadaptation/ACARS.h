
#ifndef ACARS_H
#define	ACARS_H
#include <iostream>
#include <math.h>
#include <map>

class ACARS {
public:
    ACARS(char opMode);

    double getBitRate(int bitrate,double snr);
    double getSnrMap(int idxRate);

    ACARS(const ACARS& orig);
    virtual ~ACARS();

private:

    char opMode;
    std::map<int,double> mapLastSNR; //map(bitrate,snr)
    double current_SNR;
    double idxBitrate_currentSNR;

};

#endif	/* ACARS_H */

