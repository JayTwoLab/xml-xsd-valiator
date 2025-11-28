#include "xml_validator_app.hpp"
#include "simple_error_handler.hpp"

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUni.hpp>

#include <filesystem>
#include <memory>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace {

// Helper function that converts std::string to XMLCh*
XMLCh* to_xml_ch(const std::string& s) {
    return ::xercesc::XMLString::transcode(s.c_str());
}

} // anonymous namespace

namespace j2::xercesc {

xml_validator_app::xml_validator_app()
{
    // Creating loggers for console color output
    const std::string logger_name = "xml_validator";
    auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    logger_ = std::make_shared<spdlog::logger>(logger_name, sink);
    spdlog::register_logger(logger_);
}

void xml_validator_app::set_xml_path(const std::string& xml_path) {
    xml_path_ = xml_path;
}

void xml_validator_app::set_xsd_path(const std::string& xsd_path) {
    xsd_path_ = xsd_path;
}

xml_validator_result xml_validator_app::run() {
    // XML/XSD path validation
    if (xml_path_.empty() || xsd_path_.empty()) {
        logger_->error("XML path or XSD path is not set.");
        return xml_validator_result::file_not_found;
    }
    if (!std::filesystem::exists(xml_path_)) {
        logger_->error("XML file does not exist: {}", xml_path_);
        return xml_validator_result::file_not_found;
    }
    if (!std::filesystem::exists(xsd_path_)) {
        logger_->error("XSD file does not exist: {}", xsd_path_);
        return xml_validator_result::file_not_found;
    }

    try {
        // Xerces-C++ Initialization (requires once per process)
        ::xercesc::XMLPlatformUtils::Initialize();
    } catch (const ::xercesc::XMLException& e) {
        char* message = ::xercesc::XMLString::transcode(e.getMessage());
        logger_->error("XMLPlatformUtils::Initialize failed: {}", message);
        ::xercesc::XMLString::release(&message);
        return xml_validator_result::init_failed;
    }

    xml_validator_result result = xml_validator_result::success;

    // Create SAX2 XML parser (manual delete required)
    ::xercesc::SAX2XMLReader* parser =
        ::xercesc::XMLReaderFactory::createXMLReader();

    try {
        simple_error_handler error_handler(logger_);
        parser->setErrorHandler(&error_handler);
        parser->setContentHandler(&error_handler);

        // Set verification and schema-related options
        parser->setFeature(::xercesc::XMLUni::fgSAX2CoreValidation, true);
        parser->setFeature(::xercesc::XMLUni::fgXercesSchema, true);
        parser->setFeature(::xercesc::XMLUni::fgXercesSchemaFullChecking, true);

        // Setting the location of the no-namespace schema
        XMLCh* no_ns_location = to_xml_ch(xsd_path_);
        parser->setProperty(
            ::xercesc::XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation,
            no_ns_location);
        ::xercesc::XMLString::release(&no_ns_location);

        // parsing XML files
        XMLCh* xml_file = to_xml_ch(xml_path_);
        parser->parse(xml_file);
        ::xercesc::XMLString::release(&xml_file);

        if (error_handler.has_errors()) {
            logger_->error("XML failed XSD schema validation.");
            result = xml_validator_result::schema_validation_failed;
        } else {
            logger_->info("XML passed XSD schema validation.");
        }
    } catch (const ::xercesc::XMLException& e) {
        char* message = ::xercesc::XMLString::transcode(e.getMessage());
        logger_->error("XMLException occurred: {}", message);
        ::xercesc::XMLString::release(&message);
        result = xml_validator_result::xml_exception;
    } catch (const ::xercesc::SAXParseException& e) {
        char* message = ::xercesc::XMLString::transcode(e.getMessage());
        logger_->error("SAXParseException occurred: {}", message);
        ::xercesc::XMLString::release(&message);
        result = xml_validator_result::sax_parse_exception;
    } catch (...) {
        logger_->error("Unknown exception occurred");
        result = xml_validator_result::unknown_exception;
    }

    if (parser) {
        delete parser;
    }

    // Xerces-C++ exit (paired with Initialize)
    ::xercesc::XMLPlatformUtils::Terminate();

    return result;
}

} // namespace j2::xercesc
