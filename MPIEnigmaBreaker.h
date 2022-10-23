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
	int size;
	uint *expected = 0;
	uint expectedLength;
	MPIEnigmaBreaker( Enigma *enigma, MessageComparator *comparator );
	
	void setSampleToFind( uint *expected, uint expectedLength );
	void setMessageToDecode( uint *message, uint messageLength );
	void crackMessage();
	void getResult( uint *rotorPositions );

	virtual ~MPIEnigmaBreaker();
};

#endif /* MPIENIGMABREAKER_H_ */
