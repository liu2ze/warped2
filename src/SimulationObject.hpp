#ifndef SIMULATION_OBJECT_HPP
#define SIMULATION_OBJECT_HPP

#include <string>
#include <vector>
#include <memory>
#include <list>

#include "FileStream.hpp"
#include "RandomNumberGenerator.hpp"

namespace warped {

class Event;
struct ObjectState;
class FileStream;
class EventDispatcher;

// SimulationObjects are the core of the simulation. All models must define at
// least one class implementing this interface. Each SimulationObject has a
// name_ that is used to identify the object. It must be unique across all
// SimulationObjects.
class SimulationObject {
public:
    SimulationObject(const std::string& name);
    virtual ~SimulationObject() {}

    // Return the state of this object.
    //
    // Any state that is mutable once the simulation has begun must be stored
    // in an ObjectState class. This object is saved and restored repeatedly
    // during the course of the simulation.
    virtual ObjectState& getState() = 0;

    // This is the main function of the SimulationObject which processes incoming events.
    //
    // It is called when an object is sent an event. The receive time of the
    // event is the current simulation time. The object may create new events
    // and return then as a vector, or return an empty vector if no events are
    // created. Events must not be created with a timestamp less than the
    // current simulation time.
    virtual std::vector<std::shared_ptr<Event>> receiveEvent(const Event& event) = 0;

    // Initialize object before the simulation starts.
    //
    // This is an optional method that is called before the simulation begins.
    // If the object needs any random number generators, they must be registered here. The object
    // may also create new events and return them as a vector.
    virtual std::vector<std::shared_ptr<Event>> initializeObject();

    FileStream& getInputFileStream(const std::string& filename);

    FileStream& getOutputFileStream(const std::string& filename, std::shared_ptr<Event> this_event);

    const std::string name_;

    unsigned int last_fossil_collect_gvt_ = 0;

    unsigned long long generation_ = 0;

    template<class RNGType>
    void registerRNG(std::shared_ptr<RNGType>);

    std::list<std::shared_ptr<RandomNumberGenerator>> rng_list_;

};

template<class RNGType>
void SimulationObject::registerRNG(std::shared_ptr<RNGType> new_rng) {
    auto rng = std::make_shared<RNGDerived<RNGType>>(new_rng);
    rng_list_.push_back(rng);
}

} // namespace warped

#endif
