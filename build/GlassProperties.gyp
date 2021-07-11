{
    'targets': [
        {
            'target_name': 'GlassProperties',
            'type': 'static_library',
            'includes': [
                'GlassProperties_sources.gypi',
                '../../iZFrameworks/iZBase/build/iZBasePCH.gypi',
		'../../iZFrameworks/iZBase/build/iZBaseCommon.gypi',
		'../../iZFrameworks/iZBase/build/EnableWarnAsError.gypi',
		'../../iZFrameworks/iZBase/build/FilterSources.gypi',
            ],
            'dependencies': [
                '../../iZFrameworks/iZBase/build/iZBase.gyp:iZBase',
            ],
            'include_dirs': [
                '../src'
            ],
            'all_dependent_settings': {
		'include_dirs': [
		    '../src',
	        ],
            },
        },
	{
	    'target_name': 'GlassProperties Unit Tests',
	    'dependencies': [
		'GlassProperties',
	    ],
	    'includes': [
		'GlassProperties_sources.gypi',
		'../../iZFrameworks/iZBase/build/iZBasePCH.gypi',
                '../../iZFrameworks/iZBase/build/iZBaseCommon.gypi',
		'../../iZFrameworks/iZBase/build/FilterUnitTestSources.gypi',
		'../../iZFrameworks/iZBase/build/UnitTestConfig.gypi',
	    ],
	},
    ],
}
