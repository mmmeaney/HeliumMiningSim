/********************************************************************************************
 * File: main.hpp                                                                           *
 *                                                                                          *
 * Description:                                                                             *
 *  Contains all the includes, macros, and classes necessary to run the Helium-3 Mining     *
 *  Simulator                                                                               *
 *                                                                                          *
 * Changes:                                                                                 *
 *  8/27/2024 - Created (Matt Meaney)                                                       *
 *                                                                                          *
 ********************************************************************************************/
#ifndef MAIN_HPP
#define MAIN_HPP

/********************************************************************************************
 * Includes                                                                                 *
 ********************************************************************************************/
#include <iostream>
#include <random>
#include <chrono>
#include <functional>

/********************************************************************************************
 * Defines/Macros                                                                           *
 ********************************************************************************************/
#define TRAVEL_TIME     6u      /*Scale: 5 mins/bit, 12*5  = 30 minutes   = Travel time     */
#define ONE_HOUR        12u     /*Scale: 5 mins/bit, 12*5  = 60 minutes   = 1 hour          */
#define FIVE_HOUR       60u     /*Scale: 5 mins/bit, 60*5  = 300 minutes  = 5 hours         */
#define MAX_TIME        864u    /*Scale: 5 mins/bit, 864*5 = 4320 minutes = 72 hours        */

#define WAITING_INC     0x0000000000000001
#define UNLOADING_INC   0x0000000000010000
#define TRAVELING_INC   0x0000000100000000
#define MINING_INC      0x0001000000000000

#define WAITING_MASK    0x000000000000FFFF
#define UNLOADING_MASK  0x00000000FFFF0000
#define TRAVELING_MASK  0x0000FFFF00000000
#define MINING_MASK     0xFFFF000000000000

#define RETRIEVE_TIME(DATA, MASK, SHIFT)  ((DATA & MASK) >> SHIFT)

/********************************************************************************************
 * Enumerations/Classes                                                                     *
 ********************************************************************************************/
enum class TruckState {
    Mining,
    TravelStation,
    Waiting,
    Unloading,
    TravelMining
};

/********************************************************************************************
 * Station                                                                                  *
 * @brief Represents a station in the simulation where trucks can wait and unload.          *
 *                                                                                          *
 * The Station class models the behavior of a station, tracking the number of trucks        *
 * currently in the station's queue and the total number of trucks that have been           *
 * unloaded at the station.                                                                 *
 ********************************************************************************************/
class Station {
public:
    /****************************************************************************************
     * Station Constructor                                                                  *
     * @brief Initializes a Station object with default values.                             *
     *                                                                                      *
     * The constructor initializes the station's queue and the number of trucks             *
     * unloaded to zero.                                                                    *
     ****************************************************************************************/
    Station();

    /****************************************************************************************
     * ~Station                                                                             *
     * @brief Destructor for the Station class.                                             *
     *                                                                                      *
     * Cleans up any resources used by the Station object. Since this class does not        *
     * dynamically allocate any resources, the destructor is trivial.                       *
     *                                                                                      *
     * @param: None                                                                         *
     * @return: None                                                                        *
     ****************************************************************************************/
    ~Station();

    /****************************************************************************************
     * logging                                                                              *
     * @brief Outputs the number of trucks unloaded at the station to the console.          *
     *                                                                                      *
     * This function prints the total number of trucks that have been unloaded at the       *
     * station.                                                                             *
     *                                                                                      *
     * @param: None                                                                         *
     * @return: None                                                                        *
     ****************************************************************************************/
    void logging();

    /****************************************************************************************
     * get_queue                                                                            *
     * @brief Retrieves the current number of trucks in the station's queue.                *
     *                                                                                      *
     * This function returns the current value of the station's queue, which represents     *
     * the number of trucks waiting at the station.                                         *
     *                                                                                      *
     * @param: None                                                                         *
     * @return: uint16_t - The number of trucks currently in the station's queue.           *
     ****************************************************************************************/
    uint16_t get_queue();

    /****************************************************************************************
     * increment_queue                                                                      *
     * @brief Increases the station's queue count by one.                                   *
     *                                                                                      *
     * This function increments the `queue` variable by one, representing the arrival of    *
     * a new truck at the station.                                                          *
     *                                                                                      *
     * @param: None                                                                         *
     * @return: None                                                                        *
     ****************************************************************************************/
    void increment_queue();

    /****************************************************************************************
     * decrement_queue                                                                      *
     * @brief Decreases the station's queue count by one, ensuring it doesn't go below zero.*
     *                                                                                      *
     * This function decrements the `queue` variable by one, representing the departure of  *
     * a truck from the station. If the queue is already at zero, it remains unchanged.     *
     *                                                                                      *
     * @param: None                                                                         *
     * @return: None                                                                        *
     ****************************************************************************************/
    void decrement_queue();

    /****************************************************************************************
     * increment_trucks_unloaded                                                            *
     * @brief Increments the count of trucks that have been unloaded at the station.        *
     *                                                                                      *
     * This function increments the `num_trucks_unloaded` variable by one, representing     *
     * another truck that has successfully unloaded at the station.                         *
     *                                                                                      *
     * @param: None                                                                         *
     * @return: None                                                                        *
     ****************************************************************************************/
    void increment_trucks_unloaded();

private:
    /* Number of trucks currently in the station's queue                                    */
    uint16_t queue;
    /* Total number of trucks that have been unloaded at this station                       */
    uint16_t num_trucks_unloaded;
};

/********************************************************************************************
 * Truck                                                                                    *
 * @brief Represents a truck in the simulation, managing its state and operations           *
 *        as it transitions through various activities (Mining, Traveling, Waiting,         *
 *        Unloading).                                                                       *
 *                                                                                          *
 * The Truck class models the behavior of a truck within the simulation. It tracks the      *
 * truck's state, time spent in each activity, and manages transitions between states       *
 * (e.g., Mining to Traveling, Traveling to Unloading). The class uses a combination        *
 * of timers and state management to simulate the truck's operations, including logging     *
 * its activities and interacting with station queues.                                      *
 ********************************************************************************************/
class Truck {

public:
    /****************************************************************************************
     * Truck Constructor                                                                    *
     * @brief Initializes a Truck object with default values and sets up its                *
     *        initial state for the simulation.                                             *
     *                                                                                      *
     * This constructor initializes the Truck object with the following default             *
     * settings:                                                                            *
     * - `state`: The truck starts in the `Mining` state, representing that the             *
     *   truck is initially engaged in mining activities.                                   *
     * - `total_time`: Initialized to 0, this 64-bit integer will track the                 *
     *   cumulative time spent by the truck in various states (Mining, Traveling,           *
     *   Unloading, Waiting) during the simulation.                                         *
     * - `station_idx`: Initialized to 0, used to track which station the truck             *
     *    is unloading/queued at.                                                           *
     * - `timer`: This value is initialized by drawing a random time duration from          *
     *   a uniform distribution between 1 and 5 hours with a scaling of 5 mins/bit          *
     *   using the `rmt` distribution and `gen` random number generator.                    *
     *                                                                                      *
     * @param: None                                                                         *
     * @return: None                                                                        *
     ****************************************************************************************/
    Truck();

    /****************************************************************************************
     * ~Truck                                                                               *
     * @brief Destructor for the Truck class.                                               *
     *                                                                                      *
     * Cleans up any resources used by the Truck object. Since this class does not          *
     * dynamically allocate any resources, the destructor is trivial.                       *
     *                                                                                      *
     * @param: None                                                                         *
     * @return: None                                                                        *
     ****************************************************************************************/
    ~Truck();

    /****************************************************************************************
     * logging                                                                              *
     * @brief Displays the operating statistics of a specific truck over the course of a    *
     *        simulation.                                                                   *
     *                                                                                      *
     * This function calculates and prints the percentage of time a truck has spent in      *
     * various states (Waiting, Unloading, Traveling, and Mining) throughout the simulation *
     * The percentages are derived from the total recorded time for each state, which is    *
     * stored within a 64-bit integer. The function uses bitwise operations to extract the  *
     * time spent in each state and then calculates the corresponding percentage relative   *
     * to the total simulation time (`total_sim_time`).                                     *
     *                                                                                      *
     * @param total_sim_time: The total duration of the simulation, used as the reference   *
     *                        time to calculate the percentage of time spent in each state. *
     * @return: None                                                                        *
     ****************************************************************************************/
    void logging(uint16_t total_sim_time);
    
    /****************************************************************************************
     * run                                                                                  *
     * @brief Simulates the operations of a truck as it transitions through                 *
     *        different states (Mining, Traveling, Waiting, Unloading).                     *
     *                                                                                      *
     * This function updates the state of the truck based on its current activity           *
     * (e.g., mining, traveling to a station, waiting at a station, or unloading).          *
     * The truck's state and timers are adjusted accordingly, with each state               *
     * decrementing its associated timer and updating the truck's cumulative time           *
     * (`total_time`) using predefined bitwise increments.                                  *
     *                                                                                      *
     * The truck can transition between the following states:                               *
     * - `Mining`: Decrements mining time and switches to traveling to a station            *
     *   when mining is complete.                                                           *
     * - `TravelStation`: Decrements travel time to the station and determines              *
     *   whether the truck should wait or unload upon arrival.                              *
     * - `Waiting`: Decrements the waiting time before the truck can begin unloading.       *
     * - `Unloading`: Increments unloading time, updates the station's unloaded             *
     *   truck count, and then transitions back to traveling towards the mining             *
     *   area.                                                                              *
     * - `TravelMining`: Decrements travel time to the mining area, then returns            *
     *   the truck to the mining state.                                                     *
     *                                                                                      *
     * The function also manages the station queue by updating the queue count              *
     * when a truck arrives and increments the number of trucks unloaded after              *
     * unloading.                                                                           *
     *                                                                                      *
     * @param stations: A reference to a vector of Station objects, which represent         *
     *                  the stations where trucks can wait and unload.                      *
     * @param curr_idx: A reference to the current index in the stations vector,            *
     *                  used to track which station the truck is interacting with.          *
     *                                                                                      *
     * @return: None                                                                        *
     * @throws: std::runtime_error if an unexpected state is encountered.                   *
     ****************************************************************************************/
    void run(std::vector<Station>& stations, size_t& curr_idx);

    /****************************************************************************************
     * get_total_time                                                                       *
     * @brief Retrieves the total recorded time the truck has spent across all states.      *
     *                                                                                      *
     * This function returns the cumulative time the truck has spent in all states          *
     * (Mining, Traveling, Waiting, Unloading) during the simulation. The total time        *
     * is stored as a 64-bit integer that tracks the time spent in each state using         *
     * bitwise operations.                                                                  *
     *                                                                                      *
     * @param: None                                                                         *
     * @return: uint64_t - The total time recorded for the truck across all states.         *
     ****************************************************************************************/
    uint64_t get_total_time();

private:

    /* Current truck state                                                                  */
    TruckState state;

    /* Timer to keep track of the current time left to spend in a particular state          */
    uint16_t timer;

    /****************************************************************************************
     * I've decided to store all the data into one 64 bit unsigned integer. Each timer can  *
     * be stored using 16 bits, which is perfect for tracking the time of the 4 categories  *
     *                                                                                      *
     * All times are at a scale of 5 mins/bit, so with a max value of 65535, we can track   *
     * up to 5461 hours per category which is more than sufficient to meet the required 72  *
     * total hours                                                                          *
     ****************************************************************************************
     *                                                                                      *
     *          Mining            Traveling           Unloading            Waiting          *
     *   _________|_________ _________|_________ _________|_________ _________|_________    *
     *   |                 | |                 | |                 | |                 |    *
     *   |                 | |                 | |                 | |                 |    *
     *   0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000    *
     *                                                                                      *
     ****************************************************************************************/
    uint64_t total_time;

    /* Station index where the truck is being unloaded                                      */
    size_t station_idx;
};

/********************************************************************************************
 * Simulation                                                                               *
 * @brief Manages the overall simulation of trucks and stations, orchestrating the          *
 *        interactions between them over a defined period.                                  *
 *                                                                                          *
 * The Simulation class is responsible for running a simulation that models the             *
 * behavior of trucks as they transition through various states (Mining, Traveling,         *
 * Waiting, Unloading) while interacting with stations. The class maintains a               *
 * collection of trucks and stations, tracks the current station index for                  *
 * round-robin assignment, and handles the timing and logging of events throughout          *
 * the simulation.                                                                          *
 *                                                                                          *
 * Key Features:                                                                            *
 * - **Trucks and Stations**: Manages a dynamic collection of trucks and stations,          *
 *   initializing them based on user-defined parameters.                                    *
 * - **Round-Robin Assignment**: Distributes trucks evenly across stations using a          *
 *   round-robin strategy to prevent bottlenecks and ensure balanced load distribution.     *
 * - **Queue Management**: Decrements the queue sizes of stations uniformly after each      *
 *   simulation step, simulating the processing of trucks at stations.                      *
 * - **Simulation Control**: Runs the simulation for a fixed number of iterations,          *
 *   controlling the flow of time and the progression of truck activities.                  *
 * - **Logging and Debugging**: Provides detailed logging of truck and station              *
 *   statistics at the end of the simulation. Optional debugging mode offers additional     *
 *   consistency checks during the simulation.                                              *
 ********************************************************************************************/
class Simulation {

public:

    /****************************************************************************************
     * Simulation Constructor                                                               *
     * @brief Initializes a Simulation object with a specified number of trucks and         *
     *        stations, and sets up the initial simulation parameters.                      *
     *                                                                                      *
     * This constructor initializes the Simulation with a specified number of trucks and    *
     * stations. It also sets the initial station index and the total simulation time.      *
     * Optionally, debugging mode can be enabled to perform additional checks during the    *
     * simulation.                                                                          *
     *                                                                                      *
     * @param num_trucks: The number of trucks to be simulated.                             *
     * @param num_stations: The number of stations available in the simulation.             *
     * @param debug: Optional parameter that enables debug mode if set to true. Debug mode  *
     *               performs additional consistency checks during the simulation.          *
     * @return: None                                                                        *
     ****************************************************************************************/
    Simulation(uint16_t num_trucks, uint16_t num_stations, bool debug = false);

    /****************************************************************************************
     * ~Simulation                                                                          *
     * @brief Destructor for the Simulation class.                                          *
     *                                                                                      *
     * Cleans up any resources used by the Simulation object. Since this class manages      *
     * objects through containers that handle their own memory management, the destructor   *
     * is trivial.                                                                          *
     *                                                                                      *
     * @param: None                                                                         *
     * @return: None                                                                        *
     ****************************************************************************************/
    ~Simulation();

    /****************************************************************************************
     * run_sim                                                                              *
     * @brief Executes the simulation, running all trucks through their respective states   *
     *        and managing station queues over the course of the simulation time.           *
     *                                                                                      *
     * This method performs the following steps during the simulation:                      *
     * 1. Initializes the simulation time to the maximum time allowed.                      *
     *                                                                                      *
     * 2. Iterates through each truck, invoking its `run` method to transition through      *
     *    various states (Mining, Traveling, Waiting, Unloading).                           *
     *                                                                                      *
     * 3. If debugging mode is enabled, consistency checks are performed after each truck   *
     *    runs, ensuring the current station index matches the station with the minimum     *
     *    queue size.                                                                       *
     *                                                                                      *
     * 4. After processing all trucks, decrements the queue count for each station,         *
     *    ensuring that no queue falls below zero.                                          *
     *                                                                                      *
     * 5. Decrements the simulation time and repeats until the simulation time reaches zero.*
     *                                                                                      *
     * 6. After the simulation loop, logs the operational statistics for each truck and     *
     *    station.                                                                          *
     *                                                                                      *
     * 7. If debugging mode is enabled during logging, additional checks are performed to   *
     *    verify that the total time recorded for each truck matches the maximum time.      *
     *                                                                                      *
     * @param: None                                                                         *
     * @return: None                                                                        *
     ****************************************************************************************/
    void run_sim();

    /* Store the index of the station queue with the shortest wait time                     */
    size_t curr_station_idx;

    /* Store the total execution time of the simulation                                     */
    uint16_t total_time;

    /* Flag to determine whether to run additional consistency checks during the simulation */
    bool debug;

    /* list of stations                                                                     */
    std::vector<Station> stations;

    /* list of trucks                                                                       */
    std::vector<Truck> trucks;
};

/********************************************************************************************
 * Notes                                                                                    *
 ********************************************************************************************/

/********************************************************************************************
 * Shortest Wait Time Algorithm                                                             *
 * @brief Ensures that each truck is directed to the station with the shortest              *
 *        wait time by leveraging a round-robin strategy in conjunction with                *
 *        uniform queue decrementing.                                                       *
 *                                                                                          *
 * This approach works as follows:                                                          *
 *                                                                                          *
 * 1. **Round-Robin Assignment**:                                                           *
 *    - Trucks are assigned to stations in a cyclic manner, with each truck                 *
 *      being assigned to the next station in the list (using `curr_idx` as the             *
 *      index). This ensures that the workload is evenly distributed among                  *
 *      stations initially, avoiding overloading any single station.                        *
 *                                                                                          *
 * 2. **Uniform Queue Decrementing**:                                                       *
 *    - After each loop iteration, the wait times (queues) at all stations are              *
 *      uniformly decremented. This simulates the processing of trucks at all               *
 *      stations, reducing their queues over time.                                          *
 *                                                                                          *
 * 3. **Minimizing Wait Times**:                                                            *
 *    - Since each truck is placed in a station's queue in a cyclic manner, and             *
 *      all queues are uniformly decremented, the station with the shortest                 *
 *      queue is always favored. When a truck finishes its current task and                 *
 *      is ready to go to a new station, it will encounter the station with                 *
 *      the shortest wait time due to the round-robin's even distribution and               *
 *      the uniform decrementing of queues.                                                 *
 *                                                                                          *
 * 4. **Balanced Load**:                                                                    *
 *    - The round-robin strategy helps to prevent any single station from                   *
 *      becoming a bottleneck by ensuring that trucks are spread out across                 *
 *      all stations. The combination of this even distribution with the                    *
 *      consistent reduction of queue sizes allows trucks to naturally                      *
 *      gravitate towards stations with shorter wait times.                                 *
 *                                                                                          *
 * 5. **Runtime Complexity**:                                                               *
 *    - The round-robin assignment has a runtime complexity of O(1) per truck,              *
 *      as it involves only a simple index update and a modulo operation to assign          *
 *      the truck to the next station. The uniform queue decrementing operation has         *
 *      a runtime complexity of O(m) per simulation loop, where m is the number of          *
 *      stations, because it involves decrementing the queue for each station. Over the     *
 *      entire simulation, which runs for a fixed number of iterations (864), the total     *
 *      complexity becomes O(m * 864). Since 864 is a constant, this simplifies to          *
 *      O(m), indicating that the algorithm scales linearly with the number of stations.    *
 ********************************************************************************************/

#endif // MAIN_HPP
