#include "CommandLineConfiguration.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "tclap/CmdLine.h"
#include "json/json.h"

namespace {

// Remove chars from both ends of a string
std::string trim(std::string str, std::string chars=" \t\r\n") {
    auto start = str.find_first_not_of(chars);
    if (start == std::string::npos) {
        start = 0;
    }
    auto len = str.find_last_not_of(chars);
    if (len != std::string::npos) {
        len = len - start + 1;
    }
    return str.substr(start, len);
}

// Create TCLAP args from a json doc and add them to a CmdLine
std::vector<std::unique_ptr<TCLAP::Arg>> createArgs(const std::string& current_path,
                                                    Json::Value& root,
                                                    TCLAP::CmdLine& cmd_line,
std::unordered_map<std::string, Json::Value*>& values_by_name) {
    std::string arg_name;
    std::string arg_desc;
    std::vector<std::unique_ptr<TCLAP::Arg>> args;

    for (const auto& key : root.getMemberNames()) {
        auto& value = root[key];
        if (value.isObject()) {
            createArgs(current_path + trim(key) + "-", value, cmd_line, values_by_name);
        } else {
            arg_name = current_path + trim(key);
            arg_desc = trim(value.getComment(Json::CommentPlacement::commentBefore), " \t\r\n/*");
            if (value.isString()) {
                args.emplace_back(new TCLAP::ValueArg<std::string> {"", arg_name, arg_desc, false,
                                                                    value.asString(),
                                                                    "string", cmd_line
                                                                   });
            } else if (value.isBool()) {
                args.emplace_back(new TCLAP::ValueArg<bool> {"", arg_name, arg_desc, false,
                                                             value.asBool(), "bool", cmd_line
                                                            });
            }  else if (value.isDouble()) {
                args.emplace_back(new TCLAP::ValueArg<double> {"", arg_name, arg_desc, false,
                                                               value.asDouble(), "float", cmd_line
                                                              });
            } else if (value.isInt()) {
                args.emplace_back(new TCLAP::ValueArg<int> {"", arg_name, arg_desc, false,
                                                            value.asInt(), "int", cmd_line
                                                           });
            } else {
                throw std::runtime_error(std::string("Configuration key ") + arg_name +
                                         " contains unsupported type.");
            }

            values_by_name[arg_name] = &value;
        }
    }

    return args;
}

// Get the values from command line args and update the parsed JSON doc
void readArgs(std::vector<std::unique_ptr<TCLAP::Arg>>& args,
              std::unordered_map<std::string, Json::Value*>& values_by_name) {
    for (auto& arg : args) {
        if (auto string_arg = dynamic_cast<TCLAP::ValueArg<std::string>*>(arg.get())) {
            *values_by_name[string_arg->getName()] = string_arg->getValue();
        } else if (auto bool_arg = dynamic_cast<TCLAP::ValueArg<bool>*>(arg.get())) {
            *values_by_name[bool_arg->getName()] = bool_arg->getValue();
        } else if (auto double_arg = dynamic_cast<TCLAP::ValueArg<double>*>(arg.get())) {
            *values_by_name[double_arg->getName()] = double_arg->getValue();
        } else if (auto int_arg = dynamic_cast<TCLAP::ValueArg<int>*>(arg.get())) {
            *values_by_name[int_arg->getName()] = int_arg->getValue();
        } else {
            throw std::runtime_error(std::string("Argument ") + arg->getName() +
                                     " is unsupported type.");
        }
    }
}

} // namespace

namespace warped {

std::pair<bool, std::string> CommandLineConfiguration::parse(
    const std::string& description, int argc, const char* const* argv,
    const std::vector<TCLAP::Arg*>& user_args) {

    TCLAP::CmdLine cmd_line(description);

    // Add JSON specified args
    std::unordered_map<std::string, Json::Value*> values_by_name;
    auto args = createArgs("", *root_, cmd_line, values_by_name);

    // Add default args
    TCLAP::ValueArg<std::string> config_arg("c", "config", "Warped configuration file",
                                            false, "", "file", cmd_line);
    TCLAP::SwitchArg dump_arg("", "dump-config",
                              "print the configured settings as a json object to stdout",
                              cmd_line, false);

    // Add user provided args
    for (auto arg : user_args) {
        cmd_line.add(arg);
    }

    cmd_line.parse(argc, argv);

    // Read the values from the command line into the JSON root
    readArgs(args, values_by_name);

    return std::make_pair(dump_arg.getValue(), config_arg.getValue());
}


} // namespace warped