/*
 * MPIEnigmaBreaker.h
 *
 *  Created on: 22.10 2022
 *      Author: Jakub Legutko
 */

#ifndef MPIENIGMABREAKER_H_
#define MPIENIGMABREAKER_H_

#include "EnigmaBreaker.h"
#include "mpi.h"

class MPIEnigmaBreaker : public EnigmaBreaker {
private:
	bool solutionFound( uint *rotorSettingsProposal );
public:
	int rank;
	MPIEnigmaBreaker( Enigma *enigma, MessageComparator *comparator );

	void crackMessage();
	void getResult( uint *rotorPositions );

	virtual ~MPIEnigmaBreaker();
};

#endif /* MPIENIGMABREAKER_H_ */
