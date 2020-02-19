add_library(types
        encrypted/encrypted_integer_type.cpp
        unencrypted/integer_type.cpp
        value.cpp
        type.cpp
        bool_result.cpp
        type_id.h
        ../expression/expression.cpp
        ../expression/expression.h
        ../expression/expression_id.h
        ../expression/bool_expression.cpp
        ../expression/bool_expression.h
        ../expression/arithmetic_expression.cpp
        ../expression/arithmetic_expression.h encrypted/encrypted_boolean_type.cpp encrypted/encrypted_boolean_type.h unencrypted/boolean_type.cpp unencrypted/boolean_type.h)
target_link_libraries(types emp-tool)
