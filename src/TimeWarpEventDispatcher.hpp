#ifndef TIME_WARP_EVENT_DISPATCHER_H
#define TIME_WARP_EVENT_DISPATCHER_H

#include <atomic>
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>
#include <deque>

#include "EventDispatcher.hpp"
#include "Event.hpp"
#include "TimeWarpFileStream.hpp"
#include "TimeWarpCommunicationManager.hpp"

namespace warped {

class LTSFQueue;
class Partitioner;
class SimulationObject;
class TimeWarpCommunicationManager;
class TimeWarpGVTManager;
class TimeWarpStateManager;
class TimeWarpOutputManager;
class TimeWarpFileStreamManager;
class TimeWarpEventSet;

// This is the EventDispatcher that will run a Time Warp synchronized parallel simulation.

class TimeWarpEventDispatcher : public EventDispatcher {
public:
    TimeWarpEventDispatcher(unsigned int max_sim_time,
        unsigned int num_worker_threads,
        std::shared_ptr<TimeWarpCommunicationManager> comm_manager,
        std::unique_ptr<TimeWarpEventSet> event_set,
        std::unique_ptr<TimeWarpGVTManager> gvt_manager,
        std::unique_ptr<TimeWarpStateManager> state_manager,
        std::unique_ptr<TimeWarpOutputManager> output_manager,
        std::unique_ptr<TimeWarpFileStreamManager> twfs_manager);

    void startSimulation(const std::vector<std::vector<SimulationObject*>>& objects);

    void initialize(const std::vector<std::vector<SimulationObject*>>& objects);

    MessageFlags receiveEventMessage(std::unique_ptr<TimeWarpKernelMessage> kmsg);

    void sendLocalEvent(std::unique_ptr<Event> event);

    void fossilCollect(unsigned int gvt);

    void cancelEvents(std::unique_ptr<std::vector<std::unique_ptr<Event>>> events_to_cancel);

    void rollback(unsigned int straggler_time, unsigned int local_object_id, SimulationObject* ob);

    unsigned int getMinimumLVT();

    unsigned int getSimulationTime(SimulationObject* object);

    FileStream& getFileStream(SimulationObject *object, const std::string& filename,
        std::ios_base::openmode mode);

    void enqueueRemoteEvent(std::unique_ptr<Event> event, unsigned int receiver_id);

    void sendRemoteEvents();

private:
    void processEvents(unsigned int id);

    unsigned int num_worker_threads_;

    std::unordered_map<std::string, SimulationObject*> objects_by_name_;
    std::unordered_map<std::string, unsigned int> local_object_id_by_name_;
    std::unordered_map<std::string, unsigned int> object_node_id_by_name_;

    const std::shared_ptr<TimeWarpCommunicationManager> comm_manager_;
    const std::unique_ptr<TimeWarpEventSet> event_set_;
    const std::unique_ptr<LTSFQueue> events_;
    const std::unique_ptr<TimeWarpGVTManager> gvt_manager_;
    const std::unique_ptr<TimeWarpStateManager> state_manager_;
    const std::unique_ptr<TimeWarpOutputManager> output_manager_;
    const std::unique_ptr<TimeWarpFileStreamManager> twfs_manager_;

    std::unique_ptr<unsigned int []> object_simulation_time_;

    std::deque<std::pair <std::unique_ptr<Event>, unsigned int>> remote_event_queue_;
    std::mutex remote_event_queue_lock_;

    static thread_local unsigned int thread_id;

    // flag to initiate minimum lvt calculation
    std::atomic<unsigned int> min_lvt_flag_ = ATOMIC_VAR_INIT(0);
    // local min lvt of each worker thread
    std::unique_ptr<unsigned int []> min_lvt_;
    // minimum timestamp of sent events used for minimum lvt calculation
    std::unique_ptr<unsigned int []> send_min_;
    std::unique_ptr<unsigned int []> local_min_lvt_flag_;
    // flag to determine if worker thread has already calculated min lvt
    std::unique_ptr<bool []> calculated_min_flag_;
};

enum class MatternColor;

struct EventMessage : public TimeWarpKernelMessage {
    EventMessage() = default;
    EventMessage(unsigned int sender, unsigned int receiver, std::unique_ptr<Event> e, int c) :
        TimeWarpKernelMessage(sender, receiver),
        event(std::move(e)),
        gvt_mattern_color(c) {}

    std::unique_ptr<Event> event;
    int gvt_mattern_color; // 0 for white, 1 for red

    MessageType get_type() { return MessageType::EventMessage; }

    WARPED_REGISTER_SERIALIZABLE_MEMBERS(cereal::base_class<TimeWarpKernelMessage>(this), event,
        gvt_mattern_color)
};


} // namespace warped

#endif
