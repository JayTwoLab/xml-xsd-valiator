#include "xml_validator_app.hpp"

#include <filesystem>

int main(int argc, char* argv[]) {
#ifndef CMAKE_SOURCE_DIR_PATH
#define CMAKE_SOURCE_DIR_PATH "."
#endif

    // Source directory reference path defined by CMakeLists.txt
    std::filesystem::path base = std::filesystem::path(CMAKE_SOURCE_DIR_PATH);

    // XML/XSD file path (based on project root)
    const std::string xml_path = (base / "person_invalid.xml").string();
    const std::string xsd_path = (base / "person_int_schema.xsd").string();

    j2::xercesc::xml_validator_app app;
    app.set_xml_path(xml_path);
    app.set_xsd_path(xsd_path);

    j2::xercesc::xml_validator_result result = app.run();
    return static_cast<int>(result);
}
