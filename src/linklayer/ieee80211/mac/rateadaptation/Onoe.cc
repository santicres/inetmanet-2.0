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

#include "Onoe.h"

Onoe::Onoe()
{
    // TODO Auto-generated constructor stub
    creditThreshold = 0;
    credits = 0;
    failedCounter = 0;
    successCounter = 0;
    retryCounter = 0;
    messagesWithRetry = 0;

}

Onoe::~Onoe()
{
    // TODO Auto-generated destructor stub
}
/**
 * Decide if the bitrate must be increase, decrease or none.
 *     These checks are made on each period:

        1. Si no se ha podido entregar ningún paquete -> baja un bitrate.
        1. If no packet has been sent -> decrease a bitrate

        2. Si 10 o más paquetes se han mandado y la media de reintentos por paquete es mayor de 1 -> baja un bitrate
            averageRetry

        3. Si más de un 10% de los paquetes necesitan un reintento, se decrementa el número de créditos (nunca por debajo de 0).
            retryCounter
        4. Si menos de un 10% de los paquetes necesitan un reintento, se incrementa el número de créditos.
            retryCounter
        5. Si el bitrate actual tiene 10 o más créditos, se incrementa el bit-rate.

        6. Continuar con el mismo bit-rate.
     RES: TypeDecision = (NONE, INCREASE_BITRATE, DECREASE_BITRATE)
 */
TypeRateDecision Onoe::checkBitrateDecision()
{
    TypeRateDecision resultDecision;
    resultDecision = NONE;
    //Any message is delivered on the timer
    if(successCounter == 0)
    {
        resultDecision = DECREASE_BITRATE;
        reset();

    }
    else if(successCounter > 10 && (failedCounter/successCounter) > 1)
    {
        resultDecision = DECREASE_BITRATE;
        reset();
    }
    else if((messagesWithRetry/successCounter) > 0.1)
    {
        credits--;
    }
    else if((messagesWithRetry/successCounter) < 0.1)
    {
        credits++;
    }

    if(resultDecision == NONE && credits >= 10)
    {
        resultDecision = INCREASE_BITRATE;
        reset();
    }

    return resultDecision;
}
void Onoe::reportDataOk()
{
    if(retryCounter > 0)
        messagesWithRetry++;

    successCounter++;
    retryCounter = 0;

}
void Onoe::reportDataFailed()
{
    retryCounter++;
    failedCounter++;
}
void Onoe::reset()
{
    successCounter = 0;
    failedCounter = 0;
    messagesWithRetry = 0;
    credits = 0;
}

std::string Onoe::toString()
{
    std::ostringstream os;
    os <<"Onoe object successCounter="<<successCounter<< " failedCounter=" << failedCounter << " retryCounter=" << retryCounter << "messagesWithRetry=" << messagesWithRetry << " credits=" << credits;
    std::string s = os.str();

    return s;
}

