#ifndef TESTING_HPP
#include "../include/testing.hpp"
#endif

/********************************************************************************************
 * compare_idx_val_to_actual_min                                                            *
 * @brief Verifies that the station with the current index has the shortest queue           *
 *        in the list of stations.                                                          *
 *                                                                                          *
 * This function checks if the station at the current index (`curr_idx`) has the            *
 * shortest wait time (queue length) among all stations in the provided vector.             *
 * If the station does not have the shortest wait time, it logs an error message            *
 * and throws a `std::runtime_error` exception. This is done to verify that the             *
 * algorithm used to track the shortest wait time station queue is working correctly        *
 *                                                                                          *
 * @param stations: A reference to a vector of `Station` objects, representing all          *
 *                  stations in the simulation.                                             *
 * @param curr_idx: A reference to the current index in the stations vector, which          *
 *                  is expected to point to the station with the shortest wait time.        *
 * @return: None                                                                            *
 * @throws: std::runtime_error if the station at `curr_idx` does not have the               *
 *          shortest queue in the vector.                                                   *
 ********************************************************************************************/
void compare_idx_val_to_actual_min(std::vector<Station>& stations, size_t& curr_idx) {

    /* Find the station with the shortest wait time                                         */
    std::vector<Station>::iterator min = std::min_element(stations.begin(), stations.end(), 
    [](Station& a, Station& b) {
            return a.get_queue() < b.get_queue();
    });

    /* Compare the two values, if they are not equal log error info and throw an exception  */
    if(min->get_queue() != stations[curr_idx].get_queue()) {
        std::cerr << "The minimum queue is not selected. The selected station wait time is: " 
        << stations[curr_idx].get_queue() << " while the shortest wait time is: " 
        << min->get_queue() << std::endl;
        
        throw std::runtime_error("The station with the shortest wait time was not found");
    }
}

/********************************************************************************************
 * compare_total_time_to_max_time                                                           *
 * @brief Verifies that the total time recorded by a truck matches the expected             *
 *        maximum simulation time.                                                          *
 *                                                                                          *
 * This function retrieves the total recorded time across all states for the specified      *
 * truck and compares it to the maximum allowed simulation time (`max_time`).               *
 * If the total recorded time does not match the expected maximum time, the function        *
 * logs an error message and throws a `std::runtime_error` exception. This is done to       *
 * verify that the time spent in the sim was accurately logged                              *
 *                                                                                          *
 * @param truck: A reference to the `Truck` object whose total time is being verified.      *
 * @param max_time: The expected maximum simulation time that the truck's total             *
 *                  recorded time should match.                                             *
 * @return: None                                                                            *
 * @throws: std::runtime_error if the truck's total recorded time does not match            *
 *          the expected maximum simulation time.                                           *
 ********************************************************************************************/
void compare_total_time_to_max_time(Truck& truck, size_t max_time) {

    /* Get the cumulative amount of time the truck spent in the simulation                  */
    uint64_t total_time = truck.get_total_time();

    size_t cumulative_time = RETRIEVE_TIME(total_time, WAITING_MASK, 0) +
                             RETRIEVE_TIME(total_time, UNLOADING_MASK, 16) +
                             RETRIEVE_TIME(total_time, TRAVELING_MASK, 32) +
                             RETRIEVE_TIME(total_time, MINING_MASK, 48);

    /* Compare the two values, if they are not equal log error info and throw an exception  */
    if(cumulative_time != max_time) {

        std::cerr << "The cumulative time spent by the truck in the simulation " 
        << "does not match the total simulation time" << std::endl 
        << "Truck time: " << cumulative_time << " Simulation time: "
        << max_time << std::endl;

        throw std::runtime_error("Truck time does not match simulation time");
    }
}