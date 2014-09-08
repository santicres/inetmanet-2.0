
#include "ACARS.h"

using namespace std;

ACARS::ACARS()
{
    int i = 0, j = 0;

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

    //Mark P Band Rates in Mbps
    p_band_rates[0] = 3.000;
    p_band_rates[1] = 4.500;
    p_band_rates[2] = 6.000;
    p_band_rates[3] = 9.000;
    p_band_rates[4] = 12.000;
    p_band_rates[5] = 18.000;
    p_band_rates[6] = 24.000;
    p_band_rates[7] = 27.000;

    //Initializing Arrays
    for( i = 0 ; i < 8 ; i++)
    {
        no_last_PERs[i] = 0;
        for( j = 0 ; j < 10 ; j++)
        {
            last_PERs[i][j] = 0;
        }
    }



}

void ACARS::setUpdateInterval(double val)//periodic update time interval in ms
{
    update_interval = val;
}

double ACARS::getAlpha(double speed)//This speed is relative speed between two vehicles
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

void ACARS::setMaxRetransmissions(long a)
{
    N = a;
}

double ACARS::getBitRate(double speed,long len)//should remove double debug_PER
{
    int current_rate_index = 0;
    double Best_Rate = 3.0,PER = 0.0;
    double Max_Thr = 0.0,alpha = 0.0, avg_retries = 0.0;
    double temp1 = 0.0, temp2 = 0.0, temp3 = 0.0, Thr = 0.0;

    alpha = getAlpha(speed);
    updateDistance();// This method updates the distance value need for ec()

    for(current_rate_index = 0 ; current_rate_index < 8 ; current_rate_index++)
    {
        temp1 = ec(speed,p_band_rates[current_rate_index],len);
        temp2 = eh(p_band_rates[current_rate_index], len);

        if(((int)(temp2 + 1)) == 0)
        {// this means that there are no any past PERs so, eh can not work.
            //cout<<"\nNo Past PERs\n";
        }else
        {
            PER = (alpha*temp1) + ((1- alpha)*(temp2));
        }



        //PER = debug_PER;//should remove after debugging

        avg_retries = (( (N*pow(PER,(N+1))) - ((N+1)*pow(PER,N)) + 1)/(1 - PER)) + (N*(pow(PER,N)));

        temp3 = pow((1 - pow(PER,N)),sigma);//I change this and below formula will have to make sure it is correct

        Thr = (p_band_rates[current_rate_index]/avg_retries)*temp3;//I change this too..

        if(Thr > Max_Thr)
        {
            Best_Rate = p_band_rates[current_rate_index];
            Max_Thr = Thr;
        }

    }


    return Best_Rate;
}

double ACARS::ec(double speed,double rate, long len)
{
    double PER = 0.0;
    double temp1 = 0.0, temp2 = 0.0;

    /*
     *  Effect of the distance and rate are modelled according
     *  to Free Space Path Loss Model. In this model PER propotional
     *  to the square of distance and frequency.
     *  Which means if distance and bitarate increase by factor of two,
     *  then PER will increase by factor of 16.
     *
    */


    temp1 = (rate/previous_bitrate);
    temp1 = (temp1*temp1);

    //Effect of Distance
    temp2 = (current_distance/previous_distance);
    temp2 = (temp2*temp2);

    PER = (previous_PER*temp1*temp2);

    //last_bitrate = rate;
    //previous_PER = PER;
    //last_distance = distance;


    return PER;
}

void ACARS::setFirstPER(double val, double rate)//this the PER and rate of the first transmission
{
    int i = 0, j = 0,temp1 = 0;

    first_PER = val;
    previous_PER = val;
    previous_bitrate = rate;

    for(i = 0 ; i < 8 ; i++)
    {
        temp1 = (int)(rate - p_band_rates[i]);

        if(temp1 == 0)
        {
            insertNewPER(i,val);
            if(no_last_PERs[i] < 10)
            {
                no_last_PERs[i]++;
            }
        }
    }

}

void ACARS::setCurrentPER(double val, double rate)
{
    int i = 0, j = 0,temp1 = 0;

    previous_PER = val;
    previous_bitrate = rate;

    for(i = 0 ; i < 8 ; i++)
    {
        temp1 = (int)(rate - p_band_rates[i]);

        if(temp1 == 0)
        {
            insertNewPER(i,val);
            if(no_last_PERs[i] < 10)
            {
                no_last_PERs[i]++;
            }
        }
    }

}

void ACARS::setPositions(double px1, double py1, double px2, double py2)
{
    x1 = px1;
    y1 = py1;
    x2 = px2;
    y2 = py2;
}

void ACARS::updatePositions(double speed1,double angle1, double speed2, double angle2)
{
    double PI = 3.14159265;
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

double ACARS::updateDistance()//Calculate Distance between two vehicles.
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

void ACARS::setNoRetransmissions(int val)
{
    no_retransmissions = val;
}

double ACARS::eh(double rate, long len)
{
    int i = 0 , j = 0, temp1 = 0;
    double temp2 = 0.0, temp3 = 0.0;
    double PER = 0.0;

    for( i = 0 ; i < 8 ; i++)
    {
        temp1 = (int)(p_band_rates[i] - rate);//In C++ floating point arithematic is
                    //sometimes affected with garbage values, hence for the pupose
                    //of comparing difference converted to int
        if(temp1 == 0)
        {
            if(no_last_PERs[i] > 0)
            {
                temp2 = getCumulativeTotal(i, no_last_PERs[i]);
                temp3 = getTotal(no_last_PERs[i]);
                PER = temp2/temp3;
            }else
            {
                PER = getTotalAverage();
            }
            break;
        }
    }
    return PER;
}

void ACARS::insertNewPER(int index, double val)
{
    int i = 8;

    //cout<<"\nLattooooo\n";

    for( i = 8 ; i > -1; i--)
    {
        last_PERs[index][(i + 1)] = last_PERs[index][i];
    }

    last_PERs[index][0] = val;
}

int ACARS::getTotal(int val)
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

double ACARS::getCumulativeTotal(int index,int val)
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

double ACARS::getTotalAverage()
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



ACARS::ACARS(const ACARS& orig) {
}

ACARS::~ACARS() {
}

