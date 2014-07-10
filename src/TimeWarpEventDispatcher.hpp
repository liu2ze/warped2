#ifndef TIME_WARP_EVENT_DISPATCHER_H
#define TIME_WARP_EVENT_DISPATCHER_H

#include <atomic>
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>
#include <array>

#include "EventDispatcher.hpp"
#include "Event.hpp"
#include "EventMessage.hpp"

namespace warped {

class LTSFQueue;
class Partitioner;
class SimulationObject;
class CommunicationManager;
class GVTManager;
class StateManager;
class OutputManager;

// This is the EventDispatcher that will run a Time Warp synchronized parallel simulation.

// TODO: This is currently entirely incomplete. At this point, it's just a
// possibility for how the class might be structured in the future. All the
// configurable components should be passed in to the constructor and saved as
// const std::unique_ptrs. Any configuration values should be stored as const
// members. Any mutable members should be stored in thread_local storage or in
// a std::atomic, depending on the member.
class TimeWarpEventDispatcher : public EventDispatcher {
public:
    TimeWarpEventDispatcher(unsigned int max_sim_time,
        unsigned int num_worker_threads,
        std::shared_ptr<CommunicationManager> comm_manager,
        std::unique_ptr<LTSFQueue> events,
        std::unique_ptr<GVTManager> gvt_manager,
        std::unique_ptr<StateManager> state_manager,
        std::unique_ptr<OutputManager> output_manager);

    void startSimulation(const std::vector<std::vector<SimulationObject*>>& objects);

    void initialize(const std::vector<std::vector<SimulationObject*>>& objects);

    bool receiveEventMessage(std::unique_ptr<KernelMessage> kmsg);

    void sendRemoteEvent(std::unique_ptr<Event> event, unsigned int receiver_id);

    void sendLocalEvent(std::unique_ptr<Event> event, unsigned int thread_id);

    void fossilCollect(unsigned int gvt);

    void cancelEvents(std::unique_ptr<std::vector<std::unique_ptr<Event>>> events_to_cancel);

    void rollback(unsigned int straggler_time, unsigned int local_object_id, SimulationObject* ob);

    unsigned int getMinimumLVT();

private:
    void processEvents();

    unsigned int num_worker_threads_;

    std::unordered_map<std::string, SimulationObject*> objects_by_name_;
    std::unordered_map<std::string, unsigned int> local_object_id_by_name_;
    std::unordered_map<std::string, unsigned int> object_node_id_by_name_;

    const std::shared_ptr<CommunicationManager> comm_manager_;
    const std::unique_ptr<LTSFQueue> events_;
    const std::unique_ptr<GVTManager> gvt_manager_;
    const std::unique_ptr<StateManager> state_manager_;
    const std::unique_ptr<OutputManager> output_manager_;

    // flag to initiate gvt calculation
    std::atomic<bool> calculate_gvt_ = ATOMIC_VAR_INIT(false);

    // flag to initiate minimum lvt calculation
    std::atomic<unsigned int> min_lvt_flag_ = ATOMIC_VAR_INIT(0);

    // local min lvt of each worker thread
    std::unique_ptr<unsigned int []> min_lvt_by_thread_id_;

    // minimum timestamp of sent events used for minimum lvt calculation
    std::unique_ptr<unsigned int []> send_min_by_thread_id_;

    // flag to determine if worker thread has already calculated min lvt
    std::unique_ptr<bool []> calculated_min_lvt_by_thread_id_;
};

} // namespace warped

#endif
