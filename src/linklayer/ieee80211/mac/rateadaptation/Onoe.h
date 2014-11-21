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

#ifndef ONOE_H_
#define ONOE_H_
/**
 * Implementation of Onoe adaptative bitrate algorithm.
 * Parámetros entrada:
        Threshold value of credits
        Bitrates iniciales: 24Mbits in 802.11g, 802.11a. 11Mbits en 802.11b.
        Value of credits inicialmente: 0
        Default period: 1 second

    Estas comprobaciones se hacen en cada periodo:

        1. Si no se ha podido entregar ningún paquete -> baja un bitrate.

        2. Si 10 o más paquetes se han mandado y la media de reintentos por paquete es mayor de 1 -> baja un bitrate

        3. Si más de un 10% de los paquetes necesitan un reintento, se decrementa el número de créditos (nunca por debajo de 0).

        4. Si menos de un 10% de los paquetes necesitan un reintento, se incrementa el número de créditos.

        5. Si el bitrate actual tiene 10 o más créditos, se incrementa el bit-rate.

        6. Continuar con el mismo bit-rate.

    Notas:
        el algoritmo funciona si la variable "recovery" en ieee80211Mac está a true.

 */
class Onoe {
public:
    Onoe();
    virtual ~Onoe();

    enum TypeOnoeDecision
    {
        INCREASE_BITRATE,
        DECREASE_BITRATE,
        NONE
    };//Decision about increase, decrease or nothing with credits.


    /**
     * Reset the variables of algorithm
     */
    void reset();

    /**
     * Packet has been sent with success.
     * Reset the variables.
     */
    void reportDataOk();

    /**
     * Report of packet data failed
     */
    void reportDataFailed();

    /**
     * Report of recovery failure
     */
    void reportRecoveryFailure();

    /**
     * Se ejecuta a la finalización del timer de control del algoritmo.
     */
    void endTimer();

    /**
     * Decide if the bitrate must be increase or decrease the bitrate.
     * When NONE is decided, the credit will be increase or decrease.
     */
    TypeOnoeDecision onoe_decision();

    void resetCredits();



private:
    int creditThreshold;//Threshold for credits to increase the threshold. 0 by default
    int credits;





};

#endif /* ONOE_H_ */
