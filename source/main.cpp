#include "c4d.h"
#include "main.h"


#define PLUGIN_VERSION	String("Can Stack 0.9")


Bool PluginStart()
{
	if (!RegisterStackObject())
		return false;

	GePrint(PLUGIN_VERSION);

	return true;
}


void PluginEnd()
{ }


Bool PluginMessage(Int32 id, void* data)
{
	switch (id)
	{
		case C4DPL_INIT_SYS:
			if (!resource.Init())
				return false;
	}

	return false;
}
