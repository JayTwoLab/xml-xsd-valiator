#include "simple_error_handler.hpp"

#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/util/XMLString.hpp>
#include <spdlog/spdlog.h>

namespace j2::xercesc {

simple_error_handler::simple_error_handler(const std::shared_ptr<spdlog::logger>& logger)
    : has_errors_(false), logger_(logger)
{
}

bool simple_error_handler::has_errors() const {
    return has_errors_;
}

const std::vector<std::string>& simple_error_handler::error_messages() const {
    return error_messages_;
}

void simple_error_handler::warning(const ::xercesc::SAXParseException& exc) {
    print_message("Warning", exc);
}

void simple_error_handler::error(const ::xercesc::SAXParseException& exc) {
    has_errors_ = true;
    std::string current_tag = element_stack_.empty() ? "" : element_stack_.back();
    print_message("Error", exc, current_tag);
    collect_message("Error", exc, current_tag);
}

void simple_error_handler::fatalError(const ::xercesc::SAXParseException& exc) {
    has_errors_ = true;
    print_message("Fatal", exc);
    collect_message("Fatal", exc);
}

void simple_error_handler::startElement(const XMLCh* const /*uri*/,
                                        const XMLCh* const localname,
                                        const XMLCh* const /*qname*/,
                                        const ::xercesc::Attributes& /*attrs*/)
{
    // Save the starting tag name to the stack
    char* tag = ::xercesc::XMLString::transcode(localname);
    element_stack_.emplace_back(tag);
    ::xercesc::XMLString::release(&tag);
}

void simple_error_handler::endElement(const XMLCh* const /*uri*/,
                                      const XMLCh* const /*localname*/,
                                      const XMLCh* const /*qname*/)
{
    // Remove one from the stack from the exit tag
    if (!element_stack_.empty()) {
        element_stack_.pop_back();
    }
}

void simple_error_handler::print_message(const char* level,
                                         const ::xercesc::SAXParseException& exc,
                                         const std::string& current_tag)
{
    // Output error messages as logs
    char* message = ::xercesc::XMLString::transcode(exc.getMessage());

    if (logger_) {
        if (!current_tag.empty()) {
            logger_->error("[{}] <{}> line {}, column {} : {}",
                           level,
                           current_tag,
                           static_cast<unsigned long>(exc.getLineNumber()),
                           static_cast<unsigned long>(exc.getColumnNumber()),
                           message);
        } else {
            logger_->error("[{}] line {}, column {} : {}",
                           level,
                           static_cast<unsigned long>(exc.getLineNumber()),
                           static_cast<unsigned long>(exc.getColumnNumber()),
                           message);
        }
    } else {
        if (!current_tag.empty()) {
            spdlog::error("[spdlog logger not set] [{}] <{}> line {}, column {} : {}",
                          level,
                          current_tag,
                          static_cast<unsigned long>(exc.getLineNumber()),
                          static_cast<unsigned long>(exc.getColumnNumber()),
                          message);
        } else {
            spdlog::error("[spdlog logger not set] [{}] line {}, column {} : {}",
                          level,
                          static_cast<unsigned long>(exc.getLineNumber()),
                          static_cast<unsigned long>(exc.getColumnNumber()),
                          message);
        }
    }

    ::xercesc::XMLString::release(&message);
}

void simple_error_handler::collect_message(const char* level,
                                           const ::xercesc::SAXParseException& exc,
                                           const std::string& current_tag)
{
    // Accumulate error messages to vectors in string form
    char* message = ::xercesc::XMLString::transcode(exc.getMessage());

    std::string formatted;
    if (!current_tag.empty()) {
        formatted = "[" + std::string(level) + "] <" + current_tag + "> line " +
            std::to_string(static_cast<unsigned long>(exc.getLineNumber())) +
            ", column " + std::to_string(static_cast<unsigned long>(exc.getColumnNumber())) +
            " : " + message;
    } else {
        formatted = "[" + std::string(level) + "] line " +
            std::to_string(static_cast<unsigned long>(exc.getLineNumber())) +
            ", column " + std::to_string(static_cast<unsigned long>(exc.getColumnNumber())) +
            " : " + message;
    }

    error_messages_.push_back(formatted);
    ::xercesc::XMLString::release(&message);
}

} // namespace j2::xercesc
