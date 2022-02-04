{
	"targets": [
		{
			"target_name": "transport",
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
				"src/transports/ysocket.cpp",																		
				"src_gyp/nodetransport.cpp",
				"src_gyp/exportclass.cpp",
				"src_gyp/thread-safe-callback.cpp",
				"packages/kcp/ikcp.c",
				"packages/yasio/yasio/xxsocket.cpp",
				"packages/yasio/yasio/yasio.cpp"
			],
			"include_dirs": [
				"<!@(node -p \"require('node-addon-api').include\")",
				"node_modules/napi-thread-safe-callback-cancellable",
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
				"NAPI_DISABLE_CPP_EXCEPTIONS",
				"YASIO_HAVE_KCP=1"			
			]
		}
	]
}