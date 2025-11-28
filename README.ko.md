# xe_xerces_example

[English README](README.md)

## 개요
이 프로젝트는 **Xerces-C++** 와 **spdlog** 를 사용하여 XML 파일을 XSD 스키마로 검증하는 예제입니다.

## 주요 기능
- Xerces-C++ 기반 XML 검증
- 커스텀 SAX2 Error Handler
- spdlog 로깅 지원
- MSVC / MinGW / Linux 크로스 플랫폼 CMake 구성

## 파일 구성
- `main.cpp` – 프로그램 시작점  
- `xml_validator_app.*` – 검증 실행 컨트롤러  
- `simple_error_handler.*` – SAX2 에러 핸들러  
- `person_int_schema.xsd` – 샘플 스키마  
- `person_invalid.xml` – 오류가 포함된 테스트 XML  

## 빌드 방법
```bash
cmake -B build
cmake --build build
```

## 동작 방식
1. XML / XSD 경로 로드  
2. Xerces‑C++ 초기화  
3. SAX2 파서 및 스키마 검증 옵션 설정  
4. 에러 핸들러를 통한 메시지 수집  

