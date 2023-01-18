/*
 * MPIEnigmaBreaker.cpp
 *
 *  Created on: 22.10 2022
 *      Author: Jakub Legutko
 */

#include "MPIEnigmaBreaker.h"
#include "mpi.h"

MPIEnigmaBreaker::MPIEnigmaBreaker( Enigma *enigma, MessageComparator *comparator ) : EnigmaBreaker(enigma, comparator ) {
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
}

MPIEnigmaBreaker::~MPIEnigmaBreaker() {
	delete[] rotorPositions;
}
void MPIEnigmaBreaker::setMessageToDecode( uint *message, uint messageLength ) {
	comparator->setMessageLength(messageLength);
	this->messageLength = messageLength;
	this->messageToDecode = message;
	messageProposal = new uint[ messageLength ];
	MPI_Bcast(&messageLength,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(message,messageLength,MPI_INT,0,MPI_COMM_WORLD);
	//cout << "Podzielilem sie message " << rank << endl;
}

void MPIEnigmaBreaker::setSampleToFind(uint *expected, uint expectedLength ) {
	comparator->setExpectedFragment(expected, expectedLength);
	//cout << sizeof(expected) << endl;
	MPI_Bcast(&expectedLength,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(expected,expectedLength,MPI_INT,0,MPI_COMM_WORLD);
	//cout << "Podzielilem sie expected " << rank << endl;
}
void MPIEnigmaBreaker::crackMessage() {
	uint rotorLargestSetting = enigma->getLargestRotorSetting();
	
	if (rank != 0) {
		//cout << "Zaczynam lapac message " << rank << endl;
		MPI_Bcast(&messageLength,1,MPI_INT,0,MPI_COMM_WORLD);
		messageToDecode = new uint[ messageLength ];
		MPI_Bcast(messageToDecode,messageLength,MPI_INT,0,MPI_COMM_WORLD);
		MPI_Bcast(&expectedLength,1,MPI_INT,0,MPI_COMM_WORLD);
		expected = new uint[ expectedLength ];
		MPI_Bcast(expected,expectedLength,MPI_INT,0,MPI_COMM_WORLD);
		comparator->setMessageLength(messageLength);
		comparator->setExpectedFragment(expected, expectedLength);
		//cout << "JESTEM RANK " << rank << " I wlasnie ustawilem mase rzeczy, konkretnie length, expected i length "<< messageLength  << " " << expectedLength << " "<< endl;
	}
	
		//cout << "JESTEM RANK " << rank << endl;
	/**
	 * Poniższy kod (w szczególności pętle) jest paskudny. Można to
	 * napisac ładniej i, przede wszystkim w wersji uniwersalnej,
	 * ale szkoda czasu. Ta proteza tu wystarczy.
	 * Umawiamy się niniejszym, że więcej niż maxRotors rotorów nie będzie!
	 */
	MPI_Comm duplicated_communicator;
    MPI_Comm_dup(MPI_COMM_WORLD, &duplicated_communicator);
	MPI_Request endOfComputing;
	MPI_Request broadcastResult;
	MPI_Request finderRank;
	int *dummy = new int[1];
	int *rankr = new int[1];
	int *rankl = new int[1];
	rankl[0] = rank;
	dummy[0] = 0;
	int gotoCheck =0;
	int endCheck = 0;
	if (rank == 0)
	rankr[0] = -1;
	uint *rMax = new uint[ MAX_ROTORS ];
	for ( uint rotor = 0; rotor < MAX_ROTORS; rotor++ ) {
		if ( rotor < rotors )
			rMax[ rotor ] = rotorLargestSetting;
		else
			rMax[ rotor ] = 0;
	}

	uint *r = new uint[ MAX_ROTORS ];
	if (rank != 0)
	MPI_Ibcast(dummy,1,MPI_INT,0,duplicated_communicator,&endOfComputing);
rMax[0] = ((rotorLargestSetting * (rank + 1)) / size) < rotorLargestSetting ? ((rotorLargestSetting * (rank + 1)) / size) : rotorLargestSetting;
		//cout << "JESTEM RANK ustawiłem rMAx na " << rank << rMax[0] << endl;
	for (r[0] = (rotorLargestSetting * rank) / size;r[0] <= rMax[0]; r[0]++ ){

				
		for (r[1]=0;r[1] <= rMax[1]; r[1]++) {

			for (r[2]=0;r[2] <= rMax[2]; r[2]++)
				for (r[3]=0;r[3] <= rMax[3]; r[3]++)
					for (r[4]=0;r[4] <= rMax[4]; r[4]++)
						for (r[5]=0;r[5] <= rMax[5]; r[5]++)
							for (r[6]=0;r[6] <= rMax[6]; r[6]++)
								for (r[7]=0;r[7] <= rMax[7]; r[7]++)
									for (r[8]=0;r[8] <= rMax[8]; r[8]++)
										for (r[9]=0; r[9] <= rMax[9]; r[9]++) {
											MPI_Test(&endOfComputing,&endCheck,MPI_STATUS_IGNORE);
											//if (rank == 3 && endCheck)
											//cout << "proces " << rank << dummy[0] << endl;
											if (endCheck == 1) {
												//cout << "Odebralem ibcast " << rank << endl;
												goto EXIT_ALL_LOOPS;
											}
											if (rank == 0) {
												MPI_Irecv(rankr,1,MPI_INT,MPI_ANY_SOURCE,3,MPI_COMM_WORLD,&finderRank);
												MPI_Test(&finderRank,&gotoCheck,MPI_STATUS_IGNORE);
												if (rankr[0] != -1)
													goto EXIT_ALL_LOOPS;
											}
											if ( solutionFound( r ) ) {
												//cout <<"Ja proces " << rank<< " Znalazłem rozwiązanie Wysyłam do procesu 0 coś takiego " << r[0] << endl;
												
												MPI_Isend(rankl,1,MPI_INT,0,3,MPI_COMM_WORLD,&finderRank);
												MPI_Send(r,MAX_ROTORS,MPI_INT,0,2,MPI_COMM_WORLD);
												goto EXIT_ALL_LOOPS;
											}
												
										}
		}
	}
	EXIT_ALL_LOOPS:
	//cout << "JA,dotarłem tutaj rank " << rank << endl;
	if (rank ==0){
		MPI_Recv(r,MAX_ROTORS,MPI_INT,rankr[0],2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		dummy[0] = 1;
		MPI_Ibcast(dummy,1,MPI_INT,0,duplicated_communicator,&endOfComputing);
		//cout << "Wyslalem ibcast " << rank << endl;
		
		for ( uint rotor = 0; rotor < rotors; rotor++ ) {
			this->rotorPositions[ rotor ] = r[ rotor ];
		}
	}
	delete[] rMax;
	delete[] r;
}

bool MPIEnigmaBreaker::solutionFound( uint *rotorSettingsProposal ) {
	for ( uint rotor = 0; rotor < rotors; rotor++ )
		rotorPositions[ rotor ] = rotorSettingsProposal[ rotor ];

	enigma->setRotorPositions(rotorPositions);
	uint *decodedMessage = new uint[ messageLength ];

	for (uint messagePosition = 0; messagePosition < messageLength; messagePosition++ ) {
		decodedMessage[ messagePosition ] = enigma->code(messageToDecode[ messagePosition ] );
	}

	bool result = comparator->messageDecoded(decodedMessage);
	delete[] decodedMessage;

	return result;
}

void MPIEnigmaBreaker::getResult( uint *rotorPositions ) {
	for ( uint rotor = 0; rotor < rotors; rotor++ ) {
		rotorPositions[ rotor ] = this->rotorPositions[ rotor ];
	}
}


