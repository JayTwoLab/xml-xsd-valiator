#pragma once

#include <xercesc/sax2/DefaultHandler.hpp>
#include <memory>
#include <string>
#include <vector>

namespace spdlog {
class logger;
}

namespace j2::xercesc {

class simple_error_handler : public ::xercesc::DefaultHandler {
public:
    explicit simple_error_handler(const std::shared_ptr<spdlog::logger>& logger);
    simple_error_handler(const simple_error_handler&) = delete;
    simple_error_handler& operator=(const simple_error_handler&) = delete;

    bool has_errors() const;
    const std::vector<std::string>& error_messages() const;

    void warning(const ::xercesc::SAXParseException& exc) override;
    void error(const ::xercesc::SAXParseException& exc) override;
    void fatalError(const ::xercesc::SAXParseException& exc) override;

    void startElement(const XMLCh* const uri,
                      const XMLCh* const localname,
                      const XMLCh* const qname,
                      const ::xercesc::Attributes& attrs) override;

    void endElement(const XMLCh* const uri,
                    const XMLCh* const localname,
                    const XMLCh* const qname) override;

private:
    void print_message(const char* level,
                       const ::xercesc::SAXParseException& exc,
                       const std::string& current_tag = "");

    void collect_message(const char* level,
                         const ::xercesc::SAXParseException& exc,
                         const std::string& current_tag = "");

    bool has_errors_{false};
    std::shared_ptr<spdlog::logger> logger_;
    std::vector<std::string> error_messages_;
    std::vector<std::string> element_stack_;
};

} // namespace j2::xercesc
