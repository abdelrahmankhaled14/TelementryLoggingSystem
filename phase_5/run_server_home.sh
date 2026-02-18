#!/bin/bash

# Create config in home directory
mkdir -p ~/.config
cat > ~/.config/commonapi.ini << 'EOF'
[libcommonapi]
default_binding=someip

[libcommonapi-someip]
default_context_name=someip
default_context_libname=libCommonAPI-SomeIP.so.3
EOF

# Set environment to use our config
export CommonAPI_CONF=~/.config/commonapi.ini
export VSOMEIP_CONFIGURATION=$(pwd)/vsomeip_config.json

echo "========================================="
echo "CommonAPI Config: $CommonAPI_CONF"
echo "vSOMEIP Config: $VSOMEIP_CONFIGURATION"
echo "========================================="
echo ""

# Run server
./server
