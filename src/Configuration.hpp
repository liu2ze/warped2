#ifndef WARPED_CONFIGURATION_HPP
#define WARPED_CONFIGURATION_HPP

#include <memory>
#include <string>
#include <vector>

namespace TCLAP { class Arg; }
namespace Json { class Value; }

namespace warped {

class EventDispatcher;
class Partitioner;
class CommunicationManager;

// This class is responsible for creating and configuring the EventDispatcher
// to be used in the simulation. There are three tiers of configuration data
// that can be used. There are default values for all necessary configuration
// options. The user can also specify a configuration file that will override
// any specified values. Finally, any configuration value can also be
// specified from the command line, which will override values from the first
// two sources.
class Configuration {
public:
    Configuration(const std::string& model_description, int argc, const char* const* argv);
    Configuration(const std::string& model_description, int argc, const char* const* argv,
                  const std::vector<TCLAP::Arg*>& cmd_line_args);
    Configuration(const std::string& config_file_name, unsigned int max_time);
    ~Configuration();

    // Create a fully configured EventDispatcher
    std::tuple<std::unique_ptr<EventDispatcher>, unsigned int>
        makeDispatcher(unsigned int num_objects);

    // Create a partitioner based on the chosen configuration.
    std::unique_ptr<Partitioner> makePartitioner();

    // Create a partitioner if configured, or return the given user provided
    // partitioner.
    std::unique_ptr<Partitioner> makePartitioner(std::unique_ptr<Partitioner> user_partitioner);

    std::unique_ptr<CommunicationManager> makeCommunicationManager();

private:
    void init(const std::string& model_description, int argc, const char* const* argv,
              const std::vector<TCLAP::Arg*>& cmd_line_args);
    void readUserConfig();

    std::string config_file_name_;
    unsigned int max_sim_time_;
    std::unique_ptr<Json::Value> root_;
};

} // namespace warped

#endif

