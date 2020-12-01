{
	"targets": [
		{
			"target_name": "libtransport",
			"cflags!": [],
			"cflags_cc": [
				"-std=c++17",
				"-O0",
				"-Wall",
				"-fexceptions",
				"-march=native",
				"-fPIC"
			],
			"cflags_cc!": [
				"-fno-exceptions",
				"-fno-rtti"
			],
			"sources": [				
				"src/utils/random.cpp",
				"src/utils/time.cpp",	
				"src/transports/base.cpp",
				"src/transports/kcp.cpp",											
				"src_gyp/nodetransport.cpp",
				"src_gyp/exportclass.cpp",
				"packages/kcp/ikcp.c",
				"packages/yasio/yasio/ibstream.cpp",
				"packages/yasio/yasio/obstream.cpp",
				"packages/yasio/yasio/xxsocket.cpp",
				"packages/yasio/yasio/yasio.cpp"
			],
			"include_dirs": [
				"<!@(node -p \"require('node-addon-api').include\")",
				"include",
				"src_gyp",					
				"packages",
				"packages/yasio",				
				"packages/kcp",				
				"packages/json/include"				
			],
			"libraries": [				
			],
			"link_settings": {
				"libraries": [								
				]
			},
			"defines": [
				"NAPI_DISABLE_CPP_EXCEPTIONS"				
			]
		}
	]
}