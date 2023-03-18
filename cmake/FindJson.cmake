# Copyright (c) 2023-present, Ian Dinwoodie.
# Distributed under the MIT License (http://opensource.org/licenses/MIT).

set(json_version "3.11.2")
message(STATUS "Fetching JSON ${json_version}")

include(FetchContent)

FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY "https://github.com/nlohmann/json.git"
    GIT_TAG "v${json_version}"
)

FetchContent_MakeAvailable(nlohmann_json)
