
# Currently we are in the Code folder: ${CMAKE_CURRENT_LIST_DIR}
# Get the platform specific folder ${pal_dir} for the current folder: ${CMAKE_CURRENT_LIST_DIR}/Platform/${PAL_PLATFORM_NAME}
# Note: ly_get_list_relative_pal_filename will take care of the details for us, as this may be a restricted platform
#       in which case it will see if that platform is present here or in the restricted folder.
#       i.e. It could here in our gem : Gems/RecastO3DEGem/Code/Platform/<platorm_name>  or
#            <restricted_folder>/<platform_name>/Gems/RecastO3DEGem/Code
ly_get_list_relative_pal_filename(pal_dir ${CMAKE_CURRENT_LIST_DIR}/Platform/${PAL_PLATFORM_NAME} ${o3de_gem_restricted_path} ${o3de_gem_path} ${o3de_gem_name})

# Now that we have the platform abstraction layer (PAL) folder for this folder, thats where we will find the
# traits for this platform. Traits for a platform are defines for things like whether or not something in this gem
# is supported by this platform.
include(${pal_dir}/PAL_${PAL_PLATFORM_NAME_LOWERCASE}.cmake)

# Add the RecastO3DEGem.Static target
# Note: We include the common files and the platform specific files which are set in recasto3degem_common_files.cmake
# and in ${pal_dir}/recasto3degem_${PAL_PLATFORM_NAME_LOWERCASE}_files.cmake
ly_add_target(
    NAME RecastO3DEGem.Static STATIC
    NAMESPACE Gem
    FILES_CMAKE
        recasto3degem_files.cmake
        ${pal_dir}/recasto3degem_${PAL_PLATFORM_NAME_LOWERCASE}_files.cmake
    INCLUDE_DIRECTORIES
        PUBLIC
            Include
            ../External/DebugUtils/Include
            ../External/Detour/Include
            ../External/DetourCrowd/Include
            ../External/DetourTileCache/Include
            ../External/Recast/Include
        PRIVATE
            Source
    BUILD_DEPENDENCIES
        PUBLIC
            AZ::AzCore
            AZ::AzFramework
            Gem::DebugDraw.Static
)

# Here add RecastO3DEGem target, it depends on the RecastO3DEGem.Static
ly_add_target(
    NAME RecastO3DEGem ${PAL_TRAIT_MONOLITHIC_DRIVEN_MODULE_TYPE}
    NAMESPACE Gem
    FILES_CMAKE
        recasto3degem_shared_files.cmake
        ${pal_dir}/recasto3degem_shared_${PAL_PLATFORM_NAME_LOWERCASE}_files.cmake
    INCLUDE_DIRECTORIES
        PUBLIC
            Include
        PRIVATE
            Source
    BUILD_DEPENDENCIES
        PRIVATE
            Gem::RecastO3DEGem.Static
)

# By default, we will specify that the above target RecastO3DEGem would be used by
# Client and Server type targets when this gem is enabled.  If you don't want it
# active in Clients or Servers by default, delete one of both of the following lines:
ly_create_alias(NAME RecastO3DEGem.Clients NAMESPACE Gem TARGETS Gem::RecastO3DEGem)
ly_create_alias(NAME RecastO3DEGem.Servers NAMESPACE Gem TARGETS Gem::RecastO3DEGem)

# If we are on a host platform, we want to add the host tools targets like the RecastO3DEGem.Editor target which
# will also depend on RecastO3DEGem.Static
if(PAL_TRAIT_BUILD_HOST_TOOLS)
    ly_add_target(
        NAME RecastO3DEGem.Editor.Static STATIC
        NAMESPACE Gem
        FILES_CMAKE
            recasto3degem_editor_files.cmake
        INCLUDE_DIRECTORIES
            PRIVATE
                Source
            PUBLIC
                Include
        BUILD_DEPENDENCIES
            PUBLIC
                AZ::AzToolsFramework
                Gem::RecastO3DEGem.Static
    )

    ly_add_target(
        NAME RecastO3DEGem.Editor GEM_MODULE
        NAMESPACE Gem
        AUTOMOC
        FILES_CMAKE
            recasto3degem_editor_shared_files.cmake
        INCLUDE_DIRECTORIES
            PRIVATE
                Source
            PUBLIC
                Include
        BUILD_DEPENDENCIES
            PUBLIC
                Gem::RecastO3DEGem.Editor.Static
    )

    # By default, we will specify that the above target RecastO3DEGem would be used by
    # Tool and Builder type targets when this gem is enabled.  If you don't want it
    # active in Tools or Builders by default, delete one of both of the following lines:
    ly_create_alias(NAME RecastO3DEGem.Tools    NAMESPACE Gem TARGETS Gem::RecastO3DEGem.Editor)
    ly_create_alias(NAME RecastO3DEGem.Builders NAMESPACE Gem TARGETS Gem::RecastO3DEGem.Editor)


endif()

################################################################################
# Tests
################################################################################
# See if globally, tests are supported
if(PAL_TRAIT_BUILD_TESTS_SUPPORTED)
    # We globally support tests, see if we support tests on this platform for RecastO3DEGem.Static
    if(PAL_TRAIT_RECASTO3DEGEM_TEST_SUPPORTED)
        # We support RecastO3DEGem.Tests on this platform, add RecastO3DEGem.Tests target which depends on RecastO3DEGem.Static
        ly_add_target(
            NAME RecastO3DEGem.Tests ${PAL_TRAIT_TEST_TARGET_TYPE}
            NAMESPACE Gem
            FILES_CMAKE
                recasto3degem_files.cmake
                recasto3degem_tests_files.cmake
            INCLUDE_DIRECTORIES
                PRIVATE
                    Tests
                    Source
            BUILD_DEPENDENCIES
                PRIVATE
                    AZ::AzTest
                    AZ::AzFramework
                    Gem::RecastO3DEGem.Static
        )

        # Add RecastO3DEGem.Tests to googletest
        ly_add_googletest(
            NAME Gem::RecastO3DEGem.Tests
        )
    endif()

    # If we are a host platform we want to add tools test like editor tests here
    if(PAL_TRAIT_BUILD_HOST_TOOLS)
        # We are a host platform, see if Editor tests are supported on this platform
        if(PAL_TRAIT_RECASTO3DEGEM_EDITOR_TEST_SUPPORTED)
            # We support RecastO3DEGem.Editor.Tests on this platform, add RecastO3DEGem.Editor.Tests target which depends on RecastO3DEGem.Editor
            ly_add_target(
                NAME RecastO3DEGem.Editor.Tests ${PAL_TRAIT_TEST_TARGET_TYPE}
                NAMESPACE Gem
                FILES_CMAKE
                    recasto3degem_editor_tests_files.cmake
                INCLUDE_DIRECTORIES
                    PRIVATE
                        Tests
                        Source
                BUILD_DEPENDENCIES
                    PRIVATE
                        AZ::AzTest
                        Gem::RecastO3DEGem.Editor
            )

            # Add RecastO3DEGem.Editor.Tests to googletest
            ly_add_googletest(
                NAME Gem::RecastO3DEGem.Editor.Tests
            )
        endif()
    endif()
endif()
