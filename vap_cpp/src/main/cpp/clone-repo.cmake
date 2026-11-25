# 构建期自动获取三方源码：目录缺失或不完整时 git 浅克隆，幂等（目录完整则跳过）
# 离线/内网回退：以 -DVAP_JSON_GIT_URL=file://<本地镜像路径> 覆盖默认源（Young242 检视建议：提供回退方案）
set(VAP_JSON_GIT_URL "https://gitee.com/mirrors/nlohmann-json.git" CACHE STRING "nlohmann-json git source (支持 file:// 本地镜像回退)")
set(VAP_JSON_GIT_TAG "v3.12.0" CACHE STRING "nlohmann-json tag to clone (与历史 gitlink 55f93686 对应)")
macro(clone_repo name url tag MY_PATH)
    set(GIT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${MY_PATH}/${OHOS_ARCH}/${name}/)
    # 以 CMakeLists.txt 存在性判定完整性，而非仅判目录（防止克隆中途异常中断留下的残缺目录）
    if(NOT EXISTS "${GIT_DIR}CMakeLists.txt")
        file(REMOVE_RECURSE "${GIT_DIR}")
        find_package(Git REQUIRED)
        message(STATUS "clone_repo: git clone ${url} -b ${tag} --depth=1 -> ${GIT_DIR}")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} clone "${url}" "${GIT_DIR}" -b "${tag}" --depth=1
            RESULT_VARIABLE GIT_CLONE_RESULT
            TIMEOUT 180
        )
        # 失败快速终止并给出明确指引，避免滞后到 add_subdirectory 才报出难懂的错误
        if(NOT GIT_CLONE_RESULT EQUAL 0)
            message(FATAL_ERROR "clone_repo failed: 无法获取 ${url} (tag ${tag}) 到 ${GIT_DIR}。请检查 git 与网络；离线环境可用 -DVAP_JSON_GIT_URL=file://<本地nlohmann-json镜像> 作为回退源重试")
        endif()
    else()
        message(STATUS "clone_repo: already present at ${GIT_DIR}, skip")
    endif()
endmacro()
