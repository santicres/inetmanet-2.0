
#ifndef CARS_H
#define	CARS_H
//#include <iostream>
#include <math.h>
#include <map>
#include <vector>
#include "Coord.h"

class CARS {
public:

    //Methods should be called to initialize parameter at the begining
    void   setUpdateInterval(double val);//Update interval in ms
    void   setMaxRetransmissions(long a);
    double cars_getBitRate(double speed,long len);//select and return the BitRate should use to
                //transmit Pacts
    double getBitRate(double context_information, double alpha_weight, int packet_length);
    double getAlpha(double speed);
    void   setFirstPER(double val, double rate);//val = PER, rate = p band rate in Mbps
    void   setCurrentPER(double val, double rate);//val = PER, rate = p band rate in Mbps

    void   updatePositions(double speed1,double angle1, double speed2, double angle2);//Update Positions
            //of Vehicles using speed and angles; speed should be in
            //meters per second (ms^-1) and angles should be in degrees
    void   setPositions(double px1, double py1, double px2, double py2);//Directly update positions

    void   setNoRetransmissions(int val);

    double getDistance(Coord node1, Coord node2);
    void   setPrevious_Bitrate(double bitrate);
    void   setPreviousIdx(double idx);

    CARS(char opMode, int transmissionLimit);
    virtual ~CARS();


private:
    double update_interval;//Update interval in ms (milliseconds)
    long N; //the maximum number of retransmissions
    int sigma; //weight that signifies the penalty given to unsuccessful packet transmission
    double first_PER, previous_PER;
    double previous_distance, previous_bitrate;
    double current_distance;
    double x1,y1,x2,y2;
    int no_retransmissions;
    double previous_rate;

    double p_band_rates[8];
    double last_PERs[8][10];//Last 10 PERs of each data rate
    int no_last_PERs[8];//Available amount of data PER per each rate
    char opMode;//from Iee802Mac
    std::map<int,std::vector<double> > mapLastPERs;//Last 10 PERs of each data rate. map<idx,vector of PERs>. vector(PER) is ordered ascending as a queue(newer at the end).
    int transmissionLimit;//from Iee802Mac
    double previous_bitrateIdx;//One bitrate before that the current bitrate. Debe ser un idx.



    int    getTotal(int val);//this return value = (2^val) + (2^(val-1)) + (2^(val-2)) + ... + 2^2 + 2^1
    double getCumulativeTotal(int index,int val);
    double getTotalAverage();//In initial situations this mothod has to be called,
                //in order to get PER depending on past data
    double updateDistance();
    double Eh(double rate, double packet_length);
    double Ec(double speed, double rate, double packet_length);
    double getCumulativeTotalEWMA(int idx_rate,std::vector<double> lastPERs);
    double getTotalEWMA(int val);
};

#endif	/* CARS_H */

