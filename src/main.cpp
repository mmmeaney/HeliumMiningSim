#ifndef MAIN_HPP
#include "../include/main.hpp"
#endif

#ifndef TESTING_HPP
#include "../include/testing.hpp"
#endif

/* Mersenne Twister and pyseudorandom number generator and a uniform distribution       */
static std::mt19937 gen(std::random_device{}());
static std::uniform_int_distribution<> rmt(ONE_HOUR, FIVE_HOUR);

/****************************************************************************************
 * Station Constructor                                                                  *
 * @brief Initializes a Station object with default values.                             *
 *                                                                                      *
 * The constructor initializes the station's queue and the number of trucks             *
 * unloaded to zero.                                                                    *
 ****************************************************************************************/
Station::Station() : queue(0), num_trucks_unloaded(0) {}

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
Station::~Station() {}

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
void Station::logging() {

    std::cout << "Number of trucks unloaded: " << this->num_trucks_unloaded << std::endl
    << std::endl;
}

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
uint16_t Station::get_queue() {
    return this->queue;
}

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
void Station::increment_queue() {
    this->queue++;
}

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
void Station::decrement_queue() {
    this->queue -= (this->queue > 0);
}

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
void Station::increment_trucks_unloaded() {
    this->num_trucks_unloaded++;
}

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
Truck::Truck() : state(TruckState::Mining),
                    total_time(0),
                    station_idx(0),
                    timer(rmt(gen)) {}

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
Truck::~Truck() {}

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
void Truck::logging(uint16_t total_sim_time) {

    float time = total_sim_time;

    std::cout << "Waiting: " << 
    (RETRIEVE_TIME(this->total_time, WAITING_MASK, 0) / time)*100
    << "%" << std::endl;

    std::cout << "Unloading: " << 
    (RETRIEVE_TIME(this->total_time, UNLOADING_MASK, 16) / time)*100
    << "%" << std::endl;

    std::cout << "Traveling: " << 
    (RETRIEVE_TIME(this->total_time, TRAVELING_MASK, 32) / time)*100
    << "%" << std::endl;
    
    std::cout << "Mining: " << 
    (RETRIEVE_TIME(this->total_time, MINING_MASK, 48) / time)*100
    << "%" << std::endl << std::endl;
}

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
void Truck::run(std::vector<Station>& stations, size_t& curr_idx) {

    if(TruckState::Mining == this->state) {

        /* Decrement the remaining mining time and increment the total                  */
        this->timer--;
        this->total_time += MINING_INC;
        
        /* Once the truck has finished mining, proceed back to the unloading station    */
        if(this->timer == 0) {
            
            /* Set the travel time                                                      */
            this->timer = TRAVEL_TIME;

            /* Proceed to the TravelStation State                                       */
            this->state = TruckState::TravelStation;
        }
    }
    else if(TruckState::TravelStation == this->state) {

        /* Decrement the remaining travel time to the station and increment the total   */
        this->timer--;
        this->total_time += TRAVELING_INC;

        /* Once the truck has arrived, determine whether to wait or unload              */
        if(this->timer == 0) {

            /* Keep track of this station's index for when we have finished unloading   */
            this->station_idx = curr_idx;
            
            /* Set the wait time to the number of the trucks queued ahead of this truck */
            this->timer = stations[curr_idx].get_queue();

            /* Add this truck to the station's queue and move the index to the next     *
             * station, which will have the lowest wait time (See the Shortest Wait     *
             * Time Allocation strategy in the Notes section of the header file)        */
            stations[curr_idx++].increment_queue();
            curr_idx %= stations.size();

            /* If there are trucks ahead of this one proceed to the Waiting State.      *
             * Otherwise proceed to the Unloading state                                 */
            this->state = (this->timer) ? TruckState::Waiting : TruckState::Unloading;
        }
    }
    else if(TruckState::Waiting == this->state) { 

        /* Decrement the remaining waiting time in the queue and increment the total    */
        this->timer--;
        this->total_time += WAITING_INC;

        /* Once the queue is empty, the truck will start to be unloaded                 */
        if(this->timer == 0) {

            /* Proceed to the Unloading State                                           */
            this->state = TruckState::Unloading;
        }
    }
    else if(TruckState::Unloading == this->state) {

        /* Increment the total, there is no timer since the truck only takes one loop   * 
         * i.e. 5 minutes to unload                                                     */
        this->total_time += UNLOADING_INC;

        /* Once the unloading has completed, then increment the Station's trucks        *
         * unloaded count                                                               */
        stations[this->station_idx].increment_trucks_unloaded();

        /* Set the timer to the travel time to the mines                                */
        this->timer = TRAVEL_TIME;

        /* Proceed to the TravelMining State                                            */
        this->state = TruckState::TravelMining;
    }
    else if(TruckState::TravelMining == this->state) {

        /* Decrement the remaining mining time and increment the total                  */
        this->timer--;
        this->total_time += TRAVELING_INC;

        /* Once the truck has arrived, calculate the time it will take to mine          */
        if(this->timer == 0) {
            
            /* Use the global RNG to determine the time that the truck will be mining   */
            this->timer = rmt(gen);

            /* Proceed to the Mining State                                              */
            this->state = TruckState::Mining;
        }
    }        
    else {
        /* This state should not be reached                                             */
        throw std::runtime_error("Error Occured, this state should not be reached");
    }
}

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
uint64_t Truck::get_total_time() {
    return this->total_time;
}

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
Simulation::Simulation( uint16_t num_trucks, 
                        uint16_t num_stations, 
                        bool debug) : trucks(num_trucks), 
                                      stations(num_stations),
                                      curr_station_idx(0),  
                                      debug(debug),  
                                      total_time(MAX_TIME)  {}

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
Simulation::~Simulation() {}

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
void Simulation::run_sim() {

    /* Initialize a local variable with the max sim time                                */
    size_t sim_time = this->total_time;

    while(sim_time) {

        /* Run through all the trucks                                                   */
        for(auto& truck: trucks) {

            truck.run(stations, curr_station_idx);

            if(this->debug) {
                compare_idx_val_to_actual_min(stations, curr_station_idx);
            }
        }

        /* Decrement all the queues for each station if the queue is greater than zero  */
        std::for_each(stations.begin(), stations.end(), [](Station& station) {
            station.decrement_queue();
        });

        /* Decrement the counter, each step represents 5 minutes                        */
        sim_time--;
    }

    /* Perform logging                                                                  */
    for(auto& truck : trucks) {

        truck.logging(MAX_TIME);

        if(this->debug) {
            compare_total_time_to_max_time(truck, MAX_TIME);
        }
    }
    for(auto& station : stations) {
        station.logging();
    }
}

/****************************************************************************************
 * get_command_line_input                                                               *
 * @brief Prompts the user to enter a value between 1 and 65535, validates the input,   *
 *        and assigns it to the provided `uint16_t` reference.                          *
 *                                                                                      *
 * This function continuously prompts the user until a valid numeric input within the   *
 * range of 1 to 65535 is entered. The input is validated to ensure it contains only    *
 * digits, and is converted to an integer using `std::stoi`. If the input is valid,     *
 * the value is assigned to the provided reference.                                     *
 *                                                                                      *
 * @param value: Reference to a `uint16_t` variable where the validated input will be   *
 *               stored.                                                                *
 * @param prompt: The message displayed to the user when prompting for input.           *
 * @return: None                                                                        *
 ****************************************************************************************/
static void get_command_line_input(uint16_t& value, const std::string prompt) {

    std::string input;

    while(true) {

        std::cout << prompt;
        std::getline(std::cin, input);

        /* Check that all the characters are numeric */
        if(!input.empty() && std::find_if(input.begin(), input.end(), [](char c) { 
            return !(isdigit(c)); 
            }) == input.end()) 
        {
            size_t num = std::stoi(input);

            if((num > 0) && (num < 65536)) {
                value = static_cast<uint16_t>(num);
                break;
            }
    
        }
        std::cout << "Invalid input" << std::endl;
    }
    std::cout << "Success" << std::endl;
}

/****************************************************************************************
 * get_command_line_input                                                               *
 * @brief Prompts the user to enter a value of 0 or 1, validates the input, and assigns *
 *        the corresponding boolean value to the provided reference.                    *
 *                                                                                      *
 * This function continuously prompts the user until a valid numeric input of 0 or 1    *
 * is entered. The input is validated to ensure it contains only digits, and is         *
 * converted to an integer using `std::stoi`. If the input is valid, the corresponding  *
 * boolean value (false for 0, true for 1) is assigned to the provided reference.       *
 *                                                                                      *
 * @param value: Reference to a `bool` variable where the validated input will be       *
 *               stored.                                                                *
 * @param prompt: The message displayed to the user when prompting for input.           *
 * @return: None                                                                        *
 ****************************************************************************************/
static void get_command_line_input(bool& value, const std::string prompt) {

    std::string input;

    while(true) {

        std::cout << prompt;
        std::getline(std::cin, input);

        /* Check that all the characters are numeric */
        if(!input.empty() && std::find_if(input.begin(), input.end(), [](char c) { 
            return !(isdigit(c)); 
            }) == input.end()) 
        {
            size_t num = std::stoi(input);

            if(num == 1) {
                value = true;
                break;
            }
            else if(num == 0) {
                value = false;
                break;
            }
        }
        std::cout << "Invalid input" << std::endl;
    }
    std::cout << "Success" << std::endl;
}

/****************************************************************************************
 * prompt_to_continue                                                                   *
 * @brief Prompts the user to decide whether to run another simulation or exit the      *
 *        program.                                                                      *
 *                                                                                      *
 * This function repeatedly asks the user whether they would like to run another        *
 * simulation by entering 'y' for yes or 'n' for no. The input is case-insensitive. If  *
 * the user enters 'y' or 'Y', the function returns true, indicating that the program   *
 * should continue with another simulation. If the user enters 'n' or 'N', the function *
 * returns false, indicating that the program should terminate. Any other input is      *
 * considered invalid, and the user is prompted again until a valid response is         *
 * provided.                                                                            *
 *                                                                                      *
 * @param: None                                                                         *
 * @return: bool - Returns true if the user wants to run another simulation,            *
 *                 false if they want to exit.                                          *
 ****************************************************************************************/
bool prompt_to_continue() {

    std::string input;

    while (true) {

        std::cout << "Would you like to run another simulation? (y/n): ";
        std::getline(std::cin, input);

        if (input == "y" || input == "Y") {
            return true;
        } 
        else if (input == "n" || input == "N") {
            return false;
        }
        std::cout << "Invalid input" << std::endl;
    }
    std::cout << "Success" << std::endl;
}


/****************************************************************************************
 * main                                                                                 *
 * @brief The entry point of the program, responsible for initializing and running      *
 *        simulations in a loop, based on user input.                                   *
 *                                                                                      *
 * The `main` function continuously prompts the user for input to configure the         *
 * simulation (number of trucks, number of stations, and debug mode). After setting     *
 * up the simulation, it runs the simulation and, upon completion, asks the user        *
 * if they would like to run another simulation. If the user chooses to exit, the       *
 * loop breaks and the program terminates.                                              *
 *                                                                                      *
 * @param: None                                                                         *
 * @return: int - Returns 0 upon successful completion of the program.                  *
 ****************************************************************************************/
int main() {

    uint16_t num_trucks;
    uint16_t num_stations;
    bool debug;

    while(true) {

        /* Get the values from user input                                               */
        get_command_line_input(num_trucks, "Number of trucks: (1 - 65535) ");
        get_command_line_input(num_stations, "Number of stations: (1 - 65535) ");
        get_command_line_input(debug, "Debug mode: (0: Debug Off, 1 : Debug On) ");

        /* Populate the simulation                                                      */
        Simulation mining_sim(num_trucks, num_stations, debug);

        /* Run the simulation                                                           */
        mining_sim.run_sim();

        /* Ask the user if they want to run another simulation                          */ 
        if (!prompt_to_continue()) {
            break;
        }
    } 

    return 0;
}