set ( JSON_VERSION  "3.11.2" )
set ( CATCH_VERSION "2.10.2" )

set ( JSON_URL  "https://github.com/nlohmann/json/releases/download/v${JSON_VERSION}/include.zip" )
set ( CATCH_URL "https://github.com/catchorg/Catch2/releases/download/v${CATCH_VERSION}/catch.hpp" )

include ( FetchContent )

function ( fetch_dependency name url headeronly )
	string ( TOLOWER ${name} lname )

	if ( ${headeronly} )	
		FetchContent_Declare ( ${name}
			URL                  ${url}
			DOWNLOAD_NO_PROGRESS TRUE
			DOWNLOAD_NO_EXTRACT  TRUE
		)
	else ()
		FetchContent_Declare ( ${name}
			URL ${url}
		)
	endif ()
	
	FetchContent_GetProperties ( ${name} )
	if ( NOT "${${name}_POPULATED}" )
		message ( "Populating ${name}" )
		FetchContent_Populate ( ${name} )
		set ( "${name}_POPULATED" TRUE PARENT_SCOPE )
	endif ()
	
	set ( "${name}_FOLDER" "${${lname}_SOURCE_DIR}" PARENT_SCOPE )
endfunction ()

# Download dependencies
fetch_dependency ( JSON  ${JSON_URL}  FALSE )
fetch_dependency ( CATCH ${CATCH_URL} TRUE )

# Verify folder paths
message ( "Dependencies downloaded to: " )
message ( "  JSON:  ${JSON_FOLDER}" )
message ( "  CATCH: ${CATCH_FOLDER}" )
