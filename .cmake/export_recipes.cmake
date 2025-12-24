# Export local recipes to Conan cache
# This script automatically exports all recipes from the recipes directory to local Conan cache

# Function to export a single recipe
function(export_recipe RECIPE_NAME RECIPE_VERSION RECIPE_USER RECIPE_CHANNEL)
    set(RECIPE_PATH "${CMAKE_CURRENT_LIST_DIR}/recipes/${RECIPE_NAME}/all")
    
    # Check if recipe directory exists
    if(NOT EXISTS "${RECIPE_PATH}")
        message(WARNING "Recipe path does not exist: ${RECIPE_PATH}")
        return()
    endif()
    
    # Check if recipe is already exported by trying to inspect it
    if(RECIPE_USER AND RECIPE_CHANNEL)
        set(RECIPE_REF "${RECIPE_NAME}/${RECIPE_VERSION}@${RECIPE_USER}/${RECIPE_CHANNEL}")
    else()
        set(RECIPE_REF "${RECIPE_NAME}/${RECIPE_VERSION}")
    endif()
    
    # Check if the recipe is already in local cache
    execute_process(
        COMMAND conan search ${RECIPE_REF} --raw
        RESULT_VARIABLE SEARCH_RESULT
        OUTPUT_QUIET
        ERROR_QUIET
    )
    
    if(SEARCH_RESULT EQUAL 0)
        message(STATUS "Recipe ${RECIPE_REF} already exists in local cache, skipping export")
    else()
        message(STATUS "Exporting recipe: ${RECIPE_REF}")
        execute_process(
            COMMAND conan export ${RECIPE_PATH} ${RECIPE_REF}
            RESULT_VARIABLE EXPORT_RESULT
            OUTPUT_VARIABLE EXPORT_OUTPUT
            ERROR_VARIABLE EXPORT_ERROR
        )
        
        if(EXPORT_RESULT EQUAL 0)
            message(STATUS "Successfully exported: ${RECIPE_REF}")
        else()
            message(WARNING "Failed to export ${RECIPE_REF}")
            message(WARNING "Output: ${EXPORT_OUTPUT}")
            message(WARNING "Error: ${EXPORT_ERROR}")
        endif()
    endif()
endfunction()

# Function to export all recipes
function(export_all_recipes)
    message(STATUS "Starting to export all local recipes...")
    
    # Export recipes without user/channel (Conan 2.x style)
    export_recipe("libjpeg" "9c" "" "")
    export_recipe("openssl" "3.3.2" "" "")
    export_recipe("jsoncpp" "1.9.5" "" "")
    export_recipe("libuv" "1.49.0" "" "")
    export_recipe("libyuv" "1892" "" "")
    export_recipe("sqlite3" "3.36.0" "" "")
    
    # Export recipes with user/channel (Conan 1.x style)
    export_recipe("tinyxml" "2.6.2" "yunxin" "stable")
    export_recipe("nim" "10.9.33" "yunxin" "stable")
    export_recipe("nertc" "4.1.1" "yunxin" "stable")
    export_recipe("libcef" "3.2623.1401" "yunxin" "stable")
    export_recipe("ne_live_player" "1.6.9" "yunxin" "stable")
    export_recipe("image_ole" "4.2.0" "yunxin" "stable")
    export_recipe("ne_transfer" "0.1.0" "yunxin" "stable")
    
    message(STATUS "Finished exporting all local recipes")
endfunction()

# Auto-detect and export recipes from recipes directory
function(auto_export_recipes)
    message(STATUS "Auto-detecting recipes in recipes directory...")
    
    # Get all directories in recipes folder
    file(GLOB RECIPE_DIRS "${CMAKE_CURRENT_LIST_DIR}/recipes/*")
    
    foreach(RECIPE_DIR ${RECIPE_DIRS})
        if(IS_DIRECTORY ${RECIPE_DIR})
            get_filename_component(RECIPE_NAME ${RECIPE_DIR} NAME)
            
            # Check if config.yml exists to get version info
            set(CONFIG_FILE "${RECIPE_DIR}/config.yml")
            if(EXISTS ${CONFIG_FILE})
                # Read config.yml to extract version information
                file(READ ${CONFIG_FILE} CONFIG_CONTENT)
                
                # Try to extract versions using regex
                string(REGEX MATCHALL "\"[^\"]+\":" VERSIONS_MATCHES ${CONFIG_CONTENT})
                
                if(VERSIONS_MATCHES)
                    # Get the first version found
                    list(GET VERSIONS_MATCHES 0 FIRST_VERSION)
                    string(REGEX REPLACE "\"([^\"]+)\":" "\\1" VERSION ${FIRST_VERSION})
                    
                    # Try to detect if it needs user/channel from conanfile.py
                    set(CONANFILE_PATH "${RECIPE_DIR}/all/conanfile.py")
                    if(EXISTS ${CONANFILE_PATH})
                        file(READ ${CONANFILE_PATH} CONANFILE_CONTENT)
                        
                        # Check if it's a yunxin package based on typical patterns
                        string(FIND ${CONANFILE_CONTENT} "yunxin" YUNXIN_FOUND)
                        if(NOT YUNXIN_FOUND EQUAL -1)
                            export_recipe(${RECIPE_NAME} ${VERSION} "yunxin" "stable")
                        else()
                            export_recipe(${RECIPE_NAME} ${VERSION} "" "")
                        endif()
                    endif()
                endif()
            endif()
        endif()
    endforeach()
endfunction()

# Alternative: Create a script to dynamically export all recipes
function(export_recipes_dynamically)
    message(STATUS "Dynamically exporting all recipes from recipes directory...")
    
    # Define the mapping of package names to versions and channels based on conanfile.py
    set(PACKAGE_VERSIONS
        "libjpeg/9f@yunxin/stable"
        "openssl/3.6.0@yunxin/stable"
        "jsoncpp/1.9.6@yunxin/stable"
        "libuv/1.51.0@yunxin/stable"
        "libyuv/1892@yunxin/stable"
        "sqlite3/3.51.0@yunxin/stable"
        "tinyxml/2.6.2@yunxin/stable"
        "nim/10.9.72-beta.0@yunxin/stable"
        "nertc/4.1.1@yunxin/stable"
        "libcef/3.2623.1401@yunxin/stable"
        "ne_live_player/1.6.9@yunxin/stable"
        "image_ole/4.2.0@yunxin/stable"
        "ne_transfer/0.1.0@yunxin/stable"
    )
    
    foreach(PACKAGE_REF ${PACKAGE_VERSIONS})
        # Parse package reference
        string(REGEX MATCH "^([^/]+)" PACKAGE_NAME ${PACKAGE_REF})
        
        set(RECIPE_PATH "${CMAKE_CURRENT_LIST_DIR}/recipes/${PACKAGE_NAME}/all")
        
        # Check if recipe directory exists
        if(EXISTS "${RECIPE_PATH}")
            # Check if recipe is already exported
            execute_process(
                COMMAND conan search ${PACKAGE_REF} --raw
                RESULT_VARIABLE SEARCH_RESULT
                OUTPUT_QUIET
                ERROR_QUIET
            )
            
            if(SEARCH_RESULT EQUAL 0)
                message(STATUS "Recipe ${PACKAGE_REF} already exists in local cache, skipping export")
            else()
                message(STATUS "Exporting recipe: ${PACKAGE_REF} from ${RECIPE_PATH}")
                execute_process(
                    COMMAND conan export ${RECIPE_PATH} ${PACKAGE_REF}
                    RESULT_VARIABLE EXPORT_RESULT
                    OUTPUT_VARIABLE EXPORT_OUTPUT
                    ERROR_VARIABLE EXPORT_ERROR
                )
                
                if(EXPORT_RESULT EQUAL 0)
                    message(STATUS "Successfully exported: ${PACKAGE_REF}")
                else()
                    message(WARNING "Failed to export ${PACKAGE_REF}")
                    if(EXPORT_OUTPUT)
                        message(WARNING "Output: ${EXPORT_OUTPUT}")
                    endif()
                    if(EXPORT_ERROR)
                        message(WARNING "Error: ${EXPORT_ERROR}")
                    endif()
                endif()
            endif()
        else()
            message(WARNING "Recipe directory does not exist: ${RECIPE_PATH}")
        endif()
    endforeach()
    
    message(STATUS "Finished dynamically exporting recipes")
endfunction()

# Function to clean up Conan cache if needed
function(clean_conan_cache)
    message(STATUS "Cleaning Conan cache...")
    execute_process(
        COMMAND conan remove "*" --force
        RESULT_VARIABLE CLEAN_RESULT
        OUTPUT_QUIET
        ERROR_QUIET
    )
    if(CLEAN_RESULT EQUAL 0)
        message(STATUS "Conan cache cleaned successfully")
    else()
        message(WARNING "Failed to clean Conan cache")
    endif()
endfunction()

# Function to remove remote repositories to force local usage
function(setup_local_only_mode)
    message(STATUS "Setting up local-only mode for Conan...")
    
    # Remove all remote repositories except local ones
    execute_process(
        COMMAND conan remote list
        OUTPUT_VARIABLE REMOTES_LIST
        ERROR_QUIET
    )
    
    if(REMOTES_LIST)
        # Parse remote names and remove non-local ones
        string(REPLACE "\n" ";" REMOTES_LINES ${REMOTES_LIST})
        foreach(REMOTE_LINE ${REMOTES_LINES})
            if(REMOTE_LINE)
                string(REGEX MATCH "^([^:]+):" REMOTE_NAME_MATCH ${REMOTE_LINE})
                if(REMOTE_NAME_MATCH)
                    string(REGEX REPLACE "^([^:]+):.*" "\\1" REMOTE_NAME ${REMOTE_LINE})
                    string(STRIP ${REMOTE_NAME} REMOTE_NAME)
                    
                    # Keep local remotes, remove others
                    if(NOT REMOTE_NAME STREQUAL "local" AND NOT REMOTE_NAME STREQUAL "")
                        message(STATUS "Temporarily disabling remote: ${REMOTE_NAME}")
                        execute_process(
                            COMMAND conan remote disable ${REMOTE_NAME}
                            OUTPUT_QUIET
                            ERROR_QUIET
                        )
                    endif()
                endif()
            endif()
        endforeach()
    endif()
endfunction()
