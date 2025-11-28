#pragma once

#include <memory>
#include <string>

namespace spdlog {
class logger;
}

namespace j2::xercesc {

// XML Verification Result Code
enum class xml_validator_result {
    success = 0,
    init_failed = 1,
    schema_validation_failed = 2,
    xml_exception = 3,
    sax_parse_exception = 4,
    unknown_exception = 5,
    file_not_found = 6
};

class xml_validator_app {
public:
    xml_validator_app();

    void set_xml_path(const std::string& xml_path);
    void set_xsd_path(const std::string& xsd_path);

    // Execute XML/XSD verification
    xml_validator_result run();

private:
    std::string xml_path_;
    std::string xsd_path_;
    std::shared_ptr<spdlog::logger> logger_;
};

} // namespace j2::xercesc
