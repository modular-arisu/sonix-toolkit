# CMakeList.txt: sonix-toolkit에 대한 CMake 프로젝트, 여기에 소스를 포함하고
# 프로젝트 특정 논리를 정의합니다.
#

# 이 프로젝트의 실행 파일에 소스를 추가합니다.
add_executable (sonix-toolkit "sonix-toolkit.cpp" "sonix-toolkit.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET sonix-toolkit PROPERTY CXX_STANDARD 20)
endif()

# Add and link required libraries
find_package(hidapi CONFIG REQUIRED)

target_link_libraries(sonix-toolkit PRIVATE hidapi::hidapi)

# TODO: 필요한 경우 테스트를 추가하고 대상을 설치합니다.
