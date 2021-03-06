CONTAINER Ostack
{
	NAME Ostack;
	INCLUDE Obase;

	GROUP STACK_GROUP_STACK
	{
		DEFAULT 1;

		LINK	STACK_BASE_PATH					{ ACCEPT { Ospline; } }
		REAL	STACK_BASE_LENGTH				{ UNIT METER; MIN 0.0; STEP 0.01; }

		SEPARATOR	STACK_GROUP_ITEMS		{ }

		GROUP
		{
			COLUMNS 2;

			LONG	STACK_BASE_COUNT				{ MIN 1; }
			STATICTEXT										{ }

			LONG	STACK_ROWS_COUNT				{ MIN 1; }
			STATICTEXT										{ }

			REAL	STACK_ROWS_HEIGHT				{ UNIT METER; MIN 0.0; STEP 0.01; }
			BUTTON	STACK_CMD_FITHEIGHT		{ }

			BOOL	STACK_RENDERINSTANCES 	{ }
			STATICTEXT										{ }
		}

		SEPARATOR	STACK_GROUP_RANDOM	{ }

		LONG	STACK_RANDOM_SEED				{ MIN 0; }
		REAL	STACK_RANDOM_ROT				{ UNIT DEGREE; STEP 0.01; }
		REAL	STACK_RANDOM_OFF_X			{ UNIT METER; STEP 0.01; }
		REAL	STACK_RANDOM_OFF_Z			{ UNIT METER; STEP 0.01; }
	}
}
