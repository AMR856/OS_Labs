#include <pthread.h>
#include "caltrain.h"


/**
 * station_init - Initializes a train station structure.
 * 
 * This function initializes the fields of a `struct station` to ensure proper
 * synchronization between trains and passengers in a multi-threaded environment.
 *
 * @station: Pointer to the station structure to be initialized.
 */

void
station_init(struct station *station)
{
	// FILL ME IN
	// station->seats_in_the_train = 0;
	station->passengeres_trying_to_get_into_the_train = 0;
	station->waiting_passengers_in_the_station = 0;
	pthread_mutex_init(&station->mutex, NULL);
	pthread_cond_init(&station->train_arrived_to_the_station, NULL);
	pthread_cond_init(&station->train_ready_to_move, NULL);
}

// The train is loading new passengers, so it has to lock some variables and store the number of seats,
// First of all, we lock the mutex if we're updating some variables used in many threads, and then we check if we have
// some seats available and after that we signal the passengers to ride the train and wait for all the passengers to take
// their seats and board, when the loop finishes this means the seats have finished or 
// there are no other waiting passengers or no other available seats
// So this means the train has completed its loading process and is ready to go


/**
 * station_load_train - Handles the boarding process for a train at the station.
 *
 * This function is called when a train arrives at the station with a specified
 * number of available seats. It allows waiting passengers to board the train and it waits
 * for them by the condition variable `train_ready_to_move` and after this the train leaves
 *
 * @station: Pointer to the station structure.
 * @count: The number of available seats in the arriving train.
 *
 */

void
station_load_train(struct station *station, int count)
{
	// FILL ME IN
	pthread_mutex_lock(&station->mutex);
	station->seats_in_the_train = count;
	while (station->waiting_passengers_in_the_station > 0 &&
		station->seats_in_the_train > 0){
		pthread_cond_broadcast(&station->train_arrived_to_the_station);
		pthread_cond_wait(&station->train_ready_to_move, &station->mutex);
	}
	station->seats_in_the_train = 0;
	pthread_mutex_unlock(&station->mutex);
}


// The passenger is waiting for the train to arrive and we handle this using the condition 
// variable `train arrived at the station` and in the function, we handle the synchronization and
// the current number of seats, waiting passengers, and the passengers trying to board the train.

/**
 * station_wait_for_train - Handles a passenger waiting for and boarding a train.
 *
 * This function is called when a passenger arrives at the station and waits for a train.
 * It ensures that passengers only board when seats are available while maintaining proper
 * synchronization using mutexes and condition variables, and it handles the number of waiting passensgers,
 * available seats and the passengers borading the train
 *
 * @station: Pointer to the station structure.
 */

void
station_wait_for_train(struct station *station)
{
	// FILL ME IN
	pthread_mutex_lock(&station->mutex);
	station->waiting_passengers_in_the_station++;
	while (station->seats_in_the_train == 0) 
		pthread_cond_wait(&station->train_arrived_to_the_station, &station->mutex);
	station->waiting_passengers_in_the_station--;
	station->seats_in_the_train--;
	station->passengeres_trying_to_get_into_the_train++;
	pthread_mutex_unlock(&station->mutex);
}

// This function was the problem in the code because I didn't know what it was doing, but its job is
// making the current passenger board and this is done by decreasing the number of passengers trying to board the train
// after doing this we check if there are still another passengers trying to board the train also we check the available
// number of seats in the train and waiting passengers

/**
 * station_on_board - Signals that a passenger has successfully boarded the train.
 *
 * This function is called when a passenger has finished boarding and is seated on the train.
 * It ensures proper synchronization and notifies the train when all boarding is complete.
 *
 * @station: Pointer to the station structure.
 */

void
station_on_board(struct station *station)
{
	// FILL ME IN
	pthread_mutex_lock(&station->mutex);
	station->passengeres_trying_to_get_into_the_train--;
	if (station->passengeres_trying_to_get_into_the_train == 0 &&
	(station->waiting_passengers_in_the_station == 0 || station->seats_in_the_train == 0)
	)
		pthread_cond_signal(&station->train_ready_to_move);
	pthread_mutex_unlock(&station->mutex);
}
