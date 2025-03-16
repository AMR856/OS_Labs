#include <pthread.h>

/**
 * struct station - The train station struct that contains everything we need
 *
 * This struct is used to synchronize train and passenger interactions
 * using the mutex and condition variables
 * 
 * @seats_in_the_train: The number of available seats in the arriving train.
 * @waiting_passengers_in_the_station: The number of passengers waiting at the station.
 * @passengeres_trying_to_get_into_the_train: The number of passengers currently boarding the train.
 * @mutex: A mutex lock to protect access to shared resources.
 * @train_arrived_to_the_station: A condition variable to signal that a train has arrived.
 * @train_ready_to_move: A condition variable to signal that the train is ready to depart.
 */

struct station {
	// FILL ME IN
	int seats_in_the_train;
	int waiting_passengers_in_the_station;
	int passengeres_trying_to_get_into_the_train;
	pthread_mutex_t mutex;
	pthread_cond_t train_arrived_to_the_station;
	pthread_cond_t train_ready_to_move;
};

void station_init(struct station *station);

void station_load_train(struct station *station, int count);

void station_wait_for_train(struct station *station);

void station_on_board(struct station *station);