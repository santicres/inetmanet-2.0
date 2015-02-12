
#include "CARS.h"
#include "Ieee80211DataRate.h"


using namespace std;

CARS::CARS(char opMode, int transmissionLimit) {
    sigma = 8;//This value  is set according to the CARS research Paper
    N = 4;//Default retransmissions
    first_PER = 0.5;
    previous_PER = 0.5; //Just a default value. This value should be changed initially by
                        // setFirstPER() and if possible in each circle by setCurrentPER()
    update_interval = 100; //default Update interval 100ms.

    //Default Psitions, these should change accordingly
    x1 = 0;
    y1 = 0;
    x2 = 0;
    y2 = 0;
    previous_distance = -1;//This is a default value, should be change as prgram runs.
    current_distance = -1;

    no_retransmissions = 0;
    previous_rate = 0;

    this->opMode = opMode;
    this->transmissionLimit = transmissionLimit;
}

CARS::~CARS() {
}

void CARS::setUpdateInterval(double val)//periodic update time interval in ms
{
    update_interval = val;
}

double CARS::getAlpha(double speed)//This speed is relative speed between two vehicles
{
    double S = 30.0;
    double alpha = 0.0;
    double temp1 = 0.0,temp2 = 0.0;

    if(speed < 0)
    {
        speed = (-1)*speed;
    }

    temp1 = speed/S;

    if(1 > temp1)
    {
        temp2 = temp1;
    }else
    {
        temp2 = 1;
    }

    if(0 > temp2)
    {
        alpha = 0.0;
    }else
    {
        alpha = temp2;
    }

    return alpha;
}

void CARS::setMaxRetransmissions(long a)
{
    N = a;
}


/**
 * Select the best bitrate using CARS (Adapted Context-aware rate selection) algorithm.
 * POST: res = best_rate in Ieee80211DataDescriptor
 */
double CARS::getBitRate(double context_information, double alpha_weight, int packet_length)
{
    double bitrate;
    double max_throughput = 0, throughput = 0;
    double PER;//Packet Error Rate
    double avg_retries, gamma = 8;

    int min_idx_rate = Ieee80211Descriptor::getMinIdx(opMode);
    int max_idx_rate = Ieee80211Descriptor::getMaxIdx(opMode);
    double best_rate = Ieee80211Descriptor::getDescriptor(min_idx_rate).bitrate;
//?update_distance?
    int idx_rate = min_idx_rate;
    while (idx_rate <= max_idx_rate)
    {
        bitrate = Ieee80211Descriptor::getDescriptor(idx_rate).bitrate;

        PER = (alpha_weight * Ec(context_information,bitrate,packet_length)) + ((1 - alpha_weight)*Eh(bitrate,packet_length));
        //insert the per in the map
        if(mapLastPERs.find(idx_rate) != mapLastPERs.end())//If idx_rate exist
        {
            mapLastPERs.at(idx_rate).push_back(PER);
        }
        else
        {
            std::vector<double> vectorPER;
            vectorPER.push_back(PER);
            mapLastPERs.insert(std::pair<int,std::vector<double> >(idx_rate,vectorPER));
            //std::pair<char,int>('a',100)
        }

        avg_retries = ((transmissionLimit * pow(PER,transmissionLimit + 1)) - ((transmissionLimit + 1)*pow(PER,transmissionLimit)) + 1) / ((1 - PER) + transmissionLimit * pow(PER,transmissionLimit));

        throughput = bitrate / (avg_retries * pow((1 - pow(PER,transmissionLimit)),gamma));//gamma will be 8 for a reseach CARS paper

        if (throughput > max_throughput)
        {
            best_rate = bitrate;
            max_throughput = throughput;
        }
        idx_rate++;
    }//end while

    return best_rate;

}

/**
 * The function EC uses the
context information, transmission rate and packet length as
input parameters, and outputs the estimated packet error rate.
Post: result = PER (packet error rate)
 */
double CARS::Ec(double speed, double rate, double packet_length)
{
    double PER = 0.0;
    double temp1 = 0.0, temp2 = 0.0;
    double previous_PER = 0.0;

    /*
     *  Effect of the distance and rate are modelled according
     *  to Free Space Path Loss Model. In this model PER propotional
     *  to the square of distance and frequency.
     *  Which means if distance and bitarate increase by factor of two,
     *  then PER will increase by factor of 16.
     *
    */

    temp1 = (rate/Ieee80211Descriptor::getDescriptor(previous_bitrateIdx).bitrate);
    temp1 = (temp1*temp1);

    //Effect of Distance
    if(previous_distance == 0)
    {
        temp2 = 1;
    }
    else
    {
        temp2 = (current_distance/previous_distance);
    }
    temp2 = (temp2*temp2);

    //previous_PER se obtiene desde el mapaq
    if(mapLastPERs.find(previous_bitrateIdx) != mapLastPERs.end())
    {
        previous_PER = mapLastPERs.at(previous_bitrateIdx).back();//coge el ?ltimo PER obtenido para este bitrate
        //TODO falta limitar el n?mero de PER para almacenar. Son 10 PER
    }
    else
    {
        previous_PER = 1;
    }
    PER = (previous_PER*temp1*temp2);

    return PER;
}




void CARS::setPositions(double px1, double py1, double px2, double py2)
{
    x1 = px1;
    y1 = py1;
    x2 = px2;
    y2 = py2;
}

void CARS::updatePositions(double speed1,double angle1, double speed2, double angle2)
{
    double l_PI = 3.14159265;

    double movement = 0.0;

    //Update Position of vehicle one
    movement = (speed1*update_interval)/1000.0;//Devide by 1000, because update interval in ms (milliseconds)

    x1 = x1 + (movement*cos(angle1*PI/180));
    y1 = y1 + (movement*sin(angle1*PI/180));

    //Update Position of vehicle two
    movement = (speed2*update_interval)/1000.0;//Devide by 1000, because update interval in ms (milliseconds)

    x2 = x2 + (movement*cos(angle2*PI/180));
    y2 = y2 + (movement*sin(angle2*PI/180));
}

double CARS::updateDistance()//Calculate Distance between two vehicles.
{
    double temp = 0.0;

    temp = ((x1-x2)*(x1-x2)) + ((y1-y2)*(y1-y2));
    temp = sqrt(temp);

    if(previous_distance <= 0)
    {
        previous_distance = temp;
    }else
    {
        previous_distance = current_distance;
    }
    current_distance = temp;
    return temp;
}

void CARS::setNoRetransmissions(int val)
{
    no_retransmissions = val;
}


/**
 * The function EH uses an exponentially weighted
moving average (EWMA) of past frame transmission statistics
for each bitrate, similar to schemes such as SampleRate [7].
PER (packet error rate)
EWMA with PER:

PRE: rate in (Ieee80211DateRate)
POST: PER (rate)_currentperiod = error packets / sending packets
    result = PER = alpha * PER(rate)_currentperiod + (1- alpha)*PER(rate)_periodsbefore
 */
double CARS::Eh(double rate, double packet_length)
{
    double PER = 0.0;
    double temp2 = 0.0;
    double temp3 = 0.0;

    int min_idx_rate = Ieee80211Descriptor::getMinIdx(opMode);
    int max_idx_rate = Ieee80211Descriptor::getMaxIdx(opMode);

    int idx_rate = min_idx_rate;
    while (idx_rate <= max_idx_rate)
    {//Recorre los bitrates de un opMode
        if(Ieee80211Descriptor::getDescriptor(idx_rate).bitrate == rate)
        {
            if(mapLastPERs.find(idx_rate) != mapLastPERs.end())
            {
                std::vector<double> lastPERs = mapLastPERs.at(idx_rate);
                temp2 = getCumulativeTotalEWMA(idx_rate,lastPERs);
                temp3 = getTotalEWMA(lastPERs.size());
                PER = temp2 / temp3;
            }
        }
        idx_rate++;
    }

    return PER;
}


int CARS::getTotal(int val)
{
    //This method return value = (2^val) + (2^(val-1)) + (2^(val-2)) + ... + 2^2 + 2^1
    int i = 0;
    int total = 0;


    for(i = val ; i > 0 ; i--)
    {
        total = total + (int)pow((double)2,i);
    }

    return total;
}

double CARS::getCumulativeTotal(int index,int val)
{
    //In this Method Exponentially Weghited Total is calculated
    int i = 0 , j = 0;
    double total = 0.0;
    double temp1 = 0.0;

    for(i = 0 ; i < val ; i++)
    {

        temp1 = pow((double)2,(val - i));
        total = total + (last_PERs[index][i]*temp1);
    }

    return total;
}

double CARS::getTotalAverage()
{
    int i = 0, j = 0, temp1 = 0, num = 0;
    double total = 0.0,avg = 0.0;

    for(i = 0 ; i < 8 ; i++)
    {
        temp1 = no_last_PERs[i];
        num = num + temp1;

        for(j = 0 ; j < temp1 ; j++)
        {
            total = total + last_PERs[i][j];
        }
    }

    if(num == 0)
    {
        avg = -1;
    }else
    {
        avg = (total/(double)num);
    }

    return avg;
}

double CARS::getDistance(Coord node1, Coord node2)
{
    double distance = 0.0;

    distance = ((node1.x-node2.x)*(node1.x-node2.x)) + ((node1.y-node2.y)*(node1.y-node2.y));
    distance = sqrt(distance);

    return distance;
}


double CARS::getCumulativeTotalEWMA(int idx_rate,std::vector<double> lastPERs)
{
    double res = 0.0;
    double temp1 = 0.0;
    double temp2 = 0.0;

    for(int i = 1;i <= (int)lastPERs.size();i++)
    {
        temp1 = pow((double)2,i);
        temp2 = lastPERs[i-1];
        res = res + (temp2 * temp1);
    }
    return res;
}
double CARS::getTotalEWMA(int val)
{
    //This method return value = (2^val) + (2^(val-1)) + (2^(val-2)) + ... + 2^2 + 2^1
    int i = 0;
    double total = 0;


    for(i = val ; i > 0 ; i--)
    {
        total = total + pow((double)2,i);
    }

    return total;
}
void CARS::setPrevious_Bitrate(double bitrate)
{
    previous_bitrate = bitrate;
}
void CARS::setPreviousIdx(double idx)
{
    previous_bitrateIdx = idx;
}
