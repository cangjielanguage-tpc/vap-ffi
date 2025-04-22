# Checkout remote repository
macro(clone_repo name url tag MY_PATH)
    set(GIT_EXECUTABLE git)
    set(GIT_REPO ${url})
    set(GIT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${MY_PATH}/${OHOS_ARCH}/${name}/)
    # 检查目标目录是否存在，如果不存在则执行git clone
    if(NOT EXISTS "${GIT_DIR}")
      message(STATUS "Cloning ${GIT_EXECUTABLE} clone ${GIT_REPO} ${GIT_DIR} ${tag}")
      execute_process(
        COMMAND ${GIT_EXECUTABLE} clone ${GIT_REPO} ${GIT_DIR} -b ${tag} --depth=1
        RESULT_VARIABLE GIT_CLONE_RESULT
        OUTPUT_VARIABLE GIT_CLONE_OUTPUT
      )
    else()
      message(STATUS "Repository already cloned at ${GIT_DIR}")
    endif()
endmacro()
